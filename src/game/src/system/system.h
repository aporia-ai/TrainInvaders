
#pragma once

#include "system_base.h"

typedef struct {
	uint32_t *palette;
	uint8_t *spritemem;
	int sprite_w;
	int sprite_h;
} SystemConfiguration;

#include "system_video.h"
#include "system_input.h"

void system_init(SystemConfiguration *configuration);
