
#pragma once

typedef struct _Bullet Bullet;

#include "game_object.h"

struct _Bullet {
	int damage;
	double speed;
};

void game_init_player_bullet(GameState *state, int x, int y);
void game_init_alien_bullet(GameState *state, int x, int y);
