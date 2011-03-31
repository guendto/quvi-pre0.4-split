/* quvi
 * Copyright (C) 2009,2010,2011  Toni Gundogdu <legatvs@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

/* quvi.c - query media tool. */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <curl/curl.h>

#include "platform.h"

#include "quvi/quvi.h"
#include "cmdline.h"

/* strepl.c */
extern char *strepl(const char *s, const char *what, const char *with);

static int verbose_flag = 1;

/* prints to std(e)rr. */
static void spew_e(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

/* respects (q)uiet, prints to std(e)rr. */
static void spew_qe(const char *fmt, ...)
{
  va_list ap;
  if (!verbose_flag)
    return;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

/* glorified printf. */
static void spew(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap);
}

typedef struct gengetopt_args_info opts_s;

static void handle_shortened_status(quvi_word type)
{
  if (type == QUVISTATUSTYPE_DONE)
    spew_qe("done.\n");
  else
    spew_qe(":: Check for shortened URL ...");
}

static void handle_fetch_status(quvi_word type, void *p)
{
  switch (type)
    {
    default:
      spew_qe(":: Fetch %s ...", (char *)p);
      break;
    case QUVISTATUSTYPE_CONFIG:
      spew_qe(":: Fetch config ...");
      break;
    case QUVISTATUSTYPE_PLAYLIST:
      spew_qe(":: Fetch playlist ...");
      break;
    case QUVISTATUSTYPE_DONE:
      spew_qe("done.\n");
      break;
    }
}

static void handle_verify_status(quvi_word type)
{
  switch (type)
    {
    default:
      spew_qe(":: Verify media URL ...");
      break;
    case QUVISTATUSTYPE_DONE:
      spew_qe("done.\n");
      break;
    }
}

static int status_callback(long param, void *data)
{
  quvi_word status, type;

  status = quvi_loword(param);
  type = quvi_hiword(param);

  switch (status)
    {
    case QUVISTATUS_SHORTENED:
      handle_shortened_status(type);
      break;

    case QUVISTATUS_FETCH:
      handle_fetch_status(type, data);
      break;

    case QUVISTATUS_VERIFY:
      handle_verify_status(type);
      break;
    }

  fflush(stderr);

  return (0);
}

static size_t write_callback(void *p, size_t size, size_t nmemb,
                             void *data)
{
  size_t r = quvi_write_callback_default(p, size, nmemb, data);
  /* Could do something useful here. */
#ifdef _0
  puts(__func__);
#endif
  return r;
}

/* Divided into smaller blocks. Otherwise -pedantic objects. */

#define LICENSE_1 \
"/* quvi\n" \
" * Copyright (C) 2009,2010,2011  Toni Gundogdu <legatvs@gmail.com>\n"

#define LICENSE_2 \
" * This library is free software; you can redistribute it and/or\n" \
" * modify it under the terms of the GNU Lesser General Public\n" \
" * License as published by the Free Software Foundation; either\n" \
" * version 2.1 of the License, or (at your option) any later version.\n"

#define LICENSE_3 \
" * This library is distributed in the hope that it will be useful,\n" \
" * but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n" \
" * Lesser General Public License for more details.\n"

#define LICENSE_4 \
" * You should have received a copy of the GNU Lesser General Public\n" \
" * License along with this library; if not, write to the Free Software\n" \
" * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA\n" \
" * 02110-1301  USA\n" " */"

static void license(opts_s opts)
{
  printf("%s *\n%s *\n%s *\n%s\n", LICENSE_1, LICENSE_2, LICENSE_3,
         LICENSE_4);
  cmdline_parser_free(&opts);
  exit(0);
}

#undef LICENSE_4
#undef LICENSE_3
#undef LICENSE_2
#undef LICENSE_1

static void version(opts_s opts)
{
  printf("quvi version %s\n", quvi_version(QUVI_VERSION_LONG));
  cmdline_parser_free(&opts);
  exit(0);
}

static void dump_host(char *domain, char *formats)
{
  printf("%s\t%s\n", domain, formats);
  quvi_free(domain);
  quvi_free(formats);
}

/* Wraps quvi_supported. */
static void supported(quvi_t quvi, opts_s opts)
{
  QUVIcode rc;
  int i;

  for (i = 0; i < opts.inputs_num; ++i)
    {
      rc = quvi_supported(quvi, (char *)opts.inputs[i]);
      if (rc == QUVI_OK)
        spew_qe("%s: OK\n", (char *)opts.inputs[i]);
      else
        spew_qe("error: %s\n", quvi_strerror(quvi, rc));
    }

  quvi_close(&quvi);
  cmdline_parser_free(&opts);

  exit(rc);
}

static void format_help(quvi_t quvi, opts_s opts)
{
  int quit = 0;

  if (strcmp(opts.format_arg, "help") == 0)
    {
      printf("Usage:\n"
             "   --format arg            get format arg\n"
             "   --format list           list websites and supported formats\n"
             "   --format list arg       match arg to websites, list formats for matches\n"
             "Examples:\n"
             "   --format mp4_360p       get format mp4_360p (youtube)\n"
             "   --format list youtube   list youtube formats\n"
             "   --format list dailym    list dailym(otion) formats\n"
            );
      quit = 1;
    }
  else if (strcmp(opts.format_arg, "list") == 0)
    {
      int done = 0;
      char *d, *f;

      while(!done)
        {
          const int rc = quvi_next_supported_website(quvi, &d, &f);
          switch (rc)
            {
            case QUVI_OK:
            {
              int print = 1;
              /* -f list <pattern> */
              if (opts.inputs_num > 0)
                print = strstr(d, (char *)opts.inputs[0]) != 0;
              /* -f list */
              if (print)
                printf("%s:\n  %s\n\n", d, f);
              quvi_free(d);
              quvi_free(f);
            }
            break;
            case QUVI_LAST:
              done = 1;
              break;
            default:
              spew_e("%s\n", quvi_strerror(quvi, rc));
              break;
            }
        }
      quit = 1;
    }

  if (quit)
    {
      quvi_close(&quvi);
      cmdline_parser_free(&opts);
      exit(0);
    }
}

/* dumps all supported hosts to stdout. */
static void support(quvi_t quvi, opts_s opts)
{
  int done = 0;

  if (opts.inputs_num > 0)
    supported(quvi, opts);

  while (!done)
    {
      char *domain, *formats;
      QUVIcode rc;

      rc = quvi_next_supported_website(quvi, &domain, &formats);

      switch (rc)
        {
        case QUVI_OK:
          dump_host(domain, formats);
          break;
        case QUVI_LAST:
          done = 1;
          break;
        default:
          spew_e("%s\n", quvi_strerror(quvi, rc));
          break;
        }
    }

  quvi_close(&quvi);
  cmdline_parser_free(&opts);

  exit(0);
}

static void invoke_exec(quvi_media_t media, const char *media_url,
                        opts_s opts)
{
  char *quoted_url, *arg;
  int rc;

  asprintf(&quoted_url, "\"%s\"", media_url);

  arg = strdup(opts.exec_arg);
  arg = strepl(arg, "%u", quoted_url);

  free(quoted_url);
  quoted_url = NULL;

  rc = system(arg);

  switch (rc)
    {
    case 0:
      break;
    case -1:
      spew_e("error: failed to execute `%s'\n", arg);
      break;
    default:
      spew_e("error: child exited with: %d\n", rc >> 8);
      break;
    }

  free(arg);
  arg = NULL;
}

static void
dump_media_link_xml(CURL * curl,
                    int i,
                    char *media_url,
                    double file_length, char *file_ct,
                    char *file_suffix)
{
  char *url;

  url = curl_easy_escape(curl, media_url, 0);

  spew("   <link id=\"%d\">\n"
       "       <length_bytes>%.0f</length_bytes>\n"
       "       <content_type>%s</content_type>\n"
       "       <file_suffix>%s</file_suffix>\n"
       "       <url>%s</url>\n"
       "   </link>\n",
       i, file_length, file_ct, file_suffix, url ? url : media_url);

  if (url)
    {
      curl_free(url);
      url = NULL;
    }
}

static void
dump_media_link_old(int i,
                    char *media_url,
                    double file_length, char *file_suffix,
                    char *file_ct)
{
  spew("link %02d  : %s\n"
       ":: length: %.0f\n:: suffix: %s\n:: content-type: %s\n\n",
       i, media_url, file_length, file_suffix, file_ct);
}

static void
dump_media_link_json(int i,
                     char *media_url,
                     double file_length, char *file_suffix,
                     char *file_ct)
{
  spew("    {\n"
       "      \"id\": \"%d\",\n"
       "      \"length_bytes\": \"%.0f\",\n"
       "      \"content_type\": \"%s\",\n"
       "      \"file_suffix\": \"%s\",\n"
       "      \"url\": \"%s\"\n"
       "    }%s\n",
       i, file_length, file_ct, file_suffix, media_url,
       i > 1 ? "," : "");
}

static void dump_media_links(quvi_media_t media, opts_s opts,
                             CURL * curl)
{
  int i = 0;
  do
    {
      char *media_url, *file_suffix, *file_ct;
      double file_length;

      quvi_getprop(media, QUVIPROP_MEDIAURL, &media_url);
      quvi_getprop(media, QUVIPROP_MEDIACONTENTTYPE, &file_ct);
      quvi_getprop(media, QUVIPROP_MEDIACONTENTLENGTH, &file_length);
      quvi_getprop(media, QUVIPROP_FILESUFFIX, &file_suffix);

      ++i;

      if (opts.xml_given)
        dump_media_link_xml(curl, i, media_url, file_length,
                            file_ct, file_suffix);

      else if (opts.old_given)
        dump_media_link_old(i, media_url, file_length, file_suffix,
                            file_ct);
      else
        dump_media_link_json(i, media_url, file_length, file_suffix,
                             file_ct);
    }
  while (quvi_next_media_url(media) == QUVI_OK);
}

static void
dump_media_xml(CURL * curl,
               char *media_id, char *host, char *format,
               char *page_title, char *page_link, char *start_time,
               char *thumb_url, double duration)
{
  char *url;

  url = curl_easy_escape(curl, page_link, 0);

  spew("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
       "<media id=\"%s\" host=\"%s\">\n"
       "   <format_requested>%s</format_requested>\n"
       "   <page_title>%s</page_title>\n"
       "   <page_url>%s</page_url>\n"
       "   <start_time>%s</start_time>\n"
       "   <thumbnail_url>%s</thumbnail_url>\n",
       media_id, host, format, page_title, url ? url : page_link,
       start_time ? start_time : "",
       thumb_url ? thumb_url : "");

  if (duration)
    spew("   <duration>%.0f</duration>\n", duration);

  if (url)
    {
      curl_free(url);
      url = NULL;
    }

}

static void
dump_media_old(char *media_id, char *host, char *format,
               char *page_title, char *page_link, char *start_time,
               char *thumb_url, double duration)
{
  spew(" > Dump media:\n"
       "host    : %s\n"
       "url     : %s\n"
       "title   : %s\n"
       "id      : %s\n"
       "format  : %s (requested)\n"
       "start time: %s\n"
       "thumbnail url: %s\n",
       host, page_link, page_title, media_id, format,
       start_time ? start_time : "",
       thumb_url ? thumb_url : "");

  if (duration)
    spew("duration: %.0f\n", duration);
}

static void
dump_media_json(char *media_id, char *host, char *format,
                char *page_title, char *page_link, char *start_time,
                char *thumb_url, double duration)
{
  char *t;

  t = strdup(page_title);
  t = strepl(t, "\"", "\\\"");

  spew("{\n"
       "  \"host\": \"%s\",\n"
       "  \"page_title\": \"%s\",\n"
       "  \"page_url\": \"%s\",\n"
       "  \"id\": \"%s\",\n"
       "  \"format_requested\": \"%s\",\n"
       "  \"start_time\": \"%s\",\n"
       "  \"thumbnail_url\": \"%s\",\n",
       host, t, page_link, media_id, format,
       start_time ? start_time : "",
       thumb_url ? thumb_url : "");

  if (duration)
    spew("  \"duration\": \"%.0f\",\n", duration);

  spew("  \"link\": [\n");

  free(t);
  t = NULL;
}

static void dump_media(quvi_media_t media, opts_s opts, CURL * curl)
{
  char *page_link, *page_title, *media_id, *format, *host, *start_time, *thumb_url;
  double duration;

  quvi_getprop(media, QUVIPROP_HOSTID, &host);
  quvi_getprop(media, QUVIPROP_PAGEURL, &page_link);
  quvi_getprop(media, QUVIPROP_PAGETITLE, &page_title);
  quvi_getprop(media, QUVIPROP_MEDIAID, &media_id);
  quvi_getprop(media, QUVIPROP_FORMAT, &format);
  quvi_getprop(media, QUVIPROP_STARTTIME, &start_time);
  quvi_getprop(media, QUVIPROP_MEDIATHUMBNAILURL, &thumb_url);
  quvi_getprop(media, QUVIPROP_MEDIADURATION, &duration);

  if (opts.xml_given)
    dump_media_xml(curl, media_id, host, format, page_title, page_link, start_time, thumb_url, duration);
  else if (opts.old_given)
    dump_media_old(media_id, host, format, page_title, page_link, start_time, thumb_url, duration);
  else
    dump_media_json(media_id, host, format, page_title, page_link, start_time, thumb_url, duration);

  dump_media_links(media, opts, curl);

  if (opts.xml_given)
    spew("</media>\n");
  else if (opts.old_given) ;
  else
    spew("  ]\n}\n");
}

static void
expect_error(const char *what, const char *expected, const char *got)
{
  fprintf(stderr, "error: %s:\n  expected: \"%s\"\n  got: \"%s\"\n\n",
          what, expected, got);
}

static void
expect_error_d(const char *what, const double expected,
               const double got)
{
  fprintf(stderr,
          "error: %s:\n  expected: \"%.0f\"\n  got: \"%.0f\"\n\n", what,
          expected, got);
}

static void dump_error(quvi_t quvi, QUVIcode rc, opts_s opts)
{
  fprintf(stderr, "error: %s\n", quvi_strerror(quvi, rc));
  quvi_close(&quvi);
  cmdline_parser_free(&opts);
  exit(rc);
}

static quvi_t init_quvi(opts_s opts, CURL ** curl)
{
  QUVIcode rc;
  quvi_t quvi;

  if ((rc = quvi_init(&quvi)) != QUVI_OK)
    dump_error(quvi, rc, opts);
  assert(quvi != 0);

  /* Set quvi options. */

  if (opts.format_given)
    quvi_setopt(quvi, QUVIOPT_FORMAT, opts.format_arg);

  quvi_setopt(quvi, QUVIOPT_NOSHORTENED, opts.no_shortened_given);
  quvi_setopt(quvi, QUVIOPT_NOVERIFY, opts.no_verify_given);

  if (opts.category_all_given)
    quvi_setopt(quvi, QUVIOPT_CATEGORY, QUVIPROTO_ALL);
  else
    {
      long n = 0;
      if (opts.category_http_given)
        n |= QUVIPROTO_HTTP;
      if (opts.category_mms_given)
        n |= QUVIPROTO_MMS;
      if (opts.category_rtsp_given)
        n |= QUVIPROTO_RTSP;
      if (opts.category_rtmp_given)
        n |= QUVIPROTO_RTMP;
      if (n > 0)
        quvi_setopt(quvi, QUVIOPT_CATEGORY, n);
    }

  quvi_setopt(quvi, QUVIOPT_STATUSFUNCTION, status_callback);
  quvi_setopt(quvi, QUVIOPT_WRITEFUNCTION, write_callback);

  /* Use the quvi created cURL handle. */

  quvi_getinfo(quvi, QUVIINFO_CURL, curl);
  assert(*curl != 0);

  if (opts.agent_given)
    curl_easy_setopt(*curl, CURLOPT_USERAGENT, opts.agent_arg);

  if (opts.proxy_given)
    curl_easy_setopt(*curl, CURLOPT_PROXY, opts.proxy_arg);

  if (opts.no_proxy_given)
    curl_easy_setopt(*curl, CURLOPT_PROXY, "");

  curl_easy_setopt(*curl, CURLOPT_VERBOSE, opts.verbose_libcurl_given);

  curl_easy_setopt(*curl, CURLOPT_CONNECTTIMEOUT,
                   opts.connect_timeout_arg);

  return (quvi);
}

int main(int argc, char *argv[])
{
  const char *url, *home, *no_config, *fname;
  quvi_media_t media;
  int no_config_flag;
  opts_s opts;
  QUVIcode rc;
  quvi_t quvi;
  CURL *curl;
  int i;

  curl = NULL;
  url = NULL;

  no_config = getenv("QUVI_NO_CONFIG");

  home = getenv("QUVI_HOME");
  if (!home)
    home = getenv("HOME");

  no_config_flag = 1;

#ifndef HOST_W32
  fname = "/.quvirc";
#else
  fname = "\\quvirc";
#endif

  /* Init cmdline parser. */

  if (home && !no_config)
    {
      char *path;
      FILE *f;

      asprintf(&path, "%s%s", home, fname);
      f = fopen(path, "r");

      if (f != NULL)
        {
          struct cmdline_parser_params *pp;

          fclose(f);
          f = NULL;

          pp = cmdline_parser_params_create();
          pp->check_required = 0;

          if (cmdline_parser_config_file(path, &opts, pp) == 0)
            {
              pp->initialize = 0;
              pp->override = 1;
              pp->check_required = 1;

              if (cmdline_parser_ext(argc, argv, &opts, pp) == 0)
                no_config_flag = 0;
            }
          free(pp);
          pp = NULL;
        }

      free(path);
      path = NULL;
    }

  if (no_config_flag)
    {
      if (cmdline_parser(argc, argv, &opts) != 0)
        return (QUVI_INVARG);
    }

  if (opts.version_given)
    version(opts);

  if (opts.license_given)
    license(opts);

  verbose_flag = !opts.quiet_given;

  quvi = init_quvi(opts, &curl);

  if (opts.support_given)
    support(quvi, opts);

  if (opts.format_given)
    format_help(quvi, opts);

  /* User input */

  if (opts.inputs_num == 0)
    fprintf(stderr, "error: no input links\n");

  for (i = 0; i < opts.inputs_num; ++i)
    {
      rc = quvi_parse(quvi, (char *)opts.inputs[i], &media);

      if (rc != QUVI_OK)
        dump_error(quvi, rc, opts);

      assert(media != 0);
      dump_media(media, opts, curl);

      if (opts.exec_given)
        {
          char *media_url = NULL;
          do
            {
              quvi_getprop(media, QUVIPROP_MEDIAURL, &media_url);
              invoke_exec(media, media_url, opts);
            }
          while (quvi_next_media_url(media) == QUVI_OK);
        }

      quvi_parse_close(&media);
      assert(media == 0);
    }

  /* Cleanup. */

  quvi_close(&quvi);
  assert(quvi == 0);

  cmdline_parser_free(&opts);

  return (QUVI_OK);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
