<?php


$buffer_handle = new JZ_Buffer(1024);

var_dump($buffer_handle->append('123'));
var_dump($buffer_handle->length);

var_dump($buffer_handle->append('123'));
var_dump($buffer_handle->length);

var_dump((string)$buffer_handle);

var_dump($buffer_handle->substr(1, 2));
var_dump($buffer_handle->substr(2));

var_dump($buffer_handle->deprecated(2));

var_dump((string)$buffer_handle);

var_dump($buffer_handle->length);

$buffer_handle->clear();
var_dump((string)$buffer_handle);
var_dump($buffer_handle->length);




