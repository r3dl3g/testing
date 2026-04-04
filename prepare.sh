. ./.project_defines

if [ "$PLATFORM" == "emc" ]; then
    INSTALL_PREFIX=~/.local/emc
    CMAKE_CMD="emcmake cmake"
elif [ "$PLATFORM" == "win32" ]; then
    INSTALL_PREFIX=~/.local/win32
    DEFINES=-DWIN32=ON
fi

prepare Debug $@
prepare Release $@
