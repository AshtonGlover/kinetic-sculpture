#include "test.h" 

void resetVars() {
  currentState = INIT;
  sampleIndex = 0;
  playbackIndex = 0;
  lastSampleTime = 0;
  lastPlaybackTime = 0;
  buttonInterruptFlag = false;
  lastButtonHandledTime = 0;
}

void reportTest(const char* name, bool result) {
  Serial.print("[");
  Serial.print(name);
  Serial.print("] ");
  Serial.println(result ? "PASS" : "FAIL");
}

void test_init_to_listening() {
  resetVars();
  currentState = INIT;

  handleInitState();

  reportTest("INIT -> LISTENING (handleInitState)", (currentState == LISTENING && sampleIndex == 0 && playbackIndex == 0));
}

void test_listening_to_moving_when_samples() {
  resetVars();
  currentState = LISTENING;
  sampleIndex  = 10; 

  handleButtonPress();

  reportTest("LISTENING -> MOVING (sampleIndex>0)", (currentState == MOVING && playbackIndex == 0));
}

void test_listening_stays_when_no_samples() {
  resetVars();
  currentState = LISTENING;
  sampleIndex  = 0;  

  handleButtonPress();

  reportTest("LISTENING no transition when sampleIndex==0", (currentState == LISTENING));
}

void test_moving_to_listening() {
  resetVars();
  currentState = MOVING;
  sampleIndex  = 10;  
  playbackIndex = 3;

  handleButtonPress();
  reportTest("MOVING -> LISTENING", (currentState == LISTENING && sampleIndex == 0 && playbackIndex == 0));
}

void test_listening_sampling_happens_only_after_interval() {
  resetVars();
  currentState   = LISTENING;
  lastSampleTime = 1000;
  unsigned long now = 1005;  // less than SAMPLE_INTERVAL (10 ms)

  handleListeningState(now);

  bool res1 = (sampleIndex == 0); // no sample yet

  now = 1010; // exactly 10 ms later
  handleListeningState(now);

  bool res2 = (sampleIndex == 1); // should have sampled once

  reportTest("LISTENING timing (no early sample)", res1 && res2);
}

void test_moving_playback_index_wraps() {
  resetVars();
  currentState = MOVING;
  sampleIndex = 3;  // indices 0,1,2 valid
  playbackIndex = 2;  // at last valid index
  lastPlaybackTime = 1000;
  unsigned long now = 1010; // >= SAMPLE_INTERVAL

  // mock some data
  micSamples[0] = 100;
  micSamples[1] = 200;
  micSamples[2] = 300;

  handleMovingState(now);

  bool result = (playbackIndex == 0); // should wrap to 0

  reportTest("MOVING playback index wraps", result);
}

void runAllTests() {
  Serial.println("Running tests");

  test_init_to_listening();
  test_listening_to_moving_when_samples();
  test_listening_stays_when_no_samples();
  test_moving_to_listening();
  test_listening_sampling_happens_only_after_interval();
  test_moving_playback_index_wraps();

  Serial.println("Finished tests");
}