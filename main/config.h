#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define MIC_PIN A1
#define MOTOR_PIN 2  // D2 on some boards, just use pin number

// Audio processing constants
#define SAMPLE_RATE 1000              // 1kHz sampling rate (1000 samples/second)
#define BUFFER_SIZE 20                 // Small rolling buffer for smoothing
#define DC_OFFSET 512                  // Typical ADC midpoint (may need calibration)

// Motor control constants
#define MIN_MOTOR_SPEED 80             // Minimum speed to prevent motor stalling
#define MAX_MOTOR_SPEED 255            // Maximum PWM value
#define MOTOR_UPDATE_INTERVAL 10       // Update motor every 10ms (100Hz)

// Timer interrupt setup (for precise sampling)
// Using FspTimer (Renesas RA4M1 GPT timer)
// Configured for 1kHz sampling rate
// Note: Timer configuration is handled in timer_setup.cpp using FspTimer API

// Watchdog timer timeout (8 seconds = 8000 milliseconds)
#define WATCHDOG_TIMEOUT 8000

// Debug output interval (milliseconds)
#define DEBUG_INTERVAL 100

// --- Audio thresholding / FSM tuning ---
// If amplitude stays below this threshold for > IDLE_TIMEOUT_MS, the system enters IDLE (motor off).
#define SILENCE_THRESHOLD 5
// Hysteresis: require a slightly higher threshold to enter ACTIVE than to remain ACTIVE.
#define ACTIVE_ENTER_THRESHOLD 15
#define ACTIVE_EXIT_THRESHOLD 8
// Debounce entering ACTIVE (ms) to avoid chatter on noise.
#define ACTIVE_ENTER_DEBOUNCE_MS 50
// Time with no meaningful audio before entering IDLE (motor off).
#define IDLE_TIMEOUT_MS 2000

// --- Safety / health monitoring ---
// If the audio sampling timer stops advancing for this long, enter FAULT.
#define SAMPLE_STALL_TIMEOUT_MS 250

// --- Motor smoothing ---
// Max PWM delta per MOTOR_UPDATE_INTERVAL tick (slew-rate limiting for smooth motion).
#define PWM_SLEW_STEP 8

// On-device test mode:
// - 0: run normal program
// - 1: run unit tests at boot, print results to Serial, then idle
#define ENABLE_ON_DEVICE_TESTS 0

#endif // CONFIG_H

