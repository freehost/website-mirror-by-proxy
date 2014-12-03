<?php

function http_build_scheme_host($mixed)
{
	if (is_array($mixed)) {
		$components = $mixed;
	} else {
		$components = parse_url($mixed);
	}
	
	$scheme_and_host = $components['scheme'] . '://';
	$scheme_and_host .= $components['host'];
	return $scheme_and_host;
}

function http_build_path_query_fragment($mixed)
{
	if (is_array($mixed)) {
		$components = $mixed;
	} else {
		$components = parse_url($mixed);
	}
	
	$url = '';
	if (isset($components['path'])) {
		$url .= $components['path'];
	}
	if (isset($components['query'])) {
		$url .= '?' . $components['query'];
	}
	if (isset($components['fragment'])) {
		$url .= '#' . $components['fragment'];
	}
	return $url;
}