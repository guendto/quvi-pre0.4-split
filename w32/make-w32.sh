#!/bin/sh

PREFIX=`pwd`/dist
MINGW=/opt/mingw32

export CFLAGS="-Wall -Werror -Os -pipe -march=i686"

export libcurl_CFLAGS="`$MINGW/bin/curl-config --cflags`"
export libcurl_LIBS="`$MINGW/bin/curl-config --libs`"

export libpcre_CFLAGS="`$MINGW/bin/pcre-config --cflags`"
export libpcre_LIBS="`$MINGW/bin/pcre-config --libs`"

export liblua_CFLAGS="-I$MINGW/include"
export liblua_LIBS="-L$MINGW/lib -llua51"

pack_it()
{
    version=`awk '/PACKAGE_VERSION = / {print $3}' Makefile`
    archive="quvi-$version-win32-i686-bin.7z"
    distdir="quvi-$version"

    rm -rf dist quvi-$version $archive \
    && make install-strip \
    && make pod2html \
    && cp -v $MINGW/bin/libcurl-4.dll dist/bin \
    && cp -v $MINGW/bin/libpcre-0.dll dist/bin \
    && cp -v $MINGW/bin/libiconv-2.dll dist/bin \
    && cp -v $MINGW/bin/lua51.dll dist/bin \
    && mkdir -p dist/licenses \
    && cp -v -r licenses dist/licenses \
    && mv dist/share/quvi/lua dist/bin \
    && rm -r dist/share/man dist/share/quvi \
    && rm -rf dist/lib/pkgconfig \
    && cp -v ../ChangeLog dist/ChangeLog.txt \
    && cp -v ChangeLog.w32.txt dist/ \
    && cp -v man1/quvi.1.html dist/share/doc/quvi \
    && mv dist $distdir \
    && 7za a $archive $distdir
    exit $?
}

clean_up() {
    make distclean 2>/dev/null
    rm -rf doc examples include lib src tests share man1
    exit $?
}

pack_flag=off
clean_flag=off
while [ $# -gt 0 ]
do
    case "$1" in
        -p) pack_flag=on;;
        -c) clean_flag=on;;
         *) break;;
    esac
    shift
done

if [ x"$pack_flag" = "xon" ]; then
    pack_it
fi

if [ x"$clean_flag" = "xon" ]; then
    clean_up
fi

../configure \
    --host=i486-mingw32 \
    --prefix=$PREFIX \
    --with-libiconv-prefix=$MINGW \
    --without-man


