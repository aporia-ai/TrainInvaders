
#include "game.h"

static void text_draw(GameObject *obj, GameState *state) {
	int l = strlen(obj->title_data.text);
	int x = (SCREEN_WIDTH - l*(LETTER_WIDTH+1))/2 - 1;
	int y = SCREEN_HEIGHT/2 - 1;
	int c = 3;
	if (obj->title_data.ticks % 3 == 1) c = 4;
	else if (obj->title_data.ticks % 3 == 2) c = 7;

	text(obj->title_data.text, x+1, y+1, 0);
	text(obj->title_data.text, x, y, c);
}

static void text_update(GameObject *obj, GameState *state) {
	obj->title_data.ticks--;
	if (obj->title_data.ticks == 0) {
		if (obj->title_data.next != NULL)
			obj->title_data.next(state);
		game_object_delete(obj);
	}
}

GameObject *game_init_title(GameState *state) {
	GameObject *text = game_object_alloc(state);
	text->visible = 1;
	text->update = text_update;
	text->draw = text_draw;
	return text;
}
