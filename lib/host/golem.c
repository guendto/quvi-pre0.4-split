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

_host_constants(golem, "golem.de", "flv|ipod|high");

_host_re(re_id,     "(?i)\"id\", \"(.*?)\"");
_host_re(re_title,  "(?i)<title>(.*?)</title>");

QUVIcode
handle_golem(const char *url, _quvi_video_t video) {
    char *content, *config_url, *config, *format;
    QUVIcode rc;

    format = video->quvi->format;

    /* host id */
    _host_id("golem");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, NULL);

    _free(content);

    if (rc != QUVI_OK)
        return (rc);

    /* config */
    asprintf(&config_url,
        "http://video.golem.de/xml/%s", video->id);

    rc = fetch_to_mem(video, config_url, QUVIST_CONFIG, &config);

    _free(config_url);

    if (rc != QUVI_OK)
        return (rc);

    _free(video->title);

    rc = regexp_capture(
        video->quvi,
        config,
        re_title,
        0,
        0,
        &video->title,
        0
    );

    _free(config);

    if (rc != QUVI_OK)
        return (rc);

    /* video link */
    setvid(video->link, "http://video.golem.de/download/%s", video->id);

    /* format */
    if (format) {
        char *fmt;

        /*
        * Known formats: medium (sd), high, ipod
        * Listed in the config.
        * 
        * Website default is "medium" which we can
        * get without specifying the "?q=" param.
        */

        fmt = 0; /* default: medium */

        if (!strcmp(format, "best"))
            fmt = "high";
        else {
            if (!strcmp(format, "ipod"))
                fmt = "ipod";
            else if (!strcmp(format, "high"))
                fmt = "high";
        }

        if (fmt) {
            char *tmp;

            asprintf(&tmp, "%s?q=%s", video->link, fmt);
            _free(video->link);
            video->link = tmp;
        }
    }

    return (QUVI_OK);
}


