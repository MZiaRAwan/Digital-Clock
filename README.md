# Digital Clock Application

A feature-rich digital clock application built with C++ and SFML, offering multiple time-related utilities including alarms, stopwatch, countdown timer, and calendar view.

## Features

- **Digital Clock Display**
  - Real-time clock with seconds precision
  - Toggle between 12-hour (AM/PM) and 24-hour format
  - Current date display

- **Alarm System**
  - Set up to 10 custom alarms
  - Enable/disable alarms individually
  - Custom alarm descriptions
  - Visual and audio alarm notifications
  - Persistent alarm storage (saved to `alarms.txt`)

- **Stopwatch**
  - Start, pause, and reset functionality
  - Millisecond precision display
  - Running status indicator

- **Countdown Timer**
  - Set custom countdown duration (hours, minutes, seconds)
  - Visual and audio notification when timer completes
  - Start, stop, and reset controls

- **Calendar View**
  - Monthly calendar display
  - Current date highlighting
  - Day of the week display

## Requirements

### Dependencies
- **C++ Compiler** (GCC or Clang with C++11 support)
- **SFML Library** (version 2.x)
  - `libsfml-graphics`
  - `libsfml-window`
  - `libsfml-system`
  - `libsfml-audio`

### System Requirements
- Linux (tested on Ubuntu/Debian-based systems)
- X11 or Wayland display server

## Installation

### Installing SFML

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

#### Fedora
```bash
sudo dnf install SFML-devel
```

#### Arch Linux
```bash
sudo pacman -S sfml
```

### Building the Application

1. Clone the repository:
```bash
git clone <repository-url>
cd Digital-Clock-main
```

2. Build using Make:
```bash
make
```

This will create the `digital_clock` executable.

3. Run the application:
```bash
make run
```

Or directly:
```bash
./digital_clock
```

### Clean Build
To remove compiled files and alarm data:
```bash
make clean
```

## Usage

### Main Menu Navigation
- Press **1** - Switch to Clock view
- Press **2** - Switch to Alarm settings
- Press **3** - Switch to Stopwatch
- Press **4** - Switch to Timer
- Press **5** - Switch to Calendar
- Press **6** or **ESC** - Exit application (or return to main menu)

### Clock View
- Press **F** - Toggle between 12-hour and 24-hour format
- Press **SPACE** or **ENTER** - Dismiss active alarm

### Alarm Settings
- Press **A** - Add a new alarm
  - Enter hour (0-23) and press ENTER
  - Enter minute (0-59) and press ENTER
- Press **1-9** - Select alarm by number
- Press **T** - Toggle selected alarm on/off
- Press **D** - Delete selected alarm
- Press **ESC** or **6** - Return to main menu

### Stopwatch
- Press **S** - Start the stopwatch
- Press **P** - Pause/Resume the stopwatch
- Press **R** - Reset the stopwatch
- Press **ESC** or **6** - Return to main menu

### Timer
- Press **T** - Set timer duration
  - Enter hours and press ENTER
  - Enter minutes (0-59) and press ENTER
  - Enter seconds (0-59) and press ENTER
- Press **S** - Start the timer
- Press **P** - Stop the timer
- Press **R** - Reset the timer
- Press **SPACE** or **ENTER** - Dismiss timer completion notification
- Press **ESC** or **6** - Return to main menu

### Calendar
- Press **ESC** or **6** - Return to main menu

## File Structure

```
Digital-Clock-main/
├── digital_clock.cpp    # Main source code
├── Makefile             # Build configuration
├── alarms.txt           # Alarm data storage (auto-generated)
├── digital_clock        # Compiled executable (after build)
└── README.md            # This file
```

## Technical Details

- **Language**: C++11
- **Graphics Library**: SFML 2.x
- **Window Size**: 1000x700 pixels
- **Frame Rate**: 60 FPS
- **Maximum Alarms**: 10
- **Alarm Storage**: Plain text file (`alarms.txt`)

## Notes

- The application automatically saves alarms when modified
- Alarms persist between application sessions
- Font loading attempts multiple system font paths for compatibility
- Audio notifications are generated programmatically (800Hz beep tone)

## Troubleshooting

### Font Not Loading
If you see a warning about font loading, ensure you have one of these fonts installed:
- Liberation Sans
- DejaVu Sans
- Noto Sans
- Arial

### SFML Not Found
If compilation fails with SFML errors, ensure SFML development libraries are installed:
```bash
sudo apt-get install libsfml-dev  # Ubuntu/Debian
```

### Audio Not Working
Ensure your system audio is properly configured. The application uses SFML's audio system for alarm sounds.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source and available for educational purposes.

## Author

Created as part of an Open Ended Lab project.

---

**Enjoy your Digital Clock Application!** ⏰

