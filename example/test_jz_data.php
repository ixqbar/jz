<?php
$data = array(
    'name' => array('foo', 'bar')
);

$jz_data_handle = new JZ_Data($data);
print_r($jz_data_handle->to_array());
print_r($jz_data_handle->get('name')->to_array());
var_dump($jz_data_handle->get('name')->get(0));
var_dump($jz_data_handle->get('none'));
var_dump($jz_data_handle->get('none', [1,2,3]));
var_dump($jz_data_handle->get('none', [1,2,3])->to_array());
