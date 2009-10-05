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

_host_constants(liveleak, "liveleak.com", "flv");

_host_re(re_id,         "(?i)token=(.*?)'");
_host_re(re_title,      "(?i)<title>(.*?)</title>");
_host_re(re_config,     "(?i)'config','(.*?)'");
_host_re(re_playlist,   "(?i)<file>(.*?)</file>");
_host_re(re_lnk,        "(?i)<location>(.*?)</location>");

QUVIcode
handle_liveleak(const char *url, _quvi_video_t video) {
    char *content, *config_url, *config, *playlist_url, *playlist;
    QUVIcode rc;

    /* host id */
    _host_id("liveleak");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, re_title);

    if (rc != QUVI_OK)
        return (rc);

    video->title = strepl(video->title, "LiveLeak.com - ", "");

    /* config */
    rc = regexp_capture(
        video->quvi,
        content,
        re_config,
        &config_url,
        0
    );

    _free(content);

    if (rc != QUVI_OK)
        return (rc);

    config_url = unescape(video->quvi, config_url);

    rc = fetch_to_mem(video, config_url, QUVIST_CONFIG, &config);

    _free(config_url);

    if (rc != QUVI_OK)
        return (rc);

    /* playlist */
    rc = regexp_capture(
        video->quvi,
        config,
        re_playlist,
        &playlist_url,
        0
    );

    _free(config);

    if (rc != QUVI_OK)
        return (rc);

    playlist_url = unescape(video->quvi, playlist_url);

    rc = fetch_to_mem(video, playlist_url, QUVIST_PLAYLIST, &playlist);

    _free(playlist_url);

    if (rc != QUVI_OK)
        return (rc);

    /* video link */
    _free(video->link);

    rc = regexp_capture(
        video->quvi,
        playlist,
        re_lnk,
        &video->link,
        0
    );

    _free(playlist);

    return (rc);
}


