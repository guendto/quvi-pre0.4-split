/* quvi
 * Copyright (C) 2010  Toni Gundogdu <legatvs@gmail.com>
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

#include <string.h>
#include <errno.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <dirent.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <assert.h>

#include <lualib.h>
#include <lauxlib.h>

#include "quvi/quvi.h"
#include "internal.h"
#include "util.h"
#include "net_wrap.h"
#include "curl_wrap.h"
#include "lua_wrap.h"

#ifdef _0
static void dump_lua_stack(lua_State * L)
{
  /* http://www.lua.org/pil/24.2.3.html */
  int i, top;

  top = lua_gettop(L);

  for (i = 1; i <= top; i++)    /* repeat for each level */
    {
      int t = lua_type(L, i);
      switch (t)
        {
        case LUA_TSTRING:          /* strings */
          printf("`%s'", lua_tostring(L, i));
          break;

        case LUA_TBOOLEAN:         /* booleans */
          printf(lua_toboolean(L, i) ? "true" : "false");
          break;

        case LUA_TNUMBER:          /* numbers */
          printf("%g", lua_tonumber(L, i));
          break;

        default:                   /* other values */
          printf("%s", lua_typename(L, t));
          break;
        }
      printf("  ");               /* put a separator */
    }
  printf("\n");                 /* end the listing */
}
#endif

static _quvi_media_t qv = NULL;

/* "quvi" object functions for LUA scripts. */

static int l_quvi_fetch(lua_State * l)
{
  _quvi_net_t n;
  QUVIcode rc;

  if (!lua_isstring(l, 1))
    luaL_error(l, "`quvi.fetch' expects `url' argument");

  rc = fetch_wrapper(qv->quvi, l, &n); /* net_wrap.c */

  if (rc == QUVI_OK)
    {
      luaL_Buffer b;

      if (!qv->charset)
        run_lua_charset_func(qv, n->fetch.content);

      luaL_buffinit(l, &b);
      luaL_addstring(&b, n->fetch.content);
      luaL_pushresult(&b);
    }

  free_net_handle(&n);

  if (rc != QUVI_OK)
    luaL_error(l, qv->quvi->errmsg);

  return (1);
}

static int l_quvi_resolve(lua_State *l)
{
  char *redirect_url;
  QUVIcode rc;

  if (!lua_isstring(l,1))
    luaL_error(l, "`quvi.resolve' expects `url' argument");

  /* net_wrap.c */
  rc = resolve_wrapper(qv->quvi, lua_tostring(l,1), &redirect_url);

  if (rc == QUVI_OK)
    {
      luaL_Buffer b;
      luaL_buffinit(l,&b);
      luaL_addstring(&b, redirect_url ? redirect_url : "");
      luaL_pushresult(&b);
    }

  _free(redirect_url);

  if (rc != QUVI_OK)
    luaL_error(l, qv->quvi->errmsg);

  return (1);
}

#ifdef _0 /* Replaced by quvi/util:unescape in 0.2.14 */
static int l_quvi_unescape(lua_State * l)
{
  luaL_Buffer b;
  char *tmp;

  if (!lua_isstring(l, -1))
    luaL_error(l, "expected a string");

  tmp = unescape(qv->quvi, strdup((char *)lua_tostring(l, 1)));
  lua_pop(l, 1);

  luaL_buffinit(l, &b);
  luaL_addstring(&b, tmp);
  _free(tmp);
  luaL_pushresult(&b);

  return (1);
}
#endif

static QUVIcode new_lua_script(_quvi_lua_script_t * dst)
{
  struct _quvi_lua_script_s *qls;

  qls = calloc(1, sizeof(*qls));
  if (!qls)
    return (QUVI_MEM);

  *dst = qls;

  return (QUVI_OK);
}

typedef int (*filter_func) (const struct dirent *);

static QUVIcode
scan_dir(_quvi_llst_node_t * dst, const char *path, filter_func filter)
{
  char *show_scandir, *show_script;
  struct dirent *de;
  DIR *dir;

  show_scandir = getenv("QUVI_SHOW_SCANDIR");
  show_script = getenv("QUVI_SHOW_SCRIPT");

  dir = opendir(path);
  if (!dir)
    {
      if (show_scandir)
        {
          fprintf(stderr, "quvi: %s: %s: %s\n",
                  __PRETTY_FUNCTION__, path, strerror(errno));
        }
      return (QUVI_OK);
    }

  if (show_scandir)
    fprintf(stderr, "quvi: %s: %s\n", __PRETTY_FUNCTION__, path);

  while ((de = readdir(dir)))
    {
      if (filter(de))
        {
          _quvi_lua_script_t qls;

          QUVIcode rc = new_lua_script(&qls);

          if (rc != QUVI_OK)
            return (rc);

          asprintf((char **)&qls->basename, "%s", de->d_name);
          asprintf((char **)&qls->path, "%s/%s", path, de->d_name);

          if (show_script)
            {
              fprintf(stderr, "quvi: %s: found script: %s\n",
                      __PRETTY_FUNCTION__, qls->path);
            }

          quvi_llst_append((quvi_llst_node_t*)dst, qls);
        }
    }

  closedir(dir);

  return (QUVI_OK);
}

