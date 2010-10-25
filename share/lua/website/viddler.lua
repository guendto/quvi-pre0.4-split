-- Copyright (C) 2010 Toni Gundogdu.
--
-- This file is part of quvi <http://quvi.googlecode.com/>.
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

-- Identify the script.
function ident (page_url)
    local t   = {}
    t.domain  = "viddler.com"
    t.formats = "default|original|iphone|best"
    t.handles = (page_url ~= nil and page_url:find(t.domain) ~= nil)
    return t
end

-- Parse video URL.
function parse (video)
    video.host_id = "viddler"
    local page    = quvi.fetch(video.page_url)

    local _,_,s = page:find('<meta name="description" content="(.-)"')
    video.title = s or error ("no match: video title")

    local _,_,s   = page:find('title=".+" href="(.+)">Flash')
    local default = s or error ("no match: flash")

    local rfmt = video.requested_format
    local path = default

    if (rfmt == "best" or rfmt == "original") then -- Assumes original is the best.
        _,_,s = page:find('title=".+" href="(.+)">Original')
        path  = s or error ("no match: original")
    else
        if (rfmt == "iphone") then
            _,_,s = page:find('title=".+" href="(.+)">iPhone')
            path  = s or error ("no match: iphone")
        end
    end

    video.url   = { quvi.unescape("http://viddler.com" .. path) }

    local _,_,s = path:find("/videos/(%d+)")
    video.id    = s or error ("no match: video id")

    return video
end


