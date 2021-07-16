{
  description = "A flake for building Hello World";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/master;

  outputs = { self, nixpkgs }: let pkgs = import nixpkgs { system = "x86_64-linux"; }; in {

    defaultPackage.x86_64-linux =
      pkgs.stdenv.mkDerivation {
        name = "hello";
        src = self;
        buildInputs = [ pkgs.pkg-config pkgs.openssl pkgs.qt5.qtbase pkgs.libzip ];
        installPhase = ''
            mkdir -p $out/bin $out/lib
            cp libgourou.so $out/lib
            cp utils/{activate,acsmdownloader} $out/bin
        '';
      };

  };
}