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

/** 
* @file quvi.h
* @brief Query video library C API
* @author Toni Gundogdu
* @version 0.1.0
* @date 2009-09-23
*/

/** @example quvi.c */
/** @example simple.c */

#ifndef quvi_h
#define quvi_h

/* C99 header */
#include <stdint.h>

/** @enum QUVIversion Types for quvi_version() */
typedef enum {
QUVI_VERSION = 0x00,    /**< Version string only */
QUVI_VERSION_LONG,      /**< Version string, build date and misc. features */
} QUVIversion;

/** @enum QUVIcode Return codes */
typedef enum {
/* Static */
QUVI_OK  = 0x00,      /**< OK */
QUVI_MEM,             /**< Memory allocation failed */
QUVI_BADHANDLE,       /**< Bad session handle */
QUVI_INVARG,          /**< Invalid function argument */
QUVI_CURLINIT,        /**< libcurl initialization failed */
QUVI_LASTHOST,        /**< Reached the last host */
_QUVI_LAST,           /**< Unused: for libquvi internal use only */
/* Dynamic */
QUVI_PCRE = 0xff + 1, /**< libpcre error occurred */
QUVI_NOSUPPORT,       /**< libquvi does not support the video host */
QUVI_CURL,            /**< libcurl error occurred */
QUVI_ICONV,           /**< libiconv error occurred */
} QUVIcode;

/** @enum QUVIstatus Status codes */
typedef enum {
QUVIS_FETCH  = 0x00,  /**< Status changed to fetch (page, config, etc.) */
QUVIS_VERIFY,         /**< Status changed to verify video link */
} QUVIstatus;

/** @enum QUVIstatusType Status type codes */
typedef enum {
/* QUVIS_FETCH: */
QUVIST_PAGE = 0x00,   /**< Fetching video page */
QUVIST_CONFIG,        /**< Fetching config */
QUVIST_PLAYLIST,      /**< Fetching playlist */
/* Generic types: */
QUVIST_DONE,          /**< General purpose "done" status type */
} QUVIstatusType;

/** @enum QUVIoption Option codes to be used with quvi_setopt()
*
* If you use QUVIOPT_NOVERIFY, the library will return zero for video
* file length and an empty string for video suffix. Both of them are
* parsed during the video link verification process from the server
* returned HTTP header fields content-length and content-type.
*/
typedef enum {
QUVIOPT_FORMAT = 0x00,  /**< Requested video file format */
QUVIOPT_NOVERIFY,       /**< Do not verify video link */
QUVIOPT_STATUSFUNCTION, /**< Callback function for status updates */
QUVIOPT_NOESCAPE,       /**< Do not escape video link */
} QUVIoption;

/** @enum QUVIconst Misc. constants */
typedef enum {
QUVIINFO_VOID         = 0x100000, /**< void type */
QUVIINFO_LONG         = 0x200000, /**< long type */
QUVIINFO_STRING       = 0x300000, /**< string type */
QUVIINFO_DOUBLE       = 0x400000, /**< double type */
QUVIINFO_TYPEMASK     = 0xf00000, /**< type mask */
QUVIPROPERTY_STRING   = 0x100000, /**< string type */
QUVIPROPERTY_LONG     = 0x200000, /**< long type */
QUVIPROPERTY_DOUBLE   = 0x300000, /**< double type */
QUVIPROPERTY_TYPEMASK = 0xf00000  /**< type mask */
} QUVIconst;

/** @enum QUVIinfo Info codes to be used with quvi_getinfo()
*
* The library creates a cURL handle which is used to fetch and
* verify parsed video details. The cURL handle is initialized
* with the following libcurl options:
*   - CURLOPT_USERAGENT         ("Mozilla/5.0")
*   - CURLOPT_FOLLOWLOCATION    (1)
*   - CURLOPT_NOBODY            (0)
* 
* You can, of course, override those settings in your program. You
* can even use the cURL handle in your program until quvi_close()
* is called which will release the handle. See the src/quvi.c for
* an example of this. Note that libquvi uses the libcurl easy
* interface and not the multi interface.
*
* @warning If you use the libquvi created cURL handle in your program,
* leave the releasing of the handle for the library to do.
*/
typedef enum {
QUVII_NONE        = 0x00,             /**< Unused */
QUVII_CURL        = QUVIINFO_VOID + 1,/**< Session libcurl handle */
QUVII_CURLCODE    = QUVIINFO_LONG + 2,/**< Last libcurl returned code */
QUVII_HTTPCODE    = QUVIINFO_LONG + 3,/**< Last libcurl returned HTTP code */
/* Add new ones below. */
QUVII_LAST        = 3                 /**< A placeholder */
} QUVIinfo;

