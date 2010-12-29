/* quvi
 * Copyright (C) 2009,2010  Toni Gundogdu <legatvs@gmail.com>
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

/*
 * simple.c -- a simplistic libquvi example.
 * Ignores errors, see src/quvi.c for a more complete example.
 */

#include <stdio.h>
#include <quvi/quvi.h>

static int status_callback(long param, void *data)
{
  quvi_word status, type;

  status = quvi_loword(param);
  type = quvi_hiword(param);

  printf("status: %d, type: %d\n", status, type);

  return (0);
}

int main(int argc, char **argv)
{
  quvi_t q;                     /* library handle */
  quvi_video_t v;               /* video handle */
  char *lnk;                    /* holds parsed video link */

  quvi_init(&q);
  quvi_setopt(q, QUVIOPT_STATUSFUNCTION, &status_callback);
  quvi_parse(q, "http://vimeo.com/1485507", &v);
  quvi_getprop(v, QUVIPROP_VIDEOURL, &lnk);
  puts(lnk);
  quvi_parse_close(&v);
  quvi_close(&q);

  return (0);
}
