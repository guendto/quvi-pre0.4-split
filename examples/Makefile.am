
if ENABLE_EXAMPLES
noinst_PROGRAMS = simple callback_libsoup

simple_SOURCES = simple.c common.c common.h
simple_CPPFLAGS = -I$(top_builddir)/include $(AM_CPPFLAGS)
simple_CFLAGS = $(AM_CFLAGS)
simple_LDFLAGS = $(top_builddir)/lib/libquvi.la $(AM_LDFLAGS)

callback_libsoup_SOURCES= callback_libsoup.c common.c common.h
callback_libsoup_CPPFLAGS=-I$(top_builddir)/include $(libsoup_CFLAGS) $(AM_CPPFLAGS)
callback_libsoup_CFLAGS= $(AM_CFLAGS)
callback_libsoup_LDFLAGS=$(top_builddir)/lib/libquvi.la $(libsoup_LIBS) $(AM_LDFLAGS)

endif


