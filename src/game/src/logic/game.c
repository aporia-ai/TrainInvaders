
#include "game.h"

//
// WAVE
//

static void draw_status_panel(GameState *state) {
	// Draw Score
	char score_buffer[21];
	int maxlen = 20;
	char *start = score_buffer;
	start = put_string("Score: ", start, &maxlen);
	start = put_number(state->score, start, &maxlen);
	rectfill(0, 0, SCREEN_WIDTH, LETTER_HEIGHT + 4, 0);
	rectfill(0, LETTER_HEIGHT+4, SCREEN_WIDTH, 1, 3);
	text(score_buffer, 2, 2, 3);

	// Draw lives
	for (int i = 0; i < 3; i++) {
		spr(state->player->player_data.lives >i ? 6 : 7, 1, 1, SCREEN_WIDTH - (3*8) + i*8, 0);
	}
}

static void game_update_common(GameState *state) {
	game_objects_update(state);
	cls(0);
	game_objects_draw(state);
	draw_status_panel(state);
}

static void game_update_wave(GameState *state) {
	game_update_common(state);

	if (state->player->player_data.lives <= 0) {
		state->next_scene = SCENE_GAMEOVER;
	} else if(state->alien_counter == 0 && state->scene_ticks > 30 * 3) {
		int bosses = 1 + state->wave/3;
		if (bosses > 6)
				bosses = 6;
		for(int i = 0; i < bosses; i++) {
			game_init_alien_boss(state);
		}
		state->next_scene = SCENE_BOSS;
	}
}

void game_init_wave(GameState *state) {
	GameObject *title = game_init_title(state);
	int len = TITLE_MAX_LENGTH;
	char *s = title->title_data.text;
	s = put_string("Wave ", s, &len);
	s = put_number(state->wave, s, &len);

	title->title_data.ticks = 30 * 2; // 2 seconds
	title->title_data.next = game_init_alien_armada;
}

//
// BOSS
//

static void game_update_boss(GameState *state) {
	game_update_common(state);
	if(state->alien_counter == 0) {
		state->wave++;
		game_init_wave(state);
		state->next_scene = SCENE_WAVE;
	}

	if (state->player->player_data.lives <= 0) {
		state->next_scene = SCENE_GAMEOVER;
	}
}

//
// MAIN MENU
//

static void game_start_new(GameState *state) {
	game_objects_clear(state);
	state->player = game_init_player(state);
	state->wave = 1;
	state->alien_counter = 0;
	state->score = 0;
	game_init_wave(state);
	state->next_scene = SCENE_WAVE;
}

static void game_update_mainmenu(GameState *state) {
	cls(0);

	text("Train invaders", (SCREEN_WIDTH-14*(LETTER_WIDTH+1))/2, 25, 3);
	if ((state->scene_ticks / 10) % 2 == 0) {
		text("PRESS X to start", (SCREEN_WIDTH-16*(LETTER_WIDTH+1))/2, SCREEN_HEIGHT-32, 4);
	}
	text("move with arrow keys", (SCREEN_WIDTH-20*(LETTER_WIDTH+1))/2, SCREEN_HEIGHT-32+LETTER_HEIGHT+2, 2);
	if (btnp(KEY_A)) {
		game_start_new(state);
	}
}

//
// GAME OVER
//

static void game_update_gameover(GameState *state) {
	int boxw = 9*(LETTER_WIDTH+1) - 1 + 10;
	int boxh =   LETTER_HEIGHT + 6;
	rectfill((SCREEN_WIDTH-boxw)/2,     (SCREEN_HEIGHT-boxh)/2,     boxw,     boxh,     3);
	rectfill((SCREEN_WIDTH-boxw)/2 + 1, (SCREEN_HEIGHT-boxh)/2 + 1, boxw - 2, boxh - 2, 0);
	if ((state->scene_ticks / 10) % 2 == 0) {
		text("Game over", (SCREEN_WIDTH-9*(LETTER_WIDTH+1))/2, (SCREEN_HEIGHT-LETTER_HEIGHT)/2, 3);
	}

	if (state->scene_ticks > 30 * 5 - 1) { // > 5 seconds

		state->next_scene = SCENE_MAINMENU;
	}
}

//
// MAIN UPDATE
//

GameState *game_init() {
	GameState *state = (GameState *) malloc(sizeof(GameState));
	copyzero((void *)state, sizeof(GameState));

	state->player = game_init_player(state);
	state->scene = SCENE_MAINMENU;
	state->next_scene = 0;

	return state;
}

void game_update(GameState *state) {
	switch(state->scene) {
		case SCENE_MAINMENU:
			game_update_mainmenu(state);
			break;
		case SCENE_WAVE:
			game_update_wave(state);
			break;
		case SCENE_GAMEOVER:
			game_update_gameover(state);
			break;
		case SCENE_BOSS:
			game_update_boss(state);
			break;
		default:
			cls(1);
	}

	// Change scene?
	if (state->next_scene != 0) {
		state->scene = state->next_scene;
		state->next_scene = 0;
		state->scene_ticks = 0;
	} else {
		state->scene_ticks++;
	}
}
