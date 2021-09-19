#include "game.h"

static int no_other_alien_below(GameObject *alien, GameState *state) {
	int offset = 0;
	GameObject *obj;
	while((obj = game_object_find(OBJECT_ALIEN, state, &offset))) {
		if (game_object_in_rect(obj, alien->x - 16, alien->y + 10, 40, SCREEN_HEIGHT)) return 0;
	}
	return 1;
}


static void alien_hit(GameObject *bullet, GameState *state, GameObject *alien) {
	alien->alien_data.lives -= bullet->bullet_data.damage;
	if (alien->alien_data.lives > 0)
		alien->flicker = 3;
	else {
		if (randn(12 - 2 * (3-state->player->player_data.lives)) == 0 && no_other_alien_below(alien, state)) {
			game_init_powerup(state, alien->x, alien->y);
		}
		game_object_delete(alien);
		state->alien_counter--;
		state->score += 1;
	}
}

static int alien_update_offset(GameObject *alien) {
	if (alien->alien_data.offset > 0) {
		int dy = 3 <= alien->alien_data.offset ? 3 : alien->alien_data.offset;
		alien->y += dy;
		alien->alien_data.offset -= dy;
		return 1;
	}
	return 0;
}

static void alien_update(GameObject *alien, GameState *state) {
	if(alien_update_offset(alien)) return;

	alien->x += alien->alien_data.direction;
	if (
		(alien->x + 8 >= SCREEN_WIDTH) ||
		(alien->x < 1)
	) {
		int offset = 0;
		GameObject *obj;
		while((obj = game_object_find(OBJECT_ALIEN, state, &offset))) {
			if (obj <= alien) { // Has been updated before
				obj->x -= 2*alien->alien_data.direction;
			}
			obj->y += 2;
			obj->alien_data.direction *= -1.0;
		}
	}

	if (game_object_collide(alien, state->player)) {
		state->player->player_data.lives = 0;
	}

	if (no_other_alien_below(alien, state) && randn(alien->alien_data.shoot_prob) == 0) {
		game_init_alien_bullet(state, alien->x, alien->y + 3);
	}
}

static void alien_boss_update(GameObject *alien, GameState *state) {
	if(alien_update_offset(alien)) return;
	if (randn(alien->alien_data.shoot_prob) == 0) {
		if (state->wave < 3)
			game_init_alien_bullet(state, alien->x + 4, alien->y + 3);
		else {
			game_init_alien_bullet(state, alien->x, alien->y + 3);
			game_init_alien_bullet(state, alien->x + 8, alien->y + 3);
		}
	}

	if (randn(150) == 0) {
		double speed = randn(state->wave) / 2;
		if (speed > 4) speed = 4;
		if (alien->alien_data.direction < 0) {
			alien->alien_data.direction = -1 * speed;
		} else {
			alien->alien_data.direction = 1 * speed;
		}

	}

	alien->x += alien->alien_data.direction;

	if (
		(alien->x + 16 >= SCREEN_WIDTH) ||
		(alien->x < 1)
	) {
		alien->x -= 2*alien->alien_data.direction;
		alien->alien_data.direction *= -1;
	}
}

GameObject *game_init_alien(GameState *state, int x, int y) {
	GameObject *alien = game_object_construct(state, OBJECT_ALIEN, 16, 1, 1, x, y);
	alien->update = alien_update;
	alien->alien_data.onBulletHit = alien_hit;
	alien->alien_data.lives = state->wave/3;
	if (alien->alien_data.lives > 5)
		alien->alien_data.lives = 5;
	alien->alien_data.direction = 0.5;

	alien->animation.n = 1;
	alien->animation.delay = 6;
	alien->animation.sprites[0].id = 17;

	state->alien_counter++;

	return alien;
}

void game_init_alien_armada(GameState *state) {
	int rows = 1 + state->wave/2;
	int cols = 4 + state->wave/4;
	if (rows > 5) rows = 5;
	if (cols > 6) cols = 6;

	for(int y = 0; y < rows; y++) {
		for(int x = 0; x < cols; x++) {
			GameObject *alien = game_init_alien(state, 4 + x*16, rows*-16 + y*16);
			alien->alien_data.offset = rows*16 + STATUS_BAR_HEIGHT + 4;
			alien->alien_data.direction = 0.1 + 0.05 * state->wave;
			alien->alien_data.shoot_prob = 230 - state->wave * 10;
			if (alien->alien_data.shoot_prob < 50)
				alien->alien_data.shoot_prob = 50;
		}
	}
}

void game_init_alien_boss(GameState *state) {
	GameObject *alien = game_init_alien(state, 10 + randn(SCREEN_WIDTH-20-16), -8*16);
	alien->sprite.id = 19;
	alien->animation.n = 0;
	alien->sw = 2;
	alien->alien_data.offset = STATUS_BAR_HEIGHT + 8*16 + 16;
	alien->update = alien_boss_update;
	alien->alien_data.shoot_prob = 100 - state->wave * 8;
	alien->alien_data.lives = 2 + state->wave;
	if (alien->alien_data.shoot_prob < 20)
				alien->alien_data.shoot_prob = 20;
}
