#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

/**
 * Initialize the audio processing system
 * Sets up the rolling buffer with DC offset values
 */
void initAudioProcessor();

/**
 * Process the latest audio samples from the buffer
 * Calculates smoothed amplitude from the rolling buffer
 * Should be called when newSampleReady flag is set
 * 
 * @return The smoothed audio amplitude (0-512)
 */
int processAudio();

/**
 * Get the current smoothed amplitude value
 * 
 * @return The current smoothed amplitude
 */
int getSmoothedAmplitude();

/**
 * Enable/disable automatic DC offset calibration.
 * When enabled, the audio processor slowly adapts its DC offset estimate based on the rolling average.
 * Typically enabled in IDLE (quiet) to track baseline drift.
 */
void setAutoCalibrationEnabled(bool enabled);

/**
 * Get the current DC offset estimate used for amplitude calculations.
 */
int getDcOffsetEstimate();

/**
 * Check if a new audio sample is ready for processing
 * 
 * @return true if new sample available, false otherwise
 */
bool isNewSampleReady();

/**
 * Clear the new sample ready flag
 */
void clearSampleReadyFlag();

#endif // AUDIO_PROCESSOR_H

