
-- quvi
-- Copyright (C) 2010,2011  Toni Gundogdu <legatvs@gmail.com>
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

-- Formats.
local formats = {
    'default',
    'best',
    'vp6_315p',
    'vp6_544p'
}

local format_lookup = {
    default  = "VP6_388", -- vp6_135p
    vp6_315p = "VP6_576",
    vp6_544p = "VP6_928",
    best     = "H264_1400" -- h264_544p
}

-- Identify the script.
function ident (self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "spiegel.de"
    r.formats    = table.concat(formats, "|")
    r.categories = C.proto_http
    local U      = require 'quvi/util'
    r.handles    = U.handles(self.page_url, {r.domain}, {"/video/"})
    return r
end

-- Parse video URL.
function parse (self)
    self.host_id = "spiegel"

    local _,_,s = self.page_url:find("/video/video%-(.-)%.")
    self.id     = s or error ("no match: video id")

    local playlist_url = string.format(
        "http://www1.spiegel.de/active/playlist/fcgi/playlist.fcgi/"
        .. "asset=flashvideo/mode=id/id=%s", self.id)

    local playlist = quvi.fetch (playlist_url, {fetch_type = 'playlist'})

    local _,_,s = playlist:find("<headline>(.-)</")
    self.title  = s or error ("no match: video title")

    local config_url = string.format(
        "http://video.spiegel.de/flash/%s.xml", self.id)

    local config = quvi.fetch (config_url, {fetch_type = 'config'})

    local r_fmt  = self.requested_format
    r_fmt = (not format_lookup[r_fmt]) and 'default' or r_fmt

    local format = format_lookup[r_fmt]

    -- Match the format string to a link in config.
    local pattern = "<filename>(%d+)_(%d+)x(%d+)_(%w+)_(%d+).(%w+)"

    for id,w,h,c,b,s in config:gfind(pattern) do
        fname = string.format("%s_%sx%s_%s_%s.%s",id,w,h,c,b,s)
        if (format == string.format("%s_%s",c,b)) then
            self.url = {"http://video.spiegel.de/flash/"..fname}
            break
        end
    end

    return self
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
