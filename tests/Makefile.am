
TESTS_T = \
t/format_default_nonhttp.t \
t/mem.t \
t/nosupport.t \
t/redirect.t \
t/format_default.t \
t/format_other.t \
t/shortened.t

TESTS_NSFW_T = t/format_default_nsfw.t
TESTS_M_T = t/d/url.t
TESTS_TODO_T =

if ENABLE_NSFW
TESTS_T += $(TESTS_NSFW_T)
endif

if ENABLE_TODO
TESTS_T += $(TESTS_TODO_T)
endif

TESTS_LIB = lib/Test/Quvi.pm

TESTS_JSON = \
data/resolve/inscript_redirect.json \
data/resolve/shortened.json \
data/format/default/charlierose.json \
data/format/default/clipfish.json \
data/format/default/liveleak.json \
data/format/default/publicsenat.json \
data/format/default/tmunderground.json \
data/format/default/gaskrank.json \
data/format/default/spiegel.json \
data/format/default/funnyordie.json \
data/format/default/funnyhub.json \
data/format/default/nsfw/xvideos.json \
data/format/default/nsfw/tube8.json \
data/format/default/nsfw/keezmovies.json \
data/format/default/nsfw/xhamster.json \
data/format/default/nsfw/youjizz.json \
data/format/default/academicearth.json \
data/format/default/ignore/length_bytes/dailymotion.json \
data/format/default/collegehumor.json \
data/format/default/theonion.json \
data/format/default/ted.json \
data/format/default/tagtele.json \
data/format/default/break.json \
data/format/default/youtube.json \
data/format/default/golem.json \
data/format/default/google.json \
data/format/default/soundcloud.json \
data/format/default/globo.json \
data/format/default/cbsnews.json \
data/format/default/sapo.json \
data/format/default/tvlux.json \
data/format/default/bloomberg.json \
data/format/default/buzzhumor.json \
data/format/default/sevenload.json \
data/format/default/vimeo.json \
data/format/other/cbsnews_mp4_240p.json \
data/format/other/cbsnews_m4v_216p.json \
data/format/other/ted_best.json \
data/format/other/spiegel_1400k.json \
data/format/other/spiegel_best.json \
data/format/other/youtube_mp4_720p.json \
data/format/other/cbsnews_mp4_480p.json \
data/format/other/youtube_mp4_360p.json \
data/format/other/youtube_mp4_3072p.json \
data/format/other/youtube_mp4_1080p.json \
data/format/other/cbsnews_m4v_360p.json \
data/format/other/spiegel_576k.json \
data/format/other/golem_ipod.json \
data/format/other/ted_hd.json \
data/format/other/ignore/length_bytes/dailymotion_hq.json \
data/format/other/ignore/length_bytes/dailymotion_hd.json \
data/format/other/ignore/length_bytes/dailymotion_best.json \
data/format/other/cbsnews_best.json \
data/format/other/cbsnews_mp4_180p.json \
data/format/other/cbsnews_m4v_480p.json \
data/format/other/golem_high.json \
data/format/other/youtube_best.json \
data/format/other/youtube_flv_480p.json \
data/format/other/vimeo_best.json \
data/format/other/spiegel_928k.json \
data/format/other/youtube_flv_240p.json \
data/format/other/golem_best.json \
data/format/other/youtube_flv_360p.json \
data/format/other/vimeo_hd.json

README = README \
data/format/default/ignore/README \
data/format/default/non-http/README \
data/format/other/ignore/README

EXTRA_DIST = $(README) \
 $(TESTS_T) $(TESTS_NSFW_T) $(TESTS_M_T) \
 $(TESTS_TODO_T) $(TESTS_LIB) $(TESTS_JSON)

PROVE_OPTS = --nocolor -I $(top_srcdir)/tests/lib/

TESTQUVI_OPTS = \
 -q $(top_builddir)/src/quvi \
 -d $(top_srcdir)/tests \
 -b $(top_srcdir)/share

if HAVE_VALGRIND
TESTQUVI_OPTS += -v $(VALGRIND)
endif

test: all
	@ for t in $(TESTS_T) ; do \
		( $(PROVE) $(PROVE_OPTS) $(top_srcdir)/tests/$$t \
		:: $(TESTQUVI_OPTS) ) ; \
	done
