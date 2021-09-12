Introduction
------------

A very simple PDF parser that will load PDF objects without interpretation (zlib, streams, string encoding...).
It currently only allows to update PDF file with new objects.


Compilation
-----------

Use _make_ command

    make [CROSS=XXX] [DEBUG=1] [BUILD_STATIC=(0|1)] [BUILD_SHARED=(0|1)]

CROSS can define a cross compiler prefix (ie arm-linux-gnueabihf-)

DEBUG can be set to compile in DEBUG mode

BUILD_STATIC build libupdfparser.a if 1, nothing if 0 (default value), can be combined with BUILD_SHARED

BUILD_SHARED build libupdfparser.so if 1 (default value), nothing if 0, can be combined with BUILD_STATIC


Copyright
---------

Grégory Soutadé


License
-------

LGPL v3 or later
