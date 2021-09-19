
#include "game.h"

static void player_fire(GameObject *player, GameState *state) {
	if (player->player_data.level == 1 || player->player_data.level == 3) {
		game_init_player_bullet(state, ((int)player->x) + 4, SCREEN_HEIGHT - 10 - 2);
	}

	if (player->player_data.level > 1) {
		game_init_player_bullet(state, ((int)player->x) - 2, SCREEN_HEIGHT - 10);
		game_init_player_bullet(state, ((int)player->x) + 8 + 2, SCREEN_HEIGHT - 10);
	}
}

static void game_player_update(GameObject *player, GameState *state) {
	player->player_data.velocity *= 0.65;

	if (btn(KEY_RIGHT)) {
		player->player_data.velocity += 2.0;
	}

	if(btn(KEY_LEFT)) {
		player->player_data.velocity -= 2.0;
	}

	player->x += player->player_data.velocity;

	if (player->x+16 > SCREEN_WIDTH) {
		player->x = SCREEN_WIDTH-16;
		player->player_data.velocity = 0;
	}

	if (player->x < 0) {
		player->x = 0;
		player->player_data.velocity = 0;
	}

	if (btnp(KEY_A)) {
		player_fire(player, state);
	}
}

static void player_levelup(GameObject *player) {
	if (player->player_data.level >= 3) return;
	player->flicker = 0b0101010101;

	player->player_data.level++;
	player->sprite.id += 2;
}

static void player_leveldown(GameObject *player) {
	if (player->player_data.level == 1) return;

	player->player_data.level--;
	player->sprite.id -= 2;
}

static int player_can_be_hit(GameObject *player) {
	return player->flicker == 0;
}

static void player_hit_by_alien_bullet(GameObject *bullet, GameState *state, GameObject *player) {
	if(!player_can_be_hit(player)) return;

	player_leveldown(player);
	player->player_data.lives -= bullet->bullet_data.damage;
	player->flicker = 0b110011;
}

static void player_powerup(GameObject *powerup, GameState *state, GameObject *player) {
	if(powerup->powerup_data.kind == POWER_UP_LIVE) {
		player->player_data.lives++;
		if (player->player_data.lives > 3) player->player_data.lives = 3;
	} else if (powerup->powerup_data.kind == POWER_UP_UPGRADE) {
		player_levelup(player);
	}
}

static void player_onCollision(GameObject *other, GameState *state, GameObject *player) {
	if (other->type == OBJECT_ALIEN_BULLET) {
		player_hit_by_alien_bullet(other, state, player);
	} else if(other->type == OBJECT_POWERUP) {
		player_powerup(other, state, player);
	}
}

GameObject *game_init_player(GameState *state) {
	GameObject *player = game_object_construct(state,
		OBJECT_PLAYER,
		0, 2, 1,
		(SCREEN_WIDTH-16) / 2, SCREEN_HEIGHT - 8 - 2);
	player->update = game_player_update;
	player->player_data.velocity = 0;
	player->player_data.level = 1;
	player->player_data.lives = 3;
	player->player_data.onCollision = player_onCollision;
	return player;
}
