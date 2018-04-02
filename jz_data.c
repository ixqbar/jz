/*
 * jz_data.c
 *
 *  Created on: Jan 8, 2016
 *      Author: xingqiba
 */

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "jz_data.h"

#define	JZ_DATA_PROPERT_NAME "_config"
#define JZ_DATA_PROPERT_NAME_READONLY "_readonly"

zend_class_entry *jz_data_class_entry;

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

ZEND_BEGIN_ARG_INFO_EX(jz_data_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jz_data_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jz_data_rget_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jz_data_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jz_data_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jz_data_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

zval *jz_data_instance(zval *this_ptr, zval *values, zval *readonly)
{
	switch (Z_TYPE_P(values)) {
		case IS_ARRAY:
			if (Z_ISUNDEF_P(this_ptr)) {
				object_init_ex(this_ptr, jz_data_class_entry);
			}
			zend_update_property(jz_data_class_entry, this_ptr, ZEND_STRL(JZ_DATA_PROPERT_NAME), values);
			if (readonly) {
				convert_to_boolean(readonly);
				zend_update_property_bool(jz_data_class_entry, this_ptr, ZEND_STRL(JZ_DATA_PROPERT_NAME_READONLY), Z_TYPE_P(readonly) == IS_TRUE ? 1 : 0);
			}
			return this_ptr;
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid parameters provided, must be an array");
			return NULL;
	}
}

zval *jz_data_format(zval *instance, zval *pzval, zval *rv)
{
	zval *readonly, *ret;
	readonly = zend_read_property(jz_data_class_entry, instance, ZEND_STRL(JZ_DATA_PROPERT_NAME_READONLY), 1, NULL);
	ret = jz_data_instance(rv, pzval, readonly);
	return ret;
}

PHP_METHOD(jz_data, __construct) {
	zval *values, *readonly = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &values, &readonly) == FAILURE) {
		zval prop;

		array_init(&prop);
		zend_update_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), &prop);
		zval_ptr_dtor(&prop);

		return;
	}

	jz_data_instance(getThis(), values, readonly);
}

PHP_METHOD(jz_data, get) {
	zval *ret, *pzval, *default_value = NULL;
	zend_string *name = NULL;
	uint32_t num_args = ZEND_NUM_ARGS();

	if (zend_parse_parameters(num_args, "|Sz", &name, &default_value) == FAILURE) {
		return;
	}

	if (!name) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *properties;
		HashTable *hash;
		zend_long lval;
		double dval;

		properties = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
		hash  = Z_ARRVAL_P(properties);

		if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, &dval, 0) != IS_LONG) {
			if ((pzval = zend_hash_find(hash, name)) == NULL) {
				goto GET_RETURN;
			}
		} else {
			if ((pzval = zend_hash_index_find(hash, lval)) == NULL) {
				goto GET_RETURN;
			}
		}

		if (Z_TYPE_P(pzval) == IS_ARRAY) {
			zval rv = {{0}};
			if ((ret = jz_data_format(getThis(), pzval, &rv))) {
				RETURN_ZVAL(ret, 1, 1);
			} else {
				RETURN_NULL();
			}
		} else {
			RETURN_ZVAL(pzval, 1, 0);
		}
	}

GET_RETURN: {
	if (1 == num_args || Z_TYPE_P(default_value) == IS_NULL) {
		RETURN_NULL();
	} else {
		if (Z_TYPE_P(default_value) == IS_ARRAY) {
			zval rv = {{0}};
			if ((ret = jz_data_format(getThis(), default_value, &rv))) {
				RETURN_ZVAL(ret, 1, 1);
			} else {
				RETURN_NULL();
			}
		}

		RETURN_ZVAL(default_value, 1, 0);
	}
}
}

PHP_METHOD(jz_data, to_array) {
	zval *properties = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	RETURN_ZVAL(properties, 1, 0);
}

