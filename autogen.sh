#!/bin/sh
# autogen.sh for quvi.

source=.gitignore
cachedir=autom4te.cache

cleanup()
{
    echo "WARNING!
Removes _files_ listed in $source and $cachedir directory.
Last chance to bail out (^C) before we continue."
    read -s n1
    for file in `cat $source`; do # Remove files only.
        [ -e "$file" ] && [ -f "$file" ] && rm "$file"
    done
    [ -e "$cachedir" ] && rm -rf "$cachedir"
    exit $?
}

help()
{
    echo "Usage: $0 [-c|-h]
-h  Show this help and exit
-c  Make the source tree 'maintainer clean'
Run without options to (re)generate the configuration files."
    exit 0
}

clean_flag=off
while [ $# -gt 0 ]
do
    case "$1" in
        -c) cleanup;;
        -h) help;;
         *) break;;
    esac
    shift
done

autoreconf -if && echo "You can now run 'configure'"
