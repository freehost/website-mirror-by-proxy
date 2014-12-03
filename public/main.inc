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

RedirectWhenBlockedFull::setUrls(Conf::$rwb_alt_url_bases, 
	Conf::$rwb_alt_url_collections);

TextExternalUrlFilters::addHost(Conf::getDefaultUpstreamBaseUrlComponent('host'));

DomUrlFilters::addAttribute('href');
DomUrlFilters::addAttribute('src');