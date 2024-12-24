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
          cppcheck

          # /!\ Will need updates
          lldb_19
          clang_19
          (with llvmPackages_19; [
            stdenv
            clang-tools
            llvm-manpages
            # libunwind
            # libllvm
            libcxx
            bintools
          ])
        ];
      };    
    };
    
}
