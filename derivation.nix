# https://discourse.nixos.org/t/how-to-use-a-nix-derivation-from-a-local-folder/5498/4

{ stdenv, lib, pkgs } :

stdenv.mkDerivation {
  pname = "testing";
  version = "1.1.0";

  src = ./.;

  nativeBuildInputs = with pkgs; [ cmake ];

  enableParallelBuilding = true;

  outputs = [ "out" "dev" ];

  cmakeFlags = [
    "-DTESTING_CONFIG_INSTALL=ON"
    "-DTESTING_BUILD_STATIC_MODULE_LIB=OFF"
  ];

  meta = with lib; {
    description = "A c++ testing framework";
    homepage = "https://github.com/r3dl3g/testing";
    changelog = "https://github.com/r3dl3g/testing/releases/tag/v${version}";
    license = licenses.mit;
    maintainers = with maintainers; [ r3dl3g ];
    platforms = with platforms; linux;
  };
}
