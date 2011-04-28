/* quvi
 * Copyright (C) 2011  Toni Gundogdu <legatvs@gmail.com>
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

/* callback_libsoup.c -- Use libsoup instead of libcurl */

#include "config.h"

#include <stdio.h>
#include <string.h>

#include <quvi/quvi.h>

#ifdef HAVE_LIBSOUP_GNOME
#include <libsoup/soup-gnome.h>
#else
#include <libsoup/soup.h>
#endif

#include "common.h"

static QUVIcode status_callback(long param, void *data)
{
  quvi_word status, type;

  status = quvi_loword(param);
  type = quvi_hiword(param);

  switch (status)
    {
    case QUVISTATUS_RESOLVE:
      handle_resolve_status(type);
      break;

    case QUVISTATUS_FETCH:
      handle_fetch_status(type, data);
      break;

    case QUVISTATUS_VERIFY:
      handle_verify_status(type);
      break;
    }

  fflush(stderr);

  return (QUVI_OK);
}

static void set_opt(quvi_net_t n, SoupMessage *m,
                    QUVInetPropertyOptionName opt,
                    const char *hdr)
{
  char *s = quvi_net_get_one_prop_opt(n, opt);
  if (s)
    soup_message_headers_append(m->request_headers, hdr, s);
}

static void set_opts_from_lua_script(quvi_net_t n, SoupMessage *m)
{
  set_opt(n, m, QUVI_NET_PROPERTY_OPTION_ARBITRARYCOOKIE, "Cookie");
  set_opt(n, m, QUVI_NET_PROPERTY_OPTION_USERAGENT, "User-Agent");
#ifdef _0
  /* Same as above. */
  quvi_llst_node_t opt;
  quvi_net_getprop(n, QUVI_NET_PROPERTY_OPTIONS, &opt);

  while (opt)
    {
      char *opt_name, *opt_value;
      quvi_net_propopt_t popt;

      popt = (quvi_net_propopt_t) quvi_llst_data(opt);

      quvi_net_getprop_opt(popt, QUVI_NET_PROPERTY_OPTION_NAME, &opt_name);
      quvi_net_getprop_opt(popt, QUVI_NET_PROPERTY_OPTION_VALUE, &opt_value);

      if (strcmp(opt_name, "arbitrary_cookie") == 0)
        {
          soup_message_headers_append(
            m->request_headers, "Cookie", opt_value);
        }

      if (strcmp(opt_name, "user_agent") == 0)
        {
          soup_message_headers_append(
            m->request_headers, "User-Agent", opt_value);
        }

      opt = quvi_llst_next(opt);
    }
#endif
}

static SoupSession *session = NULL;

static void send_message(quvi_net_t n, SoupMessage **message,
                         guint *status, SoupMessageFlags msg_flags,
                         const int head_flag, const int lua_opts)
{
  char *url = NULL;

  quvi_net_getprop(n, QUVI_NET_PROPERTY_URL, &url);

  *message = soup_message_new(head_flag ? "HEAD":"GET", url);

  if (msg_flags)
    soup_message_set_flags(*message, msg_flags);

  /* Even if this is conditional here, the current library design sets
   * these options (set in the LUA website scripts with quvi.fetch
   * call) for fetch callback only. */
  if (lua_opts)
    set_opts_from_lua_script(n, *message);

  *status = soup_session_send_message(session, *message);

  quvi_net_setprop(n, QUVI_NET_PROPERTY_RESPONSECODE, *status);
}

static QUVIcode fetch_callback(quvi_net_t n)
{
  SoupMessage *m;
  guint status;

  send_message(n, &m, &status, 0, 0, 1 /* Set LUA opts flag */);

  if (SOUP_STATUS_IS_SUCCESSFUL(status))
    {
      quvi_net_setprop(n, QUVI_NET_PROPERTY_CONTENT, m->response_body->data);
      return (QUVI_OK);
    }

  quvi_net_seterr(n, "%s (http/%d)", m->reason_phrase, status);

  return (QUVI_CALLBACK);
}

