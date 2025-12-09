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
// Using Timer1 on Arduino Uno (16-bit timer)
// For 1kHz: 16MHz / 64 prescaler / 250 = 1000Hz (1kHz)
// Formula: OCR1A = (F_CPU / (prescaler * target_frequency)) - 1
#define TIMER_PRESCALER 64
#define TIMER_COMPARE_VALUE 249        // (16000000 / (64 * 1000)) - 1 = 249

// Watchdog timer timeout (8 seconds)
#define WATCHDOG_TIMEOUT WDTO_8S

// Debug output interval (milliseconds)
#define DEBUG_INTERVAL 100

#endif // CONFIG_H

