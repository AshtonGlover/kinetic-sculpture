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

void initAudioProcessor() {
  // Initialize audio buffer with DC offset (silence baseline)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = DC_OFFSET;
  }
  bufferIndex = 0;
  smoothedAmplitude = 0;
  newSampleReady = false;
}

int processAudio() {
  // Calculate average of buffer (smoothing)
  long sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += audioBuffer[i];
  }
  int average = sum / BUFFER_SIZE;
  
  // Remove DC offset and get amplitude
  int amplitude = abs(average - DC_OFFSET);
  
  // Apply exponential smoothing for even smoother transitions
  // Alpha = 0.7 means 70% new value, 30% old value
  smoothedAmplitude = (smoothedAmplitude * 3 + amplitude * 7) / 10;
  
  return smoothedAmplitude;
}

int getSmoothedAmplitude() {
  return smoothedAmplitude;
}

bool isNewSampleReady() {
  return newSampleReady;
}

void clearSampleReadyFlag() {
  newSampleReady = false;
}

