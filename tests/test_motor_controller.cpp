#include "mock_arduino.h"

// Define config constants locally
#define MOTOR_PIN 2
#define MIN_MOTOR_SPEED 80
#define MAX_MOTOR_SPEED 255
#define DEBUG_INTERVAL 100

// Copy motor controller implementation inline
static unsigned long lastDebugTime = 0;

void initMotorController() {
  pinMode(MOTOR_PIN, OUTPUT);
  analogWrite(MOTOR_PIN, 0);
}

void updateMotorSpeed(int amplitude) {
  int motorSpeed = map(amplitude, 0, 512, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  motorSpeed = constrain(motorSpeed, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  analogWrite(MOTOR_PIN, motorSpeed);
  
  unsigned long currentTime = millis();
  if (currentTime - lastDebugTime >= DEBUG_INTERVAL) {
    Serial.print("Amplitude: ");
    Serial.print(amplitude);
    Serial.print(" | Motor Speed: ");
    Serial.println(motorSpeed);
    lastDebugTime = currentTime;
  }
}

void stopMotor() {
  analogWrite(MOTOR_PIN, 0);
}

void setMotorSpeed(int speed) {
  speed = constrain(speed, 0, 255);
  analogWrite(MOTOR_PIN, speed);
}

#include <cassert>
#include <iostream>

void test_motor_controller_initialization() {
    std::cout << "Test: Motor Controller Initialization... ";
    
    initMotorController();
    
    // Check that motor stops on init
    int pwm = getSimulatedPWMOutput(MOTOR_PIN);
    assert(pwm == 0);
    
    std::cout << "PASS" << std::endl;
}

void test_motor_speed_mapping() {
    std::cout << "Test: Motor Speed Mapping... ";
    
    initMotorController();
    
    // Test minimum amplitude
    updateMotorSpeed(0);
    delay(10);
    int pwm = getSimulatedPWMOutput(MOTOR_PIN);
    assert(pwm >= MIN_MOTOR_SPEED && pwm <= MAX_MOTOR_SPEED);
    
    // Test medium amplitude
    updateMotorSpeed(256);
    delay(10);
    pwm = getSimulatedPWMOutput(MOTOR_PIN);
    assert(pwm >= MIN_MOTOR_SPEED && pwm <= MAX_MOTOR_SPEED);
    
    // Test maximum amplitude
    updateMotorSpeed(512);
    delay(10);
    pwm = getSimulatedPWMOutput(MOTOR_PIN);
    assert(pwm >= MIN_MOTOR_SPEED && pwm <= MAX_MOTOR_SPEED);
    
    std::cout << "PASS" << std::endl;
}

void test_motor_speed_constraints() {
    std::cout << "Test: Motor Speed Constraints... ";
    
    initMotorController();
    
    // Test that speed is constrained within valid PWM range
    setMotorSpeed(0);
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 0);
    
    setMotorSpeed(128);
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 128);
    
    setMotorSpeed(255);
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 255);
    
    // Test out-of-range values
    setMotorSpeed(-50);
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 0);
    
    setMotorSpeed(300);
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 255);
    
    std::cout << "PASS" << std::endl;
}

void test_motor_stop() {
    std::cout << "Test: Motor Stop... ";
    
    initMotorController();
    
    // Set motor to some speed
    setMotorSpeed(200);
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 200);
    
    // Stop motor
    stopMotor();
    assert(getSimulatedPWMOutput(MOTOR_PIN) == 0);
    
    std::cout << "PASS" << std::endl;
}

void test_amplitude_response() {
    std::cout << "Test: Amplitude Response... ";
    
    initMotorController();
    
    // Test that higher amplitude gives higher speed
    updateMotorSpeed(100);
    delay(10);
    int speed1 = getSimulatedPWMOutput(MOTOR_PIN);
    
    updateMotorSpeed(300);
    delay(10);
    int speed2 = getSimulatedPWMOutput(MOTOR_PIN);
    
    assert(speed2 > speed1);
    
    std::cout << "PASS" << std::endl;
}

int main() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  MOTOR CONTROLLER TESTS" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    try {
        test_motor_controller_initialization();
        test_motor_speed_mapping();
        test_motor_speed_constraints();
        test_motor_stop();
        test_amplitude_response();
        
        std::cout << "\n✓ All Motor Controller tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

