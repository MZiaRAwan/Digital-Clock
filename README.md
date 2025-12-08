Digital Clock Application (C++ / SFML)
Overview

This project is a feature-rich Digital Clock Application developed in C++ using the SFML graphics and audio library.
It provides an interactive graphical interface that includes:

Real-time Digital Clock

Alarm System (with sound notifications)

Stopwatch Functionality

Countdown Timer

Calendar View

Keyboard-driven Menus and Controls

Persistent Alarm Storage

The application is designed to be simple, responsive, and user-friendly, with a clean UI built entirely using SFML.

Features
1. Digital Clock

Displays current hours, minutes, and seconds.

Updates in real time.

Centered graphical text display.

2. Alarm System

Add, edit, and delete alarms.

Alarms are saved to disk and automatically loaded on startup.

Plays an alarm sound when time is reached.

Option to dismiss alarms using keyboard input.

3. Stopwatch

Start, stop, pause, and reset functionality.

Tracks hours, minutes, seconds, and milliseconds.

Smooth real-time updates.

4. Timer

Set hours, minutes, and seconds for countdown.

Plays alert when timer reaches zero.

Option to dismiss timer alert.

5. Calendar

Displays current date (day, month, year).

Basic navigational and visual structure.

Requirements
Software Requirements

C++ Compiler (GCC, Clang, or MSVC)

SFML 2.5+ (Graphics, Window, System, Audio)

Build Dependencies

Your system must have the following installed:

sfml-window
sfml-graphics
sfml-system
sfml-audio

Build Instructions
Using the Provided Makefile

Navigate to the project directory:

cd proj


Build the application:

make


Run the application:

./clock

Manual Compilation (Without Makefile)

If you want to compile manually:

g++ digital_clock.cpp -o clock \
  -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

Controls
Global Navigation
Key	Action
Up/Down	Navigate menu items
Enter	Select option
Esc	Return to main menu
Alarm Controls
Key	Action
A	Add new alarm
D	Delete selected alarm
SPACE	Dismiss ringing alarm
Stopwatch Controls
Key	Action
S	Start / Stop stopwatch
R	Reset stopwatch
Timer Controls
Key	Action
T	Start / Stop timer
R	Reset timer
Arrow Keys	Adjust hours/minutes/seconds
SPACE	Dismiss timer alert
File Structure
proj/
│── digital_clock.cpp     # Main application source code
│── Makefile              # Build script


You may add additional folders such as:

/res for fonts

/sounds for alarm audio

/data for storing alarm configuration

Persistent Storage

Alarms are stored in a local text file.

They automatically reload when the app starts.

Prevents losing alarm settings between sessions.

Known Limitations

Window size is fixed.

Calendar is display-only (no navigation to other months).

No mouse interaction; keyboard-only UI.

Future Enhancements (Optional Ideas)

Add GUI buttons (mouse interaction)

Support multiple alarm tones

Add themes (light/dark mode)

Monthly calendar navigation

Export stopwatch/timer logs

Author

Developed as a C++/SFML project to strengthen implementation skills in graphics programming, event handling, and real-time system design.
