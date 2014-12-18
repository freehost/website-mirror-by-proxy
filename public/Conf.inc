<?php

class Conf
{
	
	// Using the same QUERY_STRING_PARAM_NAME as in RWB so make sure this value isn't used by RWB.
	const OUTPUT_TYPE_ALT_BASE_URLS = 10;

	public static $alt_base_urls_key;

	public static $default_cache_control_max_age = 60;

	public static $default_upstream_base_url;

	public static $html_body_appendix = '';

	public static $html_head_appendix = '';

	public static function appendToHtmlBody($str)
	{
		self::$html_body_appendix .= $str;
	}

	public static function appendToHtmlHead($str)
	{
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