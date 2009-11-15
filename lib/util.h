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

#ifndef util_h
#define util_h

QUVIcode
regexp_capture(
    _quvi_t quvi,
    const char *content,
    const char *regexp,
    ...
);

QUVIcode
contenttype_to_suffix(_quvi_video_t video);

QUVIcode
parse_charset(_quvi_video_t video, const char *content);

QUVIcode
format_output_filename(_quvi_video_t video, char **dst);

QUVIcode
parse_page_common(
    const char *url,
    _quvi_video_t video,
    char **content,
    const char *re_id,
    const char *re_title
);

QUVIcode
to_utf8(_quvi_video_t video);

char *
strepl(const char *s, const char *what, const char *with);

char *
unescape(_quvi_t quvi, char *s);

int
is_format_supported(const char *fmt, const char *lst);

char *
from_html_entities(char *src);

#endif


