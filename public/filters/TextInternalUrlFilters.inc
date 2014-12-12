<?php

class TextInternalUrlFilters
{

	private static $searches = array();

	public static function addSearch($search, $replace_prefix = '')
	{
		self::$searches[$search] = $replace_prefix;
	}

	public static function applyAll(&$body)
	{
		foreach (self::$searches as $search => $replace_prefix) {
			$body = str_ireplace($search, self::getReplace($replace_prefix), 
				$body);
		}
	}

	public static function getReplace($replace_prefix)
	{
		return $replace_prefix . './';
	}
}