static QUVIcode
scan_known_dirs(_quvi_llst_node_t * dst, const char *spath,
                filter_func filter)
{
  char *homedir, *path, *basedir, *buf;

#define _scan \
    do { \
        QUVIcode rc = scan_dir (dst, path, filter); \
        _free (path); \
        if (rc != QUVI_OK) \
            return (rc); \
    } while (0)

  /* QUVI_BASEDIR. */
  basedir = getenv("QUVI_BASEDIR");
  if (basedir)
    {
      asprintf(&path, "%s/%s", basedir, spath);
      _scan;
      return (QUVI_OK);
    }

  /* Current working directory. */
  buf = getcwd(NULL,0);
  if (!buf)
    return(QUVI_MEM);

  asprintf(&path, "%s/%s", buf, spath);
  _free(buf);
  _scan;

  /* Home directory. */
  homedir = getenv("HOME");
  if (homedir)
    {
      asprintf(&path, "%s/.quvi/%s", homedir, spath);
      _scan;
      asprintf(&path, "%s/.config/quvi/%s", homedir, spath);
      _scan;
      asprintf(&path, "%s/.local/share/quvi/%s", homedir, spath);
      _scan;
    }

  /* --datadir. */
  asprintf(&path, "%s/%s", DATADIR, spath);
  _scan;

  /* pkgdatadir. */
  asprintf(&path, "%s/%s", PKGDATADIR, spath);
  _scan;

#undef _scan

  return (QUVI_OK);
}

static const luaL_Reg reg_meth[] =
{
  {"fetch", l_quvi_fetch},
  {"resolve", l_quvi_resolve},
#ifdef _0
  {"unescape", l_quvi_unescape},
#endif
  {NULL, NULL}
};

static int lua_files_only(const struct dirent *de)
{
  const char *ext = strrchr(de->d_name, '.');
  return (de->d_name[0] != '.' && ext != 0 && strcmp(ext, ".lua") == 0);
}

/* Init. */

int init_lua(_quvi_t quvi)
{
  QUVIcode rc;

  quvi->lua = (lua_State *) lua_open();
  if (!quvi->lua)
    return (QUVI_LUAINIT);

  luaL_openlibs(quvi->lua);
  luaL_openlib(quvi->lua, "quvi", reg_meth, 1);

  rc = scan_known_dirs(&quvi->util_scripts, "lua/util", lua_files_only);

  if (rc != QUVI_OK)
    return (rc);

  if (quvi_llst_size(quvi->util_scripts) == 0)
    return (QUVI_NOLUAUTIL);

  rc = scan_known_dirs(&quvi->website_scripts, "lua/website",
                       lua_files_only);

  if (rc != QUVI_OK)
    return (rc);

  return (quvi_llst_size(quvi->website_scripts)
          ? QUVI_OK : QUVI_NOLUAWEBSITE);
}

void free_lua(_quvi_t * quvi)
{

#define _rel(w) \
    do { \
        _quvi_llst_node_t curr = w; \
        while (curr) { \
            _quvi_lua_script_t s = (_quvi_lua_script_t) curr->data; \
            _free (s->basename); \
            _free (s->path); \
            curr = curr->next; \
        } \
    } while (0)

  _rel((*quvi)->util_scripts);
  _rel((*quvi)->website_scripts);

#undef _rel

  quvi_llst_free((quvi_llst_node_t*)&(*quvi)->util_scripts);
  assert((*quvi)->util_scripts == NULL);

  quvi_llst_free((quvi_llst_node_t*)&(*quvi)->website_scripts);
  assert((*quvi)->website_scripts == NULL);

  lua_close((*quvi)->lua);
  (*quvi)->lua = NULL;
}

static void set_key(lua_State * l, const char *key)
{
  lua_pushstring(l, key);
  lua_gettable(l, -2);
}

