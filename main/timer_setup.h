#ifndef TIMER_SETUP_H
#define TIMER_SETUP_H

/**
 * Initialize Timer1 for precise audio sampling at SAMPLE_RATE Hz
 * Sets up interrupt service routine for consistent sampling intervals
 */
void initAudioTimer();

// Returns the number of audio samples captured since boot.
// Useful for debugging whether the timer callback is running.
unsigned long getAudioSampleCount();

#endif // TIMER_SETUP_H

