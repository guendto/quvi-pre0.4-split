/* quvi
 * Copyright (C) 2009,2010  Toni Gundogdu <legatvs@gmail.com>
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

#include "config.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <curl/curl.h>

#include "quvi/quvi.h"
#include "internal.h"
#include "util.h"
#include "lua_wrap.h"
#include "curl_wrap.h"

static void *_realloc(void *p, const size_t size)
{
  if (p)
    return realloc(p, size);
  return malloc(size);
}

struct mem_s
{
  size_t size;
  char *p;
};

typedef struct mem_s *mem_t;

size_t
quvi_write_callback_default(void *p, size_t size, size_t nmemb,
                            void *data)
{
  mem_t m = (mem_t)data;
  const size_t rsize = size * nmemb;
  void *tmp = _realloc(m->p, m->size + rsize + 1);
  if (tmp)
    {
      m->p = (char *)tmp;
      memcpy(&(m->p[m->size]), p, rsize);
      m->size += rsize;
      m->p[m->size] = '\0';
    }
  return (rsize);
}

/* lua_wrap.c */
extern char *lua_get_field_s(lua_State *, const char *);

QUVIcode
fetch_to_mem(_quvi_media_t video, const char *url, lua_State * l,
             char **dst)
{
  char *fetch_type, *arbitrary_cookie, *user_agent;
  QUVIstatusType fetch_type_n;
  long respcode, conncode;
  CURLcode curlcode;
  struct mem_s mem;
  QUVIcode rc;

  if (!video)
    return (QUVI_BADHANDLE);

  if (!dst)
    return (QUVI_INVARG);

  *dst = NULL;
  fetch_type = NULL;
  user_agent = NULL;
  arbitrary_cookie = NULL;
  fetch_type_n = QUVISTATUSTYPE_PAGE;   /* default */

  /* Additional settings from LUA table */

  if (lua_istable(l, 2))
    {
      fetch_type = lua_get_field_s(l, "fetch_type");
      if (fetch_type)
        {
          if (strcmp(fetch_type, "config") == 0)
            fetch_type_n = QUVISTATUSTYPE_CONFIG;
          else if (strcmp(fetch_type, "playlist") == 0)
            fetch_type_n = QUVISTATUSTYPE_PLAYLIST;
        }
      arbitrary_cookie = lua_get_field_s(l, "arbitrary_cookie");
      user_agent = lua_get_field_s(l, "user_agent");
    }

  if (video->quvi->status_func)
    {
      if (video->quvi->
          status_func(makelong(QUVISTATUS_FETCH, fetch_type_n),
                      (void *)url) != QUVI_OK)
        {
          return (QUVI_ABORTEDBYCALLBACK);
        }
    }

  curl_easy_setopt(video->quvi->curl, CURLOPT_URL, url);
  curl_easy_setopt(video->quvi->curl, CURLOPT_ENCODING, "");

  memset(&mem, 0, sizeof(mem));
  curl_easy_setopt(video->quvi->curl, CURLOPT_WRITEDATA, &mem);

  curl_easy_setopt(video->quvi->curl, CURLOPT_WRITEFUNCTION,
                   video->quvi->write_func
                   ? (curl_write_callback) video->quvi->write_func
                   : (curl_write_callback) quvi_write_callback_default);

  if (arbitrary_cookie != NULL && *arbitrary_cookie != '\0')
    {
      curl_easy_setopt(video->quvi->curl, CURLOPT_COOKIE,
                       arbitrary_cookie);
    }

  if (user_agent != NULL && *user_agent != '\0')
    {
      curl_easy_setopt(video->quvi->curl, CURLOPT_USERAGENT, user_agent);
    }

  curlcode = curl_easy_perform(video->quvi->curl);
  respcode = 0;
  conncode = 0;
  rc = QUVI_OK;

  curl_easy_getinfo(video->quvi->curl, CURLINFO_RESPONSE_CODE,
                    &respcode);
  curl_easy_getinfo(video->quvi->curl, CURLINFO_HTTP_CONNECTCODE,
                    &conncode);

  if (curlcode == CURLE_OK && respcode == 200)
    {
      if (video->quvi->status_func)
        {
          if (video->quvi->status_func(makelong(QUVISTATUS_FETCH,
                                                QUVISTATUSTYPE_DONE),
                                       0) != QUVI_OK)
            {
              rc = QUVI_ABORTEDBYCALLBACK;
            }
        }
    }
  else
    {
      if (curlcode == CURLE_OK)
        {
          freprintf(&video->quvi->errmsg,
                    "server response code %ld (conncode=%ld)", respcode,
                    conncode);
        }
      else
        {
          freprintf(&video->quvi->errmsg,
                    "%s (curlcode=%d, conncode=%ld)",
                    curl_easy_strerror(curlcode), curlcode, conncode);
        }

      rc = QUVI_CURL;
    }

  if (mem.p)
    {
      *dst = mem.p;
      if (rc == QUVI_OK && !video->charset)       /* charset */
        run_lua_charset_func(video, mem.p);
    }

  video->quvi->httpcode = respcode;
  video->quvi->curlcode = curlcode;

  return (rc);
}

