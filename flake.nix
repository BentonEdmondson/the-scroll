{
    inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    inputs.updfparser = {
        url = "git://soutade.fr/updfparser";
        flake = false;
    };
    inputs.base64 = {
        url = "git+https://gist.github.com/f0fd86b6c73063283afe550bc5d77594.git";
        flake = false;
    };

    outputs = flakes: let
        nixpkgs = flakes.nixpkgs.legacyPackages.x86_64-linux;
        self = flakes.self;
        updfparser = flakes.updfparser;
        base64 = flakes.base64;
    in {
        defaultPackage.x86_64-linux = nixpkgs.stdenv.mkDerivation {
            pname = "the-scroll";
            version = "0.5.3";
            src = [
                self
                updfparser
                base64
            ];
            nativeBuildInputs = [
                nixpkgs.pkg-config
                nixpkgs.meson
                nixpkgs.ninja
            ];
            buildInputs = [
                nixpkgs.openssl
                nixpkgs.qt5.qtbase
                nixpkgs.libzip
                nixpkgs.pugixml
            ];
            unpackPhase = ''
                mkdir -p lib/updfparser lib/base64
                cp -r ${updfparser}/* lib/updfparser
                cp -r ${base64}/* lib/base64
                cp -r ${self}/* .
            '';
            configurePhase = ''
                meson build
            '';
            buildPhase = ''
                cd build
                ninja
            '';
            installPhase = ''
                mkdir -p $out/lib $out/bin
                cp libthe-scroll.a $out/lib/the-scroll.a
                cp gourou-activate $out/bin
                cp gourou-download $out/bin
                cp gourou-dedrm $out/bin
            '';
            dontWrapQtApps = true;
        };
    };
}