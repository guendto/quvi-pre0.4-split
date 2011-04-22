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

#include "config.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "quvi/quvi.h"
#include "internal.h"
#include "curl_wrap.h"
#include "lua_wrap.h"
#include "util.h"
#include "net.h"

_quvi_net_t new_net_handle()
{
  return (calloc(1, sizeof(struct _quvi_net_s)));
}

#ifdef _0
static _quvi_net_propopt_t new_option()
{
  return (calloc(1, sizeof(struct _quvi_net_propopt_s)));
}
#endif

void free_net_handle(_quvi_net_t *n)
{
  if (*n && n)
    {
      /* TODO: free options llst */
      _free((*n)->errmsg);
      _free((*n)->url);
      _free((*n)->fetch.content);
      _free((*n)->redirect.url);
      _free((*n)->verify.content_type);
      _free(*n);
    }
}

#ifdef _0
static void free_option(_quvi_net_propopt_t *n)
{
  if (*n && n)
    {
      _free((*n)->name);
      _free((*n)->value);
      _free(*n);
    }
}
#endif

/* lua_wrap.c */
extern char *lua_get_field_s(lua_State *, const char *);

QUVIcode fetch_wrapper(_quvi_t q, lua_State *l, _quvi_net_t *n)
{
  char *fetch_type, *arbitrary_cookie, *user_agent;
  QUVIstatusType status_type;
  QUVIcode rc;
  char *url;

  arbitrary_cookie = NULL;
  status_type = QUVISTATUSTYPE_PAGE;   /* default "page" */
  url         = (char*)lua_tostring(l,1);
  fetch_type  = NULL;
  user_agent  = NULL;
  rc          = QUVI_OK;

  /* Options from LUA script. */
  if (lua_istable(l, 2))
    {
      fetch_type = lua_get_field_s(l, "fetch_type");
      if (fetch_type)
        {
          if (strcmp(fetch_type, "config") == 0)
            status_type = QUVISTATUSTYPE_CONFIG;
          else if (strcmp(fetch_type, "playlist") == 0)
            status_type = QUVISTATUSTYPE_PLAYLIST;
        }
      arbitrary_cookie = lua_get_field_s(l, "arbitrary_cookie");
      user_agent = lua_get_field_s(l, "user_agent");
    }

  if (q->status_func)
    {
      if (q->status_func(makelong(QUVISTATUS_FETCH, status_type),
                         (void *)url) != QUVI_OK)
        {
          return (QUVI_ABORTEDBYCALLBACK);
        }
    }

  *n = new_net_handle();
  if (! (*n))
    return (QUVI_MEM);

  freprintf(&(*n)->url, "%s", url);
  /* TODO: Set options */

  if (q->fetch_func)
    rc = q->fetch_func(*n);
  else
    rc = curl_fetch(q, *n);

  if (rc == QUVI_OK && (*n)->resp_code == 200)
    {
      assert((*n)->fetch.content != NULL);

      if (q->status_func)
        {
          if (q->status_func(makelong(QUVISTATUS_FETCH,
                                      QUVISTATUSTYPE_DONE), 0) != QUVI_OK)
            {
              rc = QUVI_ABORTEDBYCALLBACK;
            }
        }
    }
  else
    {
      if ((*n)->errmsg)
        freprintf(&q->errmsg, "%s", (*n)->errmsg);
    }

  q->httpcode = (*n)->resp_code;

  return (rc);
}

/**
 * Check if URL redirects to another location. For example, URL many
 * shorteners (e.g. is.gd, goo.gl, dai.ly) do this. Replaces the URL
 * with the new location URL (if any).
 */
QUVIcode resolve_wrapper(_quvi_t q, const char *url, char **dst)
{
  _quvi_net_t n;
  QUVIcode rc;

  *dst = NULL;

  if (q->status_func)
    {
      if (q->status_func(QUVISTATUS_RESOLVE, 0) != QUVI_OK)
        return (QUVI_ABORTEDBYCALLBACK);
    }

  n = new_net_handle();
  if (!n)
    return (QUVI_MEM);

  freprintf(&n->url, "%s", url);

  if (q->resolve_func)
    rc = q->resolve_func(n);
  else
    rc = curl_resolve(q, n);

  if (rc == QUVI_OK)
    {
      if (n->redirect.url)
        *dst = strdup(n->redirect.url);

      if (q->status_func)
        {
          const long param =
            makelong(QUVISTATUS_RESOLVE, QUVISTATUSTYPE_DONE);

          rc = q->status_func(param, 0);
        }
    }
  else
    {
      if (n->errmsg)
        freprintf(&q->errmsg, "%s", n->errmsg);
    }

  q->httpcode = n->resp_code;

  free_net_handle(&n);

  return (rc);
}

QUVIcode verify_wrapper(_quvi_t q, _quvi_llst_node_t l)
{
  static const char *scheme = "http://";
  _quvi_video_link_t m;
  _quvi_net_t n;
  QUVIcode rc;
  char b[8];

  rc = QUVI_OK;

  m = (_quvi_video_link_t) l->data;
  m->url = from_html_entities(m->url);

  memset(&b, 0, sizeof(b));

  if (strcmp(strncpy(b, m->url, strlen(scheme)), scheme) != 0)
    return (QUVI_OK); /* Ignore non-HTTP URLs */

  if (q->status_func)
    {
      if (q->status_func(makelong(QUVISTATUS_VERIFY, 0), 0) != QUVI_OK)
        return (QUVI_ABORTEDBYCALLBACK);
    }

  n = new_net_handle();
  if (!n)
    return (QUVI_MEM);

  freprintf(&n->url, "%s", m->url);

  if (q->resolve_func)
    rc = q->verify_func(n);
  else
    rc = curl_verify(q, n);

  if (rc == QUVI_OK)
    {
      freprintf(&m->content_type, "%s", n->verify.content_type);

      m->length = n->verify.content_length;

      rc = run_lua_suffix_func(q, m); /* content-type -> suffix */

      if (q->status_func)
        {
          const long param =
            makelong(QUVISTATUS_VERIFY, QUVISTATUSTYPE_DONE);

          rc = q->status_func(param, 0);
        }
    }
  else
    {
      if (n->errmsg)
        freprintf(&q->errmsg, "%s", n->errmsg);
    }

  q->httpcode = n->resp_code;

  free_net_handle(&n);

  return (rc);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
