#!/bin/sh

# Configure
HOST=i486-mingw32 # ./configure --host=$HOST
PREFIX=`pwd`/dist # ./configure --prefix=$PREFIX
CFLAGS="-Wall -Werror -Os -pipe -march=i686"

# --

CURL_PREFIX=\
"/home/legatvs/src/non-installed/curl-7.20.1"

PCRE_PREFIX=\
"/home/legatvs/src/non-installed/pcre-8.02"

ICONV_PREFIX=\
"/home/legatvs/src/non-installed/libiconv-1.13.1"

LUA_PREFIX=\
"/home/legatvs/src/non-installed/lua-5.1.4"

# --

CURL_CONFIG=\
"$CURL_PREFIX/release/dist/bin/curl-config"

PCRE_CONFIG=\
"$PCRE_PREFIX/release/dist/bin/pcre-config"

ICONV_DIST=\
"$ICONV_PREFIX/release/dist"

LUA_DIST=\
"$LUA_PREFIX/release/dist"

pack_it()
{
    curl_prefix="`$CURL_CONFIG --prefix`"
    curl_dll="$curl_prefix/bin/libcurl-4.dll"

    pcre_prefix="`$PCRE_CONFIG --prefix`"
    pcre_dll="$pcre_prefix/bin/libpcre-0.dll"

    iconv_dll="$ICONV_DIST/bin/libiconv-2.dll"
    lua_dll="$LUA_DIST/bin/lua51.dll"

    version=`awk '/PACKAGE_VERSION = / {print $3}' Makefile`
    archive="quvi-$version-win32-i686-bin.7z"
    distdir="quvi-$version"

    rm -rf dist quvi-$version $archive \
    && make install-strip \
    && make man \
    && cp $curl_dll dist/bin \
    && cp $pcre_dll dist/bin \
    && cp $iconv_dll dist/bin \
    && cp $lua_dll dist/bin \
    && mkdir -p dist/licenses \
    && cp $CURL_PREFIX/COPYING dist/licenses/libcurl-COPYING.TXT \
    && cp $PCRE_PREFIX/LICENCE dist/licenses/libpcre-LICENSE.TXT \
    && cp $ICONV_PREFIX/COPYING.LIB dist/licenses/libiconv-COPYING.TXT \
    && cp $LUA_PREFIX/COPYRIGHT dist/licenses/liblua-COPYRIGHT.TXT \
    && cp -r ../share/lua dist/bin \
    && cp -r ../share/docs dist/ \
    && cp ../COPYING dist/licenses/quvi-COPYING.txt \
    && cp ../ChangeLog dist/ChangeLog.txt \
    && cp ChangeLog.w32.txt dist/ \
    && cp quvi.1.html dist/ \
    && rm -rf dist/share \
    && mv dist $distdir \
    && 7za a $archive $distdir
    exit $?
}

clean_up() {
    make distclean 2>/dev/null
    rm -rf docs examples include lib src tests share quvi.1
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

# No tweaking usually required.

export libcurl_CFLAGS="`$CURL_CONFIG --cflags`"
export libcurl_LIBS="`$CURL_CONFIG --libs`"

export libpcre_CFLAGS="`$PCRE_CONFIG --cflags`"
export libpcre_LIBS="`$PCRE_CONFIG --libs`"

export liblua_CFLAGS="-I$LUA_DIST/include"
export liblua_LIBS="-L$LUA_DIST/lib -llua51"

CFLAGS=$CFLAGS ../configure \
    --prefix="$PREFIX" \
    --host="$HOST" \
    --with-libiconv-prefix="$ICONV_PREFIX" \
    --enable-smut \
    --without-man
