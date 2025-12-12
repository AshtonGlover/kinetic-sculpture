/**
 * Real-Time Audio Wave Visualization
 * 
 * Main program that coordinates audio sampling, processing, and motor control
 * for creating a kinetic sculpture that responds to audio input.
 */

#include "config.h"
#include "timer_setup.h"
#include "audio_processor.h"
#include "motor_controller.h"
#include "watchdog_utils.h"
#include "tests_on_device.h"
#include "system_supervisor.h"

void setup() {
  Serial.begin(9600);

#if ENABLE_ON_DEVICE_TESTS
  // Run unit tests at boot, then idle.
  // (This avoids having a second .ino with its own setup()/loop().)
  runAllTests();
  while (true) {
    delay(1000);
  }
#endif
  
  // Initialize all subsystems
  initAudioProcessor();
  initMotorController();
  initAudioTimer();
  initWatchdog();
  initSystemSupervisor();
  
  Serial.println("=== Real-Time Audio Wave Visualization ===");
  Serial.println("System initialized. Processing audio in real-time...");
  Serial.print("Sampling rate: ");
  Serial.print(SAMPLE_RATE);
  Serial.println(" Hz");
  Serial.println("Commands: 's' shutdown, 'w' wake, 'r' reset from fault");
}

void loop() {
  // Feed watchdog timer to prevent system reset
  resetWatchdog();

  // Handle user commands (non-blocking)
  systemSupervisorHandleSerial(millis());
  
  // Debug: verify the sampling callback is firing (prints once per second)
  static unsigned long lastSampleCount = 0;
  static unsigned long lastTimerDebug = 0;
  if (millis() - lastTimerDebug >= 1000) {
    unsigned long nowCount = getAudioSampleCount();
    Serial.print("Samples/sec: ");
    Serial.println(nowCount - lastSampleCount);
    lastSampleCount = nowCount;
    lastTimerDebug = millis();
  }

  // Process audio if new sample is available
  if (isNewSampleReady()) {
    processAudio();
    clearSampleReadyFlag();
  }

  // Run the system FSM (decides IDLE/ACTIVE/FAULT/SHUTDOWN and motor PWM)
  const unsigned long nowMs = millis();
  systemSupervisorTick(nowMs, getAudioSampleCount(), getSmoothedAmplitude());
}