{
  description = "Tretis' flake. I hope it works";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {self, nixpkgs} :
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
      };
    in
    {
      devShells.${system}.default = pkgs.mkShell.override { stdenv = pkgs.llvmPackages_19.stdenv; } {
        nativeBuildInputs = with pkgs; [
          expat
          fontconfig
          cmake
          xmake
        ];
        buildInputs = with pkgs; [
          sfml # developpement library

          cppcheck
          # /!\ Will need updates
          lldb_19
          (with llvmPackages_19; [
            clang-tools
            llvm-manpages
            libunwind
            libcxx
            bintools
          ])
        ];
      };    
    };
    
}
