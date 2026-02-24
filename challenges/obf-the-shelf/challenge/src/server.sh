#!/bin/sh

echo -n "Please input flag: "
WINEPREFIX="/tmp/.wine" /usr/bin/wine ./challenge.exe && /bin/sh
