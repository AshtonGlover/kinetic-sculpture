#include "motor_controller.h"
#include "config.h"
#include <Arduino.h>

static unsigned long lastDebugTime = 0;

void initMotorController() {
  pinMode(MOTOR_PIN, OUTPUT);
  stopMotor();
}

void updateMotorSpeed(int amplitude) {
  // Map audio amplitude to motor speed
  // Higher amplitude = faster motor speed
  // Using a wider range for better responsiveness
  int motorSpeed = map(amplitude, 0, 512, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  motorSpeed = constrain(motorSpeed, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  
  // Apply motor speed
  analogWrite(MOTOR_PIN, motorSpeed);
  
  // Debug output (every DEBUG_INTERVAL ms to avoid flooding serial)
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

