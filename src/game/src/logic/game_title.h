
#pragma once

typedef struct _Title Title;
#define TITLE_MAX_LENGTH 40

#include "game_object.h"

struct _Title {
	void (*next)(GameState *state);
	char text[TITLE_MAX_LENGTH+1];
	int ticks;
};

GameObject *game_init_title(GameState *state);
