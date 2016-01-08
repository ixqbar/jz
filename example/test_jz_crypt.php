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
		$key = strlen ( $key ) <= 16 ? str_pad ( $key, 16, ' ' ) : substr ( $key, 0, 16 );
		$crc = crc32 ( $clear_text );
		$is_gzip = strlen ( $clear_text ) > 100 ? true : false;
		$clear_text = $is_gzip ? gzcompress ( $clear_text ) : $clear_text;
		$header = str_pad ( sprintf ( 'ok,%d,%d,%u', $is_gzip ? 1 : 0, strlen ( $clear_text ), $crc ), 32, ' ' );
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
		if (empty ( $clear_text )) {
			return array (
					true,
					'empty decrypt' 
			);
		}
		
		$headers = explode ( ',', rtrim ( substr ( $clear_text, 0, 32 ) ), 4 );
		if (empty ( $headers ) || 4 != count ( $headers )) {
			return array (
					true,
					'error headers' 
			);
		}
		
		if ($headers [0] !== 'ok') {
			return array (
					true,
					'check ok flag failed' 
			);
		}
		
		$clear_text = substr ( $clear_text, 32, $headers [2] );
		if ($headers [1] === '1') {
			$clear_text = gzuncompress ( $clear_text );
		}
		$real_crc = sprintf ( '%u', crc32 ( $clear_text ) );
		if ($headers [3] !== $real_crc) {
			return array (
					true,
					'check crc failed' 
			);
		}
		
		return array (
				false,
				$clear_text 
		);
	}
}

$aes_text = str_pad("hello jz", 501, "=");
$aes_key  = '1234567890123456';

$t = microtime(1);
$times = 1;
while ($times <= 10000) {
	$result = jz_encrypt ( $aes_text, $aes_key );
	//jz_decrypt($result, $aes_key);
	$times++;
}
echo microtime(1) - $t;
echo "\n";

var_dump(Service::decrypt($result, $aes_key));
var_dump(jz_decrypt($result, $aes_key));

$t = microtime(1);
$times = 1;
while ($times <= 10000) {
	$result = Service::encrypt($aes_text, $aes_key);
	Service::decrypt($result, $aes_key);
	$times++;
}
echo microtime(1) - $t;
echo "\n";



echo jz_version() . PHP_EOL;