/** @enum QUVIproperty Video property codes to be used with quvi_getprop() */
typedef enum {
QUVIP_NONE           = 0x00, /**< Unused */
QUVIP_ID             = QUVIPROPERTY_STRING + 1, /**< Video ID */
QUVIP_HOSTID         = QUVIPROPERTY_STRING + 2, /**< Host ID */
QUVIP_LINK           = QUVIPROPERTY_STRING + 3, /**< Video link */
QUVIP_TITLE          = QUVIPROPERTY_STRING + 4, /**< Video title */
QUVIP_LENGTH         = QUVIPROPERTY_DOUBLE + 5, /**< Video length in bytes */
QUVIP_PAGELINK       = QUVIPROPERTY_STRING + 6, /**< Video page link */
QUVIP_CONTENTTYPE    = QUVIPROPERTY_STRING + 7, /**< Video file content-type */
QUVIP_SUFFIX         = QUVIPROPERTY_STRING + 8, /**< Video file suffix */
QUVIP_HTTPCODE       = QUVIPROPERTY_LONG   + 9, /**< Last returned HTTP code */
/* Add new ones below. */
QUVIP_LAST           = 9 /**< Placeholder */
} QUVIproperty;


/** @brief libquvi session handle */
typedef void *quvi_t;
/** @brief Video parsing session handle */
typedef void *quvi_video_t;

/** @brief Status callback function */
typedef int (*quvi_callback_status) (long param, void *data);

/** @brief Word type */
typedef uint32_t quvi_word;
/** @brief Byte type */
typedef uint8_t  quvi_byte;

/** @brief Return a low byte from a word type variable  */
#define quvi_lobyte(w) ((quvi_byte)((uint64_t)(w) & 0xff))
/** @brief Return a high byte from a word type variable */
#define quvi_hibyte(w) ((quvi_byte)((uint64_t)(w) >> 8))

/** @brief Return a low word from a long type variable */
#define quvi_loword(l) ((quvi_word)((uint64_t)(l) & 0xffff))
/** @brief Return a high word from a long type variable */
#define quvi_hiword(l) ((quvi_word)((uint64_t)(l) >> 16))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
* @brief Start a new libquvi session
* 
* @param quvi Pointer to a new handle
*
* @note 
*   - Close each created handle with quvi_close() when done using it
*   - libcurl will use the http_proxy environment setting if it is set
*   - See http://curl.haxx.se/libcurl/c/curl_easy_setopt.html on how to override that
* 
* @return Non-zero if an error occurred
*
* @sa quvi_close
*/
QUVIcode quvi_init(quvi_t *quvi);

/** 
* @brief End a libquvi session
* 
* @param quvi Handle to a session
*
* @sa quvi_init
*/
void quvi_close(quvi_t *quvi);


/** 
* @brief Start a new video parsing session
* 
* @param quvi Handle to a session
* @param url Null-terminated string to an URL
* @param video Pointer to a new video session handle
*
* @note Close each created handle when done and/or before reusing it
* 
* @return Non-zero if an error occurred
*
* @sa quvi_parse_close
*/
QUVIcode quvi_parse(quvi_t quvi, char *url, quvi_video_t *video);

/** 
* @brief End a video parsing session
* 
* @param video Pointer to a video session
*
* @sa quvi_parse
*/
void quvi_parse_close(quvi_video_t *video);


/** 
* @brief Set options for a libquvi session handle
* 
* @param quvi Handle to a libquvi session
* @param opt Option ID
* @param ... Parameter
* 
* @return Non-zero if an error occurred
*
* @sa QUVIoption
*/
QUVIcode quvi_setopt(quvi_t quvi, QUVIoption opt, ...);


/** 
* @brief Get information from a libquvi session handle
* 
* @param quvi Handle to a libquvi session
* @param info Info ID
* @param ... Parameter
* 
* @return Non-zero if an error occurred
*
* @warning Do not attempt to free the memory returned by this function
*
* @sa QUVIinfo
*/
QUVIcode quvi_getinfo(quvi_t quvi, QUVIinfo info, ...);

/** 
* @brief Get video property information from a video session handle
* 
* @param video Handle to a video session
* @param prop Property ID
* @param ... Parameter
* 
* @return Non-zero if an error occurred
*
* @warning Do not attempt to free the memory returned by this function
*
* @sa QUVIproperty
*/
QUVIcode quvi_getprop(quvi_video_t video, QUVIproperty prop, ...);


/** 
* @brief Return the next video host
*
* Iterates through the list of the supported video hosts
* returning domain and the supported video formats.
* 
* @param domain Pointer to a null-terminated string
* @param formats Pointer to a null-terminated string (delim. '|')
*
* @note
*   - Set both params to NULL to start (or reset) the iteration
*   - Check return code for QUVI_LASTHOST which indicates the end-of-interation
* 
* @return Non-zero if reached the last host
*
* @warning Do not attempt to free the memory returned by this function
*/
QUVIcode quvi_iter_host(char **domain, char **formats);


/** 
* @brief Return a string describing the error code
* 
* @param quvi Handle to a libquvi session
* @param code Error code
* 
* @return Null-terminated string
*
* @warning Do not attempt to free the memory returned by this function
*/
char *quvi_strerror(quvi_t quvi, QUVIcode code);

/** 
* @brief Return libquvi version
*
* @param type Version type
* 
* @return Null-terminated string
*
* @warning Do not attempt to free the memory returned by this function
*/
char *quvi_version(QUVIversion type);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* quvi_h */

