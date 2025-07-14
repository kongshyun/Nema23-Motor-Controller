# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Qt-based GUI application for controlling a stepper motor via ESP32 over UART serial communication. The application allows users to control a Nema23 stepper motor by setting RPM and either rotation count or duration.

## Build System and Commands

The project uses Qt's qmake build system:

```bash
# Build the project
qmake stepperESP32.pro
make

# For Windows with MinGW
qmake stepperESP32.pro
mingw32-make

# Clean build artifacts
make clean

# Run architecture test (verification only, not built with main project)
g++ -std=c++17 test_architecture.cpp -I./inc -o test_arch && ./test_arch
```

Qt Creator is the recommended IDE for development. The project requires Qt 5/6 with serialport module.

## Architecture

### Core Components

1. **MainWindow** (`mainwindow.h/cpp`) - Main UI controller that orchestrates all interactions
   - Manages serial port selection and connection
   - Handles UI events (buttons, sliders)
   - Coordinates between SerialHandler and MotorControl
   - Real-time logging and status updates

2. **SerialHandler** (`serialhandler.h/cpp`) - UART communication layer
   - Opens/closes serial connections to ESP32
   - Sends commands and receives responses
   - Emits `dataReceived` signal for incoming data
   - Uses QSerialPort with 115200 baud rate

3. **MotorControl** (`motorcontrol.h/cpp`) - Motor command logic and state management
   - Uses Strategy pattern with IMotorCommand interface
   - Supports rotation-based and time-based commands
   - Processes ESP32 responses (`READY`, `TURN:X`, `DONE`, `STOPPED`)
   - Tracks progress and status

4. **Command Strategy Classes** - Follow SOLID principles
   - `IMotorCommand` - Interface for command strategies
   - `RotationCommand` - Builds `"RPM:X ROT:Y"` commands
   - `TimeCommand` - Builds `"RPM:X TIME:Y"` commands
   - `MotorCommandFactory` - Creates appropriate command objects

### Communication Protocol

- Connection: Send `"HELLO"` → Expect `"READY"` response
- Commands: 
  - Rotation mode: `"RPM:X ROT:Y"` format sent to ESP32
  - Time mode: `"RPM:X TIME:Y"` format sent to ESP32
- Responses: `TURN:X` for progress, `DONE`/`STOPPED` for completion

### Project Structure

```
inc/           - Header files
src/           - Implementation files  
images/        - UI resources (icons, logos)
mainwindow.ui  - Qt Designer UI layout
images.qrc     - Qt resource file
prompt/        - System documentation (YAML specs)
```

### Key Features

- Real-time serial port detection and selection
- Motor parameter validation (RPM and rotation count)
- Progress tracking with visual feedback
- Connection status monitoring
- Timestamped activity logging

### SOLID Architecture Implementation

The codebase now follows SOLID principles:

- **Single Responsibility**: Each class has one clear purpose
- **Open/Closed**: New motor modes can be added without modifying existing code
- **Strategy Pattern**: Command generation is abstracted through IMotorCommand interface
- **Dependency Injection**: MotorControl accepts command strategies via factory
- **Factory Pattern**: MotorCommandFactory creates appropriate command objects

### Development Notes

- Uses C++17 with Qt 5/6 compatibility  
- Serial communication at 115200 baud
- Korean language UI text and status messages
- Qt Designer used for UI layout (mainwindow.ui)
- Header files in `inc/`, implementation in `src/`
- Strategy pattern allows easy extension of motor control modes
- `test_architecture.cpp` provides SOLID architecture verification (standalone test)
- Build artifacts generated in `build/` directory

### Qt Module Dependencies

Required Qt modules configured in stepperESP32.pro:
- `core gui` - Basic Qt functionality
- `serialport` - UART communication with ESP32  
- `widgets` - UI components (Qt 5+ compatibility)