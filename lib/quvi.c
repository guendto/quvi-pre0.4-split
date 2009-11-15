/* 
* Copyright (C) 2009 Toni Gundogdu.
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

#include "config.h"

#include <assert.h>

#include <pcre.h>
#include <curl/curl.h>

#include "quvi/quvi.h"
#include "internal.h"
#include "host.h"

/* quvi_init */

QUVIcode
quvi_init(quvi_t *dst) {
    _quvi_t quvi;

    assert(dst != 0);
    if (!dst)
        return (QUVI_INVARG);

    *dst = 0;

    quvi = calloc(sizeof(*quvi), 1);
    if (!quvi)
        return (QUVI_MEM);

    *dst = (quvi_t)quvi;

    quvi->curl = curl_easy_init();
    if (!quvi->curl) {
        _free(quvi);
        return (QUVI_CURLINIT);
    }

    /* set defaults */
    quvi_setopt(quvi, QUVIOPT_FORMAT, "flv");

    csetopt(CURLOPT_USERAGENT,      "Mozilla/5.0");
    csetopt(CURLOPT_FOLLOWLOCATION, 1L);
    csetopt(CURLOPT_NOBODY,         0L);

    return (QUVI_OK);
}

/* quvi_close */

void
quvi_close(quvi_t *handle) {
    _quvi_t *quvi;

    quvi = (_quvi_t *)handle;

    if (quvi && *quvi) {
        _free((*quvi)->format);
        _free((*quvi)->errmsg);
        curl_easy_cleanup((*quvi)->curl);
        _free(*quvi);
    }
}

typedef QUVIcode 
    (*handle_host_func)(const char *url, _quvi_video_t video);

struct handle_host_lookup_s {
    const char *domain;
    handle_host_func func;
    const char *formats;
};