static QUVIcode resolve_callback(quvi_net_t n)
{
  SoupMessage *m;
  guint status;

  send_message(n, &m, &status, SOUP_MESSAGE_NO_REDIRECT, 0, 0);

  if (SOUP_STATUS_IS_REDIRECTION(status))
    {
      const char *r_url =
        soup_message_headers_get_one(m->response_headers, "Location");

      quvi_net_setprop(n, QUVI_NET_PROPERTY_REDIRECTURL, r_url);
    }
  else if (!SOUP_STATUS_IS_SUCCESSFUL(status))
    {
      quvi_net_seterr(n, "%s (http/%d)", m->reason_phrase, status);
      return (QUVI_CALLBACK);
    }

  return (QUVI_OK);
}

static QUVIcode verify_callback(quvi_net_t n)
{
  SoupMessage *m;
  guint status;

  send_message(n, &m, &status, 0, 1 /* HEAD */, 0);

  if (SOUP_STATUS_IS_SUCCESSFUL(status))
    {
      goffset cl =
        soup_message_headers_get_content_length(m->response_headers);

      const char *ct =
        soup_message_headers_get_content_type(m->response_headers, NULL);

      quvi_net_setprop(n, QUVI_NET_PROPERTY_CONTENTTYPE, ct);
      quvi_net_setprop(n, QUVI_NET_PROPERTY_CONTENTLENGTH, cl);

      return (QUVI_OK);
    }

  quvi_net_seterr(n, "%s (http/%d)", m->reason_phrase, status);

  return (QUVI_CALLBACK);
}

static void help()
{
  printf(
    "Usage: callback_libsoup [--help|--log] [URL]\n\n"
    "  -h,--help    .. Print help and exit\n"
    "  -l,--logger  .. Enable logger\n\n"
    "Note: Unless URL is specified, the default URL will be used\n");
  exit(0);
}

int main (int argc, char *argv[])
{
  char *url = "http://is.gd/yFNPMR";
  quvi_media_t m;
  int flag_log;
  QUVIcode rc;
  quvi_t q;

  flag_log = 0;

  if (argc > 1)
    {
      int i;
      for (i=1; i<argc; ++i)
        {
          if (strcmp(argv[i], "-l") == 0
              || strcmp(argv[i], "--logger") == 0)
            {
              flag_log = 1;
            }
          else if (strcmp(argv[i], "-h") == 0
                   || strcmp(argv[i], "--help") == 0)
            {
              help();
            }
          else
            url = argv[i];
        }
    }

  rc = quvi_init(&q);
  check_error(q,rc);

  quvi_setopt(q, QUVIOPT_STATUSFUNCTION, &status_callback);
  quvi_setopt(q, QUVIOPT_FETCHFUNCTION, &fetch_callback);
  quvi_setopt(q, QUVIOPT_RESOLVEFUNCTION, &resolve_callback);
  quvi_setopt(q, QUVIOPT_VERIFYFUNCTION, &verify_callback);

  g_type_init();

#ifdef HAVE_LIBSOUP_GNOME
  session = soup_session_async_new_with_options(
              SOUP_SESSION_ADD_FEATURE_BY_TYPE,
              SOUP_TYPE_PROXY_RESOLVER_GNOME,
              NULL
            );
#else
  session = soup_session_async_new();
#endif

  if (flag_log)
    {
      SoupLogger *log = soup_logger_new(SOUP_LOGGER_LOG_HEADERS, -1);
      soup_session_add_feature(session, SOUP_SESSION_FEATURE(log));
      g_object_unref(log);
    }

  rc = quvi_parse(q, url, &m);
  check_error(q, rc);

  quvi_parse_close(&m);
  quvi_close(&q);

  return (0);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