QUVIcode query_file_length(_quvi_t quvi, llst_node_t lnk)
{
  static const char *scheme = "http://";
  long respcode, conncode;
  _quvi_video_link_t qvl;
  CURLcode curlcode;
  struct mem_s mem;
  QUVIcode rc;
  char buf[8];

  if (!quvi)
    return (QUVI_BADHANDLE);

  if (!lnk)
    return (QUVI_BADHANDLE);

  qvl = (_quvi_video_link_t) lnk->data;
  assert(qvl != NULL);

  if (!qvl)
    return (QUVI_BADHANDLE);

  qvl->url = from_html_entities(qvl->url);

  /* We can currently check HTTP URLs only */
  memset(&buf, 0, sizeof(buf));
  if (strcmp(strncpy(buf, qvl->url, strlen(scheme)), scheme) != 0)
    return (QUVI_OK);           /* Skip video URL verification discreetly. */

  if (quvi->status_func)
    {
      if (quvi->status_func(makelong(QUVISTATUS_VERIFY, 0), 0) != QUVI_OK)
        {
          return (QUVI_ABORTEDBYCALLBACK);
        }
    }

  curl_easy_setopt(quvi->curl, CURLOPT_URL, qvl->url);
  curl_easy_setopt(quvi->curl, CURLOPT_NOBODY, 1L);     /* get -> head */

  memset(&mem, 0, sizeof(mem));
  curl_easy_setopt(quvi->curl, CURLOPT_WRITEDATA, &mem);

  curl_easy_setopt(quvi->curl, CURLOPT_WRITEFUNCTION, (quvi->write_func)
                   ? (curl_write_callback) quvi->write_func
                   : (curl_write_callback) quvi_write_callback_default);

  curlcode = curl_easy_perform(quvi->curl);

  curl_easy_setopt(quvi->curl, CURLOPT_HTTPGET, 1L);    /* reset: head -> get */

  respcode = 0;
  conncode = 0;
  rc = QUVI_OK;

  curl_easy_getinfo(quvi->curl, CURLINFO_RESPONSE_CODE, &respcode);
  curl_easy_getinfo(quvi->curl, CURLINFO_HTTP_CONNECTCODE, &conncode);

  if (curlcode == CURLE_OK)
    {
      if (respcode == 200 || respcode == 206)
        {
          const char *ct;

          curl_easy_getinfo(quvi->curl, CURLINFO_CONTENT_TYPE, &ct);

          _free(qvl->content_type);
          asprintf(&qvl->content_type, "%s", ct);

          curl_easy_getinfo(quvi->curl,
                            CURLINFO_CONTENT_LENGTH_DOWNLOAD, &qvl->length);

          if (quvi->status_func)
            {
              if (quvi->status_func(makelong
                                    (QUVISTATUS_VERIFY, QUVISTATUSTYPE_DONE),
                                    0) != QUVI_OK)
                {
                  rc = QUVI_ABORTEDBYCALLBACK;
                }
            }

          if (rc == QUVI_OK)
            {
              /* Content-Type -> suffix. */
              rc = run_lua_suffix_func(quvi, qvl);
            }
        }
      else
        {
          freprintf(&quvi->errmsg,
                    "server response code %ld (conncode=%ld)", respcode,
                    conncode);
          rc = QUVI_CURL;
        }
    }
  else
    {
      freprintf(&quvi->errmsg, "%s (curlcode=%d, conncode=%ld)",
                curl_easy_strerror(curlcode), curlcode, conncode);
      rc = QUVI_CURL;
    }

  quvi->httpcode = respcode;
  quvi->curlcode = curlcode;

  if (mem.p)
    _free(mem.p);

  return (rc);
}

