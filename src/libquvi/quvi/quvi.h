/* quvi
 * Copyright (C) 2009,2010,2011  Toni Gundogdu <legatvs@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

/**
 * @file quvi.h
 * @mainpage libquvi C API
 *
 * This documentation describes the libquvi C API.
 *
 * @author Toni Gundogdu
 *
 * @example quvi.c
 * @example simple.c
 * @example callback_libsoup.c
 */

#ifndef quvi_h
#define quvi_h

#ifndef DOXY_SKIP
#include <stdlib.h>
#include <stdint.h> /* C99 header */
#endif

/** @defgroup libquvi_types Types
 * Describes the constants and the types used with the API.
 * @{
 */

/** @enum QUVIversion Types used with quvi_version() */
typedef enum
{
  QUVI_VERSION = 0x00,    /**< Version string only */
  QUVI_VERSION_LONG       /**< Version string, build date and misc. features */
} QUVIversion;

/** @enum QUVIcode Return codes */
typedef enum
{
  /* Static error messages. */
  QUVI_OK  = 0x00,        /**< OK */
  QUVI_MEM,               /**< Memory allocation failed */
  QUVI_BADHANDLE,         /**< Bad session handle */
  QUVI_INVARG,            /**< Invalid function argument */
  QUVI_CURLINIT,          /**< libcurl initialization failed */
  QUVI_LAST,              /**< Indicates end of list iteration */
  QUVI_ABORTEDBYCALLBACK, /**< Aborted by callback function */
  QUVI_LUAINIT,           /**< Lua initialization failure */
  QUVI_NOLUAWEBSITE,      /**< Failed to find lua website scripts */
  QUVI_NOLUAUTIL,         /**< Failed to find lua util scripts */
  _INTERNAL_QUVI_LAST,    /**< For library internal use only */
  /* Non-static error messages. */
  QUVI_PCRE = 0x40, /**< libpcre error occurred @deprecated Since 0.2.9 */
  QUVI_NOSUPPORT,   /**< libquvi does not support the website */
  QUVI_CALLBACK,
  /**< Callback error occurred @since 0.2.16
   * @sa quvi_callback_fetch
   * @sa quvi_callback_resolve
   * @sa quvi_callback_verify */
  QUVI_ICONV,       /**< libiconv error occurred */
  QUVI_LUA,         /**< LUA error occurred */

  /* Deprecated as of 0.2.16. Note that these exist only for
   * backward-compatibility. Destined to be removed in 0.2.20. */

  QUVI_CURL = 0x42
  /**< libcurl error occurred
   * @deprecated Since 0.2.16, use QUVI_CALLBACK instead */

} QUVIcode;

/** @enum QUVIstatus Status codes */
typedef enum
{
  QUVISTATUS_FETCH  = 0x00, /**< Status changed to fetch data from URL */
  QUVISTATUS_VERIFY,        /**< Status changed to verify media URL */
  QUVISTATUS_RESOLVE,       /**< Status changed to resolve redirection */

  /* Add new ones below. */

  /* Deprecated as of 0.2.16. Note that these exist only for
   * backward-compatibility. Destined to be removed in 0.2.20. */

  QUVISTATUS_SHORTENED = 0x2
  /**< Status changed to check for shortened URL
   * @deprecated Since 0.2.16, use QUVISTATUS_RESOLVE instead */

} QUVIstatus;

/** @enum QUVIstatusType Status type codes */
typedef enum
{
  /* QUVISTATUS_FETCH */

  QUVISTATUSTYPE_PAGE = 0x00,   /**< Fetching page */
  QUVISTATUSTYPE_CONFIG,        /**< Fetching config */
  QUVISTATUSTYPE_PLAYLIST,      /**< Fetching playlist */

  /* Common types */
  QUVISTATUSTYPE_DONE           /**< General purpose "done" status type */

  /* Add new ones below */
} QUVIstatusType;

