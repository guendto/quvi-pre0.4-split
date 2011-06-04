
EXTRA_DIST = quvi.1.pod

if WITH_MAN
dist_man_MANS = quvi.1

quvi.1: quvi.1.pod
if HAVE_POD2MAN
	@POD2MAN@ -c "quvi manual" -n quvi -s 1 -r @VERSION@ \
		$(top_srcdir)/man1/$@.pod $@
else
	@echo "configure did not find pod2man" && exit 1
endif # HAVE_POD2MAN

dist-hook: quvi.1

endif # WITH_MAN
