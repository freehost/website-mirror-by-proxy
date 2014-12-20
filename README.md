Website-mirror-by-proxy is a server-side web proxy designed to host one or multiple dynamic mirror versions of any website. It is based on https://github.com/greatfire/redirect-when-blocked (the full edition). Whereas redirect-when-blocked requires the source/origin website to be modifed, website-mirror-by-proxy runs separately and does not need any modification of the source/origin website.

## How to set up
1. Install the required dependencies. If you are using Ubuntu or a similar OS you can use the install.sh script. Otherwise, manually install Apache, the Apache rewrite module, PHP and the PHP HTTP extension (http://php.net/manual/en/book.http.php). It has to be version 1 of the HTTP extension since version 2 is not backward compatible). The specific version used in the install script and which this project has been tested successfully on is pecl_http-1.7.6.
2. Copy conf-local-template.inc to conf-local.inc.
3. In the conf-local.inc file, add the Conf::$default_upstream_base_url that you want to proxy. The URL should be formatted like this: scheme://domain, without any trailing slash or path. Example: http://example.com.
4. Add an "AllowOverride All" directive to the Apache site configuration to allow the .htaccess files to be parsed.
5. Access the site...

There are many configuration settings, some of which are used by default in the 'main.inc' file. The static classes in the filters directory can be used to fix broken URL rewrites (usually because of URLs generated in javascript) and to proxy URLs on third-party hosts.
