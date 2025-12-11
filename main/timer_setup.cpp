#include "timer_setup.h"
#include "config.h"
#include <Arduino.h>
#include <FspTimer.h>

// External references to audio buffer (defined in audio_processor.cpp)
extern volatile int audioBuffer[];
extern volatile int bufferIndex;
extern volatile bool newSampleReady;
extern volatile int latestRawSample;

// Timer instance for Renesas RA4M1
FspTimer audioTimer;
static volatile unsigned long audioSampleCount = 0;
static bool audioTimerOk = false;

// Timer callback function - samples audio at precise intervals
void audioTimerCallback(timer_callback_args_t *args) {
  (void)args; // Unused parameter
  
  audioSampleCount++;

  // Read audio sample
  latestRawSample = analogRead(MIC_PIN);
  
  // Add to rolling buffer
  audioBuffer[bufferIndex] = latestRawSample;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  
  // Flag that new sample is ready
  newSampleReady = true;
}

unsigned long getAudioSampleCount() {
  return audioSampleCount;
}

void initAudioTimer() {
  // Setup timer for 1kHz sampling (1000 Hz) using the Arduino Renesas core's FspTimer.
  // Important: pick a real channel using get_available_timer(); passing -1 does NOT auto-select.

  uint8_t timer_type = GPT_TIMER;
  const int8_t timer_channel = FspTimer::get_available_timer(timer_type);
  if (timer_channel < 0) {
    Serial.println("ERROR: No available hardware timer channel for sampling!");
    audioTimerOk = false;
    return;
  }

  // Periodic mode: we only care about frequency; duty is ignored but must be provided.
  if (!audioTimer.begin(TIMER_MODE_PERIODIC,
                        timer_type,
                        static_cast<uint8_t>(timer_channel),
                        static_cast<float>(SAMPLE_RATE),
                        50.0f,
                        audioTimerCallback)) {
    Serial.println("ERROR: Failed to initialize audio timer (begin)!");
    audioTimerOk = false;
    return;
  }

  if (!audioTimer.setup_overflow_irq()) {
    Serial.println("ERROR: Failed to setup timer overflow IRQ!");
    audioTimerOk = false;
    return;
  }

  // Some cores require explicitly enabling the IRQ.
  audioTimer.enable_overflow_irq();

  if (!audioTimer.open()) {
    Serial.println("ERROR: Failed to open audio timer!");
    audioTimerOk = false;
    return;
  }

  if (!audioTimer.start()) {
    Serial.println("ERROR: Failed to start audio timer!");
    audioTimerOk = false;
    return;
  }

  audioTimerOk = true;
  Serial.print("Audio timer started. type=");
  Serial.print(timer_type);
  Serial.print(" channel=");
  Serial.println(timer_channel);
}

