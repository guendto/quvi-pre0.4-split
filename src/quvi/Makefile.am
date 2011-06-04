if ENABLE_QUVI

EXTRA_DIST = cmdline.ggo

cmdline.c: cmdline.ggo
	@test -f $(top_srcdir)/src/quvi/cmdline.c || \
		gengetopt < $(top_srcdir)/src/quvi/cmdline.ggo \
			-C --unamed-opts=URL --output-dir $(top_builddir)/src/quvi \
				--no-version

cmdline.h: cmdline.c

src = cmdline.c quvi.c $(top_srcdir)/src/libquvi/strepl.c
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

endif # ENABLE_QUVI
