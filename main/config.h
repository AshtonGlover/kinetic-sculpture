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

// Treat very small amplitudes as silence (prevents motor from running when mic is unplugged)
#define SILENCE_THRESHOLD 5

// On-device test mode:
// - 0: run normal program
// - 1: run unit tests at boot, print results to Serial, then idle
#define ENABLE_ON_DEVICE_TESTS 1

#endif // CONFIG_H

