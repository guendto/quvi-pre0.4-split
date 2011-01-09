#!/bin/sh

# Applies our project-wide indentation rules to C source code.
# Note that the getopt generated src/cmdline.* files will be ignored.

export SIMPLE_BACKUP_SUFFIX=.indent_c

INDENT="indent -linux --no-tabs -i2 -l72"

find ./lib -name '*.c' -exec $INDENT {} \; \
&& find ./lib -name '*.h' -exec $INDENT {} \; \
&& find ./examples -name '*.c' -exec $INDENT {} \; \
&& $INDENT src/quvi.c \
&& find . -name "*$SIMPLE_BACKUP_SUFFIX" -exec rm {} \;

exit $?
