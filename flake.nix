{
  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;

  outputs = { self, nixpkgs }: let pkgs = import nixpkgs { system = "x86_64-linux"; }; in {

    defaultPackage.x86_64-linux =
      pkgs.stdenv.mkDerivation {
        pname = "libgourou-utils";
        version = "0.1";
        src = self;
        buildInputs = [ pkgs.pkg-config pkgs.openssl pkgs.qt5.qtbase pkgs.libzip pkgs.pugixml ];
        installPhase = ''
            mkdir -p $out/bin $out/lib
            cp libgourou.so $out/lib
            cp utils/{activate,acsmdownloader} $out/bin
        '';
        dontWrapQtApps = true;
      };

  };
}