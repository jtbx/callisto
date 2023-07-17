{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
	nativeBuildInputs = with pkgs.buildPackages; [
		bmake
		gcc
		readline
	];
	shellHook = "alias make=bmake";
}
