const int micPin = A1;
const int motorPin = D2;

// Timing constants
const unsigned long LISTENING_DURATION = 30000;  // 30 seconds 
const unsigned long MOVING_DURATION = 30000;     // 30 seconds
const int SAMPLE_RATE = 100;                     // Samples per second
const int SAMPLE_INTERVAL = 1000 / SAMPLE_RATE;  // ms between samples
const int MAX_SAMPLES = (LISTENING_DURATION / SAMPLE_INTERVAL);

// FSM States
enum State {
  INIT,
  LISTENING,
  MOVING
};

State currentState = INIT;
int micSamples[MAX_SAMPLES];
int sampleIndex = 0;
int playbackIndex = 0;
unsigned long stateStartTime = 0;
unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
  
  Serial.println("=== FSM Motor Control System ===");
  Serial.println("Initializing...");
  
  currentState = INIT;
}

void loop() {
  unsigned long currentTime = millis();
  
  switch (currentState) {
    case INIT:
      handleInitState();
      break;
      
    case LISTENING:
      handleListeningState(currentTime);
      break;
      
    case MOVING:
      handleMovingState(currentTime);
      break;
  }
}

void handleInitState() {
  Serial.println("\n--- INIT STATE ---");
  Serial.println("System ready. Transitioning to LISTENING state...");
  
  // Reset variables
  sampleIndex = 0;
  playbackIndex = 0;
  
  // Clear the sample array
  for (int i = 0; i < MAX_SAMPLES; i++) {
    micSamples[i] = 0;
  }
  
  // Transition to LISTENING
  currentState = LISTENING;
  stateStartTime = millis();
  lastSampleTime = millis();
  
  Serial.print("Will collect ");
  Serial.print(MAX_SAMPLES);
  Serial.println(" samples over 30 seconds");
}

void handleListeningState(unsigned long currentTime) {
  // Check if it's time to take a sample
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    // Read from microphone
    int micValue = analogRead(micPin);
    
    // Store the sample
    if (sampleIndex < MAX_SAMPLES) {
      micSamples[sampleIndex] = micValue;
      
      // Print progress every 50 samples
      if (sampleIndex % 50 == 0) {
        Serial.print("Listening... Sample ");
        Serial.print(sampleIndex);
        Serial.print("/");
        Serial.print(MAX_SAMPLES);
        Serial.print(" - Value: ");
        Serial.println(micValue);
      }
      
      sampleIndex++;
    }
    
    lastSampleTime = currentTime;
  }
  
  // Check if listening duration is complete
  if (currentTime - stateStartTime >= LISTENING_DURATION) {
    Serial.println("\n--- LISTENING COMPLETE ---");
    Serial.print("Collected ");
    Serial.print(sampleIndex);
    Serial.println(" samples");
    Serial.println("Transitioning to MOVING state...");
    
    // Transition to MOVING
    currentState = MOVING;
    stateStartTime = currentTime;
    lastSampleTime = currentTime;
    playbackIndex = 0;
  }
}

void handleMovingState(unsigned long currentTime) {
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    if (playbackIndex < sampleIndex) {
      int micValue = micSamples[playbackIndex];
      
      // Map to motor speed (might need to adjust based on mic input)
      int motorSpeed = map(micValue, 250, 350, 0, 255);
      motorSpeed = constrain(motorSpeed, 0, 255);  // Ensure within PWM range

      motorSpeed = 255;
      
      // Drive the motor
      analogWrite(motorPin, motorSpeed);
      
      // Print progress every 50 samples
      if (playbackIndex % 50 == 0) {
        Serial.print("Moving... Sample ");
        Serial.print(playbackIndex);
        Serial.print("/");
        Serial.print(sampleIndex);
        Serial.print(" - Mic: ");
        Serial.print(micValue);
        Serial.print(" - Speed: ");
        Serial.println(motorSpeed);
      }
      
      playbackIndex++;
    }
    
    lastSampleTime = currentTime;
  }
  
  // Check if moving duration is complete
  if (currentTime - stateStartTime >= MOVING_DURATION) {
    Serial.println("\n--- MOVING COMPLETE ---");
    
    // Stop the motor
    analogWrite(motorPin, 0);
    
    Serial.println("Cycle complete. Restarting...");
    delay(2000);
    
    // Transition back to INIT
    currentState = INIT;
  }
}