#include "system_supervisor.h"

#include "config.h"
#include "audio_processor.h"
#include "motor_controller.h"
#include "timer_setup.h"

// FSM state
static SystemState state = SYSTEM_INIT;
static unsigned long stateEnterMs = 0;

// Health monitoring
static unsigned long lastSampleCount = 0;
static unsigned long lastSampleAdvanceMs = 0;

// Audio threshold timing
static unsigned long aboveEnterSinceMs = 0;
static unsigned long lastNonSilentMs = 0;

// Motor control smoothing
static unsigned long lastMotorTickMs = 0;
static int currentPwm = 0;

// Fault latch
static bool faultLatched = false;
static const char *faultReason = "";

static void latchFault(const char *reason) {
  faultLatched = true;
  faultReason = reason ? reason : "unknown";
  state = SYSTEM_FAULT;
  currentPwm = 0;
  stopMotor();
  setAutoCalibrationEnabled(false);
  Serial.print("FAULT: ");
  Serial.println(faultReason);
}

static void enterState(SystemState next, unsigned long nowMs) {
  if (state == next) return;

  state = next;
  stateEnterMs = nowMs;

  switch (state) {
    case SYSTEM_INIT:
      aboveEnterSinceMs = 0;
      lastNonSilentMs = nowMs;
      currentPwm = 0;
      stopMotor();
      setAutoCalibrationEnabled(true);
      Serial.println("STATE: INIT");
      break;

    case SYSTEM_IDLE:
      aboveEnterSinceMs = 0;
      currentPwm = 0;
      stopMotor();
      setAutoCalibrationEnabled(true);
      Serial.println("STATE: IDLE (motor off)");
      break;

    case SYSTEM_ACTIVE:
      aboveEnterSinceMs = 0;
      lastNonSilentMs = nowMs;
      setAutoCalibrationEnabled(false);
      Serial.println("STATE: ACTIVE");
      break;

    case SYSTEM_FAULT:
      currentPwm = 0;
      stopMotor();
      setAutoCalibrationEnabled(false);
      Serial.println("STATE: FAULT (motor off)");
      break;

    case SYSTEM_SHUTDOWN:
      currentPwm = 0;
      stopMotor();
      setAutoCalibrationEnabled(false);
      Serial.println("STATE: SHUTDOWN (motor off)");
      break;
  }
}

