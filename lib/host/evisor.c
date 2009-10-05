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

_host_constants(evisor, "evisor.tv", "flv");

_host_re(re_id,     "(?i)unit_long(.*?)\"");
_host_re(re_title,  "(?i)<center><h1>(.*?)</");
_host_re(re_lnk,    "(?i)file=(.*?)\"");

QUVIcode
handle_evisor(const char *url, _quvi_video_t video) {
    char *content;
    QUVIcode rc;

    /* host id */
    _host_id("evisor");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, re_title);

    if (rc != QUVI_OK)
        return (rc);

    /* video link */
    _free(video->link);

    rc = regexp_capture(
        video->quvi,
        content,
        re_lnk,
        &video->link,
        0
    );

    _free(content);

    return (rc);
}


