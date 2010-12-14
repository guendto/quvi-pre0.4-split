
-- quvi
-- Copyright (C) 2010  Toni Gundogdu <legatvs@gmail.com>
--
-- This file is part of quvi <http://quvi.sourceforge.net/>.
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2.1 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
-- 02110-1301  USA
--

-- Identify the script.
function ident (page_url)
    local t   = {}
    t.domain  = "cctv.com"
    t.formats = "default"
    t.handles = (page_url ~= nil and page_url:find(t.domain) ~= nil)
    return t
end

-- Parse video URLs.
function parse (video)
    video.host_id = "cctv"
    local page    = quvi.fetch(video.page_url)

    local _,_,s = page:find('<meta name="description"%s+content="(.-)"')
    video.title = s or error ("no match: video title")

    local _,_,s = page:find("videoId=(.-)&")
    video.id    = s or error ("no match: video id")

    local _,_,s = video.page_url:find("http://(.-)/")
    local domain = s or error ("no match: domain")

    local config_url =
        string.format("http://%s/playcfg/flv_info_new.jsp?videoId=%s",
            domain, video.id)

    local config = quvi.fetch(config_url, "config")

    local _,_,s    = config:find('"chapters":%[(.-)%]')
    local chapters = s or error ("no match: chapters")
    video.url      = {}

    -- Some cctv videos have "segments". Include all of them.
    for path in chapters:gfind('{"url":"(.-)",') do
        table.insert(video.url,
            string.format("http://v.cctv.com/flash/%s", path))
    end

    if (table.getn(video.url) == 0) then
        error ("no match: chapter url path")
    end

    return video
end


