/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_jz.h"

#include "jz_common.h"
#include "jz_mcrypt.h"
#include "jz_data.h"
#include "zlib.h"

/* If you declare any globals in php_jz.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(jz)
*/

/* True global resources - no need for thread safety here */
static int le_jz;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("jz.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_jz_globals, jz_globals)
    STD_PHP_INI_ENTRY("jz.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_jz_globals, jz_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_jz_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_jz_init_globals(zend_jz_globals *jz_globals)
{
	jz_globals->global_value = 0;
	jz_globals->global_string = NULL;
}
*/

ZEND_BEGIN_ARG_INFO_EX(arg_info_jz_encrypt, 0, 0, 2)
	ZEND_ARG_INFO(0, encrypt_str)
	ZEND_ARG_INFO(0, encrypt_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_info_jz_decrypt, 0, 0, 2)
	ZEND_ARG_INFO(0, decrypt_str)
	ZEND_ARG_INFO(0, decrypt_key)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jz)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

	JZ_STARTUP(data);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(jz)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(jz)
{
#if defined(COMPILE_DL_JZ) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(jz)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(jz)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "jz support", "enabled");
	php_info_print_table_row(2, "version", PHP_JZ_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

PHP_FUNCTION(jz_version)
{
	RETURN_STRING(PHP_JZ_VERSION);
}

PHP_FUNCTION(jz_encrypt)
{
	char *encrypt_data = NULL, *encrypt_key=NULL;
	size_t encrypt_data_len, encrypt_key_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &encrypt_data, &encrypt_data_len, &encrypt_key, &encrypt_key_len) == FAILURE) {
		RETURN_NULL();
	}

	if (0 == encrypt_data_len
		|| 0 == encrypt_key_len
		|| encrypt_key_len % 16 != 0) {
		RETURN_NULL();
	}

	//crc32
	unsigned long crc;
	crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (unsigned char *)encrypt_data, encrypt_data_len);

	//check to gzcompress
	int gzip = encrypt_data_len > 500 ? 1 : 0;

	char *aes_data = NULL;
	size_t aes_data_len = encrypt_data_len;

	if (gzip) {
		size_t g_l = compressBound(aes_data_len);
		aes_data = emalloc(g_l);
		if (!aes_data || compress((unsigned char *)aes_data, &aes_data_len, (unsigned char *)encrypt_data, encrypt_data_len) != Z_OK) {
			if (aes_data) {
				efree(aes_data);
			}
			RETURN_NULL();
		}
	} else {
		aes_data = estrndup(encrypt_data, aes_data_len);
	}

	if (!aes_data) {
		RETURN_NULL();
	}

	//header
	char header[33];
	size_t i = sprintf(header, "o,%d,%lu,%d,%u,",encrypt_data_len, crc, gzip, aes_data_len);
	for (; i < 32; i++) {
		header[i] = ' ';
	}
	header[32] = '\0';

	//all data
	size_t aes_origin_buf_len = 32 + aes_data_len;
	char * aes_origin_buf = (char *)emalloc(aes_origin_buf_len);
	memset(aes_origin_buf, 0, aes_origin_buf_len);
	//copy header
	memcpy(aes_origin_buf, header, 32);
	//copy data
	memcpy(aes_origin_buf + 32, aes_data, aes_data_len);
	efree(aes_data);

	size_t out_len = 0;
	char *out_data = jz_crypt(encrypt_key, encrypt_key_len, NULL, 0, aes_origin_buf, aes_origin_buf_len, JZ_MCRYPT_TO_ENCRYPT, &out_len);

	efree(aes_origin_buf);
	if (!out_data || 0 == out_len) {
		RETURN_NULL();
	}

	RETVAL_STRINGL(out_data, out_len);
	free(out_data);
}


PHP_FUNCTION(jz_decrypt)
{
	char *decrypt_data = NULL, *decrypt_key=NULL;
	size_t decrypt_data_len, decrypt_key_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &decrypt_data, &decrypt_data_len, &decrypt_key, &decrypt_key_len) == FAILURE) {
		RETURN_NULL();
	}

	if (decrypt_data_len < 48
		|| decrypt_data_len % 16 != 0
		|| 0 == decrypt_key_len
		|| decrypt_key_len % 16 != 0) {
		RETURN_NULL();
	}

	//iv
	char aes_iv[16];
	memcpy(aes_iv, decrypt_data, 16);

	size_t aes_origin_buf_len = 0;
	char *aes_origin_buf = jz_crypt(decrypt_key, decrypt_key_len, aes_iv, 16, decrypt_data + 16, decrypt_data_len - 16, JZ_MCRYPT_TO_DECRYPT, &aes_origin_buf_len);
	if (!aes_origin_buf || 0 == aes_origin_buf_len) {
		RETURN_NULL();
	}

	char header[33];
	memcpy(header, aes_origin_buf, 32);
	header[32] = '\0';

	char* header_flags[5];
	char* flags = strtok(header, ",");
	int i;
	for (i = 0; i < 5; i++) {
		header_flags[i] = flags;
		flags = strtok(NULL, ",");
	}

	if (strcmp(header_flags[0], "o") != 0) {
		free(aes_origin_buf);
		RETURN_NULL();
	}

	int is_zip = 0;
	if (atoi(header_flags[3]) == 1) {
		is_zip = 1;
	}

	aes_origin_buf_len = strtoul(header_flags[4], NULL, 10);
	size_t origin_data_size = 0 == is_zip ? strtoul(header_flags[4], NULL, 10) : strtoul(header_flags[1], NULL, 10);
	char *origin_data = NULL;

	if (is_zip) {
		origin_data = emalloc(origin_data_size);
		if (!origin_data || uncompress((unsigned char *)origin_data, &origin_data_size, (const unsigned char *)(aes_origin_buf + 32), aes_origin_buf_len) != Z_OK) {
			if (origin_data) {
				efree(origin_data);
			}
			RETURN_NULL();
		}
	} else {
		origin_data = estrndup(aes_origin_buf + 32, origin_data_size);
	}

	free(aes_origin_buf);

	if (!origin_data) {
		RETURN_NULL();
	}

	//crc32
	unsigned long crc;
	crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (unsigned char *)origin_data, origin_data_size);

	if (crc != strtoul(header_flags[2], NULL, 10)) {
		efree(origin_data);
		RETURN_NULL();
	}

	RETVAL_STRINGL(origin_data, origin_data_size);
	efree(origin_data);
}

/* {{{ jz_functions[]
 *
 * Every user visible function must have an entry in jz_functions[].
 */
const zend_function_entry jz_functions[] = {
	PHP_FE(jz_version, NULL)
	PHP_FE(jz_encrypt, arg_info_jz_encrypt)
	PHP_FE(jz_decrypt, arg_info_jz_decrypt)
	PHP_FE_END	/* Must be the last line in jz_functions[] */
};
/* }}} */

/* {{{ jz_module_entry
 */
zend_module_entry jz_module_entry = {
	STANDARD_MODULE_HEADER,
	"jz",
	jz_functions,
	PHP_MINIT(jz),
	PHP_MSHUTDOWN(jz),
	PHP_RINIT(jz),
	PHP_RSHUTDOWN(jz),
	PHP_MINFO(jz),
	PHP_JZ_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_JZ
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(jz)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
