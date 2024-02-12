{
  description = "Callisto, a featureful extension runtime for Lua 5.4";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    systems = [
      "x86_64-linux"
      "aarch64-linux"
    ];
    forAllSystems = fn:
      nixpkgs.lib.genAttrs systems (system:
        fn (import nixpkgs {
          inherit system;
        })
      );
  in
  {
    packages = forAllSystems (pkgs: {
      default = pkgs.stdenv.mkDerivation {
        name = "callisto";
        src = ./.;

        nativeBuildInputs = with pkgs; [
          gcc
          binutils
        ];
        buildPhase = ''
          make
        '';
        installPhase = ''
          mkdir -p $out/bin
          make DESTDIR="$out" PREFIX=/ install
        '';
      };
    });
  };
}
