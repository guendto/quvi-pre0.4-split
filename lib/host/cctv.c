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

#include "../host.h"

_host_constants(cctv, "cctv.com", "flv");

_host_re(re_id,     "(?i)videoid=(.*?)&");
_host_re(re_title,  "(?i)<meta name=\"description\"\\s+content=\"(.*?)\"");
_host_re(re_domain, "http:\\/\\/(.*?)\\/");
_host_re(re_path,   "url\":\"(.*?)\"");

QUVIcode
handle_cctv(const char *url, _quvi_video_t video) {
    char *content, *domain, *config_url, *config, *path;
    QUVIcode rc;

    /* host id */
    _host_id("cctv");

    /* common */
    rc = parse_page_common(url, video, &content, re_id, re_title);

    _free(content);

    if (rc != QUVI_OK)
        return (rc);

    /* fpath */
    rc = regexp_capture(
        video->quvi,
        url,
        re_domain,
        &domain,
        0
    );

    if (rc != QUVI_OK)
        return (rc);

    /* config url */
    asprintf(&config_url,
        "http://%s/playcfg/flv_info_new.jsp?videoId=%s", domain, video->id);

    _free(domain);

    /* config */
    rc = fetch_to_mem(video, config_url, QUVIST_CONFIG, &config);

    _free(config_url);

    if (rc != QUVI_OK)
        return (rc);

    /* video path */
    rc = regexp_capture(
        video->quvi,
        config,
        re_path,
        &path,
        0
    );

    _free(config);

    if (rc != QUVI_OK)
        return (rc);

    /* video link */
    setvid(video->link,
        "http://v.cctv.com/flash/%s", path);

    _free(path);

    return (QUVI_OK);
}


