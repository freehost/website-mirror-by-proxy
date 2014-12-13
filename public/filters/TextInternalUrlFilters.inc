<?php

class TextInternalUrlFilters
{

	private static $searches = array();

	public static function addSearch($search, $replace = '')
	{
		if (! $replace) {
			$replace = rtrim($search, '/') . './';
		}
		
		self::$searches[$search] = $replace;
	}

	public static function applyAll(&$body)
	{
		foreach (self::$searches as $search => $replace) {
			$body = str_ireplace($search, $replace, $body);
		}
	}
}