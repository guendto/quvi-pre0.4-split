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

_host_constants(dailymotion,
    "dailymotion.com", "flv|spark-mini|vp6-hq|vp6-hd|vp6|h264");

_host_re(re_id,     "(?i)video\\/(.*?)_");
_host_re(re_title,  "(?i)<h1 class=\"dmco_title\">(.*?)</");
_host_re(re_paths,  "(?i)\"video\", \"(.*?)\"");

static QUVIcode
parse(const _quvi_video_t video, char *paths) {
    int best_width, best_flag;
    char *s, *t, *format;
    QUVIcode rc;

    format = 
        !strcmp(video->quvi->format, "flv")
        ? "spark"
        : video->quvi->format;

    best_width = 0;
    best_flag  = !strcmp(format, "best");

    s = paths; /* we're not gonna take, no-o, we're not gonna take it */
    t = strtok(s, "||");

    while (t) {
        char *path, *id;

        rc = regexp_capture(
            video->quvi,
            t,
            "(.*?)@@(.*?)$",
            &path,
            &id,
            0
        );

        if (rc != QUVI_OK)
            return (rc);

        if (!best_flag) {
            if (!strcmp(id,format)) {
                setvid(video->link, "%s", path);
                _free(path);
                _free(id);
                return (rc);
            }
        }
        else { /* -f best */
            char *w, *h;

            rc = regexp_capture(
                video->quvi,
                path,
                "-(\\d+)x(\\d+)",
                &w,
                &h,
                0
            );

            if (rc != QUVI_OK) {
                _free(path);
                _free(id);
                return (rc);
            }

            if (atoi(w) > best_width) {
                best_width = atoi(w);
                setvid(video->link, "%s", path);
            }

            _free(w);
            _free(h);
        }

        _free(path);
        _free(id);

        t = strtok(0, "||");
    }

    return (rc);
}

QUVIcode
handle_dailymotion(const char *url, _quvi_video_t video) {
    char *content, *paths;
    QUVIcode rc;

    /* host id */
    _host_id("dailymotion");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, re_title);

    if (rc != QUVI_OK)
        return (rc);

    /* paths */
    rc = regexp_capture(
        video->quvi,
        content,
        re_paths,
        &paths,
        0
    );

    _free(content);

    if (rc != QUVI_OK)
        return (rc);

    paths = unescape(video->quvi, paths); /* frees paths */

    rc = parse(video, paths);

    _free(paths);

    return (rc);
}


