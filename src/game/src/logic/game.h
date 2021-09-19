#pragma once
#include "system.h"

#define STATUS_BAR_HEIGHT LETTER_HEIGHT + 4 + 1
#define MAX_OBJECTS 512
typedef struct _GameState GameState;

// The Game has 4 scenes
// We have the same map in JS
#define SCENE_MAINMENU 1
#define SCENE_WAVE 2
#define SCENE_BOSS 3
#define SCENE_GAMEOVER 4

#include "game_object.h"
#include "game_player.h"
#include "game_alien.h"

struct _GameState {
	struct _GameObject objects[MAX_OBJECTS];
	int score;
	int wave; // <- correlated with difficulty
	int alien_counter; // <- the boss-ufo will show up, if this is 0
	int scene;
	int next_scene;
	uint64_t scene_ticks;
	GameObject *player;
};

GameState *game_init();
void game_update(GameState *state);
