#include "tests_on_device.h"

#include "config.h"
#include "audio_processor.h"
#include "motor_controller.h"
#include "watchdog_utils.h"

#include <Arduino.h>

// Test statistics
static int totalTests = 0;
static int passedTests = 0;
static int failedTests = 0;

// Assertions (each test returns bool)
#define ASSERT_EQUAL(expected, actual) \
  do { \
    if ((expected) != (actual)) { \
      Serial.print("  FAIL: Expected "); \
      Serial.print(expected); \
      Serial.print(", got "); \
      Serial.println(actual); \
      return false; \
    } \
  } while (0)

#define ASSERT_TRUE(condition) \
  do { \
    if (!(condition)) { \
      Serial.println("  FAIL: Condition false"); \
      return false; \
    } \
  } while (0)

#define ASSERT_RANGE(value, minv, maxv) \
  do { \
    if ((value) < (minv) || (value) > (maxv)) { \
      Serial.print("  FAIL: Value "); \
      Serial.print(value); \
      Serial.print(" not in range ["); \
      Serial.print(minv); \
      Serial.print(", "); \
      Serial.print(maxv); \
      Serial.println("]"); \
      return false; \
    } \
  } while (0)

static void runTest(const char *name, bool (*testFunc)()) {
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

// Access internals for a couple of unit tests (OK for tests-only code)
extern volatile int audioBuffer[];

static bool test_config_constants() {
  ASSERT_TRUE(SAMPLE_RATE > 0 && SAMPLE_RATE <= 10000);
  ASSERT_TRUE(BUFFER_SIZE > 0 && BUFFER_SIZE <= 100);
  ASSERT_TRUE(MIN_MOTOR_SPEED >= 0 && MIN_MOTOR_SPEED < MAX_MOTOR_SPEED);
  ASSERT_TRUE(MAX_MOTOR_SPEED <= 255);
  ASSERT_TRUE(MOTOR_UPDATE_INTERVAL > 0);
  return true;
}

static bool test_audio_processor_initialization() {
  initAudioProcessor();
  ASSERT_EQUAL(0, getSmoothedAmplitude());
  ASSERT_TRUE(!isNewSampleReady());
  return true;
}

static bool test_audio_processor_smoothing() {
  initAudioProcessor();

  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 700; // above DC offset
  }

  int amp = processAudio();
  ASSERT_TRUE(amp > 0);
  ASSERT_RANGE(amp, 50, 300);

  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = DC_OFFSET;
  }
  for (int i = 0; i < 50; i++) {
    amp = processAudio();
  }
  ASSERT_RANGE(amp, 0, 20);
  return true;
}

static bool test_motor_controller_basic() {
  initMotorController();
  setMotorSpeed(0);
  delay(5);
  setMotorSpeed(128);
  delay(5);
  stopMotor();
  delay(5);
  return true;
}

static bool test_integration_audio_to_motor() {
  initAudioProcessor();
  initMotorController();

  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 520;
  }
  int amp = processAudio();
  updateMotorSpeed(amp);
  delay(5);

  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 800;
  }
  amp = processAudio();
  updateMotorSpeed(amp);
  delay(5);

  ASSERT_RANGE(amp, 50, 500);
  return true;
}

static bool test_watchdog_smoke() {
  // We only smoke-test that the functions can be called;
  // full watchdog validation requires intentionally hanging the loop.
  initWatchdog();
  resetWatchdog();
  resetWatchdog();
  return true;
}

bool runAllTests() {
  totalTests = passedTests = failedTests = 0;

  Serial.println();
  Serial.println("========================================");
  Serial.println("  KINETIC SCULPTURE - ON-DEVICE TESTS");
  Serial.println("========================================");
  Serial.println();

  runTest("config_constants", test_config_constants);
  runTest("audio_processor_initialization", test_audio_processor_initialization);
  runTest("audio_processor_smoothing", test_audio_processor_smoothing);
  runTest("motor_controller_basic", test_motor_controller_basic);
  runTest("watchdog_smoke", test_watchdog_smoke);
  runTest("integration_audio_to_motor", test_integration_audio_to_motor);

  Serial.println();
  Serial.println("========================================");
  Serial.println("  TEST SUMMARY");
  Serial.println("========================================");
  Serial.print("Total Tests:  ");
  Serial.println(totalTests);
  Serial.print("Passed:       ");
  Serial.println(passedTests);
  Serial.print("Failed:       ");
  Serial.println(failedTests);

  const bool ok = (failedTests == 0);
  Serial.println(ok ? "\n✓ ALL TESTS PASSED!" : "\n✗ SOME TESTS FAILED");
  Serial.println("========================================");
  Serial.println();

  return ok;
}


