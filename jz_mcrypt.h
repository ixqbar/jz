/*
 * mcrypt.h
 *
 *  Created on: Dec 31, 2015
 *      Author: xingqiba
 */

#ifndef LIB_MCRYPT_H_
#define LIB_MCRYPT_H_

#define JZ_MCRYPT_TO_DECRYPT 0
#define JZ_MCRYPT_TO_ENCRYPT 1

char * jz_crypt(const char *key,
				   size_t key_len,
				   char *iv,
				   size_t iv_len,
				   const char *data,
				   size_t data_len,
				   int action,
				   size_t *out_len);

#endif /* LIB_MCRYPT_H_ */
