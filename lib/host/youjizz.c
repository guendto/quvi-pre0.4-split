/* 
* Copyright (C) 2010 anon2u.
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

/* NOTE: ported from cclive 0.5.8. */

#include "host.h"

_host_constants(youjizz, "youjizz.com", "flv");

_host_re(re_id,     "(?i)\\?id=(\\d+)");
_host_re(re_title,  "(?i)<title>(.*?)</");
_host_re(re_lnk,    "(?i)['\"]content_video['\"],.*['\"](.*?)['\"]");

QUVIcode
handle_youjizz(const char *url, _quvi_video_t video) {
    char *content, *lnk;
    QUVIcode rc;

    /* host id */
    _host_id("youjizz");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, re_title);

    if (rc != QUVI_OK)
        return (rc);

    /* link */
    rc = regexp_capture(
        video->quvi,
        content,
        re_lnk,
        0,
        0,
        &lnk,
        (void *) 0
    );

    _free(content);

    if (rc != QUVI_OK)
        return (rc);

    /* video link */
    rc = add_video_link(&video->link, "%s", lnk);

    _free(lnk);

    return (QUVI_OK);
}


