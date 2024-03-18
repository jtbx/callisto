{
  description = "standalone scripting platform for Lua 5.4";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixpkgs-unstable;
  };

  outputs = { self, nixpkgs }:
  let
    plats = nixpkgs.lib.platforms.unix;
    forAllSystems = fn:
      nixpkgs.lib.genAttrs plats (system:
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
