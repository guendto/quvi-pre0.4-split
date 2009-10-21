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

#include "host.h"

_host_constants(spiegel, "spiegel.de", "vp6_64|vp6_576|vp6_928|h264_1400");

_host_re(re_id,     "(?i)\\/video\\/video-(.*?)\\.");
_host_re(re_title,  "(?i)<headline>(.*?)</");

/* TODO: -f best by rate */
/* TODO: default to vp6_64 if $fmt remains unmatched */

static QUVIcode
match_fname(_quvi_video_t video,
            const char *config,
            char **fname,
            char *format)
{
    char *re_fname, *prefix, *suffix;
    QUVIcode rc;

    asprintf(&re_fname,
        "(?i)<filename>(.*?)%s(.*?)</", format);

    rc = regexp_capture(
        video->quvi,
        config,
        re_fname,
        &prefix,
        &suffix,
        0
    );

    _free(re_fname);

    if (rc != QUVI_OK)
        return (rc);

    asprintf(fname,
        "http://video.spiegel.de/flash/%s%s%s",
            prefix, strupr(format), suffix);

    return (rc);
}

/* handler */

QUVIcode
handle_spiegel(const char *url, _quvi_video_t video) {
    char *playlist_url, *playlist, *config_url, *config;
    char *format, *fname;
    QUVIcode rc;

    fname = 0;

    /* host id */
    _host_id("spiegel");

    /* note: skip page fetch -> parse_page_common is unnecessary */

    /* id */
    _free(video->id);

    rc = regexp_capture(
        video->quvi,
        video->page_link,
        re_id,
        &video->id,
        0
    );

    if (rc != QUVI_OK)
        return (rc);

    /* playlist */
    asprintf(&playlist_url,
        "http://www1.spiegel.de/active/playlist/fcgi/playlist.fcgi/"
        "asset=flashvideo/mode=id/id=%s", video->id);

    rc = fetch_to_mem(video, playlist_url, QUVIST_PLAYLIST, &playlist);

    _free(playlist_url);

    if (rc != QUVI_OK)
        return (rc);

    /* title */
    _free(video->title);

    rc = regexp_capture(
        video->quvi,
        playlist,
        re_title,
        &video->title,
        0
    );

    _free(playlist);

    if (rc != QUVI_OK)
        return (rc);

    /* config */
    asprintf(&config_url,
        "http://video.spiegel.de/flash/%s.xml", video->id);

    rc = fetch_to_mem(video, config_url, QUVIST_CONFIG, &config);

    _free(config_url);

    if (rc != QUVI_OK)
        return (rc);

    /* link */
    format = strdup(
        !strcmp(video->quvi->format,"flv")
        ? "vp6_64" /* same as "flv" */
        : video->quvi->format
    );

    /* Let's go ahead and make a bold assumption this
    is their best. */
    if (!strcmp(format, "best"))
        format = strdup("h264_1400");

    rc = match_fname(video, config, &fname, format);

    _free(format);
    _free(config);

    if (rc == QUVI_OK)
        setvid(video->link, "%s", fname);

    _free(fname);

    return (rc);
}


