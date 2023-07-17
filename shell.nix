{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
	nativeBuildInputs = with pkgs.buildPackages; [
		bmake
		clang
		readline
	];
	shellHook = "alias make=bmake";
}
