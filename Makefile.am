ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
libquvi.pc.in \
libquvi.ver \
doxygen.conf.in \
ChangeLog.pre-gitlog

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libquvi.pc

SUBDIRS = doc src examples share scripts

if WITH_DOC
SUBDIRS += doc
endif

if ENABLE_TESTS
SUBDIRS += tests

check: test

test:
	@(cd tests; $(MAKE) all test)
endif

.PHONY: dox
dox:
if HAVE_DOXYGEN
	@$(DOXYGEN) $(top_builddir)/doxygen.conf
	@echo doxygen generated pages can be found in doc/ subdir
else
	@echo configure did not find doxygen
endif

local-clean:
	@rm -rf *.tmp *.core *.html

ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		git log --stat --after="Sun Nov 14 11:03:50 2010" > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

distclean-local: local-clean
