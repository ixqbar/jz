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
    int read_index;
    int write_index;
    char *str;
} JZBuffer;

#define JZ_STRING_BUFFER_DEFAULT 1024
#define JZ_STRING_BUFFER_MAXLEN  (1024 * 1024)

static size_t get_buffer_str(JZBuffer *buffer, char *result, size_t expect_len, int remove) {
	if (buffer->length == 0) {
		return 0;
	}

	int final_len = expect_len;
	if (expect_len <= 0 || expect_len > buffer->length) {
		final_len = buffer->length;
	}

	int stop_index = buffer->size - 1;
	if (buffer->read_index + final_len <= buffer->size + 1) {
		stop_index = buffer->read_index + final_len - 1;
	}

	size_t old_read_index = buffer->read_index;
	int fill_index = 0;
	while (buffer->read_index <= stop_index) {
		result[fill_index] = buffer->str[buffer->read_index];
		buffer->read_index++;
		fill_index++;
		if (buffer->read_index == buffer->size) {
			stop_index = buffer->write_index - 1;
			buffer->read_index = 0;
		}
		if (fill_index == final_len) {
			break;
		}
	}

	if (remove) {
		buffer->length -= final_len;
	} else {
		buffer->read_index = old_read_index;
	}

	return final_len;
}

static size_t append_buffer_str(JZBuffer *buffer, char *data, size_t data_len) {
	if (buffer->length + data_len > buffer->size) {
		char *tmp_buff = emalloc(sizeof(char) * buffer->length);
		if (get_buffer_str(buffer, tmp_buff, buffer->length, 0) != buffer->length) {
			efree(tmp_buff);
			return 0;
		}

		buffer->size += 2 * data_len;
		buffer->read_index = 0;
		buffer->write_index = buffer->length;
		buffer->str = erealloc(buffer->str, buffer->size);
		memcpy(buffer->str, tmp_buff, buffer->length);
		efree(tmp_buff);
	}

	buffer->length += data_len;

	int stop_index = buffer->size - 1;
	if (buffer->read_index > buffer->write_index) {
		stop_index = buffer->read_index - 1;
	}

	int fill_index = 0;
	while (buffer->write_index <= stop_index) {
		buffer->str[buffer->write_index] = data[fill_index];
		buffer->write_index++;
		fill_index++;
		if (buffer->write_index == buffer->size) {
			stop_index = buffer->read_index - 1;
			buffer->write_index = 0;
		}
		if (fill_index == data_len) {
			break;
		}
	}

	return data_len;
}

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
	long size = 0;

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
	buffer->read_index = 0;
	buffer->write_index = 0;
	buffer->str = emalloc(size);
	if (!buffer->str) {
		efree(buffer);
		php_error_docref(NULL, E_ERROR, "malloc buffer failed");
		RETURN_FALSE;
	}
	jz_add_buffer_object(getThis(), buffer);
	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);
	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("size"), buffer->size);
}

PHP_METHOD(jz_buffer, append) {
	char *input = NULL;
	size_t input_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &input, &input_len) == FAILURE
		|| input_len == 0) {
		RETURN_FALSE;
	}

	JZBuffer *buffer = jz_get_buffer_object(getThis());
	if (append_buffer_str(buffer, input, input_len) != input_len) {
		RETURN_FALSE;
	}

	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);
	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("size"), buffer->size);

	RETURN_LONG(buffer->length);
}

PHP_METHOD(jz_buffer, get) {
	long length = 0;
	zend_bool remove = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lb", &length, &remove) == FAILURE) {
		RETURN_FALSE;
	}

	JZBuffer *buffer = jz_get_buffer_object(getThis());
	if (length <= 0 || length > buffer->length) {
		length = buffer->length;
	}

	char *tmp_buff = emalloc(sizeof(char) * length);
	if (get_buffer_str(buffer, tmp_buff, length, remove ? 1 : 0) != length) {
		efree(tmp_buff);
		RETURN_FALSE;
	}

	if (remove) {
		zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);
	}

	RETVAL_STRINGL(tmp_buff, length);
	efree(tmp_buff);
}

PHP_METHOD(jz_buffer, shift) {
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length) == FAILURE
		|| length <= 0) {
		RETURN_FALSE;
	}

	JZBuffer *buffer = jz_get_buffer_object(getThis());
	if (length > buffer->length || length <= 0) {
		length = buffer->length;
	}

	char *tmp_buff = emalloc(sizeof(char) * length);
	if (get_buffer_str(buffer, tmp_buff, length, 1) != length) {
		efree(tmp_buff);
		RETURN_FALSE;
	}

	zend_update_property_long(jz_buffer_class_entry, getThis(), ZEND_STRL("length"), buffer->length);

	RETVAL_STRINGL(tmp_buff, length);
	efree(tmp_buff);
}


PHP_METHOD(jz_buffer, clear) {
	JZBuffer *buffer = jz_get_buffer_object(getThis());
	buffer->length = 0;
	buffer->read_index = 0;
	buffer->write_index = 0;
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
	buffer = NULL;
}

zend_function_entry jz_buffer_methods[] = {
	PHP_ME(jz_buffer, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(jz_buffer, __destruct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_ME(jz_buffer, append, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, shift, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, clear, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(jz_buffer, __toString, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

JZ_STARTUP_FUNCTION(buffer) {
	le_jz_buf_cls = zend_register_list_destructors_ex(jz_free_buff_object, NULL, "JZ Buffer", module_number);

	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "JZ_Buffer", jz_buffer_methods);
	jz_buffer_class_entry = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
