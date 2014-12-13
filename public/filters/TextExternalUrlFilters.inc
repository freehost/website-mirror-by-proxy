<?php

class TextExternalUrlFilters
{

	private static $hosts;

	public static function addHost($host, $pattern = null)
	{
		if (! isset(self::$hosts[$host])) {
			self::$hosts[$host] = array();
		}
	}

	public static function addHosts($hosts)
	{
		foreach ($hosts as $host) {
			self::addHost($host);
		}
	}

	public static function applyAll(&$body, $content_type = NULL)
	{
		foreach (self::$hosts as $host => $patterns) {
			
			$searches = array();
			
			$urls = array();
			$urls[] = 'http://' . $host;
			$urls[] = 'https://' . $host;
			
			foreach ($urls as $url) {
				$searches[$url] = RedirectWhenBlockedFull::getBaseUrl() . $url;
			}
			
			if ($content_type == ProxyHttpRequest::CONTENT_TYPE_JAVASCRIPT) {
				$searches['"' . $host . '"'] = '"' .
					 self::getBaseUrlHostAndPath() . '"';
			}
			
			// Because slashes are escaped in JSON, escape all search/replace patterns too.
			if ($content_type == ProxyHttpRequest::CONTENT_TYPE_JSON) {
				$searches2 = array();
				
				// Convert protocol relative URLs to HTTPS.
				$searches2['"\/\/' . $host] = '"https:\/\/' . $host;
				
				foreach ($searches as $search => $replace) {
					$search = str_replace('/', '\/', $search);
					$replace = str_replace('/', '\/', $replace);
					$searches2[$search] = $replace;
				}
				
				$searches = $searches2;
			}
			
			foreach ($searches as $search => $replace) {
				$body = str_ireplace($search, $replace, $body);
			}
		}
	}

	public static function applyReverse(&$body)
	{
		foreach (RedirectWhenBlockedFull::getAltBaseUrls() as $alt_base_url) {
			$body = str_replace($alt_base_url, '', $body);
		}
	}

	public static function matchesUrl($url)
	{
		$host = parse_url($url, PHP_URL_HOST);
		if (in_array($host, array_keys(self::$hosts))) {
			return true;
		}
	}

	private static function getBaseUrlHostAndPath()
	{
		static $host_and_path;
		if (! isset($host_and_path)) {
			$base_url_components = parse_url(
				RedirectWhenBlockedFull::getBaseUrl());
			$host_and_path = $base_url_components['host'];
			if (isset($base_url_components['path'])) {
				$host_and_path .= $base_url_components['path'];
			}
		}
		return $host_and_path;
	}
}