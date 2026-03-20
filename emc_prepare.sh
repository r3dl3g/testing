. ./.project_defines

BUILD_PREFIX=~/.cache/build/emc
INSTALL_PREFIX=~/.local/emc
CMAKE_CMD="emcmake cmake"

prepare Debug $@
prepare Release $@
