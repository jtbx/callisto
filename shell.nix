{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
	nativeBuildInputs = with pkgs.buildPackages; [
		bmake
		clang
	];
	shellHook = "alias make=bmake";
}
