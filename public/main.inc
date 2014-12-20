<?php
require 'Conf.inc';
require 'filters/DomUrlFilters.inc';
require 'filters/TextExternalUrlFilters.inc';
require 'filters/TextInternalUrlFilters.inc';
require 'Log.inc';
require 'ProxyHttpRequest.inc';
require 'simple_html_dom.php';
require 'url.inc';
require 'rwb/RedirectWhenBlockedFull.inc';

require 'conf-local.inc';

function removeHeaderFromMessage(HttpMessage &$message, $header_name)
{
	$headers = $message->getHeaders();
	if (isset($headers[$header_name])) {
		unset($headers[$header_name]);
		$message->setHeaders($headers);
	}
}

RedirectWhenBlockedFull::setUrlsFromConfDir();

TextExternalUrlFilters::addHost(
	Conf::getDefaultUpstreamBaseUrlComponent('host'));

DomUrlFilters::addAttribute('action');
DomUrlFilters::addAttribute('href');
DomUrlFilters::addAttribute('src');