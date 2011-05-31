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

#include "config.h"

#include <assert.h>
#include <string.h>

#include "quvi/quvi.h"
#include "quvi/net.h"
#include "quvi/llst.h"

#include "internal.h"
#include "lua_wrap.h"
#include "curl_wrap.h"
#include "util.h"
#include "net_wrap.h"

/* quvi_init */

QUVIcode quvi_init(quvi_t * dst)
{
  _quvi_t quvi;
  QUVIcode rc;

  _is_invarg(dst);
  *dst = 0;

  quvi = calloc(1, sizeof(*quvi));
  if (!quvi)
    return (QUVI_MEM);

  *dst = (quvi_t) quvi;

  rc = curl_init(quvi);
  if (rc != QUVI_OK)
    {
      _free(quvi);
      return (rc);
    }

  /* Set quvi defaults. */
  quvi_setopt(quvi, QUVIOPT_FORMAT, "default");
  quvi_setopt(quvi, QUVIOPT_CATEGORY, QUVIPROTO_HTTP);

  return (init_lua(quvi));
}

/* quvi_close */

void quvi_close(quvi_t * handle)
{
  _quvi_t *quvi;

  quvi = (_quvi_t *) handle;

  if (quvi && *quvi)
    {
      free_lua(quvi);
      assert((*quvi)->util_scripts == NULL);
      assert((*quvi)->website_scripts == NULL);

      _free((*quvi)->format);
      assert((*quvi)->format == NULL);

      _free((*quvi)->errmsg);
      assert((*quvi)->errmsg == NULL);

      curl_close(*quvi);
      assert((*quvi)->curl == NULL);

      _free(*quvi);
      assert((*quvi) == NULL);
    }
}

/* quvi_supported */

QUVIcode quvi_supported(quvi_t quvi, char *url)
{
  return quvi_supported_ident(quvi, url, NULL);
}

static QUVIcode resolve_unless_disabled(_quvi_media_t media)
{
  QUVIcode rc = QUVI_OK;

  if (!media->quvi->no_resolve)
    {
      char *redirect_url = NULL;

      rc = resolve_wrapper(media->quvi, media->page_url, &redirect_url);

      if (rc != QUVI_OK)
        return (rc);
      else
        {
          if (redirect_url)
            {
              freprintf(&media->page_url, "%s", redirect_url);
              _free(redirect_url);
            }
        }
    }
  return (rc);
}

/* quvi_parse */

QUVIcode quvi_parse(quvi_t quvi, char *url, quvi_media_t * dst)
{
  _quvi_media_t media;
  QUVIcode rc;

  _is_invarg(dst);
  *dst = 0;
  _is_invarg(url);
  _is_badhandle(quvi);

  media = calloc(1, sizeof(*media));
  if (!media)
    return (QUVI_MEM);

  media->quvi = quvi;
  *dst = media;

  freprintf(&media->page_url, "%s", url);

  rc = resolve_unless_disabled(media);
  if (rc != QUVI_OK)
    return (rc);

  while (1)
    {
      rc = find_host_script_and_parse(media);
      if (rc != QUVI_OK)
        return (rc);
      else
        {
          if (strlen(media->redirect_url))
            {
              /* Found an "in-script redirection instruction", not be
               * confused with "resolving redirection" */
              freprintf(&media->page_url, "%s", media->redirect_url);
              continue;
            }
          else
            break;
        }
    }

#ifdef HAVE_ICONV               /* Convert character set encoding to utf8. */
  if (media->charset)
    to_utf8(media);
#endif
  assert(media->title != NULL); /* must be set in the lua script */

  media->title = from_html_entities(media->title);

  if (!media->quvi->no_verify)
    {
      _quvi_llst_node_t curr = media->url;
      while (curr)
        {
          rc = verify_wrapper(media->quvi, curr);
#ifdef _0
          rc = query_file_length(media->quvi, curr);
#endif
          if (rc != QUVI_OK)
            break;
          curr = curr->next;
        }
    }

  /* set current media url to first url in the list. */
  media->curr = media->url;

  return (rc);
}

/* quvi_parse_close */

