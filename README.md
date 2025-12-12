## Project Structure

```
kinetic-sculpture/
├── main/                    # Arduino source code
│   ├── main.ino            # Main program entry point
│   ├── tests_on_device.*   # On-device unit tests (run via ENABLE_ON_DEVICE_TESTS)
│   ├── config.h            # Configuration constants
│   ├── audio_processor.*   # Audio sampling & processing
│   ├── motor_controller.*  # Motor control logic
│   ├── timer_setup.*       # Timer interrupt configuration
│   ├── system_supervisor.* # Finite state machine (INIT/IDLE/ACTIVE/FAULT/SHUTDOWN)
│   └── watchdog_utils.*    # Watchdog timer utilities
│
├── tests/                   # Desktop testing suite
│   ├── mock_arduino.*      # Arduino function mocks
│   ├── test_audio_processor.cpp
│   ├── test_motor_controller.cpp
│   ├── Makefile            # Build tests
│   └── README.md           # Testing documentation
│
├── docs/                    # Documentation
│   ├── system_architecture.yaml
│   └── diagram.md          # Mermaid diagrams
│
├── tools/                   # Utilities
│   └── generate_diagram.py # Diagram generator
```

### 2. Run Desktop Tests

```bash
cd tests
make run
```

## Configuration

Edit `main/config.h` to adjust:
- `SAMPLE_RATE`: Audio sampling rate (default: 1000 Hz)
- `BUFFER_SIZE`: Smoothing buffer size (default: 20)
- `DC_OFFSET`: Microphone baseline (default: 512)
- `MIN_MOTOR_SPEED`: Minimum PWM (default: 80)
- `MAX_MOTOR_SPEED`: Maximum PWM (default: 255)


## System Architecture

See `docs/diagram.md` for:
- Flowchart diagram
- Sequence diagram
- Component interactions

## Development

### Architecture
- **config.h**: Single source of configuration
- **audio_processor**: Handles audio sampling & smoothing
- **motor_controller**: Maps amplitude to motor speed
- **timer_setup**: Configures hardware timer interrupt
- **watchdog_utils**: System reliability functions

### Real-Time Processing
- Hardware timer ISR samples microphone at 1kHz (UNO R4 uses `FspTimer`)
- Rolling buffer smooths audio (20 samples)
- FSM drives motor updates at 100Hz (10ms intervals) with slew limiting
- Watchdog resets if system hangs (8s timeout)
