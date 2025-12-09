#include "mock_arduino.h"

// Define config constants locally to avoid Arduino.h dependency
#define SAMPLE_RATE 1000
#define BUFFER_SIZE 20
#define DC_OFFSET 512
#define MIN_MOTOR_SPEED 80
#define MAX_MOTOR_SPEED 255
#define MOTOR_UPDATE_INTERVAL 10

// Copy audio processor implementation inline
volatile int audioBuffer[BUFFER_SIZE];
volatile int bufferIndex = 0;
volatile bool newSampleReady = false;
volatile int latestRawSample = 0;
static int smoothedAmplitude = 0;

void initAudioProcessor() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = DC_OFFSET;
  }
  bufferIndex = 0;
  smoothedAmplitude = 0;
  newSampleReady = false;
}

int processAudio() {
  long sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += audioBuffer[i];
  }
  int average = sum / BUFFER_SIZE;
  int amplitude = std::abs(average - DC_OFFSET);
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

#include <cassert>
#include <iostream>

void test_audio_processor_initialization() {
    std::cout << "Test: Audio Processor Initialization... ";
    
    initAudioProcessor();
    
    int amplitude = getSmoothedAmplitude();
    assert(amplitude == 0);
    assert(!isNewSampleReady());
    
    std::cout << "PASS" << std::endl;
}

void test_audio_processor_smoothing() {
    std::cout << "Test: Audio Processor Smoothing... ";
    
    // Initialize
    initAudioProcessor();
    
    // Manually fill buffer with loud audio (above DC offset)
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audioBuffer[i] = 700;
    }
    
    int amplitude = processAudio();
    assert(amplitude > 0);
    assert(amplitude >= 100 && amplitude <= 250);
    
    // Fill with silence
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audioBuffer[i] = DC_OFFSET;
    }
    
    // Let smoothing settle
    for (int i = 0; i < 50; i++) {
        amplitude = processAudio();
    }
    
    assert(amplitude >= 0 && amplitude <= 20);
    
    std::cout << "PASS" << std::endl;
}

void test_audio_buffer_rolling() {
    std::cout << "Test: Audio Buffer Rolling... ";
    
    initAudioProcessor();
    
    // Simulate adding samples to buffer
    for (int i = 0; i < BUFFER_SIZE * 2; i++) {
        audioBuffer[bufferIndex] = 600 + (i % 10);
        bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
        newSampleReady = true;
    }
    
    // Process should work without crashing
    int amplitude = processAudio();
    assert(amplitude >= 0 && amplitude <= 512);
    
    std::cout << "PASS" << std::endl;
}

void test_dc_offset_removal() {
    std::cout << "Test: DC Offset Removal... ";
    
    initAudioProcessor();
    
    // Test with value exactly at DC offset
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audioBuffer[i] = DC_OFFSET;
    }
    
    int amplitude = processAudio();
    assert(amplitude == 0);
    
    // Test with value above DC offset
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audioBuffer[i] = DC_OFFSET + 100;
    }
    
    amplitude = processAudio();
    assert(amplitude > 0);
    
    // Test with value below DC offset
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audioBuffer[i] = DC_OFFSET - 100;
    }
    
    amplitude = processAudio();
    assert(amplitude > 0); // Should be absolute value
    
    std::cout << "PASS" << std::endl;
}

int main() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  AUDIO PROCESSOR TESTS" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    try {
        test_audio_processor_initialization();
        test_audio_processor_smoothing();
        test_audio_buffer_rolling();
        test_dc_offset_removal();
        
        std::cout << "\n✓ All Audio Processor tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

