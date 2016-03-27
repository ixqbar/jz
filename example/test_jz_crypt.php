<?php
class Service {
	
	/**
	 *
	 * @param string $clear_text        	
	 * @param string $key        	
	 * @return array
	 */
	public static function aes_encrypt($clear_text, $key) {
		$cipher = mcrypt_module_open ( MCRYPT_RIJNDAEL_128, '', MCRYPT_MODE_CBC, '' );
		$iv_size = mcrypt_enc_get_iv_size ( $cipher );
		$iv = mcrypt_create_iv ( $iv_size, MCRYPT_RAND );
		mcrypt_generic_init ( $cipher, $key, $iv );
		$data = mcrypt_generic ( $cipher, $clear_text );
		mcrypt_generic_deinit ( $cipher );
		mcrypt_module_close ( $cipher );
		
		return array (
				$data,
				$iv 
		);
	}
	
	/**
	 *
	 * @param string $raw        	
	 * @param string $iv        	
	 * @param string $key        	
	 * @return array
	 */
	public static function aes_decrypt($raw, $iv, $key) {
		$cipher = mcrypt_module_open ( MCRYPT_RIJNDAEL_128, '', MCRYPT_MODE_CBC, '' );
		mcrypt_generic_init ( $cipher, $key, $iv );
		$data = mdecrypt_generic ( $cipher, $raw );
		mcrypt_generic_deinit ( $cipher );
		mcrypt_module_close ( $cipher );
		
		return $data;
	}
	
	/**
	 *
	 * @param string $clear_text        	
	 * @param string $key        	
	 * @return string
	 */
	public static function encrypt($clear_text, $key) {
		$key = strlen($key) <= 16 ? str_pad ( $key, 16, ' ' ) : substr ( $key, 0, 16 );
		$crc = crc32($clear_text );
		$clear_text_len = strlen($clear_text);
		$is_gzip = strlen ( $clear_text ) > 100 ? 1 : 0;
		$clear_text = $is_gzip ? gzcompress ( $clear_text ) : $clear_text;
		$header = str_pad (sprintf('o,%d,%lu,%d,%d,', $clear_text_len, $crc, $is_gzip, strlen($clear_text)), 32, ' ' );
		list ( $data, $iv ) = self::aes_encrypt ( $header . $clear_text, $key );
		
		return $iv . $data;
	}
	
	/**
	 *
	 * @param string $raw_data        	
	 * @param string $key        	
	 * @return array
	 */
	public static function decrypt($raw_data, $key) {
		$key = strlen ( $key ) <= 16 ? str_pad ( $key, 16, ' ' ) : substr ( $key, 0, 16 );
		$clear_text = self::aes_decrypt ( substr ( $raw_data, 16 ), substr ( $raw_data, 0, 16 ), $key );
		if (empty($clear_text)) {
			return null;
		}
		
		$headers = explode(',', rtrim(substr($clear_text, 0, 32)),6);
		if (empty($headers) || 6 != count ( $headers )) {
			return null;
		}
		
		if ($headers[0] !== 'o') {
			return null;
		}
		
		$clear_text = substr($clear_text, 32, $headers[4]);
		if ($headers[3] === '1') {
			$clear_text = gzuncompress($clear_text);
		}
		$real_crc = sprintf('%lu', crc32($clear_text));
		if ($headers[2] !== $real_crc) {
			return null;
		}
		
		return $clear_text;
	}
}

$aes_text = <<<EOT
PHP is a popular general-purpose scripting language that is especially suited to web development.
Fast, flexible and pragmatic, PHP powers everything from your blog to the most popular websites in the world.

The PHP development team announces the immediate availability of PHP 7.0.0. This release marks the start of the new major PHP 7 series.

PHP 7.0.0 comes with a new version of the Zend Engine, numerous improvements and new features such as

Improved performance: PHP 7 is up to twice as fast as PHP 5.6
Significantly reduced memory usage
Abstract Syntax Tree
Consistent 64-bit support
Improved Exception hierarchy
Many fatal errors converted to Exceptions
Secure random number generator
Removed old and unsupported SAPIs and extensions
The null coalescing operator (??)
Return and Scalar Type Declarations
Anonymous Classes
Zero cost asserts

For source downloads of PHP 7.0.0 please visit our downloads page, Windows binaries can be found on windows.php.net/download/. The list of changes is recorded in the ChangeLog.

The migration guide is available in the PHP Manual. Please consult it for the detailed list of new features and backward incompatible changes.

The inconvenience of the release lateness in several time zones is caused by the need to ensure the compatibility with the latest OpenSSL 1.0.2e release. Thanks for the patience!

It is not just a next major PHP version being released today. The release being introduced is an outcome of the almost two years development journey. It is a very special accomplishment of the core team. And, it is a result of incredible efforts of many active community members. Indeed, it is not just a final release being brought out today, it is the rise of a new PHP generation with an enormous potential.

Congratulations everyone to this spectacular day for the PHP world!

Grateful thanks to all the contributors and supporters!
EOT;

$aes_key  = '1234567890123456';

$t = microtime(1);
$times = 1;
while ($times <= 100000) {
	$result = jz_encrypt($aes_text, $aes_key);
	jz_decrypt($result, $aes_key);
	$times++;
}
echo microtime(1) - $t;
echo "\n";

var_dump(Service::decrypt($result, $aes_key));
var_dump(jz_decrypt($result, $aes_key));

$t = microtime(1);
$times = 1;
while ($times <= 100000) {
	$result = Service::encrypt($aes_text, $aes_key);
	Service::decrypt($result, $aes_key);
	$times++;
}
echo microtime(1) - $t;
echo "\n";



echo jz_version() . PHP_EOL;

