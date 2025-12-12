#include <WDT.h> 
#include "test.h" 

#define RUN_FSM_TESTS 0
#define RUN_ASHTON_CAPSTONE 0

const int micPin = A1;
const int motorPin = D3;
const int buttonPin = D6;

const int SAMPLE_RATE = 100;
const int SAMPLE_INTERVAL = 1000 / SAMPLE_RATE;
const int MAX_SAMPLES = 1000;

// different states for the FSM
enum State {
  INIT,
  LISTENING,
  MOVING
};

State currentState = INIT;

int micSamples[MAX_SAMPLES];
int sampleIndex = 0;
int playbackIndex = 0;

unsigned long lastSampleTime = 0;
unsigned long lastPlaybackTime = 0;

volatile bool buttonInterruptFlag = false;
unsigned long lastButtonHandledTime = 0;
const unsigned long DEBOUNCE_MS = 200;

// setup function which starts serial, runs fsm tests if flag for testing is set, initializes pins, initalizes watchdog, and attaches interrupt
// to the button
void setup() {
  Serial.begin(9600);

#if RUN_FSM_TESTS //macro to run the tests
  runAllTests();

  Serial.println("All tests finished.");

  while (true) {
    // do nothing
  }
#else
  pinMode(micPin, INPUT);
  pinMode(motorPin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);

  Serial.println("FSM BEGINNING");

  WDT.begin(3000);
  Serial.println("Watchdog enabled");
  WDT.refresh();

  currentState = INIT;
#endif
}

// function called when button interrupt is fired to set flag to test for transition
void buttonISR() {
  buttonInterruptFlag = true;
}

// loop function to handle state changes and button press
// runs repeatedly to constantly update state
void loop() {
#if RUN_FSM_TESTS
  // nothing should be run in test mode
#elif RUN_ASHTON_CAPSTONE // this changes the loop to only monitor the serial to drive the motor
  if (Serial.available() > 0) {
    int motorSpeed = Serial.parseInt(); 
    motorSpeed = abs(constrain(motorSpeed, -200, 200));

    // Echo back
    Serial.print("Echo: ");
    Serial.println(motorSpeed);

    analogWrite(motorPin, motorSpeed);
  }
#else
  unsigned long now = millis(); // current time

  if (buttonInterruptFlag) {
    noInterrupts();
    buttonInterruptFlag = false; // resets the button interrupt flag
    interrupts();

    if (now - lastButtonHandledTime > DEBOUNCE_MS) { // denoising logic
      lastButtonHandledTime = now;
      handleButtonPress(); // handles the button press to determine if should transition
    }
  }

  switch (currentState) {
    case INIT:
      handleInitState(); // tells what to do in the init state
      break;

    case LISTENING:
      handleListeningState(now); // tells what to do in the listening state
      break;

    case MOVING:
      handleMovingState(now); // tells what to do in the moving state
      break;
  }

  WDT.refresh(); // pets the watchdog
#endif
}

// handles the transition from init state to listening state
void handleInitState() {
  Serial.println("Entering LISTENING state");
  startListening();
}

// handles the listening state
// takes in the current time to determine if the sample interval has passed and begins updating the buffer
void handleListeningState(unsigned long now) {
  if (now - lastSampleTime >= (unsigned long)SAMPLE_INTERVAL &&
      sampleIndex < MAX_SAMPLES) {

    lastSampleTime = now;

    int micValue = analogRead(micPin);
    micSamples[sampleIndex++] = micValue;

    if (sampleIndex % 50 == 0) {
      Serial.print("Sample[");
      Serial.print(sampleIndex);
      Serial.print("] = ");
      Serial.println(micValue);
    }
  }
}

// function to handle the moving state
// takes in the current time to move through the buffer and drive the motor at varying pwm speeds
void handleMovingState(unsigned long now) {
  if (sampleIndex == 0) {
    analogWrite(motorPin, 0);
    return;
  }

  if (now - lastPlaybackTime >= (unsigned long)SAMPLE_INTERVAL) {
    lastPlaybackTime = now;

    int micValue = micSamples[playbackIndex];

    int motorPower = map(micValue, 0, 1023, 170, 190);
    motorPower = constrain(motorPower, 170, 190);

    analogWrite(motorPin, motorPower);

    Serial.print("Moving: mic=");
    Serial.print(micValue);
    Serial.print(" pwm=");
    Serial.println(motorPower);

    playbackIndex++;
    if (playbackIndex >= sampleIndex) {
      playbackIndex = 0;
    }
  }
}

//function to handle what to do when the button is pressed including transitioning from init to listening, listening to moving, and moving
//back to listening
void handleButtonPress() {
  Serial.println("button pressed");

  switch (currentState) {
    case INIT:
      startListening();
      break;

    case LISTENING:
      if (sampleIndex > 0) {
        Serial.println("LISTENING -> MOVING");
        startMoving();
      } else {
        Serial.println("no samples, still in listening");
      }
      break;

    case MOVING:
      Serial.println("MOVING -> LISTENING");
      startListening();
      break;
  }
}

//function to start listening and drive motor to 0
void startListening() {
  currentState = LISTENING;
  sampleIndex = 0;
  playbackIndex = 0;
  lastSampleTime = millis();

  analogWrite(motorPin, 0);

  Serial.println("Started LISTENING: recording mic pattern...");
}

//function to start the moving state, initializing index and last played time
void startMoving() {
  currentState = MOVING;
  playbackIndex = 0;
  lastPlaybackTime = millis();

  Serial.print("Started MOVING: playing back ");
  Serial.print(sampleIndex);
  Serial.println(" samples in a loop.");
}