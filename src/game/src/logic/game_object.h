
#pragma once

typedef struct _GameObject GameObject;

#define FLICKER_HIDE_2_TICKS 3
#define FLICKER_HIDE_1_TICK 1

#define OBJECT_PLAYER 1
#define OBJECT_PLAYER_BULLET 2
#define OBJECT_ALIEN_BULLET 3
#define OBJECT_ALIEN 4
#define OBJECT_TITLE 5
#define OBJECT_POWERUP 6

#include "game_player.h"
#include "game_bullet.h"
#include "game_alien.h"
#include "game_title.h"
#include "game_powerup.h"

typedef struct {
	int margin_top;
	int margin_bottom;
	int margin_left;
	int margin_right;
} Hitbox;

typedef struct {
	uint8_t id;
	int flip_x;
	int flip_y;
} Sprite;

typedef struct {
	uint8_t n;
	Sprite sprites[3];
	int delay;
	int current_count;
	int current_index;
} Animation;

// Every object in the game is represented by a GameObject.
// GameObjects contain state information about the object
// and pointers to functions to update an draw the object.
struct _GameObject {
	// Only valid game objects are updated and rendered.
	// If an object is not valid it might me replaced by
	// the next object entering the game.
	uint8_t valid;

	// Every kind of object is identified by a specific type
	uint8_t type;

	// Only visible objects will be drawn.
	uint8_t visible;

	// Flicker is a bit mask used while drawing the object.
	// The least significant bit says if drawing is skipped (=1).
	// Flicker is shifted to the right one bit each tick.
	// This can be used to realize flickering sprites.
	uint32_t flicker;

	// Which sprite should be drawn?
	Sprite sprite;
	Animation animation;

	// What is the position
	double x;
	double y;

	// Width and height in units of 8 pixels
	int sw;
	int sh;

	// The hitbox is defined relative to spritebox
	// and used for detecting collisions.
	Hitbox hitbox;

	uint32_t tick;
	void(*update)(struct _GameObject *obj, struct _GameState *state);
	void(*draw)(struct _GameObject *obj, struct _GameState *state);

	// Depending on the type, specific data about the
	// object can be accessed
	union {
		Player player_data;
		Bullet bullet_data;
		Alien alien_data;
		Title title_data;
		PowerUp powerup_data;
	};

};


GameObject *game_object_alloc(GameState *state);
void game_objects_clear(GameState *state);
void game_object_delete(GameObject *obj);
GameObject *game_object_construct(GameState *state, uint8_t type, uint8_t sprite_id, int sw, int sh, double x, double y);
GameObject *game_object_find(uint8_t type, GameState *state, int *offset);

void game_objects_update(GameState *state);
void game_objects_draw(GameState *state);

void generic_draw(GameObject *obj, GameState *state);
void generic_noaction(GameObject *obj, GameState *state);

int game_object_collide(GameObject *a, GameObject *b);
int game_object_in_rect(GameObject *obj, int bx, int by, int bw, int bh);
