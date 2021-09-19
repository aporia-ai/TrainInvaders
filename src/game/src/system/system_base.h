
#pragma once

#define NULL ((void *)0)
#include <stdint.h>

#define EXPORT __attribute__ ((visibility ("default")))

// malloc return n bytes of free memory that can be used.
void* malloc(unsigned long n);

// free does nothing in this implementation
void free(void* mem);

// system_base_init initializes memory locations
// for time and random-seed
void system_base_init();

// access memory locations from JavaScript
void *_get_time_ref();
void *_get_seed_ref();

// randn return a random number in the interval [0,n)
uint32_t randn(uint32_t n);

// time return the time since the start of the program in ms.
uint32_t time();

// Methods for memory initialization
void copy(void *from, void *to, unsigned long len);
void copyzero(void *dst, unsigned long len);
void *memset(void *dst, int v, unsigned long len);

// String manipulation

// put_string copies bytes from str to dst, but maxlen bytes at most.
// dst is null-terminated and a pointer to the last byte in dst is returned.
char *put_string(const char *str, char *dst, int *maxlen);

// put_number converts n to a string. At most maxlen bytes are written.
// dst is null-terminated and a pointer to the last byte in dst is returned.
char *put_number(int n, char *dst, int *maxlen);

unsigned long strlen(const char *s);
