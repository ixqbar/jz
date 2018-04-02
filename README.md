
### Jz (only support php7 or later)

```php
string jz_version()
string jz_rc4(string data, string key)
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
	public string get([length, remove]) {}
	public string shift(length) {}
	public void clear() {} 
}
```

### install
```
git clone https://github.com/jonnywang/jz.git
phpize
./configure
make
make install
```

### php.ini
```
extension=jz.so
```

### contact
更多疑问请+qq群 233415606



