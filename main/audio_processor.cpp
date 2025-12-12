#include "audio_processor.h"
#include "config.h"
#include <Arduino.h>

// Rolling buffer for audio smoothing
volatile int audioBuffer[BUFFER_SIZE];
volatile int bufferIndex = 0;
volatile bool newSampleReady = false;
volatile int latestRawSample = 0;

// Audio processing variables
static int smoothedAmplitude = 0;
static int dcOffsetEstimate = DC_OFFSET;
static bool autoCalibrationEnabled = true;

void initAudioProcessor() {
  // Initialize audio buffer with DC offset (silence baseline)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = DC_OFFSET;
  }
  bufferIndex = 0;
  smoothedAmplitude = 0;
  dcOffsetEstimate = DC_OFFSET;
  autoCalibrationEnabled = true;
  newSampleReady = false;
}

int processAudio() {
  // Calculate average of buffer (smoothing)
  long sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += audioBuffer[i];
  }
  int average = sum / BUFFER_SIZE;
  
  // Optional: slowly adapt DC offset estimate (helps with drift / mic bias).
  // This should generally be enabled only when the system believes it is quiet (IDLE).
  if (autoCalibrationEnabled) {
    // IIR low-pass: ~1% new, 99% old.
    dcOffsetEstimate = (dcOffsetEstimate * 99 + average) / 100;
  }

  // Remove DC offset and get amplitude
  int amplitude = abs(average - dcOffsetEstimate);
  
  // Apply exponential smoothing for even smoother transitions
  // Alpha = 0.7 means 70% new value, 30% old value
  smoothedAmplitude = (smoothedAmplitude * 3 + amplitude * 7) / 10;
  
  return smoothedAmplitude;
}

int getSmoothedAmplitude() {
  return smoothedAmplitude;
}

void setAutoCalibrationEnabled(bool enabled) {
  autoCalibrationEnabled = enabled;
}

int getDcOffsetEstimate() {
  return dcOffsetEstimate;
}

bool isNewSampleReady() {
  return newSampleReady;
}

void clearSampleReadyFlag() {
  newSampleReady = false;
}

