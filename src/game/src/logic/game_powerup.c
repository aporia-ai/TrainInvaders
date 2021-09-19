
#include "game.h"

#define POWERUP_SPEED 0.5

static void powerup_update(GameObject *powerup, GameState *state) {
	powerup->y += POWERUP_SPEED;
	if (game_object_collide(powerup, state->player)) {
		state->player->player_data.onCollision(powerup, state, state->player);
		game_object_delete(powerup);
	} else if(powerup->y >= SCREEN_HEIGHT) {
		game_object_delete(powerup);
	}
}

void game_init_powerup(GameState *state, int x, int y) {
	int kind, sprite;

	if (state->player->player_data.lives < 3) {
		kind = POWER_UP_LIVE;
		sprite = 10;
	} else if(state->player->player_data.level < 3) {
		kind = POWER_UP_UPGRADE;
		sprite = 11;
	} else {
		return; // no powerup else
	}

	GameObject *obj = game_object_construct(state, OBJECT_POWERUP, sprite, 1, 1, x, y);
	obj->powerup_data.kind = kind;
	obj->update = powerup_update;
}
