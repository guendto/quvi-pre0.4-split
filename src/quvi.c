/* 
* Copyright (C) 2009,2010 Toni Gundogdu.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
* quvi.c - query video tool.
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <curl/curl.h>
#include <pcre.h>

#include "platform.h"

#include "quvi/quvi.h"
#include "cmdline.h"

typedef struct gengetopt_args_info opts_s;

static int
status_callback(long param, void *data) {
    quvi_word status,type;

    status = quvi_loword(param);
    type  = quvi_hiword(param);

    switch (status) {
    case QUVIS_FETCH: /* handle fetch */
        switch (type) {
        default             : printf(":: Fetch %s ...", (char *)data); break;
        case QUVIST_CONFIG  : printf(":: Fetch config ..."); break;
        case QUVIST_PLAYLIST: printf(":: Fetch playlist ..."); break;
        case QUVIST_DONE    : puts("done."); break;
        }
        break;

    case QUVIS_VERIFY: /* handle verify */
        switch (type) {
        default         : printf(":: Verify video link ..."); break;
        case QUVIST_DONE: puts("done."); break;
        }
        break;
    }

    fflush(stdout);

    return (0);
}

static const char notice[] =
"Copyright (C) 2009,2010 Toni Gundogdu. License: GNU GPL version  3  or later\n"
"This is free software; see the  source for  copying conditions.  There is NO\n"
"warranty;  not even for MERCHANTABILITY or FITNESS FOR A  PARTICULAR PURPOSE.";

static void
version(opts_s opts) {
    printf("quvi, version %s\n%s\n",
        quvi_version(QUVI_VERSION_LONG), notice);
    cmdline_parser_free(&opts);
    exit (0);
}

static void
supports(opts_s opts) {
    char *domain, *formats;

    while (quvi_next_host(&domain, &formats) == QUVI_OK)
        printf("%s\t%s\n", domain, formats);

    cmdline_parser_free(&opts);
    exit (0);
}

static void
dump_video_links(quvi_video_t video) {
    int i = 0;
    do {
        char *video_url, *file_suffix, *file_ct;
        double file_length;

        quvi_getprop(video, QUVIP_VIDEOURL, &video_url);
        quvi_getprop(video, QUVIP_VIDEOFILECONTENTTYPE, &file_ct);
        quvi_getprop(video, QUVIP_VIDEOFILESUFFIX, &file_suffix);
        quvi_getprop(video, QUVIP_VIDEOFILELENGTH, &file_length);

        printf(
            "link %02d  : %s\n"
            ":: length: %.0f\n:: suffix: %s\n:: content-type: %s\n\n",
            ++i, video_url, file_length, file_suffix, file_ct
        );
    } while (quvi_next_videolink(video) == QUVI_OK);
}

static void
dump_video(quvi_video_t video, opts_s opts) {
    char *page_link, *page_title, *video_id;
    long httpcode;

    quvi_getprop(video, QUVIP_PAGEURL, &page_link);
    quvi_getprop(video, QUVIP_PAGETITLE, &page_title);
    quvi_getprop(video, QUVIP_VIDEOID, &video_id);
    quvi_getprop(video, QUVIP_HTTPCODE, &httpcode);

    printf(" > Dump video:\n"
        "url     : %s\n"
        "title   : %s\n"
        "id      : %s\n",
        page_link, page_title, video_id);

    dump_video_links(video);

    printf("httpcode: %ld (last)\n", httpcode);
}

static void
dump_error(quvi_t quvi, QUVIcode rc, opts_s opts) {
    long httpcode, curlcode;

    quvi_getinfo(quvi, QUVII_HTTPCODE, &httpcode);
    quvi_getinfo(quvi, QUVII_CURLCODE, &curlcode);

    fprintf(stderr, "error: %s (http/%ld, curl/%ld)\n",
        quvi_strerror(quvi,rc),
        httpcode, curlcode);

    if (!opts.test_all_given) {
        quvi_close(&quvi);
        cmdline_parser_free(&opts);
        exit (rc);
    }
}

static const char *tests[] = {
"http://www.dailymotion.com/hd/video/x9fkzj_battlefield-1943-coral-sea-trailer_videogames",
"http://www.ehrensenf.de/shows/ehrensenf/getarnte-bienen-schaukelmotorrad-devitohorror",
"http://www.spiegel.de/video/video-1012584.html",
"http://vimeo.com/1485507",
"http://en.sevenload.com/videos/IUL3gda-Funny-Football-Clips",
"http://www.myubo.com/page/media_detail.html?movieid=1308f0fb-47c6-40c5-a6f9-1324dac12896",
"http://www.liveleak.com/view?i=704_1228511265",
"http://video.google.com/videoplay?docid=-8669127848070159803",
"http://video.golem.de/internet/2174/firefox-3.5-test.html",
"http://www.funnyhub.com/videos/pages/crazy-hole-in-one.html",
"http://www.clipfish.de/video/3100131/matratzendomino/",
"http://space.tv.cctv.com/video/VIDE1212909276513233", /* single-segment */
"http://space.tv.cctv.com/video/VIDE1247468077860061", /* multi-segment */
"http://www.youtube.com/watch?v=DeWsZ2b_pK4",
"http://break.com/index/beach-tackle-whip-lash.html",
"http://www.evisor.tv/tv/rennstrecken/1-runde-oschersleben-14082008--6985.htm",
#ifdef ENABLE_SMUT
"http://www.tube8.com/fetish/japanese-melon-gal-censored/186133/",
"http://www.xvideos.com/video243887/devi_emmerson_body_painting",
"http://www.youjizz.com/videos/glamour-girls---melissa-125602.html",
#endif
NULL
};

