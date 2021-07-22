{
  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;

  outputs = { self, ... }@flakes: let
    nixpkgs = flakes.nixpkgs.legacyPackages.x86_64-linux;
  in {
    defaultPackage.x86_64-linux = nixpkgs.stdenv.mkDerivation {
        pname = "libgourou-utils";
        version = "0.1";
        src = self;
        nativeBuildInputs = [ nixpkgs.pkg-config ];
        buildInputs = [ nixpkgs.openssl nixpkgs.qt5.qtbase nixpkgs.libzip nixpkgs.pugixml ];
        installPhase = ''
            mkdir -p $out/bin $out/lib
            cp libgourou.so $out/lib
            cp utils/{activate,acsmdownloader} $out/bin
        '';
        dontWrapQtApps = true;
      };
  };
}