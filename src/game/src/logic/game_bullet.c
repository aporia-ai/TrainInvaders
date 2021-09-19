#include "game.h"

static void check_alien_collision(GameObject *bullet, GameState *state) {
	for(int i = 0; i < MAX_OBJECTS; i++) {
		GameObject *obj = &state->objects[i];
		if (obj->valid && obj->type == OBJECT_ALIEN) {
			if (game_object_collide(bullet, obj)) {
				obj->alien_data.onBulletHit(bullet, state, obj);
				game_object_delete(bullet);
			}
		}
	}
}

static void check_player_collision(GameObject *bullet, GameState *state) {
	if (game_object_collide(bullet, state->player)) {
		state->player->player_data.onCollision(bullet, state, state->player);
		game_object_delete(bullet);
	}
}

static void game_bullet_update(GameObject *bullet, GameState *state) {
	bullet->y += bullet->bullet_data.speed;
	if (bullet->y <= 0 || bullet->y >= SCREEN_HEIGHT) {
		game_object_delete(bullet);
		return;
	}

	if (bullet->type == OBJECT_PLAYER_BULLET) {
		check_alien_collision(bullet, state);
	} else if(bullet->type == OBJECT_ALIEN_BULLET) {
		check_player_collision(bullet, state);
	}
}

static GameObject *game_init_bullet(GameState *state, int x, int y) {
	GameObject *bullet = game_object_construct(state, OBJECT_PLAYER_BULLET, 8, 1, 1, x, y);
	bullet->bullet_data.damage = 1;
	bullet->bullet_data.speed = -3;
	bullet->hitbox.margin_top = 3;
	bullet->hitbox.margin_left = 3;
	bullet->hitbox.margin_right = 3;
	bullet->hitbox.margin_bottom = 2;
	bullet->update = game_bullet_update;
	return bullet;
}

void game_init_player_bullet(GameState *state, int x, int y) {
	GameObject *bullet = game_init_bullet(state, x, y);
}

void game_init_alien_bullet(GameState *state, int x, int y) {
	GameObject *bullet = game_init_bullet(state, x, y);
	bullet->type = OBJECT_ALIEN_BULLET;
	bullet->sprite.id = 9;
	bullet->bullet_data.speed = 3;
}
