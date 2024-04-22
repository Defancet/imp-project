# IMP ARM-FITkit3: Game SNAKE

## Author

- **Name:** Maksim Kalutski
- **Login:** xkalut00

## Introduction

The game is implemented in the `main.c` file in C language within the Kinetis Design Studio (KDS IDE). The program
utilizes the MK60D10 microcontroller's peripheral modules to control the display and process input from the buttons.

## Implementation

- `init_snake()`: Initializes the snake at the start of the game with a fixed length of 4.
- `SystemConfig()`: Configures the MCU peripherals necessary for the correct functioning of the game.
- `delay()`: Ensures a delay in the program to create the illusion that all the snake's LEDs light up at once.
- `column_select()`: Manages column selection on the display using a 4-to-16 decoder.
- `row_select()`: Selects and displays rows of the snake on the display.
- `PORTE_IRQHandler()`: Handles button press interrupts, setting the direction of the snake or restarting the game.
- `move_snake()`: Moves the snake in the current direction and updates the positions of its segments.

## Game Description

### Display

The game uses two 8x8 matrix displays to show the snake, which is represented by 4 segments each displayed as a light
point.

### Control

Controlled by buttons on the FITkit3, each button has its specific function:

- SW2: Move snake right
- SW3: Move snake down
- SW4: Move snake left
- SW5: Move snake up
- SW6: Restart the game

## Video Demonstration

A video demonstration of the project is available on [YouTube](https://www.youtube.com/watch?v=oKXXRvAqGBU&feature=youtu.be), showcasing the implementation, control, and functions of
the game on FITkit3.
