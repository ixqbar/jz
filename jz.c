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
#include "jz_data.h"
#include "jz_buffer.h"
#include "jz_rc4.h"

static int le_jz;

ZEND_BEGIN_ARG_INFO_EX(arg_info_jz_rc4, 0, 0, 2)
	ZEND_ARG_INFO(0, encrypt_str)
	ZEND_ARG_INFO(0, encrypt_key)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(jz)
{
	JZ_STARTUP(data);
	JZ_STARTUP(buffer);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(jz)
{
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

#ifdef JZ_USE_JIEBA
	DISPLAY_INI_ENTRIES();
#endif
}
/* }}} */

PHP_FUNCTION(jz_version)
{
	RETURN_STRING(PHP_JZ_VERSION);
}

PHP_FUNCTION(jz_rc4)
{
	char *encrypt_data = NULL, *encrypt_key=NULL;
	size_t encrypt_data_len, encrypt_key_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &encrypt_data, &encrypt_data_len, &encrypt_key, &encrypt_key_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (0 == encrypt_data_len
		|| 0 == encrypt_key_len) {
		RETURN_FALSE;
	}

	unsigned char state[JZ_RC4_STATE_LEN];
	jz_rc4_init(state, encrypt_key, encrypt_key_len);
	jz_rc4_crypt(state, encrypt_data, encrypt_data_len);

	RETURN_STRINGL(encrypt_data, encrypt_data_len);
}

/* {{{ jz_functions[]
 *
 * Every user visible function must have an entry in jz_functions[].
 */
const zend_function_entry jz_functions[] = {
	PHP_FE(jz_version, NULL)
	PHP_FE(jz_rc4, arg_info_jz_rc4)
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
