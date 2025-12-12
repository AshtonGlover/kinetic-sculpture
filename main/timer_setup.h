#ifndef TIMER_SETUP_H
#define TIMER_SETUP_H

/**
 * Initialize a hardware timer for precise audio sampling at SAMPLE_RATE Hz.
 * On Arduino UNO R4 (Renesas RA4M1), this uses the Arduino Renesas core's FspTimer.
 */
void initAudioTimer();

// Returns the number of audio samples captured since boot.
// Useful for debugging whether the timer callback is running.
unsigned long getAudioSampleCount();

// Returns whether the audio sampling timer successfully started.
bool isAudioTimerOk();

#endif // TIMER_SETUP_H

