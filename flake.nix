{
	description = "Callisto, a featureful extension runtime for Lua 5.4";

	inputs = {
		nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
	};

	outputs = { self, nixpkgs }:
	let
		forAllSystems = fn:
			nixpkgs.lib.genAttrs [
				"x86_64-linux"
				"x86_64-darwin"
				"aarch64-linux"
			] (system:
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
