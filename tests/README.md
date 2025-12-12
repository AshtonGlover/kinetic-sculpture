# Desktop Testing Suite

This directory contains tests that can run on your computer without Arduino hardware.

## How It Works

- `mock_arduino.h/cpp` - Simulates Arduino functions (pinMode, analogRead, etc.)
- `test_audio_processor.cpp` - Tests audio processing logic
- `test_motor_controller.cpp` - Tests motor control logic
- `Makefile` - Build and run tests

## Running Tests

```bash
cd tests
make run
```

This will compile and run all tests, showing PASS/FAIL for each.

## What Gets Tested

### Audio Processor
- ✓ Initialization
- ✓ Buffer smoothing
- ✓ Rolling buffer behavior
- ✓ DC offset removal

### Motor Controller
- ✓ Initialization
- ✓ Speed mapping (amplitude → PWM)
- ✓ Speed constraints (0-255)
- ✓ Stop functionality
- ✓ Amplitude response curve

## What Can't Be Tested

- Timer interrupts (hardware-specific)
- Watchdog timer (would reset system)
- Actual microphone input
- Actual motor output

These require real Arduino hardware and should be tested on-device.

On this project, on-device tests live in `main/tests_on_device.cpp` and run when
`ENABLE_ON_DEVICE_TESTS` is set to `1` in `main/config.h`.




