<?php

function haha($caller_file, $caller_line, ...$p) {
    print_r($p);
    print_r(func_get_args());
    
    return [2,3];
}

