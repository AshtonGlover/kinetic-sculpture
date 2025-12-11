#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <cmath>

// Mock Arduino types
typedef bool boolean;
typedef uint8_t byte;

// Mock pin modes
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// Mock pin values
#define HIGH 1
#define LOW 0

// Mock Arduino constants
#define A0 14
#define A1 15
#define A2 16
#define A3 17

// Mock Serial class
class MockSerial {
public:
    void begin(long baud) {
        std::cout << "[Serial initialized at " << baud << " baud]" << std::endl;
    }
    
    void print(const char* str) { std::cout << str; }
    void print(int val) { std::cout << val; }
    void print(long val) { std::cout << val; }
    void print(unsigned long val) { std::cout << val; }
    void print(float val) { std::cout << val; }
    
    void println(const char* str) { std::cout << str << std::endl; }
    void println(int val) { std::cout << val << std::endl; }
    void println(long val) { std::cout << val << std::endl; }
    void println(unsigned long val) { std::cout << val << std::endl; }
    void println(float val) { std::cout << val << std::endl; }
    void println() { std::cout << std::endl; }
    
    operator bool() { return true; }
};

extern MockSerial Serial;

// Mock Arduino functions
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
int digitalRead(int pin);
void analogWrite(int pin, int value);
int analogRead(int pin);
unsigned long millis();
unsigned long micros();
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
long map(long x, long in_min, long in_max, long out_min, long out_max);
int constrain(int x, int min, int max);
int abs(int x);

// Simulated analog input for testing
void setSimulatedAnalogInput(int pin, int value);
int getSimulatedPWMOutput(int pin);

#endif // MOCK_ARDUINO_H




