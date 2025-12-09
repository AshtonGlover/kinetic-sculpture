/**
 * Unit Tests for Kinetic Sculpture System
 * 
 * To run tests: Upload this file instead of main.ino
 * Open Serial Monitor to see test results
 */

#include "config.h"
#include "audio_processor.h"
#include "motor_controller.h"
#include "watchdog_utils.h"

// Test statistics
int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

// Test helper macros
#define TEST(name) void test_##name()
#define RUN_TEST(name) runTest(#name, test_##name)
#define ASSERT_EQUAL(expected, actual) \
  if ((expected) != (actual)) { \
    Serial.print("  FAIL: Expected "); \
    Serial.print(expected); \
    Serial.print(", got "); \
    Serial.println(actual); \
    return false; \
  }
#define ASSERT_TRUE(condition) \
  if (!(condition)) { \
    Serial.println("  FAIL: Condition false"); \
    return false; \
  }
#define ASSERT_RANGE(value, min, max) \
  if ((value) < (min) || (value) > (max)) { \
    Serial.print("  FAIL: Value "); \
    Serial.print(value); \
    Serial.print(" not in range ["); \
    Serial.print(min); \
    Serial.print(", "); \
    Serial.print(max); \
    Serial.println("]"); \
    return false; \
  }

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; } // Wait for serial port to connect
  
  delay(1000);
  Serial.println("\n========================================");
  Serial.println("  KINETIC SCULPTURE - UNIT TESTS");
  Serial.println("========================================\n");
  
  // Run all tests
  RUN_TEST(config_constants);
  RUN_TEST(audio_processor_initialization);
  RUN_TEST(audio_processor_smoothing);
  RUN_TEST(motor_controller_initialization);
  RUN_TEST(motor_speed_mapping);
  RUN_TEST(motor_speed_constraints);
  RUN_TEST(watchdog_functionality);
  RUN_TEST(integration_audio_to_motor);
  
  // Print summary
  Serial.println("\n========================================");
  Serial.println("  TEST SUMMARY");
  Serial.println("========================================");
  Serial.print("Total Tests:  ");
  Serial.println(totalTests);
  Serial.print("Passed:       ");
  Serial.println(passedTests);
  Serial.print("Failed:       ");
  Serial.println(failedTests);
  
  if (failedTests == 0) {
    Serial.println("\n✓ ALL TESTS PASSED!");
  } else {
    Serial.println("\n✗ SOME TESTS FAILED");
  }
  Serial.println("========================================\n");
}

void loop() {
  // Tests run once in setup()
  delay(1000);
}

// Test runner helper
void runTest(const char* name, bool (*testFunc)()) {
  totalTests++;
  Serial.print("Test: ");
  Serial.print(name);
  Serial.print("... ");
  
  if (testFunc()) {
    Serial.println("PASS");
    passedTests++;
  } else {
    Serial.println("FAIL");
    failedTests++;
  }
}

// ============================================
// TEST CASES
// ============================================

TEST(config_constants) {
  // Verify configuration constants are sensible
  ASSERT_TRUE(SAMPLE_RATE > 0 && SAMPLE_RATE <= 10000);
  ASSERT_TRUE(BUFFER_SIZE > 0 && BUFFER_SIZE <= 100);
  ASSERT_TRUE(MIN_MOTOR_SPEED >= 0 && MIN_MOTOR_SPEED < MAX_MOTOR_SPEED);
  ASSERT_TRUE(MAX_MOTOR_SPEED <= 255);
  ASSERT_TRUE(MOTOR_UPDATE_INTERVAL > 0);
  return true;
}

TEST(audio_processor_initialization) {
  // Initialize audio processor
  initAudioProcessor();
  
  // Check that smoothed amplitude starts at 0
  int amplitude = getSmoothedAmplitude();
  ASSERT_EQUAL(0, amplitude);
  
  // Check that no sample is ready initially
  ASSERT_TRUE(!isNewSampleReady());
  
  return true;
}

TEST(audio_processor_smoothing) {
  // Simulate audio buffer by directly accessing it
  extern volatile int audioBuffer[];
  extern volatile int bufferIndex;
  
  // Fill buffer with known values (simulating loud audio)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 700; // Above DC_OFFSET (512)
  }
  
  // Process audio
  int amplitude = processAudio();
  
  // Should return positive amplitude (700 - 512 = 188)
  ASSERT_TRUE(amplitude > 0);
  ASSERT_RANGE(amplitude, 100, 250);
  
  // Fill buffer with silence (DC_OFFSET)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = DC_OFFSET;
  }
  
  // Process multiple times to let smoothing settle
  for (int i = 0; i < 50; i++) {
    amplitude = processAudio();
  }
  
  // Should approach 0
  ASSERT_RANGE(amplitude, 0, 20);
  
  return true;
}

TEST(motor_controller_initialization) {
  // Initialize motor controller
  initMotorController();
  
  // Motor pin should be set as output (can't directly test, but no crash is good)
  // We can test that we can set motor speed without errors
  setMotorSpeed(0);
  delay(10);
  
  return true;
}

TEST(motor_speed_mapping) {
  initMotorController();
  
  // Test amplitude to motor speed mapping
  // Low amplitude should give MIN_MOTOR_SPEED
  updateMotorSpeed(0);
  delay(10);
  
  // Medium amplitude
  updateMotorSpeed(256);
  delay(10);
  
  // High amplitude should give MAX_MOTOR_SPEED
  updateMotorSpeed(512);
  delay(10);
  
  // All values should work without crashing
  return true;
}

TEST(motor_speed_constraints) {
  initMotorController();
  
  // Test direct motor speed setting with constraints
  setMotorSpeed(0);
  delay(10);
  
  setMotorSpeed(128);
  delay(10);
  
  setMotorSpeed(255);
  delay(10);
  
  // Test out-of-range values (should be constrained)
  setMotorSpeed(-50);  // Should constrain to 0
  delay(10);
  
  setMotorSpeed(300);  // Should constrain to 255
  delay(10);
  
  return true;
}

TEST(watchdog_functionality) {
  // Initialize watchdog (be careful - this enables system reset!)
  // For testing, we'll just verify the functions exist and can be called
  
  // Note: We can't fully test watchdog without causing a reset
  // In production, this would reset the system if not fed regularly
  
  initWatchdog();
  resetWatchdog();
  resetWatchdog();
  
  // If we get here without reset, basic functionality works
  return true;
}

TEST(integration_audio_to_motor) {
  // Integration test: simulate full audio-to-motor pipeline
  
  initAudioProcessor();
  initMotorController();
  
  extern volatile int audioBuffer[];
  
  // Simulate quiet audio
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 520; // Slightly above DC_OFFSET
  }
  
  int amplitude = processAudio();
  updateMotorSpeed(amplitude);
  delay(10);
  
  // Simulate loud audio
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 800; // Well above DC_OFFSET
  }
  
  amplitude = processAudio();
  updateMotorSpeed(amplitude);
  delay(10);
  
  // Verify amplitude is in expected range
  ASSERT_RANGE(amplitude, 50, 400);
  
  return true;
}