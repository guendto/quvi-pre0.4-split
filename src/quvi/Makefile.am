if ENABLE_QUVI

EXTRA_DIST = cmdline.ggo

src = quvi.c cmdline.c \
    $(top_srcdir)/src/libquvi/strepl.c 

hdr = cmdline.h

if NO_ASPRINTF
src += $(top_srcdir)/src/libquvi/vasprintf.c
src += $(top_srcdir)/src/libquvi/asprintf.c
endif

bin_PROGRAMS = quvi
quvi_SOURCES = $(src) $(hdr)

quvi_CPPFLAGS = \
    -I$(top_builddir)/src/libquvi \
    -I$(top_srcdir)/src/libquvi \
    $(libcurl_CFLAGS) \
    $(AM_CPPFLAGS)

quvi_CFLAGS = \
    $(AM_CFLAGS)

quvi_LDFLAGS = \
    $(AM_LDFLAGS)

quvi_LDADD =  \
    $(top_builddir)/src/libquvi/libquvi.la \
    $(libcurl_LIBS) \
    $(LIBICONV) \
    $(LTLIBICONV)

BUILT_SOURCES = cmdline.c

cmdline.c: cmdline.ggo
if HAVE_GENGETOPT
	@GENGETOPT@ < $(top_srcdir)/src/quvi/cmdline.ggo -C --unamed-opts=URL \
		--output-dir $(top_builddir)/src/quvi --no-version
else
	@echo "error: configure did not find 'gengetopt' command" && exit 1
endif # HAVE_GENGETOPT

cmdline.h: cmdline.c

endif # ENABLE_QUVI
