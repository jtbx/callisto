{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
	nativeBuildInputs = with pkgs.buildPackages; [
		bmake
		clang

		lua53Packages.ldoc
	];
	shellHook = "alias make=bmake";
}