/** @enum QUVIoption Option codes to be used with quvi_setopt() */
typedef enum
{
  QUVIOPT_FORMAT = 0x00,
  /**< Requested format, the resulting format may be different from the
   * requested one if the LUA script was unable to parse an URL to the
   * requested format. The scripts are expected to fallback to the
   * 'default' format if the requested format could not be parsed and
   * raise an error if that failed as well.
   */

  QUVIOPT_NOVERIFY,
  /**< Do not verify URL; content-type, content-length (of HTTP), etc.
    will not be queried */

  QUVIOPT_STATUSFUNCTION,
  /**< Callback function for status updates
   * @sa quvi_callback_status */

  QUVIOPT_WRITEFUNCTION,
  /**< Callback function for writing data
   * @sa quvi_callback_write  */

  QUVIOPT_NORESOLVE,
  /**< Do not attempt to resolve URLs that may be redirections,
   * e.g. URL shortening services typically use redirections */

  QUVIOPT_CATEGORY,
  /**< Bit pattern of (OR'd) website script categories */

  QUVIOPT_FETCHFUNCTION,
  /**< Callback function for fetching data
   * @sa quvi_callback_fetch */

  QUVIOPT_RESOLVEFUNCTION,
  /**< Callback function for resolving URL redirections
   * @sa quvi_callback_resolve */

  QUVIOPT_VERIFYFUNCTION,
  /**< Callback function for verifying media stream URL
   * @sa quvi_callback_verify */

  /* Deprecated as of 0.2.16. Note that these exist only for
   * backward-compatibility. Destined to be removed in 0.2.20. */

  QUVIOPT_NOSHORTENED = 0x4
  /**< Do not "decompress" shortened URLs,
   * @deprecated since 0.2.16, use QUVIOPT_NORESOLVE instead */

} QUVIoption;

#define QUVIINFO_VOID       0x100000 /**< void type */
#define QUVIINFO_LONG       0x200000 /**< long type */
#define QUVIINFO_STRING     0x300000 /**< string type */
#define QUVIINFO_DOUBLE     0x400000 /**< double type */
#define QUVIINFO_TYPEMASK   0xf00000 /**< type mask */

/** @enum QUVIcategory Website script category
 *
 * Specify which of the website script categories the application wants
 * to use. The library defaults to QUVIPROTO_HTTP for historical reasons.
 *
 * @note Consider setting the appropriate category in your application,
 * the default behaviour is planned to change to "QUVIPROTO_ALL" in 0.2.20
 *
 * @sa quvi_setopt
 * @sa QUVIOPT_CATEGORY
 */
typedef enum
{
  QUVIPROTO_HTTP = 0x1, /**< Protocol category HTTP */
  QUVIPROTO_MMS  = 0x2, /**< Protocol category MMS */
  QUVIPROTO_RTSP = 0x4, /**< Protocol category RTSP */
  QUVIPROTO_RTMP = 0x8, /**< Protocol category RTMP */
  QUVIPROTO_ALL  =
  (QUVIPROTO_HTTP|QUVIPROTO_MMS|QUVIPROTO_RTSP|QUVIPROTO_RTMP)
  /**< All protocol categories */
} QUVIcategory;

/** @enum QUVIinfo Info codes to be used with quvi_getinfo()
 *
 * The library creates a cURL handle which is used to fetch and
 * verify parsed details. The cURL handle is initialized
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
typedef enum
{
  QUVIINFO_NONE = 0x00,
  /**< Placeholder */

  QUVIINFO_CURL = QUVIINFO_VOID + 1,
  /**< Session libcurl handle */

  QUVIINFO_RESPONSECODE = QUVIINFO_LONG + 3,
  /** Last server returned HTTP code */

  /* Deprecated as of 0.2.16. Note that these exist only for
   * backward-compatibility. Destined to be removed in 0.2.20. */

  QUVIINFO_CURLCODE = QUVIINFO_LONG + 2,
  /**< Last libcurl returned code
   * @deprecated Since 0.2.16 */

  QUVIINFO_HTTPCODE = QUVIINFO_LONG + 3,
  /**< Last returned HTTP code by a server
   * @deprecated Since 0.2.16, use QUVIINFO_RESPONSECODE instead */

  /* Add new ones below. Bump _QUVIINFO_LAST accordingly. */

  _QUVIINFO_LAST = 3 /**< Placeholder */
} QUVIinfo;

#define QUVIPROPERTY_STRING     0x100000 /**< string type */
#define QUVIPROPERTY_LONG       0x200000 /**< long type */
#define QUVIPROPERTY_DOUBLE     0x300000 /**< double type */
#define QUVIPROPERTY_VOID       0x400000 /**< void type */
#define QUVIPROPERTY_TYPEMASK   0xf00000 /**< type mask */

