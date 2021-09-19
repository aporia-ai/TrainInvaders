
#pragma once

typedef struct _Player Player;

#include "game_object.h"

struct _Player {
	double velocity;
	void (*onCollision)(GameObject *other, GameState *state, GameObject *player);
	int level;
	int lives;
};

GameObject *game_init_player(GameState *state);
