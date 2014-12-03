<?php

class Conf
{

	public static $rwb_alt_url_bases;

	public static $rwb_alt_url_collections;

	public static $default_cache_control_max_age = 60;
	
	public static $default_upstream_base_url;

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