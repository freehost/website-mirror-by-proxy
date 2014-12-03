<?php

class Log
{

	private static $enabled = false;

	private static $content = array();

	public static function add($a)
	{
		if (! self::$enabled) {
			return;
		}
		
		$a = gettype($a) . ': ' . print_r($a, true);
		self::$content[] = $a;
	}

	public static function enable()
	{
		self::$enabled = true;
		
		register_shutdown_function('Log::shutdown');
	}

	public static function shutdown()
	{
		$fh = self::getFileHandle();
		if (is_resource($fh)) {
			fwrite($fh, implode("\n------------\n", self::$content));
			fclose($fh);
		}
	}

	private static function getFileHandle()
	{
		static $fh;
		if (! isset($fh)) {
			$dir = __DIR__ . '/log';
			if (! is_dir($dir)) {
				mkdir($dir);
				chmod($dir, 0777);
			}
			$filename = tempnam($dir, 
				__CLASS__ . '.' . time() . '.' .
					 urlencode($_SERVER['SCRIPT_NAME']));
			touch($filename);
			chmod($filename, 0777);
			$fh = fopen($filename, 'w');
		}
		return $fh;
	}
}