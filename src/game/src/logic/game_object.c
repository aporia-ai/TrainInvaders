
#include "game.h"

GameObject *game_object_alloc(GameState *state) {
	GameObject *object = state->objects;
	int i = MAX_OBJECTS;
	while(i--) {
		if (!object->valid) {
			copyzero(object, sizeof(GameObject));
			object->valid = 1;
			return object;
		}
		object++;
	}

	return NULL;
}

void game_objects_clear(GameState *state) {
	copyzero(state->objects, MAX_OBJECTS * sizeof(GameObject));
}

GameObject *game_object_construct(GameState *state, uint8_t type, uint8_t sprite_id, int sw, int sh, double x, double y) {
	GameObject *obj = game_object_alloc(state);
	obj->type = type;
	obj->visible = 1;
	obj->sprite.id = sprite_id;
	obj->x = x;
	obj->y = y;
	obj->sw = sw;
	obj->sh = sh;
	obj->draw = generic_draw;
	obj->update = generic_noaction;

	return obj;
}

GameObject *game_object_find(uint8_t type, GameState *state, int *offset) {
	for(int i = *offset; i < MAX_OBJECTS; i++) {
		if (state->objects[i].valid && state->objects[i].type == type) {
			*offset = i+1;
			return &state->objects[i];
		}
	}
	return NULL;
}

void game_objects_update(GameState *state) {
	GameObject *object = state->objects;
	int i = MAX_OBJECTS;
	while(i--) {
		if (object->valid)
			object->update(object, state);
		object++;
	}
}

void game_objects_draw(GameState *state) {
	GameObject *object = state->objects;
	int i = MAX_OBJECTS;
	while(i--) {
		if (object->valid && object->visible)
			object->draw(object, state);
		object++;
	}
}

void generic_noaction(GameObject *obj, GameState *state) {}

void generic_draw(GameObject *obj, GameState *state) {
	uint8_t sprite_id = obj->sprite.id;
	if (obj->animation.n > 0) {
		obj->animation.current_count++;
		if (obj->animation.current_count == obj->animation.delay) {
			obj->animation.current_count = 0;
			obj->animation.current_index++;
			if (obj->animation.current_index > obj->animation.n)
				obj->animation.current_index = 0;
		}
		if (obj->animation.current_index > 0) {
			sprite_id = obj->animation.sprites[obj->animation.current_index-1].id;
		}
	}

	if (!(obj->flicker&1)) {
		spr(sprite_id,
		obj->sw,
		obj->sh,
		obj->x,
		obj->y);
	}

	obj->flicker >>= 1;
}

void game_object_delete(GameObject *obj) {
	obj->valid = 0;
}

typedef struct {
	int x;
	int y;
	int w;
	int h;
} Rect;

static void game_object_hitbox(GameObject *obj, Rect *box) {
	box->x = obj->x + obj->hitbox.margin_left;
	box->y = obj->y + obj->hitbox.margin_top;
	box->w = (obj->sw*8) - obj->hitbox.margin_left - obj->hitbox.margin_right;
	box->h = (obj->sh*8) - obj->hitbox.margin_top - obj->hitbox.margin_bottom;
}

static int rect_overlap(Rect *a, Rect *b) {
	if (
		(a->x >= b->x && a->x < b->x+b->w) ||
		(b->x >= a->x && b->x < a->x+a->w)) {
		if (
			(a->y >= b->y && a->y < b->y+b->h) ||
			(b->y >= a->y && b->y < a->y+a->h)) {
			return 1;
		}
	}

	return 0;
}

int game_object_in_rect(GameObject *a, int bx, int by, int bw, int bh) {
	if(a->visible == 0) return 0;
	Rect arect;
	game_object_hitbox(a, &arect);
	Rect brect = {bx, by, bw, bh};

	return rect_overlap(&arect, &brect);
}

int game_object_collide(GameObject *a, GameObject *b) {
	// Only visible objects can collide
	if(a->visible == 0 || b->visible == 0) return 0;

	Rect arect, brect;
	game_object_hitbox(a, &arect);
	game_object_hitbox(b, &brect);

	return rect_overlap(&arect, &brect);
}
