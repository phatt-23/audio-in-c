#pragma once

#include <stdio.h>
#include <unistd.h>
#include <string.h>

// ANSI escape sequences reference:
// - https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html#cursor-navigation
// - https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html

// Height of the drawing canvas
// Increase this if you need more height for drawing.
// However, you will the need to scroll further up to see the
// beginning of the drawing :)
#define RESET  "\x1b[0m"
#define GREEN  "\x1b[42;1m"
#define RED    "\x1b[41;1m"
#define BLUE   "\x1b[44;1m"
#define WHITE  "\x1b[47;1m"
#define YELLOW "\x1b[43;1m"
#define BLACK  "\x1b[40;1m"



// Height of the drawing canvas
// Increase this if you need more height for drawing.
// However, you will the need to scroll further up to see the
// beginning of the drawing :)
int canvas_height = 30;

// Use the functions declared here (`set_red_color`, `move_right`, `draw_pixel`, `animate`, etc.)
// to draw things.

void flush() {
  fflush(stdout);
}

//---Cursor management---

// Hide the blinking terminal cursor.
void hide_cursor() {
  printf("\x1b[?25l");
  flush();
}

// Show the terminal cursor.
void show_cursor() {
  printf("\x1b[?25h");
  flush();
}

//---Color management---


// Reset the drawing and background color to the default values.
char* reset_color() {
  printf(RESET);
  flush();
  return RESET;
}

// Set the background color to red.
char* set_red_color() {
  printf(RED);
  flush();
  return RED;
}

// Set the background color to green.
char* set_green_color() {
  printf(GREEN);
  flush();
  return GREEN;
}

// Set the background color to blue.
char* set_blue_color() {
  printf(BLUE);
  flush();
  return BLUE;
}

// Set the background color to white.
char* set_white_color() {
  printf(WHITE);
  flush();
  return WHITE;
}

// Set the background color to yellow.
char* set_yellow_color() {
  printf(YELLOW);
  flush();
  return YELLOW;
}

// Set the background color to black.
char* set_black_color() {
  printf(BLACK);
  flush();
  return BLACK;
}

//---Cursor movement---

// Move cursor right.
void move_right() {
  printf("\x1b[1C");
  flush();
}

// Move cursor left.
void move_left() {
  printf("\x1b[1D");
  flush();
}

// Move cursor up.
void move_up() {
  printf("\x1b[1A");
  flush();
}

// Move cursor down.
void move_down() {
  printf("\x1b[1B");
  flush();
}

// Move cursor to the specified `line` and `column` position.
// Positions are numbered from 1, the origin is at (1, 1).
void move_to(int line, int column) {
  printf("\x1b[%d;%dH", line, column);
  flush();
}

//---Screen management---

// Reset the drawing environment.
// Clears the screen, moves the cursor to the origin in the top-left corner and
// prepares the drawing canvas.
void clear_screen() {
  reset_color();
  hide_cursor();

  // Clear screen
  printf("\x1b[2J");
  // Move cursor to origin
  move_to(1, 1);
  // Create canvas
  for (int i = 0; i < canvas_height; i++) {
    printf("\n");
  }
  move_to(1, 1);
  flush();
}


//---Drawing---

// Draws a single "pixel" using the currently set background color.
// The cursor position is not changed after drawing!
// You need to move the cursor manually before drawing the next pixel,
// otherwise you will just overwrite the previous pixel.
void draw_pixel() {
  printf(" ");
  move_left();
}

// Clears the drawing session, resets colors and cursor.
// Call this before the end of the program, otherwise your terminal will get messed up.
void end_drawing() {
  reset_color();
  move_to(1, 1);
  for (int i = 0; i < canvas_height; i++) {
    move_down();
  }
  printf("\n");
  show_cursor();
}

void animate_ms(int ms) {
  flush();
  // Sleep for the given amount of ms
  usleep(1000 * ms);
}

// Pause the program for a while, so that you can see what has been drawn so far.
// If you repeatedly call animate() after drawing something, you can crate interactive animations.
// You can also combine this with clear_screen() to draw something new on every "animation frame".
void animate() {
  animate_ms(500);
}



