#include <WDT.h> 
#include "test.h" 

#define RUN_FSM_TESTS 0

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

void onButtonInterrupt() {
  buttonInterruptFlag = true;
}

void handleInitState();
void handleListeningState(unsigned long now);
void handleMovingState(unsigned long now);
void handleButtonPress();
void startListening();
void startMoving();

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
  attachInterrupt(digitalPinToInterrupt(buttonPin), onButtonInterrupt, FALLING);

  Serial.println("FSM BEGINNING");

  WDT.begin(3000);
  Serial.println("Watchdog enabled");
  WDT.refresh();

  currentState = INIT;
#endif
}

void loop() {
  unsigned long now = millis();

  if (buttonInterruptFlag) {
    noInterrupts();
    buttonInterruptFlag = false;
    interrupts();

    if (now - lastButtonHandledTime > DEBOUNCE_MS) {
      lastButtonHandledTime = now;
      handleButtonPress();
    }
  }

  switch (currentState) {
    case INIT:
      handleInitState();
      break;

    case LISTENING:
      handleListeningState(now);
      break;

    case MOVING:
      handleMovingState(now);
      break;
  }

  WDT.refresh();
}

void handleInitState() {
  Serial.println("Entering LISTENING state");
  startListening();
}

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

void startListening() {
  currentState = LISTENING;
  sampleIndex = 0;
  playbackIndex = 0;
  lastSampleTime = millis();

  analogWrite(motorPin, 0);

  Serial.println("Started LISTENING: recording mic pattern...");
}

void startMoving() {
  currentState = MOVING;
  playbackIndex = 0;
  lastPlaybackTime = millis();

  Serial.print("Started MOVING: playing back ");
  Serial.print(sampleIndex);
  Serial.println(" samples in a loop.");
}