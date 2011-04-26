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

struct content_s
{
  size_t size;
  char *p;
};

typedef struct content_s *content_t;

size_t
quvi_write_callback_default(void *p, size_t size, size_t nmemb,
                            void *data)
{
  content_t m = (content_t)data;
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

static void set_opts_from_lua_script(_quvi_t q, _quvi_net_t n)
{
  quvi_llst_node_t opt;
  quvi_net_getprop(n, QUVINETPROP_OPTIONS, &opt);

  while (opt)
    {
      char *opt_name, *opt_value;
      quvi_net_propopt_t popt;

      popt = (quvi_net_propopt_t) quvi_llst_data(opt);

      quvi_net_getpropopt(popt, QUVINETPROPOPT_NAME, &opt_name);
      quvi_net_getpropopt(popt, QUVINETPROPOPT_VALUE, &opt_value);

      if (strcmp(opt_name, "arbitrary_cookie") == 0)
        curl_easy_setopt(q->curl, CURLOPT_COOKIE, opt_value);

      if (strcmp(opt_name, "user_agent") == 0)
        curl_easy_setopt(q->curl, CURLOPT_USERAGENT, opt_value);

      opt = quvi_llst_next(opt);
    }
}

QUVIcode curl_fetch(_quvi_t q, _quvi_net_t n)
{
  struct content_s content;
  CURLcode curlcode;
  long conncode;
  QUVIcode rc;

  curl_easy_setopt(q->curl, CURLOPT_URL, n->url);
  curl_easy_setopt(q->curl, CURLOPT_ENCODING, "");

  memset(&content, 0, sizeof(struct content_s));
  curl_easy_setopt(q->curl, CURLOPT_WRITEDATA, &content);

  curl_easy_setopt(q->curl, CURLOPT_WRITEFUNCTION,
                   q->write_func
                   ? (curl_write_callback) q->write_func
                   : (curl_write_callback) quvi_write_callback_default);

  set_opts_from_lua_script(q,n);

  curlcode = curl_easy_perform(q->curl);

  curl_easy_getinfo(q->curl, CURLINFO_RESPONSE_CODE, &n->resp_code);
  curl_easy_getinfo(q->curl, CURLINFO_HTTP_CONNECTCODE, &conncode);

  if (curlcode == CURLE_OK && n->resp_code == 200)
    rc = QUVI_OK;
  else
    {
      if (curlcode == CURLE_OK)
        {
          freprintf(&n->errmsg, "server response code %ld (conncode=%ld)",
                    n->resp_code, conncode);
        }
      else
        {
          freprintf(&n->errmsg, "%s (http/%ld, conn/%ld, curl/%ld)",
                    curl_easy_strerror(curlcode), n->resp_code,
                    conncode, curlcode);
        }

      rc = QUVI_CALLBACK;
    }

  n->fetch.content = content.p;

  return (rc);
}

static void set_redirect_opts(_quvi_t q, content_t m, const char *url)
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

QUVIcode curl_resolve(_quvi_t q, _quvi_net_t n)
{
  struct content_s tmp;
  CURLcode curlcode;
  long conncode;
  QUVIcode rc;

  memset(&tmp, 0, sizeof(tmp));

  set_redirect_opts(q, &tmp, n->url);
  set_opts_from_lua_script(q,n);

  curlcode = curl_easy_perform(q->curl);
  rc       = QUVI_OK;

  restore_opts(q);

  curl_easy_getinfo(q->curl, CURLINFO_RESPONSE_CODE, &n->resp_code);
  curl_easy_getinfo(q->curl, CURLINFO_HTTP_CONNECTCODE, &conncode);

  if (curlcode == CURLE_OK)
    {
      if (n->resp_code >= 301 && n->resp_code <= 303) /* New location */
        {
          char *r_url = NULL;

          curl_easy_getinfo(q->curl, CURLINFO_REDIRECT_URL, &r_url);
          assert(r_url != NULL);

          freprintf(&n->redirect.url, "%s", r_url);
        }
    }
  else
    {
      freprintf(&n->errmsg, "%s (http/%ld, conn/%ld, curl/%ld)",
                curl_easy_strerror(curlcode), n->resp_code,
                conncode, curlcode);

      rc = QUVI_CALLBACK;
    }

  if (tmp.p)
    _free(tmp.p);

  return (rc);
}

QUVIcode curl_verify(_quvi_t q, _quvi_net_t n)
{
  struct content_s tmp;
  CURLcode curlcode;
  long conncode;
  QUVIcode rc;

  rc = QUVI_OK;

  curl_easy_setopt(q->curl, CURLOPT_URL, n->url);
  curl_easy_setopt(q->curl, CURLOPT_NOBODY, 1L);     /* get -> head */

  memset(&tmp, 0, sizeof(tmp));
  curl_easy_setopt(q->curl, CURLOPT_WRITEDATA, &tmp);

  curl_easy_setopt(q->curl, CURLOPT_WRITEFUNCTION,
                   (q->write_func)
                   ? (curl_write_callback) q->write_func
                   : (curl_write_callback) quvi_write_callback_default);

  set_opts_from_lua_script(q,n);

  curlcode = curl_easy_perform(q->curl);

  curl_easy_setopt(q->curl, CURLOPT_HTTPGET, 1L);    /* reset: head -> get */

  curl_easy_getinfo(q->curl, CURLINFO_RESPONSE_CODE, &n->resp_code);
  curl_easy_getinfo(q->curl, CURLINFO_HTTP_CONNECTCODE, &conncode);

  if (curlcode == CURLE_OK)
    {
      if (n->resp_code == 200 || n->resp_code == 206)
        {
          const char *ct;

          curl_easy_getinfo(q->curl, CURLINFO_CONTENT_TYPE, &ct);
          assert(ct != NULL);

          freprintf(&n->verify.content_type, "%s", ct);

          curl_easy_getinfo(q->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD,
                            &n->verify.content_length);
        }
      else
        {
          freprintf(&n->errmsg, "server response code %ld (conncode=%ld)",
                    n->resp_code, conncode);

          rc = QUVI_CALLBACK;
        }
    }
  else
    {
      freprintf(&n->errmsg, "%s (curlcode=%d, conncode=%ld)",
                curl_easy_strerror(curlcode), curlcode, conncode);

      rc = QUVI_CALLBACK;
    }

  if (tmp.p)
    _free(tmp.p);

  return (rc);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
