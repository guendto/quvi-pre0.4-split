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

#ifndef internal_h
#define internal_h

#include "platform.h"

#define makelong(low,high) \
    ((long)   (((quvi_word)((uint64_t)(low)  & 0xffff)) | \
    ((uint64_t)((quvi_word)((uint64_t)(high) & 0xffff))) << 16))

#define makeword(low,high) \
    ((quvi_word)(((quvi_byte)((uint64_t)(low)  & 0xff)) | \
    ((quvi_word)((quvi_byte) ((uint64_t)(high) & 0xff))) << 8))

#define _free(p) \
    do { free(p); p=0; } while(0)

#define seterr(args...) \
    do { \
        _free(quvi->errmsg); \
        asprintf(&quvi->errmsg, args); \
    } while(0)

#define setvid(prop,args...) \
    do { \
        _free(prop); \
        asprintf(&prop, args); \
    } while(0)

#define csetopt(opt,param) \
    curl_easy_setopt(quvi->curl,opt,param)

struct _quvi_s {
    char *format;
    int no_verify;
    quvi_callback_status status_func;
    void *curl;
    long httpcode;
    long curlcode;
    char *errmsg;
};

typedef struct _quvi_s *_quvi_t;

struct _quvi_video_s {
    _quvi_t quvi;
    char *id;
    char *link;
    char *title;
    char *host_id;
    char *charset;
    char *page_link;
    char *content_type;
    char *suffix;
    char *length;
};

typedef struct _quvi_video_s *_quvi_video_t;

#endif


