#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

/**
 * Initialize the motor controller
 * Sets up the motor pin as output
 */
void initMotorController();

/**
 * Update motor speed based on audio amplitude
 * Maps amplitude (0-512) to motor speed (MIN_MOTOR_SPEED - MAX_MOTOR_SPEED)
 * 
 * @param amplitude The audio amplitude value (0-512)
 */
void updateMotorSpeed(int amplitude);

/**
 * Stop the motor
 */
void stopMotor();

/**
 * Set motor speed directly (for testing)
 * 
 * @param speed PWM value (0-255)
 */
void setMotorSpeed(int speed);

#endif // MOTOR_CONTROLLER_H

