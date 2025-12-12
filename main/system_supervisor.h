#ifndef SYSTEM_SUPERVISOR_H
#define SYSTEM_SUPERVISOR_H

#include <Arduino.h>

/**
 * System-level finite state machine for the audio-reactive kinetic sculpture.
 *
 * States:
 * - INIT: one-time startup validation and calibration
 * - IDLE: motor off; baseline auto-calibration enabled
 * - ACTIVE: motor speed reacts to audio amplitude
 * - FAULT: motor off due to detected fault (e.g., sampling timer stalled)
 * - SHUTDOWN: intentional stop (motor off) until user wakes/reset
 */
enum SystemState {
  SYSTEM_INIT = 0,
  SYSTEM_IDLE,
  SYSTEM_ACTIVE,
  SYSTEM_FAULT,
  SYSTEM_SHUTDOWN
};

// Initialize supervisor state machine.
void initSystemSupervisor();

// Tick supervisor: call frequently from loop().
// - nowMs: current millis()
// - audioSampleCount: monotonic count from ISR (used for health monitoring)
// - amplitude: current smoothed amplitude from audio processor
void systemSupervisorTick(unsigned long nowMs, unsigned long audioSampleCount, int amplitude);

// Handle user commands from Serial (non-blocking).
// Commands:
// - 's'/'S': enter SHUTDOWN (motor off)
// - 'w'/'W': wake from SHUTDOWN (go to IDLE)
// - 'r'/'R': clear FAULT and re-enter INIT (attempt recovery)
void systemSupervisorHandleSerial(unsigned long nowMs);

// Current state getter (for tests/debugging).
SystemState getSystemState();

// Human-readable state name.
const char *getSystemStateName(SystemState s);

// Returns true if a fault is latched.
bool isFaultLatched();

// Returns the last fault reason string (may be empty).
const char *getLastFaultReason();

#endif // SYSTEM_SUPERVISOR_H


