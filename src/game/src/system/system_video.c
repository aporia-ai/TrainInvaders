
#include "system.h"

static uint32_t *_screen_memory;
static uint8_t *_sprite_memory;
static uint16_t _sprite_memory_width;
static uint16_t _sprite_memory_height;
static const uint32_t *_palette;
static uint32_t _current_palette[16];
static const uint32_t _default_palette[16] = { 0xff000000,0xff555555,0xffaaaaaa,0xffffffff,0xff5555ff,0xff0000aa,0xff0055aa,0xff55ffff,0xff55ff55,0xff00aa00,0xffffff55,0xffaaaa00,0xffff55ff,0xffff5555,0xffaa00aa,0xffaa0000};

static int clip_x = 0;
static int clip_y = 0;
static int clip_width = SCREEN_WIDTH;
static int clip_height = SCREEN_HEIGHT;

void system_video_init(SystemConfiguration *configuration) {
	_screen_memory = (uint32_t *) malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
	_sprite_memory = configuration->spritemem;
	_sprite_memory_width = configuration->sprite_w;
	_sprite_memory_height = configuration->sprite_h;

	// We keep a copy of the pallet to allow color translations
	_palette = configuration->palette != NULL ? configuration->palette : _default_palette;
	copy((void *)_palette,(void *)_current_palette, 64);
}

EXPORT void *_get_screen_ref() {
	return _screen_memory;
}

// Drawing functions

static uint8_t _transparent_color = 0;

inline static void set_pixel(int x, int y, int c) {
	_screen_memory[y * SCREEN_WIDTH + x] = _current_palette[c];
}

void pset(int x, int y, uint8_t c) {
	if (c == _transparent_color) return;
	if (x < 0) return;
	if (x >= SCREEN_WIDTH) return;
	if (y < 0) return;
	if (y >= SCREEN_HEIGHT) return;
	set_pixel(x, y, c);
}

void cls(uint8_t pc) {
	uint32_t c = _current_palette[pc];
	uint64_t two = c | ((uint64_t)c<<32);
	uint64_t *screen_ptr = (uint64_t *)_screen_memory;
	uint64_t *screen_end_ptr = screen_ptr + (SCREEN_WIDTH*SCREEN_HEIGHT)/2;
	while(screen_ptr < screen_end_ptr) *screen_ptr++ = two;

	clip_x = 0;
	clip_y = 0;
	clip_width = SCREEN_WIDTH;
	clip_height = SCREEN_HEIGHT;
}

void palt(uint8_t c) {
	_transparent_color = c;
}

void rectfill(int x1, int y1, int w, int h, uint8_t pc) {
	uint32_t c = _current_palette[pc];
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	int x2 = x1 + w;
	int y2 = y1 + h;
	if (x2 > SCREEN_WIDTH) x2 = SCREEN_WIDTH;
	if (y2 > SCREEN_HEIGHT) y2 = SCREEN_HEIGHT;

	int dx = x2 - x1;
	int dy = y2 - y1;
	uint32_t *screen_ptr = &_screen_memory[SCREEN_WIDTH*y1+x1];
	for(int y = y1; y < y2; y++) {
		for(int x = x1; x < x2; x++) {
			*screen_ptr++ = c;
		}
		screen_ptr -= dx;
		screen_ptr += SCREEN_WIDTH;
	}
}

typedef struct {
	int dst_x;
	int dst_y;
	int src_x;
	int src_y;
	int w;
	int h;
} BlitInfo;

// Extract color for pixel x,y. Since the sprite map
// contains 2 pixels per byte (remember: 16 colors),
// we need some bitshifting here.
inline static uint8_t get_sprite_pixel(int x, int y) {
	uint8_t col = _sprite_memory[y * _sprite_memory_width/2 + x/2];
	return x&1 ? col&0x0F : col>>4;
}

void blit(BlitInfo *info) {
	uint8_t col;

	for(int y = 0; y < info->h; y++) {
		for(int x = 0; x < info->w; x++) {
			col = get_sprite_pixel(info->src_x+x, info->src_y+y);
			if (col != _transparent_color)
				set_pixel(info->dst_x+x, info->dst_y+y, col);
		}
	}
}

