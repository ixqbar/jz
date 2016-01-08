/*
 * mcrypt.c
 *
 * more http://mcrypt.hellug.gr/lib/mcrypt.3.html
 *
 *  Created on: Dec 31, 2015
 *      Author: xingqiba
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#include "mcrypt.h"
#include "jz_mcrypt.h"

static int jz_mcrypt_ensure_valid_key_size(MCRYPT td, int key_size) {
	int max_key_size = mcrypt_enc_get_key_size(td);
	if (key_size <= 0 || key_size > max_key_size) {
		return 1;
	}

	int key_size_count, i;
	int *key_sizes = mcrypt_enc_get_supported_key_sizes(td, &key_size_count);
	if (key_size_count == 0) {
		return 0;
	}

	for (i = 0; i < key_size_count; i++) {
		if (key_sizes[i] == key_size) {
			mcrypt_free(key_sizes);
			return 0;
		}
	}

	mcrypt_free(key_sizes);

	return 1;
}

static int jz_mcrypt_ensure_valid_iv(MCRYPT td, const char *iv, int iv_size) {
	if (mcrypt_enc_mode_has_iv(td) == 1) {
		int expected_iv_size = mcrypt_enc_get_iv_size(td);
		if (expected_iv_size == 0) {
			return 0;
		}

		if (!iv || iv_size != expected_iv_size) {
			return 1;
		}
	}

	return 0;
}

char * jz_crypt(const char *key,
				   size_t key_len,
				   char *iv,
				   size_t iv_len,
				   const char *data,
				   size_t data_len,
				   int action,
				   size_t *out_len) {
	MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
	if (td == MCRYPT_FAILED) {
		return NULL;
	}

//	if (jz_mcrypt_ensure_valid_key_size(td, (int)key_len)) {
//		mcrypt_module_close(td);
//		return NULL;
//	}

	int to_free_iv = 0;
	if (action == JZ_MCRYPT_TO_ENCRYPT && !iv) {
		iv_len = mcrypt_enc_get_iv_size(td);
		iv = malloc(iv_len);
		memset(iv, 0, iv_len);
		int n = iv_len;
		while (n) {
			iv[--n] = (char) (rand() % 255);
		}
		to_free_iv = 1;
	} else {
//		if (jz_mcrypt_ensure_valid_iv(td, iv, (int)iv_len)) {
//			mcrypt_module_close(td);
//			return NULL;
//		}
	}

	//填充data
	if (mcrypt_enc_is_block_mode(td) == 1) {
		int block_size = mcrypt_enc_get_block_size(td);
		(*out_len) = ((((int)data_len - 1) / block_size) + 1) * block_size;
	} else {
		(*out_len) = data_len;
	}

	//加密时返回out_data增加iv
	if (action == JZ_MCRYPT_TO_ENCRYPT) {
		(*out_len) += iv_len;
	}

	char *out_data = malloc((*out_len) + 1);
	memset(out_data, 0, (*out_len) + 1);

	if (mcrypt_generic_init(td, (void *)key, (int)key_len, (void *)iv) == 0) {
		if (action == JZ_MCRYPT_TO_ENCRYPT) {
			memcpy(out_data, iv, iv_len);
			memcpy(out_data + iv_len, data, data_len);
			mcrypt_generic(td, out_data + iv_len, (int)(*out_len) - iv_len);
		} else {
			memcpy(out_data, data, data_len);
			mdecrypt_generic(td, out_data, (int)(*out_len));
		}
	}

	mcrypt_generic_end(td);
	if (to_free_iv) {
		free(iv);
	}

	return out_data;
}


