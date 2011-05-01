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

#define is_invarg(p) \
  do { if (p == NULL) return (QUVI_INVARG); } while (0)

#define is_badhandle(h) \
  do { if (h == NULL) return (QUVI_BADHANDLE); } while (0)

/* quvi_init */

QUVIcode quvi_init(quvi_t * dst)
{
  _quvi_t quvi;
  QUVIcode rc;

  is_invarg(dst);
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
  _quvi_media_t media;
  QUVIcode rc;

  is_invarg(url);
  is_badhandle(quvi);

  media = calloc(1, sizeof(*media));
  if (!media)
    return (QUVI_MEM);

  media->quvi = quvi;

  freprintf(&media->page_url, "%s", url);

  rc = find_host_script(media);

  quvi_parse_close((quvi_media_t) & media);

  return (rc);
}

/* quvi_parse */

QUVIcode quvi_parse(quvi_t quvi, char *url, quvi_media_t * dst)
{
  _quvi_media_t media;
  QUVIcode rc;

  is_invarg(dst);
  *dst = 0;
  is_invarg(url);
  is_badhandle(quvi);

  media = calloc(1, sizeof(*media));
  if (!media)
    return (QUVI_MEM);

  media->quvi = quvi;
  *dst = media;

  freprintf(&media->page_url, "%s", url);

  if (!media->quvi->no_resolve)
    {
      char *redirect_url = NULL;

      rc = resolve_wrapper(quvi, media->page_url, &redirect_url);

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

#define _sets(opt) \
    do { freprintf(&opt, "%s", va_arg(arg,char*)); } while(0); break

#define _setn(opt) \
    do { opt = va_arg(arg,long); } while(0); break

  switch (opt)
    {
    case QUVIOPT_FORMAT:
      _sets(quvi->format);
    case QUVIOPT_NOVERIFY:
      _setn(quvi->no_verify);
    case QUVIOPT_STATUSFUNCTION:
      quvi->status_func = va_arg(arg, quvi_callback_status);
      break;
    case QUVIOPT_WRITEFUNCTION:
      quvi->write_func = va_arg(arg, quvi_callback_write);
      break;
    case QUVIOPT_NORESOLVE:
      _setn(quvi->no_resolve);
    case QUVIOPT_CATEGORY:
      _setn(quvi->category);
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

static QUVIcode _net_setprop(_quvi_net_t n, QUVInetProperty p, va_list arg)
{
  switch (p)
    {
    case QUVI_NET_PROPERTY_URL:
      _sets(n->url);
    case QUVI_NET_PROPERTY_FEATURES:
      break; /* Ignored: read-only */
    case QUVI_NET_PROPERTY_REDIRECTURL:
      _sets(n->redirect.url);
    case QUVI_NET_PROPERTY_CONTENT:
      _sets(n->fetch.content);
    case QUVI_NET_PROPERTY_CONTENTTYPE:
      _sets(n->verify.content_type);
    case QUVI_NET_PROPERTY_CONTENTLENGTH:
      _setn(n->verify.content_length);
    case QUVI_NET_PROPERTY_RESPONSECODE:
      _setn(n->resp_code);
    default:
      return (QUVI_INVARG);
    }
  return (QUVI_OK);
}

#undef _sets
#undef _setn

/* quvi_setopt */

QUVIcode quvi_setopt(quvi_t quvi, QUVIoption opt, ...)
{
  va_list arg;
  QUVIcode rc;

  is_badhandle(quvi);

  va_start(arg, opt);
  rc = _setopt(quvi, opt, arg);
  va_end(arg);

  return (rc);
}

static const char empty[] = "";

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

#define _initv(var,type) \
    do { \
        if ( !(var = va_arg(arg,type)) ) \
            rc = QUVI_INVARG; \
    } while (0); break

  switch (type)
    {
    case QUVIPROPERTY_DOUBLE:
      _initv(dp, double *);
    case QUVIPROPERTY_STRING:
      _initv(sp, char **);
    case QUVIPROPERTY_LONG:
      _initv(lp, long *);
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

#define _sets(with) \
    do { *sp = with ? with:(char*)empty; } while(0); break

#define _setn(var,with) \
    do { *var = with; } while(0); break

  switch (prop)
    {
    case QUVIPROP_HOSTID:
      _sets(media->host_id);
    case QUVIPROP_PAGEURL:
      _sets(media->page_url);
    case QUVIPROP_PAGETITLE:
      _sets(media->title);
    case QUVIPROP_MEDIAID:
      _sets(media->id);
    case QUVIPROP_MEDIAURL:
      _sets(qvl->url);
    case QUVIPROP_MEDIACONTENTLENGTH:
      _setn(dp, qvl->length);
    case QUVIPROP_MEDIACONTENTTYPE:
      _sets(qvl->content_type);
    case QUVIPROP_FILESUFFIX:
      _sets(qvl->suffix);
    case QUVIPROP_RESPONSECODE:
      _setn(lp, media->quvi->resp_code);
    case QUVIPROP_FORMAT:
      _sets(media->quvi->format);
    case QUVIPROP_STARTTIME:
      _sets(media->start_time);
    case QUVIPROP_MEDIATHUMBNAILURL:
      _sets(media->thumbnail_url);
    case QUVIPROP_MEDIADURATION:
      _setn(dp, media->duration);
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
      _initv(dp, double *);
    case QUVIINFO_STRING:
      _initv(sp, char **);
    case QUVIINFO_LONG:
      _initv(lp, long *);
    case QUVIINFO_VOID:
      _initv(vp, void **);
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

#define _setv(with) \
    do  { *vp = with ? with:NULL; } while(0); break

  switch (info)
    {
    case QUVIINFO_CURL:
      _setv(quvi->curl);
    case QUVIINFO_CURLCODE:
      _setn(lp, quvi->curlcode);
    case QUVIINFO_RESPONSECODE:
      _setn(lp, quvi->resp_code);
    default:
      rc = QUVI_INVARG;
    }

  return (rc);
}

static QUVIcode _net_getprop(_quvi_net_t n, QUVInetProperty p, ...)
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

  va_start(arg, p);
  type = QUVIPROPERTY_TYPEMASK & (int)p;

  switch (type)
    {
    case QUVIPROPERTY_DOUBLE:
      _initv(dp, double*);
    case QUVIPROPERTY_STRING:
      _initv(sp, char**);
    case QUVIPROPERTY_LONG:
      _initv(lp, long*);
    case QUVIPROPERTY_VOID:
      _initv(vp, void **);
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

  switch (p)
    {
    case QUVI_NET_PROPERTY_URL:
      _sets(n->url);
    case QUVI_NET_PROPERTY_REDIRECTURL:
      _sets(n->redirect.url);
    case QUVI_NET_PROPERTY_CONTENT:
      _sets(n->fetch.content);
    case QUVI_NET_PROPERTY_CONTENTTYPE:
      _sets(n->verify.content_type);
    case QUVI_NET_PROPERTY_CONTENTLENGTH:
      _setn(dp, n->verify.content_length);
    case QUVI_NET_PROPERTY_RESPONSECODE:
      _setn(lp, n->resp_code);
    case QUVI_NET_PROPERTY_FEATURES:
      _setv(n->features);
    default:
      rc = QUVI_INVARG;
    }
  return (rc);
}

static QUVIcode
_net_getprop_feat(_quvi_net_propfeat_t n, QUVInetPropertyFeature feature, ...)
{
  QUVIcode rc;
  va_list arg;
  double *dp;
  char **sp;
#ifdef UNUSED
  void **vp;
  long *lp;
#endif
  int type;

  rc = QUVI_OK;
  dp = 0;
  sp = 0;
#ifdef UNUSED
  vp = 0;
  lp = 0;
#endif

  va_start(arg, feature);
  type = QUVIPROPERTY_TYPEMASK & (int)feature;

  switch (type)
    {
    case QUVIPROPERTY_DOUBLE:
      _initv(dp, double*);
    case QUVIPROPERTY_STRING:
      _initv(sp, char**);
    default:
      rc = QUVI_INVARG;
    }
  va_end(arg);

  if (rc != QUVI_OK)
    return (rc);

  switch (feature)
    {
    case QUVI_NET_PROPERTY_FEATURE_NAME:
      _sets(n->name);
    case QUVI_NET_PROPERTY_FEATURE_VALUE:
      _sets(n->value);
    default:
      rc = QUVI_INVARG;
    }
  return (rc);
}

#undef _setv
#undef _setn
#undef _sets
#undef _initv

/* quvi_getinfo */

QUVIcode quvi_getinfo(quvi_t quvi, QUVIinfo info, ...)
{
  va_list arg;
  void *p;

  is_badhandle(quvi);

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

  is_badhandle(media);

  va_start(arg, prop);
  p = va_arg(arg, void *);
  va_end(arg);

  return (_getprop(media, prop, p));
}

/* quvi_next_media_url */

QUVIcode quvi_next_media_url(quvi_media_t handle)
{
  _quvi_media_t media;

  is_badhandle(handle);

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
quvi_next_supported_website(quvi_t handle, char **domain,
                            char **formats)
{
  struct lua_ident_s ident;
  _quvi_t quvi;
  QUVIcode rc;

  is_badhandle(handle);
  quvi = (_quvi_t) handle;

  is_invarg(domain);
  is_invarg(formats);

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

/* quvi_net_setprop */

QUVIcode quvi_net_setprop(quvi_net_t n, QUVInetProperty p, ...)
{
  va_list arg;
  QUVIcode rc;

  is_badhandle(n);

  va_start(arg, p);
  rc = _net_setprop(n, p, arg);
  va_end(arg);

  return (rc);
}

/* quvi_net_getprop */

QUVIcode quvi_net_getprop(quvi_net_propfeat_t n, QUVInetProperty prop, ...)
{
  va_list arg;
  void *p;

  is_badhandle(n);

  va_start(arg, prop);
  p = va_arg(arg, void*);
  va_end(arg);

  return (_net_getprop(n,prop,p));
}

/* quvi_net_getprop_feat */

QUVIcode
quvi_net_getprop_feat(quvi_net_propfeat_t n, QUVInetPropertyFeature opt, ...)
{
  va_list arg;
  void *p;

  is_badhandle(n);

  va_start(arg, opt);
  p = va_arg(arg, void*);
  va_end(arg);

  return (_net_getprop_feat(n,opt,p));
}

/* quvi_net_get_one_prop_feat */

extern const char *_net_property_features[];

static const char *_feat_to_str(QUVInetPropertyFeatureName id)
{
  const char *s = NULL;
  if (id > QUVI_NET_PROPERTY_FEATURE_NAME_NONE
      && id < _QUVI_NET_PROPERTY_FEATURE_NAME_LAST)
    {
      s = _net_property_features[id];
    }
  return (s);
}

char *
quvi_net_get_one_prop_feat(quvi_net_t n, QUVInetPropertyFeatureName id)
{
  quvi_llst_node_t opt;

  quvi_net_getprop(n, QUVI_NET_PROPERTY_FEATURES, &opt);

  while (opt)
    {
      const char *feat_name, *feat_value, *s;
      quvi_net_propfeat_t popt;

      popt = (quvi_net_propfeat_t) quvi_llst_data(opt);

      quvi_net_getprop_feat(popt, QUVI_NET_PROPERTY_FEATURE_NAME, &feat_name);
      quvi_net_getprop_feat(popt, QUVI_NET_PROPERTY_FEATURE_VALUE, &feat_value);

      s = _feat_to_str(id);

      if (s && !strcmp(feat_name,s))
        return ((char*)feat_value);

      opt = quvi_llst_next(opt);
    }
  return (NULL);
}

/* quvi_net_seterr */

QUVIcode quvi_net_seterr(quvi_net_t handle, const char *fmt, ...)
{
  _quvi_net_t n;
  va_list args;

  is_badhandle(handle);
  n = (_quvi_net_t) handle;

  va_start(args, fmt);
  vafreprintf(&n->errmsg, fmt, args);

  return (QUVI_OK);
}

/* quvi_llst_append */

QUVIcode quvi_llst_append(quvi_llst_node_t *l, void *data)
{
  _quvi_llst_node_t n;

  is_badhandle(l);
  is_invarg(data);

  n = calloc(1, sizeof(*n));
  if (!n)
    return (QUVI_MEM);

  if (*l) /* Insert after the last. */
    {
      _quvi_llst_node_t curr = *l;

      while (curr->next)
        curr = curr->next;

      curr->next = n;
    }
  else /* Make the first in the list. */
    {
      n->next = *l;
      *l = n;
    }

  n->data = data;

  return (QUVI_OK);
}

/* quvi_llst_size */

size_t quvi_llst_size(quvi_llst_node_t l)
{
  _quvi_llst_node_t curr = l;
  size_t n = 0;

  while (curr)
    {
      curr = curr->next;
      ++n;
    }

  return (n);
}

/* quvi_llst_next */

quvi_llst_node_t quvi_llst_next(quvi_llst_node_t l)
{
  _quvi_llst_node_t curr = l;

  if (curr)
    curr = curr->next;

  return (curr);
}

/* quvi_llst_data */

void *quvi_llst_data(quvi_llst_node_t l)
{
  _quvi_llst_node_t curr = l;

  if (curr)
    return (curr->data);

  return (curr);
}

/* quvi_llst_free */

void quvi_llst_free(quvi_llst_node_t *l)
{
  _quvi_llst_node_t curr = *l;

  while (curr)
    {
      _quvi_llst_node_t next = curr->next;
      _free(curr->data);
      _free(curr);
      curr = next;
    }

  *l = NULL;
}

#undef is_badhandle
#undef is_invarg

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
