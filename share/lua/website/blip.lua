
-- quvi
-- Copyright (C) 2011  Toni Gundogdu <legatvs@gmail.com>
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

-- Based on get-flash-videos' Blip.pm
-- <https://github.com/monsieurvideo/get-flash-videos/>

-- Identify the script.
function ident (self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "blip.tv"
    -- Usually available: source ('Source'), sd ("Blip SD"), hd ("Blip HD")
    -- There are many other formats but those seem to be available for only
    -- some media. 'default' to 'source'.
    r.formats    = "default|best|sd|hd"
    r.categories = C.proto_http
    local U      = require 'quvi/util'
    r.handles    = U.handles(self.page_url,
                    {r.domain},
                    {"/file/%d+", "/play/%w+", "/flash/%d+",
                     "/scripts/flash/"})
    return r
end

-- Parse video URL.
function parse (self)
    self.host_id  = "blip"

    local U = require 'quvi/util'
    self.page_url = U.unescape(self.page_url)

    local _,_,id = self.page_url:find('/flash/(%d+)')
    if not id then
        local r = quvi.resolve(self.page_url)
        if #r > 0 then
            r = U.unescape(r)
            _,_,id = r:find('/rss/flash/(%d+)')
        end
    end

    if not id then
        local page = quvi.fetch(self.page_url)
        _,_,id = page:find('post_masthed_(%d+)')
    end

    self.id = id or error('no match: video id')

    local config_url = 'http://blip.tv/rss/flash/' .. self.id
    local config = quvi.fetch(config_url, {fetch_type = 'config'})

    local _,_,s = config:find('media:title>(.-)</')
    self.title = s or error('no match: title')

    local p = 'media:content url="(.-)" blip:role="(.-)" .- height="(.-)"'
    local t = {}
    for u,r,h in config:gfind(p) do
        r = string.lower(r):gsub('blip ', '')
        r = r:gsub(' %d+', '')
        h = (h ~= nil) and tonumber(h) or 0
--        print(u,r,h)
        t[r] = {height=h, url=u}
    end

    local r_fmt = self.requested_format

    -- 'source' is our 'default'
    local f = (r_fmt == 'default' or not t[r_fmt]) and 'source' or r_fmt
    f = (r_fmt == 'best') and choose_best(t).role or f
    self.url =  {t[f].url}

    local _,_,s = config:find('media:thumbnail url="(.-)"')
    if s then
        if not s:find('^http://a.images.blip.tv') then
            s = 'http://a.images.blip.tv' .. s
        end
    end
    self.thumbnail_url = s or ''

    return self
end

function choose_best(t)
    local best = {role=nil, height=0, url=nil}
    for k,v in pairs(t) do
        if best.height <= v.height then
            best.height = v.height
            best.url    = v.url
            best.role   = k
        end
    end
--    print(best.role, best.url, best.height)
    return best
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
