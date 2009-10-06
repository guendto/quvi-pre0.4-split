if ENABLE_EXE
EXTRA_DIST = \
    makecmdline \
    cmdline.ggo \
    README

src = \
    quvi.c \
    cmdline.c

hdr = \
    cmdline.h

bin_PROGRAMS = quvi
quvi_SOURCES = $(src) $(hdr)

quvi_CFLAGS = \
    -I$(top_srcdir)/include \
    $(libcurl_CFLAGS) \
    $(libpcre_CFLAGS)

quvi_LDADD =  \
    $(top_builddir)/lib/libquvi.la \
    $(libcurl_LIBS) \
    $(libpcre_LIBS) \
    $(LIBICONV) \
    $(LTLIBICONV)
endif

