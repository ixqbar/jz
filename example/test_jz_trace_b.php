<?php

include './test_jz_trace_a.php';

class Todo {
    
    function test() {
        $name = [1,2, ["name" => 1]];
        var_dump(jz_trace('haha', $name));
        
        print_r($name);
    }
}

