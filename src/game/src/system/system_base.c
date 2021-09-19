
#include "system.h"

// Since this program does not use the the c stdlib
// this is a very simple replacement for malloc.
// When asked for n bytes of memory, it returns the next
// n bytes from the heap. The next request will return the
// following n bytes, etc. Memory is never reused/freed.
// The heap size is never changed.

extern uint32_t __heap_base;
static unsigned int current_heap_pointer = (uint32_t)&__heap_base;

void* malloc(unsigned long n) {
	// Make memory 4-byte-aligned
	if (n % 4 != 0)
		n += 4-(n%4);

	uint32_t tmp = current_heap_pointer;
	current_heap_pointer += n;
	return (void *) tmp;
}

void free(void* mem) {
	// Memory is not freed
}


// This program exposes memory locations for the current time
// and a random seed. This values are set from JavaScript after initialization.
static uint32_t *_time_memory;
static uint32_t *_seed_memory;


void system_base_init() {
	_time_memory = (uint32_t *) malloc(4);
	_seed_memory = (uint32_t *) malloc(4);
}

uint32_t time() {
	return *_time_memory;
}

// xorshift PRNG, taken from Marsaglia
uint32_t randn(uint32_t n) {
	uint32_t x = *_seed_memory;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*_seed_memory = x;
	return x % n;
}

void memcpy_8(void *from, void *to, uint32_t len) {
	uint64_t *src = from;
	uint64_t *dst = to;
	len /= 8;
	while(len) {
		*dst++ = *src++;
		len--;
	}
}

void copy(void *from, void *to, unsigned long len) {
	if (len % 8 == 0) {
		memcpy_8(from, to, len);
	}
	uint8_t *src = from;
	uint8_t *dst = to;
	while(len) {
		*dst++ = *src++;
		len--;
	}
}

void *memset(void *dst, int v, unsigned long len) {
	if (len % 8 == 0) {
		uint64_t *_dst = dst;
		len /= 8;
		while(len--) *_dst++ = v;
	} else if (len % 4 == 0) {
		uint32_t *_dst = dst;
		len /= 4;
		while(len--) *_dst++ = v;
	} else {
		uint8_t *_dst = dst;
		while(len--) *_dst++ = v;
	}
	return dst;
}

void copyzero(void *dst, unsigned long len) {
	memset(dst, 0, len);
}


EXPORT void *_get_time_ref() {
	return _time_memory;
}

EXPORT void *_get_seed_ref() {
	return _seed_memory;
}

static void reverse(char *start, char *end) {
	char tmp;
	while(start < end) {
		tmp = *end;
		*end = *start;
		*start = tmp;

		start++;
		end--;
	}
}

char *put_string(const char *str, char *dst, int *maxlen) {
	if (*maxlen < 1) return dst;

	while(*maxlen > 1 && *str) {
		*dst++ = *str++;
		*maxlen = (*maxlen) - 1;
	}
	*dst = 0;

	return dst;
}

char *put_number(int n, char *dst, int *maxlen) {
	if (*maxlen < 1) return dst;
	if (n < 0) {
		*dst++ = '-';
		*maxlen = (*maxlen) - 1;
		n *= -1;
	} else if(n == 0 && *maxlen > 1) {
		*dst++ = '0';
		*dst = 0;
		*maxlen = (*maxlen) - 2;
		return dst;
	}

	char *number_start = dst;
	while(*maxlen > 1 && n > 0) {
		*dst++ = (n%10) + '0';
		n /= 10;
		*maxlen = (*maxlen) - 1;
	}

	*dst = 0;

	reverse(number_start, dst-1);

	return dst;
}


unsigned long strlen(const char *s) {
	int i = 0;
	while(*s++) i++;
	return i;
}
