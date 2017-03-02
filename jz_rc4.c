/*
 * jz_rc4.c
 *
 */

#include "jz_rc4.h"

void jz_rc4_init(unsigned char *state, char *key, unsigned int len) {
	int i, j = 0, t;

	for (i = 0; i < JZ_RC4_STATE_LEN; ++i) {
		state[i] = i;
	}

	for (i = 0; i < JZ_RC4_STATE_LEN; ++i) {
		j = (j + state[i] + key[i % len]) % JZ_RC4_STATE_LEN;
		t = state[i];
		state[i] = state[j];
		state[j] = t;
	}
}

void jz_rc4_crypt(unsigned char *state, char *out, unsigned int len) {
	int i = 0, j = 0, x, t;

	for (x = 0; x < len; ++x) {
		i = (i + 1) % JZ_RC4_STATE_LEN;
		j = (j + state[i]) % JZ_RC4_STATE_LEN;
		t = state[i];
		state[i] = state[j];
		state[j] = t;
		out[x] ^= state[(state[i] + state[j]) % JZ_RC4_STATE_LEN];
	}
}