#define _host(n) \
    {domain_##n, handle_##n, formats_##n},

static const struct handle_host_lookup_s
hosts[] = {
_host(youtube)
_host(break)
_host(cctv)
_host(evisor)
_host(clipfish)
_host(funnyhub)
_host(golem)
_host(google)
_host(liveleak)
_host(myubo)
_host(sevenload)
_host(vimeo)
_host(spiegel)
_host(ehrensenf)
_host(dailymotion)
/* Add new ones below. */
{NULL, NULL, NULL}
};

#undef _host

static QUVIcode
handle_url(const char *url, _quvi_video_t video) {
    int error_offset, pcre_code;
    register unsigned int i;
    const char *pcre_errmsg;
    _quvi_t quvi;
    QUVIcode rc;

    assert(url != 0);
    assert(video->quvi != 0);

    if (!url)
        return (QUVI_INVARG);

    if (!video->quvi)
        return (QUVI_BADHANDLE);

    quvi = video->quvi;
    rc   = QUVI_OK;

    for (i=0; hosts[i].func; ++i) {
        pcre *re;

        re = pcre_compile(
            hosts[i].domain,
            PCRE_CASELESS,
            &pcre_errmsg,
            &error_offset,
            NULL
        );

        if (!re) {
            seterr(pcre_errmsg);
            return (QUVI_PCRE);
        }

        pcre_code = pcre_exec(
            re,
            0,
            url,
            strlen(url),
            0,
            0,
            0,
            0
        );

        pcre_free(re);

        if (pcre_code >= 0) /* match */ {
            return (hosts[i].func(url, video));
        }
        else if (pcre_code == PCRE_ERROR_NOMATCH)
            continue; /* move to next */
        else { /* error */
            seterr("pcre_exec: pcre_code = %d", pcre_code);
            return (QUVI_PCRE);
        }
    }

    seterr("no support: %s", url);

    return (QUVI_NOSUPPORT);
}

/* quvi_parse */

QUVIcode
quvi_parse(quvi_t quvi, char *url, quvi_video_t *dst) {
    _quvi_video_t video;
    QUVIcode rc;

    assert(quvi != 0);
    assert(url != 0);
    assert(dst != 0);
    *dst = 0;

    if (!quvi)
        return (QUVI_BADHANDLE);

    if (!url)
        return (QUVI_INVARG);

    if (!dst)
        return (QUVI_INVARG);

    video = calloc(sizeof(*video), 1);
    if (!video)
        return (QUVI_MEM);

    *dst        = video;
    video->quvi = quvi;

    setvid(video->page_link, "%s", url);

    rc = handle_url(url, video);
    if (rc != QUVI_OK)
        return (rc);

#ifdef HAVE_ICONV /* Convert character set encoding to utf8. */
    if (video->charset)
        to_utf8(video);
#endif
    video->title = from_html_entities(video->title);
    video->link  = from_html_entities(video->link);

    if (!video->quvi->no_verify)
        rc = query_file_length(video); 

    return (rc);
}

/* quvi_parse_close */

void
quvi_parse_close(quvi_video_t *handle) {
    _quvi_video_t *video;
    
    video = (_quvi_video_t*)handle;

    if (video && *video) {
        _free((*video)->id);
        _free((*video)->link);
        _free((*video)->title);
        _free((*video)->host_id);
        _free((*video)->page_link);
        _free((*video)->content_type);
        _free((*video)->suffix);
        _free((*video)->charset);
        _free(*video);
    }
}

static QUVIcode
_setopt(_quvi_t quvi, QUVIoption opt, va_list arg) {

#define _sets(opt) \
    do { \
        _free(opt); \
        asprintf(&opt, "%s", va_arg(arg,char *)); \
    } while(0); break

#define _seti(opt) \
    do { opt = va_arg(arg,int); } while(0); break

    switch (opt) {
    case QUVIOPT_FORMAT        : _sets(quvi->format);
    case QUVIOPT_NOVERIFY      : _seti(quvi->no_verify);
    case QUVIOPT_STATUSFUNCTION:
        quvi->status_func = 
            va_arg(arg, quvi_callback_status);
        break;
    default: return (QUVI_INVARG);
    }
#undef _sets
#undef _seti
    return (QUVI_OK);
}

/* quvi_setopt */

QUVIcode
quvi_setopt(quvi_t quvi, QUVIoption opt, ...) {
    va_list arg;
    QUVIcode rc;

    assert(quvi != 0);

    if (!quvi)
        return (QUVI_BADHANDLE);

    va_start(arg, opt);
    rc = _setopt(quvi, opt, arg);
    va_end(arg);

    return (rc);
}

static const char empty[] = "";

static QUVIcode
_getprop(_quvi_video_t video, QUVIproperty prop, ...) {
    va_list arg;
    double *dp;
    char **sp;
    long *lp;
    int type;

    dp = 0;
    sp = 0;
    lp = 0;

    va_start(arg, prop);
    type = QUVIPROPERTY_TYPEMASK & (int)prop;

#define _initv(var,type) \
    do { \
        var = va_arg(arg,type); \
        if (!var) return (QUVI_INVARG); \
    } while(0); break

    switch (type) {
    case QUVIPROPERTY_DOUBLE : _initv(dp, double *);
    case QUVIPROPERTY_STRING : _initv(sp, char **);
    case QUVIPROPERTY_LONG   : _initv(lp, long *);
    default                 : return (QUVI_INVARG);
    }

#define _sets(with) \
    do { *sp = with ? with:(char*)empty; } while(0); break

#define _setn(var,with) \
    do { *var = with; } while(0); break

    switch (prop) {
    case QUVIP_ID            : _sets(video->id);
    case QUVIP_HOSTID        : _sets(video->host_id);
    case QUVIP_LINK          : _sets(video->link);
    case QUVIP_TITLE         : _sets(video->title);
    case QUVIP_LENGTH        : _setn(dp, video->length);
    case QUVIP_PAGELINK      : _sets(video->page_link);
    case QUVIP_CONTENTTYPE   : _sets(video->content_type);
    case QUVIP_SUFFIX        : _sets(video->suffix);
    case QUVIP_HTTPCODE      : _setn(lp, video->quvi->httpcode);
    default                 : return (QUVI_INVARG);
    }

    return (QUVI_OK);
}

static QUVIcode
_getinfo(_quvi_t quvi, QUVIinfo info, ...) {
    va_list arg;
    double *dp;
    char **sp;
    void **vp;
    long *lp;
    int type;

    dp = 0;
    sp = 0;
    vp = 0;
    lp = 0;

    va_start(arg, info);
    type = QUVIINFO_TYPEMASK & (int)info;

    switch (type) {
    case QUVIINFO_DOUBLE: _initv(dp, double *);
    case QUVIINFO_STRING: _initv(sp, char **);
    case QUVIINFO_LONG  : _initv(lp, long *);
    case QUVIINFO_VOID  : _initv(vp, void **);
    default           : return (QUVI_INVARG);
    }

#define _setv(with) \
    do  { *vp = with ? with:NULL; } while(0); break

    switch (info) {
    case QUVII_CURL    : _setv(quvi->curl);
    case QUVII_CURLCODE: _setn(lp, quvi->curlcode);
    case QUVII_HTTPCODE: _setn(lp, quvi->httpcode);
    default          : return (QUVI_INVARG);
    }

    return (QUVI_OK);
}
#undef _setv
#undef _setn
#undef _sets
#undef _initv

/* quvi_getinfo */

QUVIcode
quvi_getinfo(quvi_t quvi, QUVIinfo info, ...) {
    va_list arg;
    void *p;

    assert(quvi != 0);

    if (!quvi)
        return (QUVI_BADHANDLE);

    va_start(arg, info);
    p = va_arg(arg, void *);

    return (_getinfo(quvi, info, p));
}

/* quvi_getprop */

QUVIcode
quvi_getprop(quvi_video_t video, QUVIproperty prop, ...) {
    va_list arg;
    void *p;

    assert(video != 0);

    if (!video)
        return (QUVI_BADHANDLE);

    va_start(arg, prop);
    p = va_arg(arg, void *);

    return (_getprop(video, prop, p));
}

/* quvi_strerror */

char *
quvi_strerror(quvi_t handle, QUVIcode code) {
    static const char *errormsgs[] = {
        "no error",
        "memory allocation failed",
        "bad handle argument to function",
        "invalid argument to function",
        "curl initialization failed",
        "last host entry",
        "invalid error code (internal _QUVI_LAST)"
    };

    _quvi_t quvi = (_quvi_t)handle;

    if (quvi) {
        if (code > _QUVI_LAST)
            return (quvi->errmsg);
    }
    else {
        if (code > _QUVI_LAST)
            code = _QUVI_LAST;
    }

    return ((char *)errormsgs[code]);

}

static int curr_host;

/* quvi_iter_host */

QUVIcode
quvi_iter_host(char **domain, char **formats) {

    if ( !(*domain) && !(*formats) )
        curr_host = -1; /* reset counter */

    if (!hosts[++curr_host].domain)
        return (QUVI_LASTHOST);

    *domain  = (char *)hosts[curr_host].domain;
    *formats = (char *)hosts[curr_host].formats;

    return (QUVI_OK);
}

/* quvi_version */

char *
quvi_version(QUVIversion type) {
    static const char version[] = PACKAGE_VERSION;
    static const char version_long[] =
    PACKAGE_VERSION
    " ("
#ifdef BUILD_DATE
    BUILD_DATE"-"
#endif
    CANONICAL_TARGET
    ")"
#ifdef HAVE_ICONV
    " *iconv"
#endif
    ;
    if (type == QUVI_VERSION_LONG)
        return ((char *)version_long);
    return ((char *)version);
}