#define _istype(t) \
    do { \
        if (!lua_is##t(l, -1)) { \
            luaL_error (l, \
                "%s: undefined value for key `%s' in the returned table, " \
                "expected `%s' type value", \
                    qls->path, key, #t); \
        } \
    } while (0)

static char *get_field_req_s(lua_State * l, _quvi_lua_script_t qls,
                             const char *key)
{
  char *s;
  set_key(l, key);
  _istype(string);
  s = (char *)lua_tostring(l, -1);
  lua_pop(l, 1);
  return (s);
}

char *lua_get_field_s(lua_State * l, const char *key)
{
  char *s;
  set_key(l, key);
  s = (char *)lua_tostring(l, -1);
  lua_pop(l, 1);
  return (s);
}

static int get_field_b(lua_State * l, _quvi_lua_script_t qls,
                       const char *key)
{
  int b;
  set_key(l, key);
  _istype(boolean);
  b = lua_toboolean(l, -1);
  lua_pop(l, 1);
  return (b);
}

static long get_field_n(lua_State * l, _quvi_lua_script_t qls,
                        const char *key)
{
  long n;
  set_key(l, key);
  _istype(number);
  n = lua_tonumber(l, -1);
  lua_pop(l, 1);
  return (n);
}

static QUVIcode
iter_media_url(lua_State * l,
               _quvi_lua_script_t qls, const char *key,
               _quvi_media_t qv)
{
  QUVIcode rc = QUVI_OK;

  set_key(l, key);
  _istype(table);
  lua_pushnil(l);

  while (lua_next(l, -2) && rc == QUVI_OK)
    {
      if (!lua_isstring(l, -1))
        luaL_error(l, "%s: expected an array of URL strings", qls->path);

      rc = add_media_url(&qv->url, "%s", lua_tostring(l, -1));

      lua_pop(l, 1);
    }

  lua_pop(l, 1);

  return (rc);
}

#undef _istype

static void set_field(lua_State * l, const char *key, const char *value)
{
  lua_pushstring(l, key);
  lua_pushstring(l, value);
  lua_settable(l, -3);
}

static void set_field_n(lua_State * l, const char *key, double value)
{
  lua_pushstring(l, key);
  lua_pushnumber(l, value);
  lua_settable(l, -3);
}

/* Util scripts. */

/* Finds the specified util script from the list. */

static _quvi_lua_script_t find_util_script(_quvi_t quvi,
    const char *basename)
{
  _quvi_llst_node_t curr = quvi->util_scripts;
  while (curr)
    {
      _quvi_lua_script_t s = (_quvi_lua_script_t) curr->data;
      if (strcmp(s->basename, basename) == 0)
        return (s);
      curr = curr->next;
    }
  return (NULL);
}

static QUVIcode
prep_util_script(_quvi_t quvi,
                 const char *script_fname,
                 const char *func_name, lua_State ** pl,
                 _quvi_lua_script_t *qls)
{
  lua_State *l;

  assert(quvi != NULL);
  assert(func_name != NULL);
  assert(script_fname != NULL);

  *pl  = NULL;
  *qls = NULL;

  *qls = find_util_script(quvi, script_fname);
  if (*qls == NULL)
    return (QUVI_NOLUAUTIL);

  l = quvi->lua;
  assert(l != NULL);

  lua_pushnil(l);
  lua_getglobal(l, func_name);

  if (luaL_dofile(l, (*qls)->path))
    luaL_error(l, "%s: %s", (*qls)->path, lua_tostring(l, -1));

  lua_getglobal(l, func_name);

  if (!lua_isfunction(l, -1))
    {
      luaL_error(l,
                 "%s: function `%s' not found", (*qls)->path, func_name);
    }

  *pl = l;

  return (QUVI_OK);
}

/* Executes the `suffix_from_contenttype' lua function. */

QUVIcode run_lua_suffix_func(_quvi_t quvi, _quvi_media_url_t qvl)
{
  const static char script_fname[] = "content_type.lua";
  const static char func_name[] = "suffix_from_contenttype";
  _quvi_lua_script_t qls;
  lua_State *l;
  QUVIcode rc;

  assert(quvi != NULL);
  assert(qvl != NULL);

  rc = prep_util_script(quvi, script_fname, func_name, &l, &qls);
  if (rc != QUVI_OK)
    return (rc);

  assert(l != NULL);
  assert(qls != NULL);

  lua_pushstring(l, qvl->content_type);

  if (lua_pcall(l, 1, 1, 0))
    luaL_error(l, "%s: %s", qls->path, lua_tostring(l, -1));

  if (lua_isstring(l, -1))
    freprintf(&qvl->suffix, "%s", lua_tostring(l, -1));
  else
    {
      luaL_error(l,
                 "%s: expected `%s' function to return a string",
                 qls->path, func_name);
    }

  lua_pop(l, 1);

  return (QUVI_OK);
}

/* Executes the `trim_fields' lua function. */

static QUVIcode run_lua_trim_fields_func(_quvi_media_t media, int ref)
{
  const static char script_fname[] = "trim.lua";
  const static char func_name[] = "trim_fields";
  _quvi_lua_script_t qls;
  _quvi_t quvi;
  lua_State *l;
  QUVIcode rc;

  assert(media != NULL);

  quvi = media->quvi;
  assert(quvi != NULL);

  rc = prep_util_script(quvi, script_fname, func_name, &l, &qls);
  if (rc != QUVI_OK)
    return (rc);

  assert(l != NULL);
  assert(qls != NULL);

  lua_rawgeti(l, LUA_REGISTRYINDEX, ref);

  if (lua_pcall(l, 1, 1, 0))
    luaL_error(l, "%s: %s", qls->path, lua_tostring(l, -1));

  if (!lua_istable(l, -1))
    {
      luaL_error(l,
                 "%s: expected `%s' function to return a table",
                 qls->path, func_name);
    }

  return (QUVI_OK);
}

/* Executes the `charset_from_data' lua function. */

QUVIcode run_lua_charset_func(_quvi_media_t media, const char *data)
{
  const static char script_fname[] = "charset.lua";
  const static char func_name[] = "charset_from_data";
  _quvi_lua_script_t qls;
  _quvi_t quvi;
  lua_State *l;
  QUVIcode rc;

  assert(media != NULL);
  quvi = media->quvi;
  assert(quvi != NULL);

  rc = prep_util_script(quvi, script_fname, func_name, &l, &qls);
  if (rc != QUVI_OK)
    return (rc);

  assert(l != NULL);
  assert(qls != NULL);

  lua_pushstring(l, data);

  if (lua_pcall(l, 1, 1, 0))
    luaL_error(l, "%s: %s", qls->path, lua_tostring(l, -1));

  if (lua_isstring(l, -1))
    freprintf(&media->charset, "%s", lua_tostring(l, -1));
  else if (lua_isnil(l, -1)) ;  /* Charset was not found. We can live with that. */
  else
    {
      luaL_error(l,
                 "%s: expected `%s' function to return a string",
                 qls->path, func_name);
    }

  lua_pop(l, 1);

  return (QUVI_OK);
}

/* Website scripts. */

/* Executes the host script's "ident" function. */

QUVIcode run_ident_func(lua_ident_t ident, _quvi_llst_node_t node)
{
  _quvi_lua_script_t qls;
  char *script_dir;
  _quvi_t quvi;
  lua_State *l;
  QUVIcode rc;

  assert(ident != NULL);
  assert(node != NULL);

  quvi = ident->quvi;
  assert(quvi != NULL);         /* seterr macro uses this. */

  l = quvi->lua;
  assert(l != NULL);

  rc = QUVI_NOSUPPORT;
  qls = (_quvi_lua_script_t) node->data;

  lua_pushnil(l);
  lua_pushnil(l);

  lua_setglobal(l, "ident");
  lua_setglobal(l, "parse");

  if (luaL_dofile(l, qls->path))
    {
      freprintf(&quvi->errmsg, "%s", lua_tostring(l, -1));
      return (QUVI_LUA);
    }

  lua_getglobal(l, "ident");

  if (!lua_isfunction(l, -1))
    {
      freprintf(&quvi->errmsg, "%s: `ident' function not found",
                qls->path);
      return (QUVI_LUA);
    }

  script_dir = dirname_from(qls->path);

  lua_newtable(l);
  set_field(l, "page_url", ident->url);
  set_field(l, "script_dir", script_dir);

  _free(script_dir);

  if (lua_pcall(l, 1, 1, 0))
    {
      freprintf(&quvi->errmsg, "%s", lua_tostring(l, -1));
      return (QUVI_LUA);
    }

  if (lua_istable(l, -1))
    {
      ident->domain = strdup(get_field_req_s(l, qls, "domain"));

      ident->formats = strdup(get_field_req_s(l, qls, "formats"));

      ident->categories = get_field_n(l, qls, "categories");

      rc = get_field_b(l, qls, "handles")
           ? QUVI_OK : QUVI_NOSUPPORT;

      if (rc == QUVI_OK)
        rc = ident->categories & quvi->
             category ? QUVI_OK : QUVI_NOSUPPORT;
    }
  else
    {
      luaL_error(l,
                 "%s: expected `ident' function to return a table", qls->path);
    }

  lua_pop(l, 1);

  return (rc);
}

/* Executes the host script's "parse" function. */

static QUVIcode
run_parse_func(lua_State * l, _quvi_llst_node_t node, _quvi_media_t media)
{
  static const char func_name[] = "parse";
  _quvi_lua_script_t qls;
  char *script_dir;
  _quvi_t quvi;
  QUVIcode rc;

  assert(node != NULL);
  assert(media != NULL);

  quvi = media->quvi;           /* seterr macro needs this. */
  qls = (_quvi_lua_script_t) node->data;
  rc = QUVI_OK;

  lua_getglobal(l, func_name);

  if (!lua_isfunction(l, -1))
    {
      freprintf(&quvi->errmsg,
                "%s: `%s' function not found", qls->path, func_name);
      return (QUVI_LUA);
    }

  script_dir = dirname_from(qls->path);

  lua_newtable(l);
  set_field(l, "page_url", media->page_url);
  set_field(l, "requested_format", media->quvi->format);
  set_field(l, "redirect_url", "");
  set_field(l, "start_time", "");
  set_field(l, "thumbnail_url", "");
  set_field(l, "script_dir", script_dir);
  set_field_n(l, "duration", 0);

  _free(script_dir);

  if (lua_pcall(l, 1, 1, 0))
    {
      freprintf(&quvi->errmsg, "%s", lua_tostring(l, -1));
      return (QUVI_LUA);
    }

  if (!lua_istable(l, -1))
    {
      freprintf(&quvi->errmsg,
                "expected `%s' function return a table", func_name);
      return (QUVI_LUA);
    }

  freprintf(&media->redirect_url, "%s",
            get_field_req_s(l, qls, "redirect_url"));

  if (strlen(media->redirect_url) == 0)
    {
      const int r = luaL_ref(l, LUA_REGISTRYINDEX);

      rc = run_lua_trim_fields_func(media, r);

      luaL_unref(l, LUA_REGISTRYINDEX, r);

      if (rc == QUVI_OK)
        {
          freprintf(&media->host_id, "%s",
                    get_field_req_s(l, qls, "host_id"));

          freprintf(&media->title, "%s",
                    get_field_req_s(l, qls, "title"));

          freprintf(&media->id, "%s",
                    get_field_req_s(l, qls, "id"));

          freprintf(&media->start_time, "%s",
                    get_field_req_s(l, qls, "start_time"));

          freprintf(&media->thumbnail_url, "%s",
                    get_field_req_s(l, qls, "thumbnail_url"));

          media->duration = get_field_n(l, qls, "duration");

          rc = iter_media_url(l, qls, "url", media);
        }
    }

  lua_pop(l, 1);

  return (rc);
}

/* Match host script to the url. */

static _quvi_llst_node_t find_host_script_node(_quvi_media_t media,
    QUVIcode * rc)
{
  _quvi_llst_node_t curr;
  _quvi_t quvi;

  qv = media;
  quvi = media->quvi;           /* seterr macro uses this. */
  curr = quvi->website_scripts;

  while (curr)
    {
      struct lua_ident_s ident;

      ident.quvi = media->quvi;
      ident.url = media->page_url;
      ident.domain = NULL;
      ident.formats = NULL;

      /* Check if script ident this url. */
      *rc = run_ident_func(&ident, curr);

      _free(ident.domain);
      _free(ident.formats);

      if (*rc == QUVI_OK)
        {
          /* Found a script. */
          return (curr);
        }
      else if (*rc != QUVI_NOSUPPORT)
        {
          /* A script error. Terminate with it. */
          return (NULL);
        }

      curr = curr->next;
    }

  /* Trust that run_ident_func sets the rc. */
  freprintf(&quvi->errmsg, "no support: %s", media->page_url);

  return (NULL);
}

/* Match host script to the url */
QUVIcode find_host_script(_quvi_media_t media)
{
  QUVIcode rc;
  find_host_script_node(media, &rc);
  return (rc);
}

/* Match host script to the url and run parse func */
QUVIcode find_host_script_and_parse(_quvi_media_t media)
{
  _quvi_llst_node_t script;
  _quvi_t quvi;
  lua_State *l;
  QUVIcode rc;

  qv = media;
  quvi = media->quvi;           /* seterr macro uses this. */
  l = quvi->lua;

  script = find_host_script_node(media, &rc);
  if (script == NULL)
    return (rc);

  /* found a script. */
  return (run_parse_func(l, script, media));
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
