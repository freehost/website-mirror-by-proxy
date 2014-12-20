<?php

class Log
{

	private static $enabled = false;

	private static $content = array();

	public static function add($x)
	{
		if (! self::$enabled) {
			return;
		}
		
		$a[] = date('Y-m-d H:i:s.u');
		$a[] = 'memory_get_usage: ' . (memory_get_usage() / 1024) . ' kilobytes';
		$a[] = 'memory_get_peak_usage: ' . (memory_get_peak_usage() / 1024) .
			 ' kilobytes';
		$a[] = gettype($x);
		$a[] = print_r($x, true);
		self::$content[] = implode("\n", $a);
	}

	public static function enable()
	{
		self::$enabled = true;
		
		register_shutdown_function('Log::shutdown');
	}

	public static function shutdown()
	{
		self::add('shutdown');
		
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