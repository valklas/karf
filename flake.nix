{
  description = "Development environment for karf - Kdenlive Automated Render Flow";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          # C Toolchain & XML Library
          gcc
          gnumake
          pkg-config
          libxml2
          
          # Runtime Dependencies
          kdePackages.kdenlive
          kdePackages.mlt      # Provides the 'melt' executable engine
          ffmpeg

          # Shell
          fish
        ];

        shellHook = ''
          echo "Entering karf development environment..."
          if [ -t 0 ] && [ "$IN_NIX_FISH" != "1" ]; then
            export IN_NIX_FISH=1
            exec fish
          fi
        '';
      };
    };
}
