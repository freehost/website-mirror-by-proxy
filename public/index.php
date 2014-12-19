<?php
require 'main.inc';

// Default cache.
// Will be overwritten by message below if it has it's own Cache-Control header.
// Send this early, to prevent caching error pages for longer than the duration.
header('Cache-Control: max-age=' . Conf::$default_cache_control_max_age);

Log::add($_SERVER);
Log::add(new Conf());

if (isset($_GET[RedirectWhenBlockedFull::QUERY_STRING_PARAM_NAME]) &&
	 $_GET[RedirectWhenBlockedFull::QUERY_STRING_PARAM_NAME] == Conf::OUTPUT_TYPE_ALT_BASE_URLS) {
	
	// Key cannot be empty.
	if (Conf::$alt_base_urls_key) {
		
		// Verify key. Set this in conf-local.inc.
		if (isset($_GET['key']) && $_GET['key'] == Conf::$alt_base_urls_key) {
			
			header('Content-Type: application/javascript');
			print json_encode(RedirectWhenBlockedFull::getAltBaseUrls());
			exit();
		}
	}
}

$request = new ProxyHttpRequest();
Log::add($request);

$message = $request->send();
Log::add($message);



if (isset($_SERVER['HTTP_ORIGIN'])) {
	$downstream_origin = $_SERVER['HTTP_ORIGIN'];
} elseif (isset($_SERVER['HTTP_REFERER'])) {
	$downstream_origin = http_build_scheme_host($_SERVER['HTTP_REFERER']);
}

if (isset($downstream_origin)) {
	foreach (RedirectWhenBlockedFull::getAltBaseUrls() as $alt_url_base) {
		if ($downstream_origin == http_build_scheme_host($alt_url_base)) {
			removeHeaderFromMessage($message, 'Access-Control-Allow-Origin');
			header('Access-Control-Allow-Origin: ' . $downstream_origin);
			
			// See http://stackoverflow.com/questions/12409600/error-request-header-field-content-type-is-not-allowed-by-access-control-allow.
			removeHeaderFromMessage($message, 'Access-Control-Allow-Headers');
			header(
				'Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept');
		}
	}
}

$message->send();