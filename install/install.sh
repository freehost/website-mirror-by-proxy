#!/bin/bash
if [ $(/usr/bin/id -u) -ne 0 ]; then
    echo "Not running as root"
    exit
fi

echo "Installing general dependencies"
apt-get install apache2 php5

echo "Installing PHP cURL extension"
apt-get install curl libcurl3 libcurl3-dev php5-curl

echo "Enabling Apache modules"
a2enmod rewrite
