#!/bin/sh

# Configure
HOST=i486-mingw32 # ./configure --host=$HOST
PREFIX=`pwd`/dist # ./configure --prefix=$PREFIX
#CFLAGS="-Wall -Werror -g -pipe"
CFLAGS="-Wall -Werror -Os -pipe -march=i686"

CURL_CONFIG=\
"/home/legatvs/src/non-installed/curl-7.19.6/debug/dist/bin/curl-config"

PCRE_CONFIG=\
"/home/legatvs/src/non-installed/pcre-8.00/debug/dist/bin/pcre-config"

ICONV_PREFIX=\
"/home/legatvs/src/non-installed/libiconv-1.13.1/debug/dist"

pack_it()
{
    curl_prefix="`$CURL_CONFIG --prefix`"
    curl_dll="$curl_prefix/bin/libcurl-4.dll"

    pcre_prefix="`$PCRE_CONFIG --prefix`"
    pcre_dll="$pcre_prefix/bin/libpcre-0.dll"

    version=`awk '/PACKAGE_VERSION = / {print $3}' Makefile`
    archive="quvi-$version-win32-i686-bin.7z"
    distdir="quvi-$version"

    iconv_dll="$ICONV_PREFIX/bin/libiconv-2.dll"

    rm -rf dist quvi-$version $archive \
    && make install \
    && make dox \
    && cp $curl_dll dist/bin \
    && cp $pcre_dll dist/bin \
    && cp $iconv_dll dist/bin \
    && cp ../COPYING dist/quvi-COPYING.TXT \
    && cp ../ChangeLog dist/ChangeLog.TXT \
    && rm -rf dist/share \
    && cp -r docs/html dist/docs \
    && mv dist $distdir \
    && 7za a $archive $distdir
    exit $?
}

clean_up() {
    make distclean 2>/dev/null
    rm -rf docs examples include lib src
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

CFLAGS=$CFLAGS ../configure \
    --prefix="$PREFIX" \
    --host="$HOST" \
    --with-libiconv-prefix="$ICONV_PREFIX"


