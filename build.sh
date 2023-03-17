export COMPILER_FLAGS="-Wall -Wno-format-security -Wno-format-nonliteral -Wno-write-strings -Wunused-variable"
export BUILD_TYPE=Release
export OUT_OF_SOURCE_BUILD=true

if [ "${OUT_OF_SOURCE_BUILD}" = "false" ] ; then unset OUT_OF_SOURCE_BUILD ; fi

. $HOME/.build_environment
. $HOME/opt/local/share/sur/build_environment.sh

export SKIP_ANALISIS=True
export COMPILER_PREFIX=i686-w64-mingw32
export MAKE_PARAMS="-j 1"

/bin/sh lib/suri/tools/ci_build.sh

cp -v WinBuild/*.exe LinuxBuild/*.deb /home/builder/packages

exit 0

