{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, ... }@flakes: let
    nixpkgs = flakes.nixpkgs.legacyPackages.x86_64-linux;
  in {
    defaultPackage.x86_64-linux = nixpkgs.stdenv.mkDerivation {
        pname = "libgourou-utils";
        version = "0.3.1";
        src = self;
        nativeBuildInputs = [ nixpkgs.pkg-config ];
        buildInputs = [ nixpkgs.openssl nixpkgs.qt5.qtbase nixpkgs.libzip nixpkgs.pugixml ];
        installPhase = ''
            mkdir -p $out/bin $out/lib
            cp libgourou.so $out/lib
            cp utils/adept_activate $out/bin/adept-register
            cp utils/acsmdownloader $out/bin/adept-download
        '';
        dontWrapQtApps = true;

        meta = {
          description = "An implementation of Adobe ADEPT for Linux";
          homepage = "https://github.com/BentonEdmondson/libgourou-utils";
          license = [ nixpkgs.lib.licenses.lgpl3Plus nixpkgs.lib.licenses.bsd3 ];
          maintainers = [{
            name = "Benton Edmondson";
            email = "bentonedmondson@gmail.com";
          }];
          # potentially others, but I'm only listed those tested
          platforms = [ "x86_64-linux" ];
        };
      };
  };
}