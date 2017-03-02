<?php

$key = "xingqiba";

$result = jz_rc4("my name is 星期八", $key);

echo jz_rc4($result, $key) . PHP_EOL;
