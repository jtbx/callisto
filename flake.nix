{
  description = "standalone scripting platform for Lua 5.4";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
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

        dontAddPrefix = true;
        installFlags = [
          "DESTDIR=$(out)"
          "PREFIX=/"
        ];
      };
    });
  };
}
