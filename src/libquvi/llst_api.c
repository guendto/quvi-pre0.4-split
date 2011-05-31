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

#include "config.h"

#include "quvi/quvi.h"
#include "quvi/net.h"
#include "quvi/llst.h"

#include "internal.h"

/* quvi_llst_append */

QUVIcode quvi_llst_append(quvi_llst_node_t *l, void *data)
{
  _quvi_llst_node_t n;

  _is_badhandle(l);
  _is_invarg(data);

  n = calloc(1, sizeof(*n));
  if (!n)
    return (QUVI_MEM);

  if (*l) /* Insert after the last. */
    {
      _quvi_llst_node_t curr = *l;

      while (curr->next)
        curr = curr->next;

      curr->next = n;
    }
  else /* Make the first in the list. */
    {
      n->next = *l;
      *l = n;
    }

  n->data = data;

  return (QUVI_OK);
}

/* quvi_llst_size */

size_t quvi_llst_size(quvi_llst_node_t l)
{
  _quvi_llst_node_t curr = l;
  size_t n = 0;

  while (curr)
    {
      curr = curr->next;
      ++n;
    }

  return (n);
}

/* quvi_llst_next */

quvi_llst_node_t quvi_llst_next(quvi_llst_node_t l)
{
  _quvi_llst_node_t curr = l;

  if (curr)
    curr = curr->next;

  return (curr);
}

/* quvi_llst_data */

void *quvi_llst_data(quvi_llst_node_t l)
{
  _quvi_llst_node_t curr = l;

  if (curr)
    return (curr->data);

  return (curr);
}

/* quvi_llst_free */

void quvi_llst_free(quvi_llst_node_t *l)
{
  _quvi_llst_node_t curr = *l;

  while (curr)
    {
      _quvi_llst_node_t next = curr->next;
      _free(curr->data);
      _free(curr);
      curr = next;
    }

  *l = NULL;
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
