
#pragma once

typedef struct _PowerUp PowerUp;
#define POWER_UP_LIVE 1
#define POWER_UP_UPGRADE 2

#include "game_object.h"

struct _PowerUp {
	int kind;
};

void game_init_powerup(GameState *state, int x, int y);