void quvi_parse_close(quvi_media_t * handle)
{
  _quvi_media_t *media = (_quvi_media_t *) handle;

  if (media && *media)
    {
      _quvi_llst_node_t curr = (*media)->url;

      while (curr)
        {
          _quvi_media_url_t l = (_quvi_media_url_t) curr->data;
          _free(l->content_type);
          _free(l->suffix);
          _free(l->url);
          curr = curr->next;
        }
      quvi_llst_free((quvi_llst_node_t)&(*media)->url);
      assert((*media)->url == NULL);

      _free((*media)->id);
      _free((*media)->title);
      _free((*media)->charset);
      _free((*media)->page_url);
      _free((*media)->host_id);
      _free((*media)->redirect_url);
      _free((*media)->start_time);
      _free((*media)->thumbnail_url);

      _free(*media);
      assert(*media == NULL);
    }
}

static QUVIcode _setopt(_quvi_t quvi, QUVIoption opt, va_list arg)
{
  switch (opt)
    {
    case QUVIOPT_FORMAT:
      _set_alloc_s(quvi->format);
    case QUVIOPT_NOVERIFY:
      _set_from_arg_n(quvi->no_verify);
    case QUVIOPT_STATUSFUNCTION:
      quvi->status_func = va_arg(arg, quvi_callback_status);
      break;
    case QUVIOPT_WRITEFUNCTION:
      quvi->write_func = va_arg(arg, quvi_callback_write);
      break;
    case QUVIOPT_NORESOLVE:
      _set_from_arg_n(quvi->no_resolve);
    case QUVIOPT_CATEGORY:
      _set_from_arg_n(quvi->category);
    case QUVIOPT_FETCHFUNCTION:
      quvi->fetch_func = va_arg(arg, quvi_callback_fetch);
      break;
    case QUVIOPT_RESOLVEFUNCTION:
      quvi->resolve_func = va_arg(arg, quvi_callback_resolve);
      break;
    case QUVIOPT_VERIFYFUNCTION:
      quvi->verify_func = va_arg(arg, quvi_callback_verify);
      break;
    default:
      return (QUVI_INVARG);
    }
  return (QUVI_OK);
}

/* quvi_setopt */

QUVIcode quvi_setopt(quvi_t quvi, QUVIoption opt, ...)
{
  va_list arg;
  QUVIcode rc;

  _is_badhandle(quvi);

  va_start(arg, opt);
  rc = _setopt(quvi, opt, arg);
  va_end(arg);

  return (rc);
}

const char empty[] = "";

