
### Jz (only support php7 or later)

```php
string jz_version()

string jz_encrypt(string $text, string $key)
array  jz_decrypt(string $text, string $key)
mixed  jz_trace($callback[, $callback_params2, $callback_params3,...])
array  jz_jieba(string $text, bool use_extract = false, long extract_limit = 10)
string jz_rc4(string $text, string $key)

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
cd jz/cjieba
make

cd ..
phpize
./configure --enable-jieba
make
make install
```
* --enable-jieba 可选
* jieba more detail please visit https://github.com/yanyiwu/cppjieba

### php.ini
```
extension=jz.so 
jz.enable_jieba=1
jz.dict_path=/Users/xingqiba/data/softs/jz/cjieba/dict    #指向jz库dict目录
```

### example
```
$result = jz_jieba('小明硕士毕业于中国科学院计算所，后在日本京都大学深造');
print_r($result);

$result = jz_jieba('小明硕士毕业于中国科学院计算所，后在日本京都大学深造', true, 6);
print_r($result);

$result = jz_jieba('他心理健康');
print_r($result);
```
 * 更新请参考example目录

### contact
更多疑问请+qq群 233415606 or [website http://www.hnphper.com](http://www.hnphper.com)


