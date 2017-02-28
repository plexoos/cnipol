with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "cnipol-shell";

  buildInputs = [
    boost
    cmake
    mariadb.client
    root
    sqlite.dev
  ];
}
