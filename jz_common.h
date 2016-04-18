/*
 * jz_common.h
 *
 *  Created on: Jan 8, 2016
 *      Author: xingqiba
 */

#ifndef EXT_JZ_JZ_COMMON_H_
#define EXT_JZ_JZ_COMMON_H_

//版本
#define PHP_JZ_VERSION "2.0.0"

#define JZ_STARTUP_FUNCTION(module)  ZEND_MINIT_FUNCTION(jz_##module)
#define JZ_RINIT_FUNCTION(module)	 ZEND_RINIT_FUNCTION(jz_##module)
#define JZ_STARTUP(module)	 		 ZEND_MODULE_STARTUP_N(jz_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define JZ_SHUTDOWN_FUNCTION(module) ZEND_MSHUTDOWN_FUNCTION(jz_##module)
#define JZ_SHUTDOWN(module)	 	     ZEND_MODULE_SHUTDOWN_N(jz_##module)(INIT_FUNC_ARGS_PASSTHRU)

#endif /* EXT_JZ_JZ_COMMON_H_ */
