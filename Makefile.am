ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
libquvi.pc.in \
doxygen.conf.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libquvi.pc

SUBDIRS = include lib src examples docs

dox:
if HAVE_DOXYGEN
	@$(DOXYGEN) $(top_builddir)/doxygen.conf
	@echo doxygen generated pages can be found in docs/ subdir
else
	@echo configure did not find `doxygen'
endif