static int clampAndMapAmplitudeToTargetPwm(int amplitude) {
  // In ACTIVE, treat values below ACTIVE_EXIT_THRESHOLD as "no drive" (target 0).
  if (amplitude <= ACTIVE_EXIT_THRESHOLD) return 0;

  // Map amplitude range [ACTIVE_EXIT_THRESHOLD..512] -> [MIN_MOTOR_SPEED..MAX_MOTOR_SPEED]
  int a = constrain(amplitude, ACTIVE_EXIT_THRESHOLD, 512);
  long target = map(a, ACTIVE_EXIT_THRESHOLD, 512, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  return constrain((int)target, 0, 255);
}

static int slewTowards(int current, int target) {
  if (current == target) return current;
  if (target > current) {
    int next = current + PWM_SLEW_STEP;
    return (next > target) ? target : next;
  } else {
    int next = current - PWM_SLEW_STEP;
    return (next < target) ? target : next;
  }
}

void initSystemSupervisor() {
  state = SYSTEM_INIT;
  stateEnterMs = millis();
  faultLatched = false;
  faultReason = "";

  lastSampleCount = getAudioSampleCount();
  lastSampleAdvanceMs = millis();

  aboveEnterSinceMs = 0;
  lastNonSilentMs = millis();

  lastMotorTickMs = 0;
  currentPwm = 0;
}

void systemSupervisorHandleSerial(unsigned long nowMs) {
  while (Serial.available() > 0) {
    const int c = Serial.read();
    if (c == 's' || c == 'S') {
      enterState(SYSTEM_SHUTDOWN, nowMs);
    } else if (c == 'w' || c == 'W') {
      if (state == SYSTEM_SHUTDOWN) enterState(SYSTEM_IDLE, nowMs);
    } else if (c == 'r' || c == 'R') {
      // Clear FAULT and attempt recovery by re-entering INIT.
      faultLatched = false;
      faultReason = "";
      enterState(SYSTEM_INIT, nowMs);
    }
  }
}

void systemSupervisorTick(unsigned long nowMs, unsigned long audioSampleCount, int amplitude) {
  // Health monitoring: detect stalled sampling timer.
  if (audioSampleCount != lastSampleCount) {
    lastSampleCount = audioSampleCount;
    lastSampleAdvanceMs = nowMs;
  } else if ((state != SYSTEM_SHUTDOWN) && (state != SYSTEM_FAULT)) {
    if (nowMs - lastSampleAdvanceMs > SAMPLE_STALL_TIMEOUT_MS) {
      latchFault("audio sampling stalled (timer not advancing)");
      return;
    }
  }

  // INIT validation: timer must be OK.
  if (state == SYSTEM_INIT) {
    if (!isAudioTimerOk()) {
      latchFault("audio timer failed to start");
      return;
    }
    // After validation, go to IDLE.
    enterState(SYSTEM_IDLE, nowMs);
  }

  // SHUTDOWN is a latched intentional stop.
  if (state == SYSTEM_SHUTDOWN) {
    stopMotor();
    return;
  }

  // FAULT keeps motor off until reset/recovery.
  if (state == SYSTEM_FAULT) {
    stopMotor();
    return;
  }

  // IDLE -> ACTIVE if amplitude is above enter threshold for debounce time.
  if (state == SYSTEM_IDLE) {
    // In IDLE, allow baseline drift calibration.
    setAutoCalibrationEnabled(true);
    stopMotor();

    // Give the DC offset estimator time to converge before allowing ACTIVE.
    if (nowMs - stateEnterMs < IDLE_CALIBRATION_WARMUP_MS) {
      aboveEnterSinceMs = 0;
      return;
    }

    if (amplitude >= ACTIVE_ENTER_THRESHOLD) {
      if (aboveEnterSinceMs == 0) aboveEnterSinceMs = nowMs;
      if (nowMs - aboveEnterSinceMs >= ACTIVE_ENTER_DEBOUNCE_MS) {
        enterState(SYSTEM_ACTIVE, nowMs);
      }
    } else {
      aboveEnterSinceMs = 0;
    }
    return;
  }

  // ACTIVE behavior: smooth motor drive; dropout -> ramp down, then enter IDLE after t_idle.
  if (state == SYSTEM_ACTIVE) {
    setAutoCalibrationEnabled(false);

    if (amplitude > ACTIVE_EXIT_THRESHOLD) {
      lastNonSilentMs = nowMs;
    }

    // Update motor at fixed cadence.
    if (nowMs - lastMotorTickMs >= MOTOR_UPDATE_INTERVAL) {
      const int target = clampAndMapAmplitudeToTargetPwm(amplitude);
      currentPwm = slewTowards(currentPwm, target);
      setMotorSpeed(currentPwm);

      // Debug (state-level) — keeps logs consistent with the FSM.
      static unsigned long lastDbg = 0;
      if (nowMs - lastDbg >= DEBUG_INTERVAL) {
        Serial.print("State=ACTIVE Amp=");
        Serial.print(amplitude);
        Serial.print(" DC=");
        Serial.print(getDcOffsetEstimate());
        Serial.print(" PWM=");
        Serial.println(currentPwm);
        lastDbg = nowMs;
      }

      lastMotorTickMs = nowMs;
    }

    // Enter IDLE only after sustained silence for t_idle AND motor has ramped down to 0.
    if ((nowMs - lastNonSilentMs > IDLE_TIMEOUT_MS) && (currentPwm == 0)) {
      enterState(SYSTEM_IDLE, nowMs);
    }
  }
}

SystemState getSystemState() {
  return state;
}

const char *getSystemStateName(SystemState s) {
  switch (s) {
    case SYSTEM_INIT: return "INIT";
    case SYSTEM_IDLE: return "IDLE";
    case SYSTEM_ACTIVE: return "ACTIVE";
    case SYSTEM_FAULT: return "FAULT";
    case SYSTEM_SHUTDOWN: return "SHUTDOWN";
    default: return "UNKNOWN";
  }
}

bool isFaultLatched() {
  return faultLatched;
}

const char *getLastFaultReason() {
  return faultReason;
}