static void
match_test(quvi_t quvi, opts_s opts) {
    unsigned register int i;
    const char *errmsg;
    int err_offset, rc;

    for (i=0; tests[i]; ++i) {
        pcre *re;

        re = pcre_compile(
            opts.test_arg,
            PCRE_CASELESS,
            &errmsg,
            &err_offset,
            NULL
        );

        if (!re) {
            fprintf(stderr, "%s\n", errmsg);
            cmdline_parser_free(&opts);
            exit (1);
        }

        rc = pcre_exec(
            re,
            0,
            tests[i],
            strlen(tests[i]),
            0,
            0,
            0,
            0
        );

        pcre_free(re);

        if (rc >= 0) /* match */ {
            quvi_video_t v;
            QUVIcode _rc;

            _rc = quvi_parse(quvi, (char *)tests[i], &v);
            if (_rc != QUVI_OK)
                dump_error(quvi, _rc, opts);

            dump_video(v, opts);
            quvi_parse_close(&v);

            cmdline_parser_free(&opts);
            exit (0);
        }
        else if (rc == PCRE_ERROR_NOMATCH) {
            continue;
        }
        else {
            fprintf(stderr, "error: pcre_exec: rc = %d\n", rc);
            cmdline_parser_free(&opts);
            exit (1);
        }
    }

    fprintf(stderr, "error: nothing matched `%s'", opts.test_arg);
    cmdline_parser_free(&opts);
    exit (1);
}

static void
test_all(quvi_t quvi, opts_s opts) {
    register unsigned int i,m;
    quvi_video_t video;
    QUVIcode rc;

    puts(":: Run built-in video link tests.");
    for (m=0; tests[m]; ++m);

    for (i=0; i<m; ++i) {
        printf(" > Test #%02d/%02d:\n", i+1, m);

        rc = quvi_parse(quvi, (char *)tests[i], &video);
        if (rc != QUVI_OK)
            dump_error(quvi, rc, opts);
        else {
            if (opts.dump_given)
                dump_video(video, opts);
        }
        quvi_parse_close(&video);
    }
    puts(":: Tests done.");
}

static quvi_t
init_quvi(opts_s opts) {
    QUVIcode rc;
    quvi_t quvi;
    CURL *curl;

    printf(":: Init ...");

    if ( (rc = quvi_init(&quvi)) != QUVI_OK )
        dump_error(quvi, rc, opts);
    assert(quvi != 0);

    /* Set quvi options. */

    if (opts.format_given)
        quvi_setopt(quvi, QUVIOPT_FORMAT, opts.format_arg);

    if (opts.no_verify_given)
        quvi_setopt(quvi, QUVIOPT_NOVERIFY, 1L);

    quvi_setopt(quvi, QUVIOPT_STATUSFUNCTION, status_callback);

    /* We can use the quvi created cURL handle for our means. */

    quvi_getinfo(quvi, QUVII_CURL, &curl);
    assert(curl != 0);

    if (opts.agent_given)
        curl_easy_setopt(curl, CURLOPT_USERAGENT, opts.agent_arg);

    if (opts.proxy_given)
        curl_easy_setopt(curl, CURLOPT_PROXY, opts.proxy_arg);

    if (opts.debug_given)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(curl,
        CURLOPT_CONNECTTIMEOUT, opts.connect_timeout_arg);

    puts("done.");

    return (quvi);
}

int
main (int argc, char *argv[]) {
    register unsigned int i;
    quvi_video_t video;
    const char *url;
    opts_s opts;
    QUVIcode rc;
    quvi_t quvi;

    url = 0;

    /* Init cmdline parser. */

    if (cmdline_parser(argc, argv, &opts) != 0)
        return (1);

    if (opts.version_given)
        version(opts);

    if (opts.hosts_given)
        supports(opts);

    /* Init quvi. */

    quvi = init_quvi(opts);

    /* Handle input. */
    if (opts.test_all_given)
        test_all(quvi, opts);
    else if (opts.test_given)
        match_test(quvi, opts);
    else
    {
        if (opts.inputs_num == 0)
            fprintf(stderr, "error: no input links\n");

        for (i=0; i<opts.inputs_num; ++i)
        {
            if ( (rc = quvi_parse(
                    quvi,
                    (char *)opts.inputs[i],
                    &video))
                != QUVI_OK )
            {
                dump_error(quvi, rc, opts);
            }

            assert(video != 0);
            dump_video(video, opts);

            quvi_parse_close(&video);
            assert(video == 0);
        }
    }

    /* Cleanup. */

    puts(":: Cleanup.");

    quvi_close(&quvi);
    assert(quvi == 0);

    cmdline_parser_free(&opts);

    puts(":: Exit.");

    return (0);
}