/** @enum QUVIproperty Media property codes to be used with quvi_getprop() */
typedef enum
{
  QUVIPROP_NONE = 0x00,
  /**< Placeholder */

  QUVIPROP_HOSTID = QUVIPROPERTY_STRING + 1,
  /**< Host ID */

  QUVIPROP_PAGEURL = QUVIPROPERTY_STRING + 2,
  /**< Page URL */

  QUVIPROP_PAGETITLE = QUVIPROPERTY_STRING + 3,
  /**< Page title */

  QUVIPROP_MEDIAID = QUVIPROPERTY_STRING + 4,
  /**< Media ID */

  QUVIPROP_MEDIAURL = QUVIPROPERTY_STRING + 5,
  /**< Media URL */

  QUVIPROP_MEDIACONTENTLENGTH = QUVIPROPERTY_DOUBLE + 6,
  /**< Media content length in bytes */

  QUVIPROP_MEDIACONTENTTYPE = QUVIPROPERTY_STRING + 7,
  /**< Media content-type */

  QUVIPROP_FILESUFFIX = QUVIPROPERTY_STRING + 8,
  /**< Parsed file suffix */

  QUVIPROP_RESPONSECODE = QUVIPROPERTY_LONG   + 9,
  /**< Last server returned HTTP code */

  QUVIPROP_FORMAT = QUVIPROPERTY_STRING + 10,
  /**< Requested format, set using QUVIOPT_FORMAT */

  QUVIPROP_STARTTIME = QUVIPROPERTY_STRING + 11,
  /**< Start time for media */

  QUVIPROP_MEDIATHUMBNAILURL = QUVIPROPERTY_STRING + 12,
  /**< Media cover URL, if any */

  QUVIPROP_MEDIADURATION = QUVIPROPERTY_DOUBLE + 13,
  /**< Media duration in msecs */

  /* Add new ones below. Bump _QUVIPROP_LAST accordingly. */

  /* Deprecated as of 0.2.15. Note that these exist only for
   * backward-compatibility. Destined to be removed in 0.2.20.
   * New applications should use the QUVIPROP_MEDIA* values instead. */

  QUVIPROP_VIDEOID = QUVIPROPERTY_STRING + 4,
  /**< Video ID
   * @deprecated Since 0.2.15, use QUVIPROP_MEDIAID instead */

  QUVIPROP_VIDEOURL = QUVIPROPERTY_STRING + 5,
  /**< Video URL
   * @deprecated Since 0.2.15, use QUVIPROP_MEDIAURL instead */

  QUVIPROP_VIDEOFILELENGTH = QUVIPROPERTY_DOUBLE + 6,
  /**< Video file length
   * @deprecated Since 0.2.15, use QUVIPROP_MEDIACONTENTLENGTH instead */

  QUVIPROP_VIDEOFILECONTENTTYPE = QUVIPROPERTY_STRING + 7,
  /**< Video file content-type
   * @deprecated Since 0.2.15, use QUVIPROP_MEDIACONTENTTYPE instead */

  QUVIPROP_VIDEOFILESUFFIX = QUVIPROPERTY_STRING + 8,
  /**< Video file suffix
   * @deprecated Since 0.2.15, use QUVIPROP_FILESUFFIX instead */

  QUVIPROP_HTTPCODE = QUVIPROPERTY_LONG   + 9,
  /**< Last libcurl returned HTTP code
   * @deprecated Since 0.2.16, use QUVIPROP_RESPONSECODE instead */

  QUVIPROP_VIDEOFORMAT = QUVIPROPERTY_STRING + 10,
  /**< Requested video format, set using QUVIOPT_FORMAT
   * @deprecated Since 0.2.15, use QUVIPROP_FORMAT instead */

  _QUVIPROP_LAST = 13
  /**< Placeholder */

} QUVIproperty;

typedef enum
{
  QUVI_IDENT_PROPERTY_NONE = 0x00,
  /**< Unused */
  QUVI_IDENT_PROPERTY_URL = QUVIPROPERTY_STRING + 1,
  /**< URL */
  QUVI_IDENT_PROPERTY_DOMAIN = QUVIPROPERTY_STRING + 2,
  /**< Domain string */
  QUVI_IDENT_PROPERTY_FORMATS = QUVIPROPERTY_STRING + 3,
  /**< Formats string */
  QUVI_IDENT_PROPERTY_CATEGORIES = QUVIPROPERTY_LONG + 4,
  /**< Formats string */
  _QUVI_IDENT_PROPERTY_LAST = 4
  /**< Placeholder */
} QUVIidentProperty;

