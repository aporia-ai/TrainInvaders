
#pragma once

#define KEY_LEFT 1
#define KEY_UP 2
#define KEY_RIGHT 4
#define KEY_DOWN 8
#define KEY_A 16
#define KEY_B 32
#define KEY_START 64
#define KEY_SELECT 128

// Initialize the memory buffer for the keyboard.
void system_input_init();

// Return location of keyboard buffer.
void *_get_input_ref();

// Return 1, if key is pressed. 0 otherwise.
uint8_t btn(uint8_t key);

// Return 1, if key is pressed and was not pressed when
// this function was called the last time. 0 otherwise.
uint8_t btnp(uint8_t key);
