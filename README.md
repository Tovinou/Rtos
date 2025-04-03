RTOS Projects

This repository contains various Real-Time Operating System (RTOS) projects built using FreeRTOS and Arduino for microcontroller platforms such as the Arduino Nano and Raspberry Pi Zero. These projects demonstrate the capabilities of FreeRTOS in managing multiple tasks, handling interrupts, and working with peripherals like LEDs, buttons, and communication protocols.

Projects Overview
This repository includes several example projects that showcase how to:
Use FreeRTOS to manage tasks on an embedded system.
Control RGB LEDs with PWM.
Handle button inputs with interrupts and debounce mechanisms.
Integrate RTOS tasks with interrupt-driven peripherals.
Implement button-based control to suspend and resume tasks.

Example Projects:
RGB LED Control with FreeRTOS

A demo of controlling an RGB LED with three separate FreeRTOS tasks, each controlling one color.
The tasks interact with the hardware via PWM, implementing a fading effect for red, blinking for green, and random brightness for blue.
Button Control with Interrupts
Buttons are used to suspend and resume FreeRTOS tasks.
The code uses interrupts for button presses, with debouncing handled by FreeRTOS tasks.

Requirements
Hardware:
Arduino Nano or Raspberry Pi Zero (with FreeRTOS support).
RGB LED (with PWM control pins).
Push buttons (with pull-up resistors).
Basic electronic components (resistors, wires, etc.).

Software:
Arduino IDE with FreeRTOS library for Arduino Nano.
Raspberry Pi Zero running Linux with FreeRTOS.

CMake for building the project (if applicable).

Dependencies:
FreeRTOS: FreeRTOS on Arduino
Raspberry Pi Zero with Linux and appropriate toolchains for CMake-based projects.

Setup
1. Clone the Repository
git clone https://github.com/tovinou/Rtos.git
cd Rtos
ls -l for to see the list of different rtos projects

2. Install Dependencies
   
For Arduino Nano projects:
Ensure you have the Arduino IDE installed with the FreeRTOS library.
Select Arduino Nano as the target board in the Tools > Board menu.

For Raspberry Pi Zero projects:
Make sure your Raspberry Pi Zero has Linux installed and the necessary development tools (e.g., gcc, make).
Ensure FreeRTOS is properly installed on your system.

3. Open the Project in Arduino IDE (for Arduino Nano)   
Open the .ino files in the Arduino IDE.
Select Arduino Nano from the Tools > Board menu.
Upload the code to the board.

4. Build with CMake (for Raspberry Pi Zero-based Projects)
For Raspberry Pi Zero projects using CMake:
Set up your FreeRTOS environment on Raspberry Pi Zero (or configure accordingly).
Build the project:
mkdir build
cd build
cmake ..
make
Flash the firmware to your Raspberry Pi Zero, or use standard Linux methods to run your project.

Tasks and Features
Task Management: Multiple tasks are created and managed by the FreeRTOS kernel.
Interrupt Handling: Button presses trigger interrupts that notify the FreeRTOS tasks.
PWM Control: PWM signals control the brightness of the RGB LED.
Debounce Mechanism: The button handling tasks include debounce logic to avoid false triggers.

Task List:
Red LED Fade: Smooth fade-in/out effect on the red channel of the RGB LED.
Green LED Blink: Periodic blinking on the green channel of the RGB LED.
Blue LED Random: Random brightness levels on the blue channel of the RGB LED.
Button Interrupts: Suspend/resume tasks based on button presses, using interrupts for efficiency.

Contributing
Feel free to fork this project and submit pull requests if you'd like to contribute improvements or add new features. Ensure any changes maintain the general style and structure of the code, and consider adding comments to explain any complex changes.
How to Contribute:
Fork this repository.
Create a feature branch: git checkout -b my-new-feature.
Commit your changes: git commit -am 'Add new feature'.
Push to your branch: git push origin my-new-feature.
Submit a pull request.

License
This project is licensed under the MIT License - see the LICENSE file for details.
