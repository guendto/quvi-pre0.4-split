
EXTRA_DIST = quviTest.pm.in $(TESTS) $(TESTS_ADULT)

if HAVE_PERL

TESTS_ENVIRONMENT = $(PERL) -w -mstrict -mquviTest -I $(top_builddir)/tests

TESTS =\
nosupport-eval.pl\
support-break.pl\
support-cctv.pl\
support-clipfish.pl\
support-dailymotion.pl\
support-ehrensenf.pl\
support-evisor.pl\
support-funnyhub.pl\
support-golem.pl\
support-liveleak.pl\
support-myubo.pl\
support-sevenload.pl\
support-spiegel.pl\
support-spyfilms.pl\
support-vimeo.pl\
support-youtube.pl

TESTS_ADULT =\
support-tube8.pl\
support-xvideos.pl\
support-youjizz.pl

if ENABLE_SMUT
TESTS += $(TESTS_ADULT)
endif

else # HAVE_PERL
	@echo SKIP: perl not found.
endif # HAVE_PERL

local-clean:
	@rm -rf quvi/quviTest.pm

distclean-local: local-clean