/** @brief libquvi session handle */
typedef void *quvi_t;

/** @brief Media parsing session handle
* @since 0.2.15
*/
typedef void *quvi_media_t;

/** @brief Video parsing session handle
* @deprecated Since 0.2.15, use quvi_media_t instead
*/
typedef void *quvi_video_t;

/** @brief Webscript ident handle
 * @since 0.2.16
 */
typedef void *quvi_ident_t;

/**
 * @brief Status callback function
 *
 * Callback function for status changes.
 *
 * @note Returning a non-zero value from the callback function will stop libquvi
 *
 * Example:
 * @code
 * static int status_callback (long param, void *data)
 * {
 *   quvi_word status, type;
 *
 *   status = quvi_loword(param);
 *   type   = quvi_hiword(param);
 *
 *   switch (status)
 *     {
 *     case QUVISTATUS_RESOLVE:
 *       switch (type)
 *       {
 *       case QUVISTATUSTYPE_DONE:
 *         puts("done.");
 *         break;
 *       default:
 *         printf (":: Check for URL redirection ...");
 *         break;
 *       }
 *     break;
 *     case QUVISTATUS_FETCH:
 *       switch (type)
 *         {
 *          case QUVISTATUSTYPE_CONFIG:
 *            printf(":: Fetch config ...");
 *            break;
 *          case QUVISTATUSTYPE_PLAYLIST:
 *            printf(":: Fetch playlist ...");
 *            break;
 *          case QUVISTATUSTYPE_DONE:
 *            puts("done.");
 *            break;
 *          default:
 *            printf(":: Fetch %s ...",(char*)data);
 *            break;
 *         }
 *       break;
 *     case QUVISTATUS_VERIFY:
 *       switch (type)
 *         {
 *         case QUVISTATUSTYPE_DONE:
 *           puts("done.");
 *           break;
 *         default:
 *           printf(":: Verify URL ...");
 *           break;
 *         }
 *       break;
 *     }
 *   fflush(stdout);
 *   return (QUVI_OK);
 * }
 *
 * int main (int argc, char *argv[])
 * {
 *   quvi_t quvi;
 *   quvi_init(&quvi);
 *   quvi_setopt(quvi, QUVIOPT_STATUSFUNCTION, status_callback);
 *   ...
 * }
 * @endcode
 */
typedef int (*quvi_callback_status) (long param, void *data);

#define QUVI_WRITEFUNC_ABORT 0x10000000 /**< Abort writing */

/**
 * @brief Write callback function
 *
 * This function gets called by libquvi (or libcurl) as soon as
 * there is data received that needs to be saved.
 *
 * @note Returning QUVI_WRITEFUNC_ABORT from the callback function will stop write
 * @note Ignored with QUVIOPT_FETCHFUNCTION, QUVIOPT_RESOLVEFUNCTION and QUVIOPT_VERIFYFUNCTION
 */
typedef int (*quvi_callback_write) (char *buffer,
                                    size_t size,
                                    size_t nitems,
                                    void *instream);

/** @brief Word type */
typedef uint32_t quvi_word;
/** @brief Byte type */
typedef uint8_t  quvi_byte;

/** @} */ /* End of libquvi_types group. */


/** @defgroup libquvi_macros Macros
 * Describes the available macros.
 * @{
 */

/** @brief Return a low byte from a word type variable  */
#define quvi_lobyte(w) ((quvi_byte)((uint64_t)(w) & 0xff))
/** @brief Return a high byte from a word type variable */
#define quvi_hibyte(w) ((quvi_byte)((uint64_t)(w) >> 8))

/** @brief Return a low word from a long type variable */
#define quvi_loword(l) ((quvi_word)((uint64_t)(l) & 0xffff))
/** @brief Return a high word from a long type variable */
#define quvi_hiword(l) ((quvi_word)((uint64_t)(l) >> 16))