void spr(int n, int w, int h, int x, int y) {
	// Check boundaries and continue with blit(...).
	BlitInfo info;

	info.src_x = 8 * (n % 16);
	info.src_y = 8 * (n / 16);
	info.w = w*8;
	info.h = h*8;

	if (x < 0) {
		info.dst_x = 0;
		info.src_x += -x;
		info.w -= -x;
	} else {
		info.dst_x = x;
	}

	if (y < 0) {
		info.dst_y = 0;
		info.src_y += -y;
		info.h -= -y;
	} else {
		info.dst_y = y;
	}

	if (info.dst_x + info.w > SCREEN_WIDTH) {
		info.w = SCREEN_WIDTH - info.dst_x;
	}

	if (info.dst_y + info.h > SCREEN_HEIGHT) {
		info.h = SCREEN_HEIGHT - info.dst_y;
	}


	blit(&info);
}


// Font

// We have 96 letters (ASCII 32-127). Each letter has size 3x5.
// Each two byte contain data for a letter (15 pixel). The most
// significant bit is unused.
const static uint16_t font_data[96] = {
	0x0, 0x4904, 0xb400, 0x0, 0x0, 0x0, 0x0, 0x9000, 0x7246, 0xc49c, 0xb55a, 0xba0, 0x48, 0x380, 0x8, 0x2548,
	0xf6de, 0x4924, 0xe7ce, 0xe79e, 0x9792, 0xf39e, 0x93de, 0xe492, 0xf7de, 0xf79e, 0x820, 0x828, 0x2a22, 0x1c70, 0x88a8, 0xf482,
	0x76c6, 0xf7da, 0xd75e, 0xf24e, 0xd6dc, 0xf3ce, 0xf348, 0xf25e, 0xb7da, 0xe92e, 0xe4de, 0xb75a, 0x924e, 0xfeda, 0xd6da, 0x76dc,
	0xf7c8, 0x56a2, 0xf75a, 0xf39e, 0xe924, 0xb6de, 0xb6d4, 0xb6fe, 0xb55a, 0xb6a4, 0xe54e, 0xf24e, 0x8922, 0xe49e, 0x5400, 0xe,
	0x8800, 0xf7da, 0xd75e, 0xf24e, 0xd6dc, 0xf3ce, 0xf348, 0xf25e, 0xb7da, 0xe92e, 0xe4de, 0xb75a, 0x924e, 0xfeda, 0xd6da, 0x76dc,
	0xf7c8, 0x56a2, 0xf75a, 0xf39e, 0xe924, 0xb6de, 0xb6d4, 0xb6fe, 0xb55a, 0xb6a4, 0xe54e, 0x6a26, 0x4924, 0xc8ac, 0x7c0, 0x100,
};

static void blit_letter(char l, int x, int y, uint32_t color) {
	if (l <= 32 || l > 127) return;
	l -= 32;

	uint32_t *dst = _get_screen_ref();
	uint16_t letter_data = font_data[l];
	int bitMask = 32768; // = 2^15

	int y_limit = y+5 >= SCREEN_HEIGHT ? SCREEN_HEIGHT-1 : y+5;
	int x_limit = x+3 >= SCREEN_WIDTH ? SCREEN_WIDTH-1 : x+3;
	for(int dst_y = y; dst_y < y_limit; dst_y++) {
		for(int dst_x = x; dst_x < x_limit; dst_x++) {
			if(letter_data&bitMask) dst[SCREEN_WIDTH*dst_y+dst_x] = color;
			bitMask >>= 1;
		}
	}
}

void text(const char *text, int x, int y, uint8_t c) {
	if(y >= SCREEN_HEIGHT) return;

	for(int i = 0; text[i]; i++) {
		if (x >= SCREEN_WIDTH) return;
		if (x < 0) continue;
		blit_letter(text[i], x, y, _current_palette[c]);
		x += 4;
	}
}
