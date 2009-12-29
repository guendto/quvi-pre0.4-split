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

#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <curl/curl.h>

#include "quvi/quvi.h"
#include "internal.h"
#include "util.h"
#include "curl.h"

static void *
_realloc(void *p, const size_t size) {
    if (p) return realloc(p,size);
    return malloc(size);
}

struct mem_s {
    size_t size;
    char *p;
};

static size_t
writemem_callback(void *p, size_t size, size_t nmemb, void *data) {
    struct mem_s *m = (struct mem_s*)data;
    const size_t rsize = size * nmemb;
    void *tmp = _realloc(m->p, m->size+rsize+1);
    if (tmp) {
        m->p = (char*)tmp;
        memcpy(&(m->p[m->size]), p, rsize);
        m->size += rsize;
        m->p[m->size] = '\0';
    }
    return (rsize);
}

QUVIcode
fetch_to_mem(
    _quvi_video_t video,
    const char *url,
    const QUVIstatusType type,
    char **dst)
{
    CURLcode curlcode;
    struct mem_s mem;
    long httpcode;
    _quvi_t quvi;
    QUVIcode rc;

    assert(video != 0);
    assert(video->quvi != 0);
    assert(dst != 0);

    quvi = video->quvi;

    if (!video)
        return (QUVI_BADHANDLE);

    if (!quvi)
        return (QUVI_BADHANDLE);

    if (!dst)
        return (QUVI_INVARG);

    *dst = 0;

    if (quvi->status_func)
        quvi->status_func(makelong(QUVIS_FETCH, type), (void *)url);

    csetopt(CURLOPT_URL,       url);
    csetopt(CURLOPT_ENCODING,  "");

    memset(&mem, 0, sizeof(mem));

    csetopt(CURLOPT_WRITEDATA, &mem);
    csetopt(CURLOPT_WRITEFUNCTION, writemem_callback);

    curlcode = curl_easy_perform(quvi->curl);
    httpcode  = 0;
    rc        = QUVI_OK;

    if (curlcode == CURLE_OK) {

        curl_easy_getinfo(quvi->curl,
            CURLINFO_RESPONSE_CODE, &httpcode);

        if (httpcode == 200) {
            if (quvi->status_func)
                quvi->status_func(makelong(QUVIS_FETCH, QUVIST_DONE), 0);
        }
        else {
            seterr("server returned http/%ld", httpcode);
            rc = QUVI_CURL;
        }
    }
    else {
        seterr("%s (curlcode=%d)",
            curl_easy_strerror(curlcode), curlcode);
        rc = QUVI_CURL;
    }

    if (mem.p) {
        *dst = strdup(mem.p);
        _free(mem.p);
        if (rc == QUVI_OK) /* charset */
            parse_charset(video, *dst);
    }

    video->quvi->httpcode = httpcode;
    video->quvi->curlcode = curlcode;

    return (rc);
}

QUVIcode
query_file_length(_quvi_video_t video, const char *url) {
    CURLcode curlcode;
    struct mem_s mem;
    long httpcode;
    _quvi_t quvi;
    QUVIcode rc;

    assert(video != 0);

    if (!video) 
        return (QUVI_BADHANDLE);

    quvi = video->quvi;
    assert(quvi != 0);

    if (!quvi)
        return (QUVI_BADHANDLE);

    if (quvi->status_func)
        quvi->status_func(makelong(QUVIS_VERIFY,0), 0);

    memset(&mem, 0, sizeof(mem));

    csetopt(CURLOPT_WRITEDATA, &mem);
    csetopt(CURLOPT_WRITEFUNCTION, writemem_callback);

    csetopt(CURLOPT_URL, url);
    csetopt(CURLOPT_NOBODY, 1L); /* get -> head */

    curlcode = curl_easy_perform(quvi->curl);

    csetopt(CURLOPT_HTTPGET, 1L); /* reset: head -> get */

    httpcode = 0;
    rc       = QUVI_OK;

    if (curlcode == CURLE_OK) {

        curl_easy_getinfo(quvi->curl,
            CURLINFO_RESPONSE_CODE, &httpcode);

        if (httpcode == 200 || httpcode == 206) {
            const char *ct;
            double length;

            curl_easy_getinfo(quvi->curl,
                CURLINFO_CONTENT_TYPE, &ct);

            _free(video->content_type);
            video->content_type = strdup(ct);

            curl_easy_getinfo(quvi->curl,
                CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length);

            if (video->length) {
                char *dup = strdup(video->length);
                setvid(video->length,
                    "%s%s%.0f", dup, quvi_delim, length);
                _free(dup);
            }
            else {
                setvid(video->length, "%.0f", length);
            }

            if (quvi->status_func)
                quvi->status_func(makelong(QUVIS_VERIFY, QUVIST_DONE), 0);

            /* Content-Type -> suffix. */
            rc = contenttype_to_suffix(video);
        }
        else {
            seterr("server returned http/%ld", httpcode);
            rc = QUVI_CURL;
        }
    }
    else {
        seterr("%s (curlcode=%d)",
            curl_easy_strerror(curlcode), curlcode);
        rc = QUVI_CURL;
    }

    video->quvi->httpcode = httpcode;
    video->quvi->curlcode = curlcode;

    if (mem.p)
        _free(mem.p);

    return (rc);
}


