
builddir=build/$(c++ --version | { read first rest ; echo $first ; })$(c++ -dumpversion)
prjdir=$PWD

build () {
  target=$1
  mkdir -p $builddir/$target
  pushd $builddir/$target
  cmake "$prjdir" -G"Unix Makefiles" -DCMAKE_BUILD_TYPE="$1"
  cmake --build .
  #make -j6
  popd
}

build Debug
build Release
