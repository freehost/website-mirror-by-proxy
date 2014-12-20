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
		
		if ($_SERVER['REQUEST_METHOD'] == 'POST') {
			parent::setMethod(HTTP_METH_POST);
		}
		
		if (isset($_POST)) {
			parent::setPostFields($_POST);
		}
	}

	public function getUrl()
	{
		static $url;
		if (! isset($url)) {
			$url = RedirectWhenBlockedFull::getRequestUriWithoutQueryStringParam();
			$this->removeThisScriptDirFromUrl($url);
			
			if (RedirectWhenBlockedFull::startsWith($url, '/http://') ||
				 RedirectWhenBlockedFull::startsWith($url, '/https://')) {
				$url = substr($url, 1);
				
				if (! TextExternalUrlFilters::matchesUrl($url)) {
					header('HTTP/1.0 403 Forbidden');
					exit();
				}
				
				// Use in DomUtlFilters for relative URLs.
				$base_url_suffix = http_build_scheme_host_directory($url) . '/';
				RedirectWhenBlockedFull::setBaseUrlSuffix($base_url_suffix);
			} else {
				
				if ($url == '/') {
					if (Conf::$default_upstream_url) {
						$url = Conf::$default_upstream_url;
					}
				}
				$url = Conf::$default_upstream_base_url . $url;
			}
		}
		
		// Reverse rewrites of parameters inside URL.
		TextExternalUrlFilters::applyReverse($url);
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
		
		// If redirect, rewrite Location header.
		$headers = $message->getHeaders();
		if (isset($headers['Location'])) {
			if (parse_url($headers['Location'], PHP_URL_HOST)) {
				TextExternalUrlFilters::applyAll($headers['Location']);
			} else {
				$headers['Location'] = dirname($_SERVER['SCRIPT_NAME']) .
					 $headers['Location'];
			}
			$message->setHeaders($headers);
		}
		
		$this->message_content_type = $this->getContentTypeFromMessage($message);
		
		switch ($this->message_content_type) {
			
			case self::CONTENT_TYPE_JAVASCRIPT:
			case self::CONTENT_TYPE_JSON:
				$body = $message->getBody();
				break;
			
			case self::CONTENT_TYPE_TEXT_CSS:
				$body = $message->getBody();
				
				$url_path_depth = count(
					explode('/', trim($this->getUrlComponent('path'), '/'))) - 1;
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
				$dom = str_get_html($body, false, false, '', false);
				
				// Might be for example JSONP data served with an incorrect header.
				if (! $dom || ! $dom->find('body')) {
					break;
				}
				
				foreach ($dom->find('head title') as $title_element) {
					$title = mb_convert_encoding_plus($title_element->text(), 
						'UTF-8', 
						$this->getCharsetFromHeadersOrDom($message, $dom));
					if ($title) {
						$title .= ' | 免翻墙镜像';
						
						// Update title in DOM.
						$title_element->innertext = mb_convert_encoding_plus(
							$title, 
							$this->getCharsetFromHeadersOrDom($message, $dom), 
							'UTF-8');
						break;
					}
				}
				
				// Default title.
				if (! isset($title) || ! $title) {
					$title = '免翻墙镜像';
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
				
				foreach ($dom->find('iframe') as $iframe) {
					$src = $iframe->getAttribute('src');
					if ($src) {
						$src = http_add_query_component($src, 
							RedirectWhenBlockedFull::QUERY_STRING_PARAM_NAME, 
							RedirectWhenBlockedFull::OUTPUT_TYPE_IFRAME);
						$iframe->setAttribute('src', $src);
					}
				}
				
				// Don't insert stuff if request failed.
				if ($message->getResponseCode() == 200) {
					if (Conf::$html_body_appendix) {
						foreach ($dom->find('body') as $b) {
							$b->innertext .= Conf::$html_body_appendix;
							break;
						}
						
						$dom_do_reset = true;
					}
					
					if (Conf::$html_head_appendix) {
						foreach ($dom->find('head') as $h) {
							$h->innertext .= Conf::$html_head_appendix;
							break;
						}
						
						$dom_do_reset = true;
					}
				}
				
				if (count($dom->find('script[async]')) > 0 && $dom_do_reset) {
					
					// Bug in Simple HTML Dom means that DOM has to be reloaded before further edits.
					$body = $dom->__toString();
					$dom->clear();
					unset($dom);
					$dom = str_get_html($body, false, false, '', false);
					$dom_do_reset = false;
				}
				
				// Remove 'async' attributes because they break the page in IE11 when loaded in our iframe.
				foreach ($dom->find('script[async]') as $x) {
					$x->removeAttribute('async', '');
				}
				
				$body = $dom->__toString();
				$dom->clear();
				unset($dom);
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
		if ($this_script_dir != '/' &&
			 substr($url, 0, strlen($this_script_dir)) == $this_script_dir) {
			$url = substr($url, strlen($this_script_dir));
		}
		return $url;
	}

	private static function getCharsetFromHeadersOrDom(HttpMessage $message, 
		simple_html_dom $dom)
	{
		// TODO: get charset from headers.
		foreach ($dom->find('meta[http-equiv=Content-Type]') as $meta) {
			$content = $meta->getAttribute('content');
			if ($content) {
				$content_chunks = explode(';', $content, 2);
				if (isset($content_chunks[1])) {
					$charset_key_value_str = trim($content_chunks[1]);
					$charset_key_value_arr = explode('=', 
						$charset_key_value_str, 2);
					if (isset($charset_key_value_arr[1])) {
						return $charset_key_value_arr[1];
					}
				}
			}
		}
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
			
			case 'application/javascript':
			case 'application/x-javascript':
				return self::CONTENT_TYPE_JAVASCRIPT;
			
			case 'text/css':
				return self::CONTENT_TYPE_TEXT_CSS;
			
			case 'text/html':
				return self::CONTENT_TYPE_TEXT_HTML;
		}
	}
}
