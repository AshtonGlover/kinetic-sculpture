#ifndef TIMER_SETUP_H
#define TIMER_SETUP_H

/**
 * Initialize Timer1 for precise audio sampling at SAMPLE_RATE Hz
 * Sets up interrupt service routine for consistent sampling intervals
 */
void initAudioTimer();

#endif // TIMER_SETUP_H

