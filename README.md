# libgourou-utils

libgourou is a library created by Grégory Soutadé located [here](http://indefero.soutade.fr/p/libgourou/). It exposes utility binaries to utilize the library. Here I have packaged the entire project up with the [Nix package manager](https://en.m.wikipedia.org/wiki/Nix_package_manager) with as few changes as possible.

The library is licensed under LGPLv3 or later and the utilities are licensed under the 3-clause BSD license, so I will operate as if the entire project is under LGPLv3 because it is more restrictive than the 3-clause BSD license. The LGPLv3 requires that I disclose the dates and contents of all changes made to the original software. These can be found in the [commit history](https://github.com/BentonEdmondson/libgourou-utils/commits/main).

I packaged the library for use in my CLI application called [Knock](https://github.com/BentonEdmondson/knock).

The original README is below.

---

Introduction
------------

libgourou is a free implementation of Adobe's ADEPT protocol used to add DRM on ePub files. It overcome the lacks of Adobe support for Linux platforms.


Architecture
------------

Like RMSDK, libgourou has a client/server scheme. All platform specific functions (crypto, network...) has to be implemented in a client class (that derives from DRMProcessorClient) while server implements ADEPT protocol.
A reference implementation using Qt, OpenSSL and libzip is provided (in _utils_ directory).

Main fucntions to use from gourou::DRMProcessor are :

  * Get an ePub from an ACSM file : _fulfill()_ and _download()_
  * Create a new device : _createDRMProcessor()_
  * Register a new device : _signIn()_ and _activateDevice()_


You can import configuration from (at least) :

  * Kobo device : .adept/device.xml, .adept/devicesalt  and .adept/activation.xml
  * Bookeen device : .adobe-digital-editions/device.xml, root/devkey.bin and .adobe-digital-editions/activation.xml
  
Or create a new one. Be careful : there is a limited number of devices that can be created bye one account.

ePub are encrypted using a shared key : one account / multiple devices, so you can create and register a device into your computer and read downloaded (and encrypted) ePub file with your eReader configured using the same AdobeID account.


Dependencies
------------

For libgourou :

  * None

For utils :

  * QT5Core
  * QT5Network
  * OpenSSL
  * libzip


Compilation
-----------

Use _make_ command

    make [CROSS=XXX] [DEBUG=1] [STATIC_UTILS=1]

CROSS can define a cross compiler prefix (ie arm-linux-gnueabihf-)

DEBUG can be set to compile in DEBUG mode

STATIC_UTILS to build utils with static library (libgourou.a) instead of default dynamic one (libgourou.so)


Utils
-----

You can import configuration from your eReader or create a new one with utils/activate :

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD
    ./utils/activate -u <AdobeID USERNAME>

Then a _./.adept_ directory is created with all configuration file

To download an ePub :

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD
    ./utils/acsmdownloader -f <ACSM_FILE>


Copyright
---------

Grégory Soutadé



License
-------

libgourou : LGPL v3 or later

utils     : BSD



Special thanks
--------------

  * _Jens_ for all test samples and utils testing
