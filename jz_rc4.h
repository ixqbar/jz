/*
 * jz_rc4.h
 *
 */

#ifndef EXT_JZ_JZ_RC4_H_
#define EXT_JZ_JZ_RC4_H_


#define JZ_RC4_STATE_LEN 256

void jz_rc4_init(unsigned char *state, char *key, unsigned int len);
void jz_rc4_crypt(unsigned char *state, char *out, unsigned int len);

#endif /* EXT_JZ_JZ_RC4_H_ */
