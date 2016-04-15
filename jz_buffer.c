/*
 * jz_buffer.c
 *
 */

#include "php.h"
#include "jz_buffer.h"

static int le_jz_buf_cls;
zend_class_entry *jz_buffer_class_entry;

typedef struct
{
    size_t length;
    size_t size;
    char *str;
} JZBuffer;

#define JZ_STRING_BUFFER_DEFAULT 1024
#define JZ_STRING_BUFFER_MAXLEN  (1024 * 1024)

static void jz_free_buff_object(zend_resource *res) {
	JZBuffer *buffer = (JZBuffer *)res->ptr;
	if (buffer) {
		efree(buffer->str);
		efree(buffer);
	}
}

static JZBuffer * jz_get_buffer_object(zval *object) {
	zval *tmp;
	JZBuffer *buffer = NULL;
	if ((tmp = zend_hash_str_find(Z_OBJPROP_P(object), "buff", sizeof("buff") - 1)) != NULL) {
		buffer = (JZBuffer *)zend_fetch_resource_ex(tmp, "buff", le_jz_buf_cls);
	} else {
		php_error_docref(NULL, E_ERROR, "Can not fetch buff object");
	}

	return buffer;
}

static void jz_add_buffer_object(zval *object, JZBuffer * buffer) {
	add_property_resource(object, "buff", zend_register_resource(buffer, le_jz_buf_cls));
}


PHP_METHOD(jz_buffer, __construct) {
	long size = JZ_STRING_BUFFER_DEFAULT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &size) == FAILURE) {
		php_error_docref(NULL, E_ERROR, "buffer size invalid");
		RETURN_FALSE;
	}

	if (size < 1) {
		size = JZ_STRING_BUFFER_DEFAULT;
	} else if (size > JZ_STRING_BUFFER_MAXLEN) {
		php_error_docref(NULL, E_ERROR, "buffer size must not exceed %d", JZ_STRING_BUFFER_MAXLEN);
		RETURN_FALSE;
	}

	JZBuffer *buffer = emalloc(sizeof(JZBuffer));
	if (!buffer) {
		php_error_docref(NULL, E_ERROR, "malloc buffer failed");
		RETURN_FALSE;
	}

	buffer->length = 0;
	buffer->size = size;
	buffer->str = emalloc(size);
	if (!buffer->str) {
		efree(buffer);
		php_error_docref(NULL, E_ERROR, "malloc buffer failed");
		RETURN_FALSE;
	}
	memset(buffer->str, ' ', buffer->length);
	jz_add_buffer_object(getThis(), buffer);
	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);
}

PHP_METHOD(jz_buffer, append) {
	char *input = NULL;
	size_t input_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE
		|| input_len == 0) {
		RETURN_FALSE;
	}

	JZBuffer *buffer = jz_get_buffer_object(getThis());
	if (buffer->length + input_len > buffer->size) {
		buffer->size += 1024;
		buffer->str = realloc(buffer->str, buffer->size);
		memset(buffer->str + 1024, ' ', 1024);
	}

	memcpy(buffer->str + buffer->length, input, input_len);
	buffer->length += input_len;

	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);

	RETURN_LONG(buffer->length);
}

PHP_METHOD(jz_buffer, substr) {
	long offset;
	long length = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &offset, &length) == FAILURE) {
		RETURN_FALSE;
	}

	JZBuffer *buffer = jz_get_buffer_object(getThis());

	if (offset < 0) {
		offset = buffer->length + offset;
	}

	if (length < 0) {
		length = buffer->length - offset;
	}

	if (offset + length > buffer->length) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "offset(%ld,%ld) out of bounds.", offset, length);
		RETURN_FALSE;
	}

	RETURN_STRINGL(buffer->str + offset, length);
}

PHP_METHOD(jz_buffer, deprecated) {
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) == FAILURE
		|| length <= 0) {
		RETURN_FALSE;
	}

	JZBuffer *buffer = jz_get_buffer_object(getThis());
	if (length > buffer->length) {
		length = buffer->length;
	}

	RETVAL_STRINGL(buffer->str, length);
	buffer->length -= length;
	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);
	memcpy(buffer->str, buffer->str + length, buffer->length);
}


PHP_METHOD(jz_buffer, clear) {
	JZBuffer *buffer = jz_get_buffer_object(getThis());
	memset(buffer->str, ' ', buffer->length);
	buffer->length = 0;
	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);
}


PHP_METHOD(jz_buffer, __toString) {
	JZBuffer *buffer = jz_get_buffer_object(getThis());
    RETURN_STRINGL(buffer->str, buffer->length);
}


PHP_METHOD(jz_buffer, __destruct) {
	JZBuffer *buffer = jz_get_buffer_object(getThis());
	efree(buffer->str);
	efree(buffer);
}

zend_function_entry jz_buffer_methods[] = {
	PHP_ME(jz_buffer, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(jz_buffer, __destruct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_ME(jz_buffer, append, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, substr, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, deprecated, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, clear, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, __toString, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

JZ_STARTUP_FUNCTION(buffer) {
	le_jz_buf_cls = zend_register_list_destructors_ex(NULL, jz_free_buff_object, "JZ Buffer", module_number);

	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "JZ_Buffer", jz_buffer_methods);
	jz_buffer_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
