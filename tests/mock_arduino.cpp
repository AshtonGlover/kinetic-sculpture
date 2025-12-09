#include "mock_arduino.h"
#include <chrono>
#include <map>

MockSerial Serial;

// State tracking for mock hardware
static std::map<int, int> pinModes;
static std::map<int, int> digitalPins;
static std::map<int, int> analogInputs;
static std::map<int, int> pwmOutputs;
static auto startTime = std::chrono::steady_clock::now();

void pinMode(int pin, int mode) {
    pinModes[pin] = mode;
}

void digitalWrite(int pin, int value) {
    digitalPins[pin] = value;
}

int digitalRead(int pin) {
    return digitalPins[pin];
}

void analogWrite(int pin, int value) {
    pwmOutputs[pin] = value;
}

int analogRead(int pin) {
    // Return simulated value or default
    if (analogInputs.find(pin) != analogInputs.end()) {
        return analogInputs[pin];
    }
    return 512; // Default to DC offset
}

unsigned long millis() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return duration.count();
}

unsigned long micros() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);
    return duration.count();
}

void delay(unsigned long ms) {
    auto start = std::chrono::steady_clock::now();
    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        if (elapsed.count() >= ms) break;
    }
}

void delayMicroseconds(unsigned int us) {
    auto start = std::chrono::steady_clock::now();
    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
        if (elapsed.count() >= us) break;
    }
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int constrain(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Use std::abs directly, don't redefine
// int abs(int x) is already in std namespace

// Test helper functions
void setSimulatedAnalogInput(int pin, int value) {
    analogInputs[pin] = value;
}

int getSimulatedPWMOutput(int pin) {
    return pwmOutputs[pin];
}

