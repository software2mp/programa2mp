export COMPILER_FLAGS="-Wall -Wno-format-security -Wno-format-nonliteral -Wno-write-strings -Wunused-variable"
export BUILD_TYPE=Release
export OUT_OF_SOURCE_BUILD=true

if [ "${OUT_OF_SOURCE_BUILD}" == "false" ] ; then unset OUT_OF_SOURCE_BUILD ; fi

. $HOME/.build_environment
. $HOME/opt/local/share/sur/build_environment.sh

export SKIP_ANALISIS=True
export COMPILER_PREFIX=i686-w64-mingw32

/bin/sh lib/suri/tools/ci_build.sh

exit 0
# Generacion del paquete .deb
# ver si es necesario eliminarlo antes
# Esto paso es necesario ya que ci_build.sh configura cmake on-the-fly y no carga
# la configuracion del proyecto para la generacion del .deb
cmake .
cpack .

deb_file=$(ls *.deb 2> /dev/null)

# fix de permisos sobre la generacion del .deb
mkdir fix_up_deb
dpkg-deb -x $deb_file fix_up_deb
dpkg-deb --control $deb_file fix_up_deb/DEBIAN
rm $deb_file
chmod 0644 fix_up_deb/DEBIAN/md5sums
find -type d -print0 |xargs -0 chmod 755
fakeroot dpkg -b fix_up_deb $deb_file
rm -rf fix_up_deb
# fin fix de permisos


exit 0

