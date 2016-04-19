<?php


$buffer_handle = new JZ_Buffer(5);

var_dump($buffer_handle->append('123'));
var_dump($buffer_handle->length);

var_dump($buffer_handle->append('123'));
var_dump($buffer_handle->length);

var_dump((string)$buffer_handle);
var_dump($buffer_handle->substr(-4));
var_dump($buffer_handle->substr(1, 2));
var_dump($buffer_handle->substr(2));

var_dump($buffer_handle->shift(2));

var_dump((string)$buffer_handle);

var_dump($buffer_handle->length);

$buffer_handle->clear();
var_dump((string)$buffer_handle);
var_dump($buffer_handle->length);

echo "\n\ntest remove\n\n";

var_dump($buffer_handle->append('0123456789'));
var_dump($buffer_handle->substr(-4, 3, 1));
var_dump((string)$buffer_handle);
var_dump($buffer_handle->length);

var_dump($buffer_handle->append('abc'));
var_dump((string)$buffer_handle);
var_dump($buffer_handle->length);





