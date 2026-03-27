{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
    esp-idf.url = "github:mirrexagon/nixpkgs-esp-dev";
  };

  outputs =
    {
      nixpkgs,
      utils,
      esp-idf,
      ...
    }:

    utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config = {
            permittedInsecurePackages = [
              "python3.13-ecdsa-0.19.1"
            ];
          };
          overlays = [
            esp-idf.overlays.default
          ];
        };

        platformSrc =
          if system == "x86_64-linux" then
            {
              url = "https://github.com/kassane/zig-espressif-bootstrap/releases/download/0.16.0-xtensa-dev/zig-relsafe-x86_64-linux-musl-baseline.tar.xz";
              sha256 = "sha256-xSo6FlCQneMK/OQu786dLg0FJ2CqaOrYuWN++er41zg=";
            }
          else if system == "aarch64-linux" then
            {
              url = "https://github.com/kassane/zig-espressif-bootstrap/releases/download/0.16.0-xtensa-dev/zig-relsafe-aarch64-linux-musl-baseline.tar.xz";
              sha256 = "sha256-UwG0NfBe+uI1qDb5Bzn9Dqxxn1NYOx3bdUALvkGbpAk=";
            }
          else if system == "aarch64-darwin" then
            {
              url = "https://github.com/kassane/zig-espressif-bootstrap/releases/download/0.16.0-xtensa-dev/zig-relsafe-aarch64-macos-baseline.tar.xz";
              sha256 = "sha256-z7msbLcOIufnMSQ6ak4WawTl+m8DhkLiJuwrT2rG7q8=";
            }
          else
            throw "Unsupported platform: ${system}";
      in
      with pkgs;
      {
        devShells.default = mkShell {
          buildInputs = [
            bashInteractive

            (pkgs.stdenv.mkDerivation {
              pname = "zig-espressif-bootstrap";
              version = "0.16.0-xtensa-dev";
              src = pkgs.fetchurl platformSrc;
              dontConfigure = true;
              dontBuild = true;
              dontFixup = true;
              installPhase = ''
                mkdir -p $out/{doc,bin,lib}
                cp -r doc/* $out/doc
                cp -r lib/* $out/lib
                cp zig $out/bin/zig
              '';
            })

            esp-idf-full
            pkgs.zls
            clang-tools
          ];
        };
      }
    );
}
