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

RedirectWhenBlockedFull::setUrlsFromConfDir();

TextExternalUrlFilters::addHost(Conf::getDefaultUpstreamBaseUrlComponent('host'));

DomUrlFilters::addAttribute('href');
DomUrlFilters::addAttribute('src');