static QUVIcode _getprop(_quvi_media_t media, QUVIproperty prop, ...)
{
  _quvi_media_url_t qvl;
  QUVIcode rc;
  va_list arg;
  double *dp;
  char **sp;
  long *lp;
  int type;

  qvl = (_quvi_media_url_t) media->curr->data;
  assert(qvl != 0);

  rc = QUVI_OK;
  dp = 0;
  sp = 0;
  lp = 0;

  va_start(arg, prop);
  type = QUVIPROPERTY_TYPEMASK & (int)prop;

  switch (type)
    {
    case QUVIPROPERTY_DOUBLE:
      _init_from_arg(dp, double *);
    case QUVIPROPERTY_STRING:
      _init_from_arg(sp, char **);
    case QUVIPROPERTY_LONG:
      _init_from_arg(lp, long *);
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

  switch (prop)
    {
    case QUVIPROP_HOSTID:
      _set_s(media->host_id);
    case QUVIPROP_PAGEURL:
      _set_s(media->page_url);
    case QUVIPROP_PAGETITLE:
      _set_s(media->title);
    case QUVIPROP_MEDIAID:
      _set_s(media->id);
    case QUVIPROP_MEDIAURL:
      _set_s(qvl->url);
    case QUVIPROP_MEDIACONTENTLENGTH:
      _set_from_value_n(dp, qvl->length);
    case QUVIPROP_MEDIACONTENTTYPE:
      _set_s(qvl->content_type);
    case QUVIPROP_FILESUFFIX:
      _set_s(qvl->suffix);
    case QUVIPROP_RESPONSECODE:
      _set_from_value_n(lp, media->quvi->resp_code);
    case QUVIPROP_FORMAT:
      _set_s(media->quvi->format);
    case QUVIPROP_STARTTIME:
      _set_s(media->start_time);
    case QUVIPROP_MEDIATHUMBNAILURL:
      _set_s(media->thumbnail_url);
    case QUVIPROP_MEDIADURATION:
      _set_from_value_n(dp, media->duration);
    default:
      rc = QUVI_INVARG;
    }

  return (rc);
}

static QUVIcode _getinfo(_quvi_t quvi, QUVIinfo info, ...)
{
  QUVIcode rc;
  va_list arg;
  double *dp;
  char **sp;
  void **vp;
  long *lp;
  int type;

  rc = QUVI_OK;
  dp = 0;
  sp = 0;
  vp = 0;
  lp = 0;

  va_start(arg, info);
  type = QUVIINFO_TYPEMASK & (int)info;

  switch (type)
    {
    case QUVIINFO_DOUBLE:
      _init_from_arg(dp, double *);
    case QUVIINFO_STRING:
      _init_from_arg(sp, char **);
    case QUVIINFO_LONG:
      _init_from_arg(lp, long *);
    case QUVIINFO_VOID:
      _init_from_arg(vp, void **);
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

  switch (info)
    {
    case QUVIINFO_CURL:
      _set_v(quvi->curl);
    case QUVIINFO_CURLCODE:
      _set_from_value_n(lp, quvi->curlcode);
    case QUVIINFO_RESPONSECODE:
      _set_from_value_n(lp, quvi->resp_code);
    default:
      rc = QUVI_INVARG;
    }

  return (rc);
}

static QUVIcode _ident_getprop(_quvi_ident_t i, QUVIidentProperty p, ...)
{
  QUVIcode rc;
  va_list arg;
  char **sp;
  long *lp;
#ifdef _UNUSED
  double *dp;
  void **vp;
#endif
  int type;

  rc = QUVI_OK;
  sp = 0;
  lp = 0;
#ifdef _UNUSED
  dp = 0;
  vp = 0;
#endif

  va_start(arg, p);
  type = QUVIPROPERTY_TYPEMASK & (int)p;

  switch (type)
    {
    case QUVIPROPERTY_STRING:
      _init_from_arg(sp, char**);
    case QUVIPROPERTY_LONG:
      _init_from_arg(lp, long*);
#ifdef _UNUSED
    case QUVIPROPERTY_DOUBLE:
      _init_from_arg(dp, double*);
    case QUVIPROPERTY_VOID:
      _init_from_arg(vp, void **);
#endif
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

  switch (p)
    {
    case QUVI_IDENT_PROPERTY_URL:
      _set_s(i->url);
    case QUVI_IDENT_PROPERTY_FORMATS:
      _set_s(i->formats);
    case QUVI_IDENT_PROPERTY_DOMAIN:
      _set_s(i->domain);
    case QUVI_IDENT_PROPERTY_CATEGORIES:
      _set_from_value_n(lp, i->categories);
    default:
      rc = QUVI_INVARG;
    }
  return (rc);
}

/* quvi_getinfo */

QUVIcode quvi_getinfo(quvi_t quvi, QUVIinfo info, ...)
{
  va_list arg;
  void *p;

  _is_badhandle(quvi);

  va_start(arg, info);
  p = va_arg(arg, void *);
  va_end(arg);

  return (_getinfo(quvi, info, p));
}

/* quvi_getprop */

QUVIcode quvi_getprop(quvi_media_t media, QUVIproperty prop, ...)
{
  va_list arg;
  void *p;

  _is_badhandle(media);

  va_start(arg, prop);
  p = va_arg(arg, void *);
  va_end(arg);

  return (_getprop(media, prop, p));
}

/* quvi_next_media_url */

QUVIcode quvi_next_media_url(quvi_media_t handle)
{
  _quvi_media_t media;

  _is_badhandle(handle);

  media = (_quvi_media_t) handle;

  /* start from the first */
  if (!media->curr)
    {
      media->curr = media->url;
      return (QUVI_OK);
    }

  /* move to the next */
  media->curr = media->curr->next;
  if (!media->curr)
    {
      media->curr = media->url;  /* reset */
      return (QUVI_LAST);
    }

  return (QUVI_OK);
}

/* quvi_next_supported_website */

QUVIcode
quvi_next_supported_website(quvi_t handle, char **domain, char **formats)
{
  struct _quvi_ident_s ident;
  _quvi_t quvi;
  QUVIcode rc;

  _is_badhandle(handle);
  quvi = (_quvi_t) handle;

  _is_invarg(domain);
  _is_invarg(formats);

  if (!quvi->website_scripts)
    return (QUVI_NOLUAWEBSITE);

  if (!quvi->curr_website)
    quvi->curr_website = quvi->website_scripts;
  else
    {
      quvi->curr_website = quvi->curr_website->next;
      if (!quvi->curr_website)
        return (QUVI_LAST);
    }

  ident.quvi = quvi;
  ident.url = NULL;
  ident.domain = NULL;
  ident.formats = NULL;

  rc = run_ident_func(&ident, quvi->curr_website);

  if (rc == QUVI_NOSUPPORT)
    {
      /* The website scripts return QUVI_NOSUPPORT in all cases. This is
       * because of the undefined URL that we pass to them above (ident.url
       * = NULL). We are only interested in the `domain' and `formats'
       * information anyway, so this is OK. */
      if (ident.categories & quvi->category)
        {
          *domain = ident.domain;
          *formats = ident.formats;
          rc = QUVI_OK;
        }
      else
        {
          _free(ident.domain);
          _free(ident.formats);
          rc = quvi_next_supported_website(handle, domain, formats);
        }
    }

  return (rc);
}

/* quvi_supported_ident */

QUVIcode quvi_supported_ident(quvi_t quvi, char *url, quvi_ident_t *ident)
{
  _quvi_media_t m;
  QUVIcode rc;

  /* ident may be NULL */
  _is_badhandle(quvi);
  _is_invarg(url);

  m = calloc(1, sizeof(*m));
  if (!m)
    return (QUVI_MEM);

  m->quvi = quvi;
  freprintf(&m->page_url, "%s", url);

  rc = find_host_script(m, (_quvi_ident_t*)ident);

  quvi_parse_close((quvi_media_t)&m);

  return (rc);
}

/* quvi_supported_ident_close */
void quvi_supported_ident_close(quvi_ident_t *handle)
{
  _quvi_ident_t *ident = (_quvi_ident_t*) handle;

  if (ident && *ident)
    {
      _free((*ident)->domain);
      _free((*ident)->formats);
      _free((*ident)->url);
      _free((*ident));
      assert(*ident == NULL);
    }
}

/* quvi_ident_getprop */

QUVIcode quvi_ident_getprop(quvi_ident_t i, QUVIidentProperty prop, ...)
{
  va_list arg;
  void *p;

  _is_badhandle(i);

  va_start(arg, prop);
  p = va_arg(arg, void*);
  va_end(arg);

  return (_ident_getprop(i,prop,p));
}

/* quvi_query_formats */

QUVIcode quvi_query_formats(quvi_t handle, char *url, char **formats)
{
  _quvi_media_t m;
  QUVIcode rc;

  _is_badhandle(handle);
  _is_invarg(url);
  _is_invarg(formats);
  *formats = NULL;

  m = calloc(1, sizeof(*m));
  if (!m)
    return (QUVI_MEM);

  m->quvi = handle;
  freprintf(&m->page_url, "%s", url);

  rc = resolve_unless_disabled(m);
  if (rc != QUVI_OK)
    return (rc);

  rc = find_host_script_and_query_formats(m, formats);

  quvi_parse_close((quvi_media_t)&m);

  return (rc);
}

/* quvi_strerror */

char *quvi_strerror(quvi_t handle, QUVIcode code)
{
  static const char *errormsgs[] =
  {
    "no error",
    "memory allocation failed",
    "bad handle argument to function",
    "invalid argument to function",
    "curl initialization failed",
    "end of list iteration",
    "aborted by callback",
    "lua initilization failed",
    "lua website scripts not found",
    "lua util scripts not found",
    "invalid error code (internal _INTERNAL_QUVI_LAST)"
  };

  _quvi_t quvi = (_quvi_t) handle;

  if (quvi)
    {
      if (code > _INTERNAL_QUVI_LAST)
        return (quvi->errmsg);
    }
  else
    {
      if (code > _INTERNAL_QUVI_LAST)
        code = _INTERNAL_QUVI_LAST;
    }

  return ((char *)errormsgs[code]);
}

/* quvi_version */

char *quvi_version(QUVIversion type)
{
  static const char version[] = PACKAGE_VERSION;
  static const char version_long[] =
#ifdef GIT_DESCRIBE
    GIT_DESCRIBE
#else
    PACKAGE_VERSION
#endif
#ifdef BUILD_DATE
    " built on " BUILD_DATE
#endif
    " for " CANONICAL_TARGET " ("
#ifdef HAVE_ICONV
    "i"
#endif
#ifdef ENABLE_TODO
    "t"
#endif
#ifdef ENABLE_NSFW
    "n"
#endif
    ")";

  if (type == QUVI_VERSION_LONG)
    return ((char *)version_long);
  return ((char *)version);
}

/* quvi_free */

void quvi_free(void *ptr)
{
  if (ptr != NULL)
    free(ptr);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
