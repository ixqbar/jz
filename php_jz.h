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

#ifndef PHP_JZ_H
#define PHP_JZ_H

extern zend_module_entry jz_module_entry;
#define phpext_jz_ptr &jz_module_entry

#ifdef PHP_WIN32
#	define PHP_JZ_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_JZ_API __attribute__ ((visibility("default")))
#else
#	define PHP_JZ_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(jz);
PHP_MSHUTDOWN_FUNCTION(jz);
PHP_RINIT_FUNCTION(jz);
PHP_RSHUTDOWN_FUNCTION(jz);
PHP_MINFO_FUNCTION(jz);

PHP_FUNCTION(jz_version);
PHP_FUNCTION(jz_encrypt);
PHP_FUNCTION(jz_decrypt);
PHP_FUNCTION(jz_trace);
PHP_FUNCTION(jz_rc4);

#ifdef JZ_USE_JIEBA
	#include "jieba.h"
	ZEND_BEGIN_MODULE_GLOBALS(jz)
		zend_bool enable_jieba;
		Extractor extractor;
		Jieba jieba;
		char *dict_path;
	ZEND_END_MODULE_GLOBALS(jz)
#endif

#define JZ_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(jz, v)

#if defined(ZTS) && defined(COMPILE_DL_JZ)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#endif	/* PHP_JZ_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
