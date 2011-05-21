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
 * The network interface allows overriding the default use of libcurl in
 * libquvi.
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
 * Called by the library when it attempts to fetch an URL.
 *
 * @note
 * - Must set QUVI_NET_PROPERTY_CONTENT
 * - Must set QUVI_NET_PROPERTY_RESPONSECODE
 * - Must return QUVI_OK or QUVI_CALLBACK if error occurred
 *   - In which case must set error with quvi_net_seterr()
 *
 * @sa quvi_net_getprop
 *
 * @since 0.2.16
 */
typedef int (*quvi_callback_fetch) (quvi_net_t);

/**
 * @brief Resolve callback function
 *
 * Called by the library when it attempts to resolve an URL to a
 * another location.
 *
 * @note
 * - Must set QUVI_NET_PROPERTY_REDIRECTURL if new location was found
 * - Must set QUVI_NET_PROPERTY_RESPONSECODE
 * - Must return QUVI_OK or QUVI_CALLBACK if error occurred
 *   - In which case must set error with quvi_net_seterr()
 *
 * @sa quvi_net_getprop
 *
 * @since 0.2.16
 */
typedef int (*quvi_callback_resolve) (quvi_net_t);

/**
 * @brief Verify callback function
 *
 * Called by the library when it attempts to verify a media stream URL.
 *
 * @note
 * - Must set QUVI_NET_PROPERTY_CONTENTTYPE
 * - Must set QUVI_NET_PROPERTY_CONTENTLENGTH
 * - Must set QUVI_NET_PROPERTY_RESPONSECODE
 * - Must return QUVI_OK or QUVI_CALLBACK if error occurred
 *   - In which case must set error with quvi_net_seterr()
 *
 * @sa quvi_net_getprop
 *
 * @since 0.2.16
 */
typedef int (*quvi_callback_verify) (quvi_net_t);

/** @enum QUVInetProperty Property codes
 *
 * @since 0.2.16
 */
typedef enum
{
  QUVI_NET_PROPERTY_NONE = 0x00,
  /**< Unused */
  QUVI_NET_PROPERTY_URL  = QUVIPROPERTY_STRING + 1,
  /**< URL (Null-terminated string) */
  QUVI_NET_PROPERTY_REDIRECTURL = QUVIPROPERTY_STRING + 2,
  /**< URL to another location (Null-terminated string) */
  QUVI_NET_PROPERTY_CONTENT = QUVIPROPERTY_STRING + 3,
  /**< Content (Null-terminated string) */
  QUVI_NET_PROPERTY_CONTENTTYPE = QUVIPROPERTY_STRING + 4,
  /**< Content-type parsed from the returned HTTP header
   * (Null-terminated string) */
  QUVI_NET_PROPERTY_CONTENTLENGTH = QUVIPROPERTY_DOUBLE + 5,
  /**< Content-length parsed from the returned HTTP header (long) */
  QUVI_NET_PROPERTY_RESPONSECODE = QUVIPROPERTY_LONG + 6,
  /**< Response code returned by the server (long) */
  QUVI_NET_PROPERTY_FEATURES = QUVIPROPERTY_VOID + 7,
  /**< Network features, e.g. arbitrary cookie, etc. (Pointer to a
   * linked list)
   * @sa quvi_llst_next
   * @sa quvi_llst_data
   */

  /* Add new ones below. Bump _QUVI_NET_PROPERTY_LAST accordingly. */

  _QUVI_NET_PROPERTY_LAST = 7 /**< Placeholder */
} QUVInetProperty;

/** @enum QUVInetPropertyFeature Property feature codes
 *
 * @since 0.2.16
 */
typedef enum
{
  QUVI_NET_PROPERTY_FEATURE_NONE = 0x00,
  /**< Unused */
  QUVI_NET_PROPERTY_FEATURE_NAME = QUVIPROPERTY_STRING + 1,
  /**< Name of the property feature (Null-terminated string) */
  QUVI_NET_PROPERTY_FEATURE_VALUE = QUVIPROPERTY_STRING + 2,
  /**< Value of the property feature (Null-terminated string) */

  /* Add new ones below. Bump _QUVI_NET_PROPERTY_FEATURE_LAST accordingly. */

  _QUVI_NET_PROPERTY_FEATURE_LAST = 2
  /**< Placeholder */

} QUVInetPropertyFeature;

/** enum QUVInetPropertyFeatureName Property feature names
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

  /** @brief Convenience function that wraps quvi_net_getprop_feat()
   *
   * @param handle Network handle
   * @param feature Feature name ID
   *
   * @return Null-terminated string or NULL
   *
   * @warning Do not attempt to free the memory returned by this function
   *
   * @note Returns the first matching result
   *
   * @sa quvi_llst_next
   * @sa quvi_llst_data
   *
   * @since 0.2.16
   */
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
