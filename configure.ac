#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])

m4_include([m4/version.m4])
AC_INIT([quvi], [_QUVI_VERSION], [legatvs@gmail.com])

# Interface:
QUVI_CURRENT=_QUVI_CURRENT
QUVI_REVISION=_QUVI_REVISION
QUVI_AGE=_QUVI_AGE
AC_SUBST(QUVI_CURRENT)
AC_SUBST(QUVI_REVISION)
AC_SUBST(QUVI_AGE)

AC_CONFIG_SRCDIR([lib/quvi.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config.aux])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM
AC_GNU_SOURCE

AC_DEFINE_UNQUOTED([CANONICAL_TARGET], "$target", [...])
AC_DEFINE_UNQUOTED([CANONICAL_HOST], "$host", [...])

AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2])

LT_INIT()
LT_PREREQ([2.2])

# Host specific.
NO_UNDEFINED=
case "$host" in
    *-*-mingw32* | *-*-cygwin*)
        AC_DEFINE([IS_W32], [1], [...])
        is_w32=yes
        if [ test x"$enable_shared" != "xno" ]; then
            NO_UNDEFINED="-no-undefined"
        fi
    ;;
esac
EXTRA_LIBQUVI_LDFLAGS="$NO_UNDEFINED"
AC_SUBST(EXTRA_LIBQUVI_LDFLAGS)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

AC_PATH_PROG(GENGETOPT, gengetopt, no)
AM_CONDITIONAL([HAVE_GENGENTOPT], [test x"$GENGETOPT" != "xno"])
AC_SUBST(GENGETOPT)

AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL([HAVE_DOXYGEN], [test x"$DOXYGEN" != "xno"])
AC_SUBST(DOXYGEN)

build_date=
AC_PATH_PROG(DATE, date, no)
if [ test x"$DATE" != "xno" ]; then
    build_date=`"$DATE" +%Y%m%d`
fi
AC_DEFINE_UNQUOTED([BUILD_DATE], "$build_date", [...])

# Checks for libraries.
AM_ICONV
PKG_CHECK_MODULES(libcurl, libcurl >= 7.18.0)
PKG_CHECK_MODULES(libpcre, libpcre >= 7.8)

# Checks for header files.
AC_HEADER_STDC # `stdlib.h', `stdarg.h', `string.h', ...
AC_CHECK_HEADERS([memory.h stdint.h stdlib.h limits.h stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
if [ test x"$is_w32" = "xno" ]; then
    AC_FUNC_MALLOC
    AC_FUNC_REALLOC
fi
AC_CHECK_FUNCS([memset strdup strstr strtol strerror])

have_asprintf=no
AC_CHECK_FUNCS([vasprintf], [
    AC_CHECK_FUNCS([asprintf], [have_asprintf=yes])
])
AM_CONDITIONAL([NO_ASPRINTF], [test x"$have_asprintf" != "xyes"])

have_strupr=no
AC_CHECK_FUNCS([strupr], [have_strupr=yes])
AM_CONDITIONAL([NO_STRUPR], [test x"$have_strupr" != "xyes"])

have_strtok_r=no
AC_CHECK_FUNCS([strtok_r], [have_strtok_r=yes])
AM_CONDITIONAL([NO_STRTOK_R], [test x"$have_strtok_r" != "xyes"])

# --enable-exe
AC_ARG_ENABLE([exe],
    [AS_HELP_STRING([--enable-exe],
        [build quvi(1) executable @<:@default=yes@:>@])],
    [],
    [enable_exe=yes])
AM_CONDITIONAL([ENABLE_EXE], [test x"$enable_exe" != "xno"])

# --enable-smut
AC_ARG_ENABLE([smut],
    [AS_HELP_STRING([--enable-smut],
        [build with adult website support @<:@default=no@:>@])],
    [],
    [enable_smut=no])
AM_CONDITIONAL([ENABLE_SMUT], [test x"$enable_smut" != "xno"])

if [ test x"$enable_smut" != "xno" ]; then
    AC_DEFINE([ENABLE_SMUT], [1], [Define to enable smut])
fi

# GNU ld?
AM_CONDITIONAL([HAVE_GNU_LD], [test x"$with_gnu_ld" = "xyes"])

AC_CONFIG_FILES([
    Makefile
    include/Makefile
    include/quvi/Makefile
    lib/Makefile
    src/Makefile
    examples/Makefile
    libquvi.pc
    doxygen.conf
])

AC_OUTPUT