static void set_redirect_opts(_quvi_t q, mem_t m, const char *url)
{
  curl_easy_setopt(q->curl, CURLOPT_WRITEDATA, m);

  curl_easy_setopt(q->curl, CURLOPT_WRITEFUNCTION,
                   (q->write_func)
                   ? (curl_write_callback) q->write_func
                   : (curl_write_callback) quvi_write_callback_default);

  /* GET -> HEAD */
  curl_easy_setopt(q->curl, CURLOPT_URL, url);
  curl_easy_setopt(q->curl, CURLOPT_FOLLOWLOCATION, 0L);
}

static void restore_opts(_quvi_t q)
{
  curl_easy_setopt(q->curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(q->curl, CURLOPT_HTTPGET, 1L); /* HEAD -> GET */
}

/**
 * Check if URL redirects to another location. For example, URL many
 * shorteners (e.g. is.gd, goo.gl, dai.ly) do this. Replaces the URL
 * with the new location URL (if any).
 */
QUVIcode resolve_redirection(_quvi_t q, const char *url, char **redirect_url)
{
  long respcode, conncode;
  CURLcode curlcode;
  struct mem_s mem;
  QUVIcode rc;

  assert(redirect_url != NULL);
  assert(url != NULL);

  *redirect_url = NULL;

  if (q->status_func)
    {
      if (q->status_func(QUVISTATUS_RESOLVE, 0) != QUVI_OK)
        return (QUVI_ABORTEDBYCALLBACK);
    }

  memset(&mem, 0, sizeof(mem));
  set_redirect_opts(q, &mem, url);
  curlcode = curl_easy_perform(q->curl);
  restore_opts(q);

  respcode = 0;
  conncode = 0;
  rc = QUVI_OK;

  curl_easy_getinfo(q->curl, CURLINFO_RESPONSE_CODE, &respcode);
  curl_easy_getinfo(q->curl, CURLINFO_HTTP_CONNECTCODE, &conncode);

  if (curlcode == CURLE_OK)
    {
      if (respcode >= 301 && respcode <= 303) /* New location */
        {
          char *u = NULL;

          curl_easy_getinfo(q->curl, CURLINFO_REDIRECT_URL, &u);
          assert(u != NULL);

          freprintf(redirect_url, "%s", u);

          rc = QUVI_OK;
        }
      else /* respcode >= 301 && respcode <= 303 */
        {
          /* Most likely not a redirection. Pass. */
          rc = QUVI_OK;
        }

      if (q->status_func)
        {
          const long param =
            makelong(QUVISTATUS_RESOLVE, QUVISTATUSTYPE_DONE);

          rc = q->status_func(param, 0);
        }
    }
  else /* Error occurred in libcurl */
    {
      freprintf(&q->errmsg, "%s (curlcode=%d, conncode=%ld)",
                curl_easy_strerror(curlcode), curlcode, conncode);

      rc = QUVI_CURL;
    }

  if (mem.p)
    _free(mem.p);

  q->httpcode = respcode;
  q->curlcode = curlcode;

  return (rc);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
