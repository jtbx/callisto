{
  description = "standalone scripting platform for Lua 5.4";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixpkgs-unstable;
  };

  outputs = { self, nixpkgs }:
  with nixpkgs.lib;
  let
    forAllSystems = fn:
      genAttrs platforms.unix (system:
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
