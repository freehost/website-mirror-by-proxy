<?php
require 'main.inc';

Log::add($_SERVER);
Log::add(new Conf());

$request = new ProxyHttpRequest();
$message = $request->send();
Log::add($message);

if (isset($_SERVER['HTTP_ORIGIN'])) {
	$downstream_origin = $_SERVER['HTTP_ORIGIN'];
} elseif (isset($_SERVER['HTTP_REFERER'])) {
	$downstream_origin = http_build_scheme_host($_SERVER['HTTP_REFERER']);
}

if (isset($downstream_origin)) {
	foreach (Conf::$rwb_alt_url_bases as $alt_url_base) {
		if ($downstream_origin == http_build_scheme_host($alt_url_base)) {
			header('Access-Control-Allow-Origin: ' . $downstream_origin);
		}
	}
}

$message->send();