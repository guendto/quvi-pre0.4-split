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

#ifndef quvi_llst_h
#define quvi_llst_h

/** @defgroup libquvi_llst Linked list interface
 *
 * The linked list interface.
 *
 * @since 0.2.16
 * @{
 */

/** @brief Linked list node handle
 * @since 0.2.16
 */
typedef void *quvi_llst_node_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /** @brief Append new node to a linked list
   *
   * @param llst Handle to a linked list node (if NULL, new linked list is created)
   * @param data Data to be appended to the list
   *
   * Example:
   * @code
   * quvi_llst_node_t l = NULL;
   * quvi_llst_append(l, strdup("foo"));
   * quvi_llst_append(l, strdup("bar"));
   * @endcode
   *
   * @return Non-zero if an error occurred
   *
   * @since 0.2.16
   *
   * @warning Make sure that the data is allocated dynamically
   *
   * @sa quvi_llst_free
   */
  QUVIcode quvi_llst_append(quvi_llst_node_t *llst, void *data);

  /** @brief Return the size (number of nodes) in linked list
   *
   * @param llst Handle to a linked list node
   *
   * @return Number of nodes
   *
   * @since 0.2.16
   */
  size_t quvi_llst_size(quvi_llst_node_t llst);

  /** @brief Return next linked node
   *
   * @param llst Handle to a linked list node
   *
   * @return Handle to the next linked list (or NULL)
   *
   * @since 0.2.16
   */
  quvi_llst_node_t quvi_llst_next(quvi_llst_node_t llst);

  /** @brief Return data of the linked list node
   *
   * @param node Handle to a linked list node
   *
   * @return Pointer to the node data
   *
   * @since 0.2.16
   */
  void *quvi_llst_data(quvi_llst_node_t node);

  /** @brief Release memory allocated by the linked list
   *
   * @param llst Handle to a linked list node
   *
   * @since 0.2.16
   */
  void quvi_llst_free(quvi_llst_node_t *llst);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */ /* End of libquvi_llst group. */

#endif /* quvi_llst_h */

/* vim: set ts=2 sw=2 tw=72 expandtab: */
