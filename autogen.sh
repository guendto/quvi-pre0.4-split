#!/bin/sh
# autogen.sh for quvi.

source=.gitignore
cachedir=autom4te.cache

gen_cmdline()
{
  echo "Generate cmdline..."
  gengetopt < src/quvi/cmdline.ggo \
    -C --unamed-opts=URL --output-dir src/quvi --no-version
}

gen_manual()
{
  echo "Generate manual..."
  M4=m4/version.m4 ; MAN=doc/man1/quvi.1 ; POD=$MAN.pod
  VER=`perl -n -e'/(\d+)\.(\d+)\.(\d+)/ && print "$1.$2.$3"' < $M4` \
    && pod2man -c "quvi manual" -n quvi -s 1 -r $VER $POD $MAN
  return $?
}

cleanup()
{
  echo "WARNING!
Removes _files_ listed in $source and $cachedir directory.
Last chance to bail out (^C) before we continue."
  read n1
  for file in `cat $source`; do # Remove files only.
    [ -e "$file" ] && [ -f "$file" ] && rm -f "$file"
  done
  [ -e "$cachedir" ] && rm -rf "$cachedir"
  exit 0
}

help()
{
  echo "Usage: $0 [-c|-h]
-h  Show this help and exit
-c  Make the source tree 'maintainer clean'
Run without options to (re)generate the configuration files."
  exit 0
}

while [ $# -gt 0 ]
do
  case "$1" in
    -c) cleanup;;
    -h) help;;
    *) break;;
  esac
  shift
done

echo "Generate configuration files..."
autoreconf -if \
  && gen_cmdline \
    && gen_manual \
      && echo "You can now run 'configure'"