/** @} */ /* End of libquvi_macros group. */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /** @defgroup libquvi_init Initialize
   * @{
   */

  /**
   * @brief Start a new libquvi session
   *
   * Initializes a new session handle and locates the website scripts
   * used to parse the page URLs. The library searches the following
   * paths in the following order:
   *
   *   - $QUVI_BASEDIR defined path (exclusive, overrides default search paths)
   *   - Current working directory (./lua/website/)
   *   - $HOME./quvi/
   *   - $HOME/.config/quvi/
   *   - $HOME/.local/share/quvi/
   *   - --datadir=DIR (and pkgdatadir) specified with configure (script)
   *     - These directories are typically $prefix/share/ and $prefix/share/quvi/
   *
   * Note that you can have libquvi dump (to stderr) the scanned directory paths
   * by setting the QUVI_SHOW_SCANDIR environment variable.
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
   *
   * Example:
   * @code
   * #include <quvi/quvi.h>
   * ...
   * quvi_t quvi;
   * int rc;
   * rc = quvi_init(&quvi);
   * if (rc != QUVI_OK)
   *   {
   *     fprintf(stderr, "error: libquvi: %s\n", quvi_strerror(quvi,rc));
   *     exit(1);
   *   }
   * @endcode
   */
  QUVIcode quvi_init(quvi_t *quvi);


  /** @defgroup libquvi_getinfo Session info
   * @{
   */

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
   *
   * Example:
   * @code
   * long resp_code;
   * quvi_getinfo(quvi, QUVIINFO_RESPONSECODE, &resp_code);
   * @endcode
   */
  QUVIcode quvi_getinfo(quvi_t quvi, QUVIinfo info, ...);

  /** @} */ /* End of libquvi_getinfo group. */


  /** @defgroup libquvi_release Release
   * @{
   */

  /**
   * @brief End a libquvi session
   *
   * @param quvi Handle to a session
   *
   * @sa quvi_init
   *
   * Example:
   * @code
   * quvi_t quvi;
   * quvi_init(&quvi);
   * ...
   * quvi_close(&quvi);
   * @endcode
   */
  void quvi_close(quvi_t *quvi);

  /** @} */ /* End of libquvi_release group. */
  /** @} */ /* End of libquvi_init group. */


  /** @defgroup libquvi_setup Configure
   * @{
   */

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
    *
   * Example:
   * @code
   * quvi_t quvi;
   * quvi_init(&quvi);
   * quvi_setopt(quvi, QUVIOPT_FORMAT, "mp4_360p");
   * ...
   * @endcode
   */
  QUVIcode quvi_setopt(quvi_t quvi, QUVIoption opt, ...);

  /**
   * @brief Default write callback.
   *
   * This is the default write callback function that libquvi
   * uses to write data received using libcurl.
   *
   * @return The number of bytes actually taken care of
   */
  size_t quvi_write_callback_default(void *ptr, size_t size, size_t nmemb, void *stream);

  /** @} */ /* End of libquvi_setup group. */


  /** @defgroup libquvi_parse Parse
   * @{
   */

  /**
   * @brief Start a new media parsing session
   *
   * @param quvi Handle to a session
   * @param url Null-terminated string to an URL
   * @param media Pointer to a new media session handle
   *
   * @note Close each created handle when done and/or before reusing it
   *
   * @return Non-zero if an error occurred
   *
   * @sa quvi_parse_close
   *
   * Example:
   * @code
   * quvi_t quvi;
   * quvi_media_t media;
   * quvi_init(&quvi);
   * quvi_parse(quvi, "http://vimeo.com/1485507", &media);
   * ...
   * @endcode
   */
  QUVIcode quvi_parse(quvi_t quvi, char *url, quvi_media_t *media);


  /** @defgroup libquvi_getprop Media properties
   * @{
   */

  /**
   * @brief Get media property information from a media session handle
   *
   * @param media Handle to a media session
   * @param prop Property ID
   * @param ... Parameter
   *
   * @return Non-zero if an error occurred
   *
   * @warning Do not attempt to free the memory returned by this function
   *
   * @sa QUVIproperty
   *
   * Example:
   * @code
   * char *url;
   * ...
   * quvi_getprop(media, QUVIPROP_MEDIAURL, &url);
   * puts(url);
   * ...
   * @endcode
   */
  QUVIcode quvi_getprop(quvi_media_t media, QUVIproperty prop, ...);

  /**
   * @brief Move to the next media URL (if any)
   *
   * Used to iterate the parsed media URLs. Typically there is only one,
   * although some websites have split the media into several segments.
   *
   * @param media Handle to a media session
   *
   * @return Non-zero if end of list was reached (QUVI_LAST) or an error occurred
   *
   * @remark Historically "cctv" website used segments
   *
   * @since 0.2.15
   *
   * @sa quvi_getprop
   *
   * Example:
   * @code
   * char *url;
   * do
   *   {
   *     quvi_getprop(media, QUVIPROP_MEDIAURL, &url);
   *     puts(url);
   *   }
   * while (quvi_next_media_url(media) == QUVI_OK);
   * @endcode
   */
  QUVIcode quvi_next_media_url(quvi_media_t media);

  /**
   * @brief Move to the next video URL (if any)
   *
   * Used to iterate the parsed video URLs. Typically there is only one,
   * although some websites have split the videos into several segments.
   *
   * @param video Handle to a video session
   *
   * @return Non-zero if end of list was reached (QUVI_LAST) or an error occurred
   *
   * @remark Historically "cctv" website used segments
   *
   * @deprecated Since 0.2.15, use quvi_next_media_url() instead
   *
   * @sa quvi_getprop
   *
   * Example:
   * @code
   * char *url;
   * do
   *   {
   *     quvi_getprop(media, QUVIPROP_MEDIAURL, &url);
   *     puts(url);
   *   }
   * while (quvi_next_videolink(video) == QUVI_OK);
   * @endcode
   */
  QUVIcode quvi_next_videolink(quvi_video_t video);

  /**
   * @brief End a media parsing session
   *
   * @param media Pointer to a media session
   *
   * @sa quvi_parse
   *
   * Example:
   * @code
   * quvi_t quvi;
   * quvi_media_t media;
   * ...
   * quvi_init(&quvi);
   * quvi_parse(quvi, ..., &media);
   * quvi_parse_close(&media);
   * quvi_close(&quvi);
   * @endcode
   */
  void quvi_parse_close(quvi_media_t *media);

  /** @} */ /* End of libquvi_getprop group. */

  /** @} */ /* End of libquvi_parse group */


  /** @defgroup libquvi_util Utility functions
   * @{
   */

  /**
   * @brief Check whether the library could parse the URL
   *
   * The library checks the URL with each website script to see whether it
   * could parse the URL. This function is designed to work without an
   * Internet connection.
   *
   * Most URL shortening services require that the URL is resolved over
   * an Internet connection. This means that most "shortened" URLs will
   * fail with this function (QUVI_NOSUPPORT).
   *
   * @param quvi Handle to session
   * @param url Null-terminated string to an URL
   *
   * @note
   *   - Fails (QUVI_NOSUPPORT) with most "shortened" URLs
   *   - Works with a limited number of URL shorteners, e.g. youtu.be and
   *   dai.ly, in which cases the LUA scripts can resolve to the
   *   destination URL without an Internet connection
   *   - Consider using quvi_supported_ident() instead
   *
   * @since 0.2.9
   *
   * @remarks
   *   - 0.2.9 - 0.2.14: Limited to basic domain name comparison
   *   - 0.2.15: Improved heuristics: Additional checks for different URL parts
   *
   * @sa quvi_supported_ident
   *
   * @return Non-zero if an error occurred
   */
  QUVIcode quvi_supported(quvi_t quvi, char *url);

  /**
   * @brief Check whether the library could parse the URL
   *
   * Identical to quvi_supported() but returns the `ident' data from
   * a matched website script.
   *
   * The library checks the URL with each website script to see whether it
   * could parse the URL. This function is designed to work without an
   * Internet connection.
   *
   * Most URL shortening services require that the URL is resolved over
   * an Internet connection. This means that most "shortened" URLs will
   * fail with this function (QUVI_NOSUPPORT).
   *
   * @param quvi Handle to session
   * @param url Null-terminated string to an URL
   * @param ident Handle to ident data (set to NULL to emulate quvi_supported())
   *
   * @remark Close ident handle with quvi_supported_ident_close() unless
   *  `ident' parameter is set to NULL (see above)
   *
   * @sa quvi_ident_getprop
   * @sa quvi_supported_ident_close
   * @sa quvi_supported
   *
   * @since 0.2.16
   *
   * @return Non-zero if an error occurred
   */
  QUVIcode quvi_supported_ident(quvi_t quvi, char *url, quvi_ident_t *ident);

  /**
   * @brief Get property information from an ident handle
   *
   * @param handle Handle to a website script returned ident data
   * @param property Property ID
   * @param ... Parameter
   *
   * @warning Do not attempt to free the memory returned by this function
   *
   * @sa quvi_supported_ident
   * @sa quvi_supported_ident_close
   *
   * @since 0.2.16
   *
   * @return Non-zero if an error occurred
   *
   * Example:
   * @code
   * quvi_ident_t ident;
   * if (quvi_supported_ident(quvi, url, &ident) == QUVI_OK)
   *  {
   *    char *formats;
   *    quvi_ident_getprop(ident, QUVI_IDENT_PROPERTY_FORMATS, &formats);
   *    puts(formats);
   *    quvi_supported_ident_close(&ident);
   *  }
   * @endcode
   */
  QUVIcode quvi_ident_getprop(quvi_ident_t handle, QUVIidentProperty property, ...);

  /**
   * @brief Close ident handle
   *
   * @sa quvi_supported_ident
   *
   * @since 0.2.16
   */
  void quvi_supported_ident_close(quvi_ident_t *ident);

  /**
   * @brief Return next supported website
   *
   * This function can be used to iterate the supported websites.
   *
   * @param quvi Handle to a session
   * @param domain Pointer to a null-terminated string (e.g. "youtube.com")
   * @param formats Pointer to a null-terminated string (e.g. "default|best|hq|hd")
   *
   * @return QUVI_OK or a non-zero value if an error occurred
   *   - QUVI_LAST indicates end of the list of the websites
   *   - Any other returned non-zero value indicates an actual error and
   *     should be handled accordingly (e.g. relaying the return code to
   *     \a quvi_strerror)
   *
   * @note
   *   - Both domain and formats string must be quvi_free()d after use
   *   - Return value QUVI_LAST is a non-zero value and indicates the end iteration
   *   - Consider handling errors
   *
   * @since 0.2.0
   *
   * Example:
   * @code
   * while (quvi_next_supported_website(quvi, &domain, &formats) == QUVI_OK)
   *   {
   *     printf("%s\t%s\n", domain, formats);
   *     quvi_free(domain);
   *     quvi_free(formats);
   *   }
   * @endcode
   */
  QUVIcode quvi_next_supported_website(quvi_t quvi, char **domain, char **formats);

  /**
   * @brief Next supported host
   *
   * Iterate the list of the supported hosts.
   *
   * @param domain Pointer to a null-terminated string
   * @param formats Pointer to a null-terminated string
   *
   * @return QUVI_LAST (always)
   *
   * @deprecated Since 0.2.0, use quvi_next_supported_website() instead
   */
  QUVIcode quvi_next_host(char **domain, char **formats);

  /**
   * @brief Return a string describing the error code
   *
   * @param quvi Handle to a libquvi session
   * @param code Error code
   *
   * @return Null-terminated string
   *
   * @warning Do not attempt to free the memory returned by this function
   *
   * Example:
   * @code
   * quvi_t quvi;
   * QUVIcode rc = quvi_init(&quvi);
   * if (rc != QUVI_OK)
   *   {
   *     fprintf(stderr, "error: %s\n", quvi_strerror(quvi,rc));
   *     exit (rc);
   *   }
   * quvi_close(&quvi);
   * @endcode
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
   *
   * Example:
   * @code
   * puts( quvi_version(QUVI_VERSION_LONG) );
   * @endcode
   */
  char *quvi_version(QUVIversion type);

  /**
   * @brief Free allocated memory
   *
   * @param ptr Pointer to data
   */
  void quvi_free(void *ptr);

  /**
   * Queries available formats to the URL.  The query is done over an
   * Internet connection.  It resolves any shortened URLs unless
   * QUVIOPT_NORESOLVE is set explicitly with quvi_setopt.  This function
   * checks also if an URL is supported, similarly to that quvi_supported.
   *
   * Unlike quvi_supported, quvi_supported_ident and
   * quvi_next_supported_website which all return a static format ID list as
   * specified in the webscript’s `ident’ function, quvi_query_formats
   * constructs the list of format IDs dynamically for each URL.
   *
   * Please note that this function returns only ‘default’ to those URLs that
   * have their corresponding webscripts handle only one (1) format. e.g.  No
   * internet connection is required in such case and a static string
   * (‘default’) is returned to the caller instead.
   *
   * @param session  Session handle
   * @param url      URL (null-terminated string)
   * @param formats  Null-terminated string (receives)
   *
   * @returns A non-zero value if an error occurred
   *
   * @note "formats" string must be quvi_free()d after use
   *
   * @since 0.2.16.1, backported from 0.2.17
   */
  QUVIcode quvi_query_formats(quvi_t session,
                              char *url,
                              char **formats);

  /** @} */ /* End of libquvi_util group. */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* quvi_h */

/* vim: set ts=2 sw=2 tw=72 expandtab: */
