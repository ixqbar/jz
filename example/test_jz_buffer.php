<?php


$buffer_handle = new JZ_Buffer(5);

echo "append:" . $buffer_handle->append('01234') . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "append:" . $buffer_handle->append('ABCDEFG') . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "get2:" . $buffer_handle->get(2) . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "get2 and remove:" . $buffer_handle->get(2, 1) . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "get all and remove:" . $buffer_handle->get(0, 1) . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;

echo "append:" . $buffer_handle->append('HIJKLMNOPQRST01234567') . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "get all:" . $buffer_handle->get() . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "shift 4:" . $buffer_handle->shift(4) . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;
echo PHP_EOL;

echo "get all:" . $buffer_handle->get() . PHP_EOL;
echo "length:" . $buffer_handle->length . PHP_EOL;
echo "size:" . $buffer_handle->size . PHP_EOL;

