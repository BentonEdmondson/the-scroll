# the-scroll

This is a fork of Grégory Soutadé's [`libgourou`](https://indefero.soutade.fr/p/libgourou/). `libgourou` is a C++ library for interacting with Adobe servers using the ADEPT protocol in order to convert ACSM files into EPUB/PDF files. The goal of this fork is to modify the library such that:

* usage doesn't require the dependent software to implement `dmrprocessorclient` (about 600 lines of code)
* it can be built completely statically
  * this involves the removal of dependence on Qt
* it is packaged with Nix

I am doing this for use in my project called [Knock](https://github.com/BentonEdmondson/knock).