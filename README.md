
##Jz (only support php7 or later)

```php
string jz_version()

string jz_encrypt($text, $key)
string jz_decrypt($text, $key)
mixed  jz_trace($callback[, $callback_params2, $callback_params3,...])

class JZ_Data() {
    public __construct($data, $readonly=true) {}
	public mixed get($name, $default_value){}
	public mixed __get($name) {}
	public mixed __isset($name) {}
	public mixed __set($name, $value) {}
	public mixed set($name, $value) {}
	public mixed count() {}
	public mixed offsetGet($name) {}
	public mixed offsetSet($name, $value) {}
	public mixed offsetExists($name) {}
	public mixed offsetUnset($name) {}
	public void rewind() {}
	public mixed key() {}
	public mixed next() {}
	public mixed current() {}
	public boolean valid() {}
	public array to_array() {}
	public boolean readonly() {}   
}

class JZ_Buffer() {
    public __construct(size) {}	
	public int append(str) {}
	public string substr(offset, length) {}
	public string deprecated(length) {}
	public void clear() {} 
}
```
