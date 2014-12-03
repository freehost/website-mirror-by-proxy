<?php

class ProxyHttpRequest extends HttpRequest
{

	const CONTENT_TYPE_TEXT_HTML = 1;

	const CONTENT_TYPE_TEXT_CSS = 2;

	const CONTENT_TYPE_JAVASCRIPT = 3;

	const CONTENT_TYPE_JSON = 4;

	private $message_content_type;

	public function __construct()
	{
		parent::__construct($this->getUrl());
	}

	public function getUrl()
	{
		static $url;
		if (! isset($url)) {
			$url = RedirectWhenBlockedFull::getRequestUriWithoutGetParam();
			$this->removeThisScriptDirFromUrl($url);
			
			if (RedirectWhenBlockedFull::startsWith($url, '/http://') ||
				 RedirectWhenBlockedFull::startsWith($url, '/https://')) {
				$url = substr($url, 1);
				
				if (! TextExternalUrlFilters::matchesUrl($url)) {
					header('HTTP/1.0 403 Forbidden');
					exit();
				}
				
				// Use in DomUtlFilters for relative URLs.
				$base_url_suffix = http_build_scheme_host($url) . '/';
				RedirectWhenBlockedFull::setBaseUrlSuffix($base_url_suffix);
				
			} else {
				$url = Conf::$default_upstream_base_url . $url;
			}
		}
		return $url;
	}

	public function getUrlComponent($name)
	{
		$components = $this->getUrlComponents();
		if (isset($components[$name])) {
			return $components[$name];
		}
	}

	public function getUrlComponents()
	{
		static $components;
		if (! isset($components)) {
			$components = parse_url($this->getUrl());
		}
		return $components;
	}

	public function send()
	{
		$message = parent::send();
		Log::add($message);
		
		$this->message_content_type = $this->getContentTypeFromMessage($message);
		
		switch ($this->message_content_type) {
			
			case self::CONTENT_TYPE_JAVASCRIPT:
			case self::CONTENT_TYPE_JSON:
				$body = $message->getBody();
				break;
			
			case self::CONTENT_TYPE_TEXT_CSS:
				$body = $message->getBody();
				
				$url_path_depth = count(
					explode('/', $this->getUrlComponent('path'))) - 2;
				if ($url_path_depth) {
					$css_base_url = str_repeat('../', $url_path_depth);
				} else {
					$css_base_url = './';
				}
				$body = str_replace('url("/', 'url("' . $css_base_url, $body);
				$body = str_replace('url(\'/', 'url(\'' . $css_base_url, $body);
				$body = str_replace('url(/', 'url(' . $css_base_url, $body);
				break;
			
			case self::CONTENT_TYPE_TEXT_HTML:
				
				$body = $message->getBody();
				$dom = str_get_html($body, false, false, DEFAULT_TARGET_CHARSET, 
					false);
				
				// Might be for example JSONP data served with an incorrect header.
				if (! $dom->find('body')) {
					break;
				}
				
				$title = '';
				foreach ($dom->find('head title') as $title) {
					$title = $title->text();
					break;
				}
				
				// Only apply RWB if response is 200.
				if ($message->getResponseCode() == 200) {
					
					RedirectWhenBlockedFull::setWebsiteTitle($title);
					RedirectWhenBlockedFull::run();
					
					// RWB will turn output into JSONP. Remove content-type header to keep header sent by RWB.
					$headers = $message->getHeaders();
					if (isset($headers['Content-Type'])) {
						unset($headers['Content-Type']);
						$message->setHeaders($headers);
					}
				}
				
				DomUrlFilters::applyAll($dom);
				$body = $dom->__toString();
				break;
		}
		
		if (isset($body)) {
			TextInternalUrlFilters::applyAll($body);
			TextExternalUrlFilters::applyAll($body, $this->message_content_type);
			$message->setBody($body);
		}
		
		return $message;
	}

	private static function removeThisScriptDirFromUrl(&$url)
	{
		$this_script_dir = dirname($_SERVER['SCRIPT_NAME']);
		if (substr($url, 0, strlen($this_script_dir)) == $this_script_dir) {
			$url = substr($url, strlen($this_script_dir));
		}
		return $url;
	}

	private static function getContentTypeFromMessage(HttpMessage $message)
	{
		$content_type = $message->getHeader('Content-Type');
		$content_type_chunks = explode(';', $content_type, 2);
		if (count($content_type_chunks) > 1) {
			$content_type = $content_type_chunks[0];
		}
		$content_type = strtolower($content_type);
		
		switch ($content_type) {
			
			case 'application/json':
				return self::CONTENT_TYPE_JSON;
			
			case 'application/x-javascript':
				return self::CONTENT_TYPE_JAVASCRIPT;
			
			case 'text/css':
				return self::CONTENT_TYPE_TEXT_CSS;
			
			case 'text/html':
				return self::CONTENT_TYPE_TEXT_HTML;
		}
	}
}