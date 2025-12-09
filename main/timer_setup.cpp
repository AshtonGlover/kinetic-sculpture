#include "timer_setup.h"
#include "config.h"
#include <avr/interrupt.h>
#include <Arduino.h>

// External references to audio buffer (defined in audio_processor.cpp)
extern volatile int audioBuffer[];
extern volatile int bufferIndex;
extern volatile bool newSampleReady;
extern volatile int latestRawSample;

void initAudioTimer() {
  // Setup Timer1 interrupt for audio sampling
  noInterrupts();
  TCCR1A = 0;  // Clear timer control registers
  TCCR1B = 0;
  TCNT1 = 0;   // Initialize counter value to 0
  
  // Set compare match register for 1kHz sampling
  OCR1A = TIMER_COMPARE_VALUE;
  
  // Turn on CTC mode (Clear Timer on Compare)
  TCCR1B |= (1 << WGM12);
  
  // Set prescaler to 64 (CS11 and CS10 bits)
  TCCR1B |= (1 << CS11) | (1 << CS10);
  
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

// Timer interrupt service routine - samples audio at precise intervals
ISR(TIMER1_COMPA_vect) {
  // Read audio sample
  latestRawSample = analogRead(MIC_PIN);
  
  // Add to rolling buffer
  audioBuffer[bufferIndex] = latestRawSample;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  
  // Flag that new sample is ready
  newSampleReady = true;
}

