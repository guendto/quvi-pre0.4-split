ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
libquvi.pc.in \
libquvi.ver \
ChangeLog.pre-gitlog

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libquvi.pc

SUBDIRS = man1 examples share scripts src

if WITH_DOC
SUBDIRS += doc
endif

if ENABLE_TESTS
SUBDIRS += tests

check: test

test:
	@(cd tests; $(MAKE) all test)
endif

.PHONY: pod2man pod2html cmdline doc dist-doc
pod2man:
if HAVE_POD2MAN
	@@POD2MAN@ -c "quvi manual" -n quvi \
		-s 1 -r @VERSION@ $(top_srcdir)/man1/quvi.1.pod \
		$(top_builddir)/man1/quvi.1 && echo See man1/
else
	@echo "error: configure did not find pod2man command"
endif
pod2html:
if HAVE_POD2HTML
	@@POD2HTML@ --title="quvi manual" \
		--infile=$(top_srcdir)/man1/quvi.1.pod \
        --outfile=$(top_builddir)/man1/quvi.1.html && echo See man1/
else
	@echo "error: configure did not find pod2html command"
endif
cmdline:
if HAVE_GENGETOPT
	@@GENGETOPT@ < $(top_srcdir)/src/quvi/cmdline.ggo -C --unamed-opts=URL \
		--output-dir $(top_srcdir)/src/quvi --no-version
else
	@echo "error: configure did not find 'gengetopt' command"
endif
doc:
if HAVE_ND
	@test -d $(top_builddir)/doc/quvi-@VERSION@ || \
		mkdir $(top_builddir)/doc/quvi-@VERSION@
	@test -d $(top_builddir)/doc/nd_temp || \
		mkdir $(top_builddir)/doc/nd_temp
	@@ND@ -i $(top_srcdir)/doc/ -i $(top_srcdir)/src/ \
		-o $(ND_FORMAT) $(top_builddir)/doc/quvi-@VERSION@/ \
		-p $(top_builddir)/doc/nd_temp/ && echo See doc/quvi-@VERSION@
else
	@echo "error: configure did not find NaturalDocs" && exit 1
endif
dist-doc: doc
	@tar chof - $(top_builddir)/doc/quvi-@VERSION@ | \
			GZIP=--best gzip -c >quvi-@VERSION@-doc.tar.gz \
			&& echo Created quvi-@VERSION@-doc.tar.gz

ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		@GIT@ log --stat --after="Sun Nov 14 11:03:50 2010" > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog
