
#include "system.h"

// This program exposes memory locations for the current
// state of the keyboard.
static uint8_t *_controller_memory;
static uint8_t _controller_memory_last_frame;
static uint8_t _controller_memory_pressed;
static uint32_t _last_time;

void system_input_init() {
	_controller_memory = (uint8_t *) malloc(1);
	_controller_memory_last_frame = 0;
}

uint8_t btn(uint8_t key) {
	return (*_controller_memory)&key;
}

uint8_t btnp(uint8_t key) {
	uint32_t t = time();
	if(t != _last_time) {
		_last_time = t;
		_controller_memory_pressed = (*_controller_memory) & (~_controller_memory_last_frame);
		_controller_memory_last_frame = *_controller_memory;
	}
	return _controller_memory_pressed&key;
}

EXPORT void *_get_input_ref() {
	return _controller_memory;
}
