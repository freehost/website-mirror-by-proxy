<?php
require 'main.inc';

Log::add($_SERVER);
Log::add(new Conf());

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
			header('Access-Control-Allow-Origin: ' . $downstream_origin);
		}
	}
}

// Default cache. Will be overwritten by message below if it has it's own Cache-Control header.
header('Cache-Control: max-age=' . Conf::$default_cache_control_max_age);

$message->send();