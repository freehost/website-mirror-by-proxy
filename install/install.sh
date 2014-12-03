#!/bin/bash
function pecl_install {
	pecl install $1
	echo "extension=$2.so" | sudo tee /etc/php5/mods-available/$2.ini
	php5enmod $2
}

if [ $(/usr/bin/id -u) -ne 0 ]; then
    echo "Not running as root"
    exit
fi

echo "Installing Apache and PHP"
apt-get install apache2 php5

echo "Installing HTTP extension"
apt-get install libpcre3-dev libcurl3-openssl-dev php5-dev php-http php5-mcrypt php-pear
pecl_install pecl/raphf raphf
pecl_install pecl/propro propro
pecl_install pecl_http-1.7.6 http

echo "Enabling Apache modules"
a2enmod rewrite
