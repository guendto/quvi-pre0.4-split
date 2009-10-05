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

_host_constants(break, "break.com", "flv");

_host_re(re_id,     "(?i)contentid='(.*?)'");
_host_re(re_title,  "(?i)id=\"vid_title\" content=\"(.*?)\"");
_host_re(re_fpath,  "(?i)contentfilepath='(.*?)'");
_host_re(re_fname,  "(?i)filename='(.*?)'");

QUVIcode
handle_break(const char *url, _quvi_video_t video) {
    char *content, *fpath, *fname;
    QUVIcode rc;

    /* host id */
    _host_id("break");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, re_title);

    if (rc != QUVI_OK)
        return (rc);

    /* fpath */
    rc = regexp_capture(
        video->quvi,
        content,
        re_fpath,
        &fpath,
        0
    );

    if (rc != QUVI_OK) {
        _free(content);
        return (rc);
    }

    /* fname */
    rc = regexp_capture(
        video->quvi,
        content,
        re_fname,
        &fname,
        0
    );

    _free(content);

    if (rc != QUVI_OK) {
        _free(fpath);
        return (rc);
    }

    /* video link */
    setvid(video->link,
        "http://video1.break.com/dnet/media/%s/%s.flv", fpath, fname);

    _free(fpath);
    _free(fname);

    return (QUVI_OK);
}


