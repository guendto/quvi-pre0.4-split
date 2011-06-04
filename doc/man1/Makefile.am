
EXTRA_DIST = quvi.1.pod

if WITH_MAN
dist_man_MANS = quvi.1

quvi.1: quvi.1.pod
	@test -f $(top_srcdir)/doc/man1/quvi.1 || \
		@POD2MAN@ -c "quvi manual" -n quvi -s 1 -r @VERSION@ \
			$(top_srcdir)/doc/man1/$@.pod $@

endif # WITH_MAN
