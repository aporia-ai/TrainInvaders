#pragma once

typedef struct _Alien Alien;

#include "game_object.h"

struct _Alien {
	int lives;
	double direction;
	int offset;
	int shoot_prob;
	void (*onBulletHit)(GameObject *bullet, GameState *state, GameObject *obj);
};

GameObject *game_init_alien(GameState *state, int x, int y);
void game_init_alien_armada(GameState *state);
void game_init_alien_boss(GameState *state);