PHP_METHOD(jz_data, set) {
	zval *readonly = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME_READONLY), 1, NULL);

	if (Z_TYPE_P(readonly) == IS_FALSE) {
		zend_string *name;
		zval *value, *props;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}

		props = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
		if (zend_hash_update(Z_ARRVAL_P(props), name, value) != NULL) {
			Z_TRY_ADDREF_P(value);
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}

PHP_METHOD(jz_data, __isset) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name));
	}
}

PHP_METHOD(jz_data, offsetUnset) {
	zval *readonly = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME_READONLY), 1, NULL);

	if (Z_TYPE_P(readonly) == IS_FALSE) {
		zval *props;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
			return;
		}

		props = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
		if (UNEXPECTED(Z_TYPE_P(props) != IS_ARRAY)) {
			RETURN_FALSE;
		}
		if (zend_hash_del(Z_ARRVAL_P(props), name) == SUCCESS) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}

PHP_METHOD(jz_data, count) {
	zval *prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop)));
}

PHP_METHOD(jz_data, rewind) {
	zval *prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(prop));
}

PHP_METHOD(jz_data, current) {
	zval *prop, *pzval, *ret;

	prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	if ((pzval = zend_hash_get_current_data(Z_ARRVAL_P(prop))) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(pzval) == IS_ARRAY) {
		zval rv = {{0}};
		if ((ret = jz_data_format(getThis(), pzval, &rv))) {
			RETURN_ZVAL(ret, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(pzval, 1, 0);
	}
}

PHP_METHOD(jz_data, key) {
	zval *prop;
	zend_string *string;
	zend_ulong index;

	prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	zend_hash_get_current_key(Z_ARRVAL_P(prop), &string, &index);
	switch(zend_hash_get_current_key_type(Z_ARRVAL_P(prop))) {
		case HASH_KEY_IS_LONG:
			RETURN_LONG(index);
			break;
		case HASH_KEY_IS_STRING:
			RETURN_STR(zend_string_copy(string));
			break;
		default:
			RETURN_FALSE;
	}
}

PHP_METHOD(jz_data, next) {
	zval *prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	zend_hash_move_forward(Z_ARRVAL_P(prop));
	RETURN_TRUE;
}

PHP_METHOD(jz_data, valid) {
	zval *prop = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME), 1, NULL);
	RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(prop)) == SUCCESS);
}

PHP_METHOD(jz_data, readonly) {
	zval *readonly = zend_read_property(jz_data_class_entry, getThis(), ZEND_STRL(JZ_DATA_PROPERT_NAME_READONLY), 1, NULL);
	RETURN_BOOL(Z_LVAL_P(readonly));
}

PHP_METHOD(jz_data, __destruct) {
}

PHP_METHOD(jz_data, __clone) {
}

zend_function_entry jz_data_methods[] = {
	PHP_ME(jz_data, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(jz_data, __destruct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_ME(jz_data, __isset, jz_data_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, get, jz_data_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, set, jz_data_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, count, jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, offsetUnset, jz_data_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, rewind, jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, current, jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, next,	jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, valid, jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, key, jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, readonly,	jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(jz_data, to_array, jz_data_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(jz_data, __set, set, jz_data_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(jz_data, __get, get, jz_data_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(jz_data, offsetGet, get, jz_data_rget_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(jz_data, offsetExists, __isset, jz_data_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(jz_data, offsetSet, set, jz_data_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

JZ_STARTUP_FUNCTION(data)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "JZ_Data", jz_data_methods);
	jz_data_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
	zend_class_implements(jz_data_class_entry, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
	zend_class_implements(jz_data_class_entry, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#else
	zend_class_implements(jz_data_class_entry, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

	jz_data_class_entry->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(jz_data_class_entry, ZEND_STRL(JZ_DATA_PROPERT_NAME), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(jz_data_class_entry, ZEND_STRL(JZ_DATA_PROPERT_NAME_READONLY), 0, ZEND_ACC_PROTECTED TSRMLS_CC);

	return SUCCESS;
}
