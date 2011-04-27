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

#include <quvi/quvi.h>

/* Deprecated API functions.
 * See include/quvi/quvi.h.in for notes. */

/* quvi_next_videolink */

QUVIcode quvi_next_videolink(quvi_video_t handle)
{
  return (quvi_next_media_url(handle));
}

/* quvi_next_host */

QUVIcode quvi_next_host(char **domain, char **formats)
{
  *domain = *formats = NULL;
  return (QUVI_LAST);
}

/* vim: set ts=2 sw=2 tw=72 expandtab: */
