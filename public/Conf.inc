<?php

class Conf
{

	public static $default_cache_control_max_age = 60;
	
	public static $default_upstream_base_url;
	
	public static $html_body_appendix = '';
	
	public static $html_head_appendix = '';

	public static function appendToHtmlBody($str) {
		self::$html_body_appendix .= $str;
	}

	public static function appendToHtmlHead($str) {
		self::$html_head_appendix .= $str;
	}
	
	public static function getDefaultUpstreamBaseUrlComponent($name)
	{
		$components = self::getDefaultUpstreamBaseUrlComponents();
		if (isset($components[$name])) {
			return $components[$name];
		}
	}

	public static function getDefaultUpstreamBaseUrlComponents()
	{
		static $components;
		if (! isset($components)) {
			$components = parse_url(self::$default_upstream_base_url);
		}
		return $components;
	}

	public function __toString()
	{
		$class = new ReflectionClass(__CLASS__);
		return print_r($class->getStaticProperties(), true);
	}
}