
#pragma once

#include "system.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define NO_TRANSPARENCY 16
#define LETTER_WIDTH 3
#define LETTER_HEIGHT 5

// The screen-buffer is an 2 dimensional array that uses
// 4 bytes per pixel.

// Initializes the screen buffer
void system_video_init(SystemConfiguration *configuration);
// Returns location of screen buffer
void *_get_screen_ref();

// Clears the screen with color c.
void cls(uint8_t c);
// Draw color c as transparent in following calls.
void palt(uint8_t c);
// Draw a filled rect.
void rectfill(int x1, int y1, int w, int h, uint8_t c);
// Draw sprite with id n
void spr(int n, int w, int h, int x, int y);
// Set pixel
void pset(int x, int y, uint8_t c);

// Print text on the screen
void text(const char *text, int x, int y, uint8_t c);
