/* quvi
 * Copyright (C) 2011  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef quvi_net_h
#define quvi_net_h

/** @defgroup libquvi_net_if Network interface
 *
 * The network interface can be used to override the default use of
 * libcurl which libquvi uses by default for tasks requiring network
 * access. Examples of this are fetching, resolving (HTTP
 * redirections) and verifying media stream URLs.
 *
 * @since 0.2.16
 * @{
 */

/** @brief Network handle
* @since 0.2.16
*/
typedef void *quvi_net_t;

/** @brief Network property feature handle
* @since 0.2.16
*/
typedef void *quvi_net_propfeat_t;

/**
 * @brief Fetch callback function
 *
 * This function gets called by libquvi when the library (e.g. website
 * script) attempts to fetch data from an URL.
 *
 * @since 0.2.16
 */
typedef int (*quvi_callback_fetch) (quvi_net_t);

/**
 * @brief Resolve callback function
 *
 * This function gets called by libquvi when the library (e.g. website
 * script) attempts to resolve an URL. A typical example of this would be
 * when the library checks if an URL is a redirection to a new URL.
 *
 * @since 0.2.16
 */
typedef int (*quvi_callback_resolve) (quvi_net_t);

/**
 * @brief Verify callback function
 *
 * This function gets called by libquvi when the library verifies
 * that the media stream URL works.
 *
 * @since 0.2.16
 */
typedef int (*quvi_callback_verify) (quvi_net_t);

/** @enum QUVInetProperty Network property codes to be used with
 * quvi_net_getprop()
 *
 * @since 0.2.16
 */
typedef enum
{
  QUVI_NET_PROPERTY_NONE = 0x00,
  /**< Unused */
  QUVI_NET_PROPERTY_URL  = QUVIPROPERTY_STRING + 1,
  /**< URL */
  QUVI_NET_PROPERTY_REDIRECTURL = QUVIPROPERTY_STRING + 2,
  /**< Redirection URL */
  QUVI_NET_PROPERTY_CONTENT = QUVIPROPERTY_STRING + 3,
  /**< Fetch content */
  QUVI_NET_PROPERTY_CONTENTTYPE = QUVIPROPERTY_STRING + 4,
  /**< Content-type parsed from the returned HTTP header */
  QUVI_NET_PROPERTY_CONTENTLENGTH = QUVIPROPERTY_DOUBLE + 5,
  /**< Content-length parsed from the returned HTTP header */
  QUVI_NET_PROPERTY_RESPONSECODE = QUVIPROPERTY_LONG + 6,
  /**< Response code returned by the server */
  QUVI_NET_PROPERTY_FEATURES = QUVIPROPERTY_VOID + 7,
  /**< Network features, e.g. arbitrary cookie, etc. */

  /* Add new ones below. Bump _QUVI_NET_PROPERTY_LAST accordingly. */

  _QUVI_NET_PROPERTY_LAST = 7 /**< Placeholder */
} QUVInetProperty;

/** @enum QUVInetPropertyFeature Network property feature codes to be used
 * with quvi_net_getprop_feat()
 *
 * @since 0.2.16
 */
typedef enum
{
  QUVI_NET_PROPERTY_FEATURE_NONE = 0x00,
  /**< Unused */
  QUVI_NET_PROPERTY_FEATURE_NAME = QUVIPROPERTY_STRING + 1,
  /**< Name of the property feature */
  QUVI_NET_PROPERTY_FEATURE_VALUE = QUVIPROPERTY_STRING + 2,
  /**< Value of the property feature */

  /* Add new ones below. Bump _QUVI_NET_PROPERTY_FEATURE_LAST accordingly. */

  _QUVI_NET_PROPERTY_FEATURE_LAST = 2
  /**< Placeholder */

} QUVInetPropertyFeature;

/** enum QUVInetPropertyFeatureName Network property feature names
 *
 * @since 0.2.16
 */
typedef enum
{
  QUVI_NET_PROPERTY_FEATURE_NAME_NONE = 0x00,
  /**< Unused */

  /* Unlike other properties, these do not specify "type". */

  QUVI_NET_PROPERTY_FEATURE_ARBITRARYCOOKIE,
  /**< Arbitrary cookie */
  QUVI_NET_PROPERTY_FEATURE_USERAGENT,
  /**< User-agent */

  /* Add new ones below. */

  _QUVI_NET_PROPERTY_FEATURE_NAME_LAST
  /**< Placeholder */

} QUVInetPropertyFeatureName;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /** @brief Get network property
   *
   * @param handle Network handle
   * @param property Property ID
   * @param ... Parameter
   *
   * @return Non-zero if an error occurred
   *
   * @warning Do not attempt to free the memory returned by this function
   *
   * @sa QUVInetProperty
   *
   * @since 0.2.16
   */
  QUVIcode quvi_net_getprop(quvi_net_t handle, QUVInetProperty property, ...);

  /** @brief Set network property
   *
   * @param handle Network handle
   * @param property Property ID
   * @param ... Parameter
   *
   * @return Non-zero if an error occurred
   *
   * @sa QUVInetProperty
   *
   * @since 0.2.16
   */
  QUVIcode quvi_net_setprop(quvi_net_t handle, QUVInetProperty property, ...);

  /** @brief Get network property feature
   *
   * @param handle Network property feature handle
   * @param feature Feature ID
   * @param ... Parameter
   *
   * @return Non-zero if an error occurred
   *
   * @sa QUVInetPropertyFeature
   *
   * @since 0.2.16
   */
  QUVIcode quvi_net_getprop_feat(quvi_net_propfeat_t handle, QUVInetPropertyFeature feature, ...);

  /** @brief Convenience function that wraps the use of quvi_llst_* and
   * quvi_net_getprop_feat().
   *
   * @param handle Network handle
   * @param feature Feature name ID
   *
   * @return Null-terminated string (or NULL)
   *
   * @warning Do not attempt to free the memory returned by this function
   *
   * @since 0.2.16 */
  char *quvi_net_get_one_prop_feat(quvi_net_t handle, QUVInetPropertyFeatureName feature);

  /** @brief Set network error message
   *
   * @param handle Network handle
   * @param fmt Format string
   * @param ... Parameter
   *
   * @return Non-zero if an error occurred
   *
   * @since 0.2.16
   */
  QUVIcode quvi_net_seterr(quvi_net_t handle, const char *fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */ /* End of libquvi_net_if Network interface */

#endif /* quvi_net_h */

/* vim: set ts=2 sw=2 tw=72 expandtab: */
