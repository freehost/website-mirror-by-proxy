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
			
			// Convert protocol relative URLs to HTTPS.
			$searches['"//' . $host] = '"https://' . $host;
			$searches['\'//' . $host] = '\'https://' . $host;
			
			$urls = array();
			$urls[] = 'http://' . $host;
			$urls[] = 'https://' . $host;
			
			foreach ($urls as $url) {
				$searches[$url] = RedirectWhenBlockedFull::getBaseUrl() . $url;
			}
			
			// Because slashes are escaped in JSON, escape all search/replace patterns too.
			// JSON-type data may be embedded in JavaScript and HTML as well.
			if (in_array($content_type, 
				array(
					ProxyHttpRequest::CONTENT_TYPE_JAVASCRIPT,
					ProxyHttpRequest::CONTENT_TYPE_JSON,
					ProxyHttpRequest::CONTENT_TYPE_TEXT_HTML
				))) {
				
				$searches2 = array();
				
				foreach ($searches as $search => $replace) {
					$search = str_replace('/', '\/', $search);
					$replace = str_replace('/', '\/', $replace);
					$searches2[$search] = $replace;
				}
				
				// For pure JSON content, use these searches only.
				if ($content_type == ProxyHttpRequest::CONTENT_TYPE_JSON) {
					$searches = $searches2;
				} 				

				// Otherwise append.
				else {
					$searches += $searches2;
				}
			}
			
			// Special for javascript (and HTML since it could be in a script tag).
			if (in_array($content_type, 
				array(
					ProxyHttpRequest::CONTENT_TYPE_JAVASCRIPT,
					ProxyHttpRequest::CONTENT_TYPE_TEXT_HTML
				))) {
				foreach (array(
					"'",
					'"'
				) as $quote) {
					if ($host != Conf::getDefaultUpstreamBaseUrlComponent(
						'host')) {
						$prefix = 'http://' . $host;
					} else {
						$prefix = '';
					}
					$searches[$quote . $host . $quote] = $quote .
						 self::getBaseUrlHostAndPath() . $prefix . $quote;
				}
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