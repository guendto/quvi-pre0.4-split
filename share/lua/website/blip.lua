
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
function ident(self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "blip.tv"
    --
    -- Typically available formats: source ('Source'), sd ("Blip SD"),
    -- hd ("Blip HD"). There are others but these three seem to be the
    -- most widely available ones.
    --
    -- 'best' is determined by parsing the available formats, picking
    -- the one with the highest resolution. 'default' to whatever the
    -- website defines as such.
    --
    r.formats    = "default|best|sd|hd"
    r.categories = C.proto_http
    local U      = require 'quvi/util'
    r.handles    = U.handles(self.page_url, {r.domain},
                    -- paths: Room for improvement.
                    {"/file/%d+",
                     "/play/%w+",
                     "/flash/%d+",
                     "/scripts/flash/",
                     "%-%d+"})
    return r
end

-- Parse video URL.
function parse(self)
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
        _,_,id = page:find('data%-posts%-id="(.-)"')
    end
    self.id = id or error('no match: video id')

    local config_url = 'http://blip.tv/rss/flash/' .. self.id
    local config = quvi.fetch(config_url, {fetch_type = 'config'})

    local _,_,s = config:find('media:title>(.-)</')
    self.title = s or error('no match: title')

    local _,_,s   = config:find('<blip:smallThumbnail>(.-)<')
    self.thumbnail_url = s or ''
 
    -- Choose format.
    local found = iter_media(config)
    local r_fmt = self.requested_format

    local url = (r_fmt == 'default' or not found[r_fmt])
                    and choose_default(found)
                    or  found[r_fmt].url

    url = (r_fmt == 'best')
            and choose_best(found).url
            or  url

    self.url = {url or error('no match: media url')}

    return self
end
 
function iter_media(config)
    local p = '<media:content url="(.-)"'
           .. '.-blip:role="(.-)"'
           .. '.-height="(.-)"'
           .. '.-isDefault="(.-)"'
           .. '.-width="(.-)"'
    local t = {}
    for u,r,h,d,w in config:gfind(p) do
        r = string.lower(r):gsub('blip%s+','')
        r = r:gsub('%s+%d+','')
        r = r:gsub(' ','_')
--        print(string.format('(%s)',r))
        w = (w ~= nil) and tonumber(w) or 0
        h = (h ~= nil) and tonumber(h) or 0
--        print('iter_media:', u, r, w, h, d)
        t[r] = {width=w, height=h, url=u, default=d}
    end
    return t
end

function choose_best(t)
    local best = {role=nil, width=0, height=0, url=nil}
    for k,v in pairs(t) do
        if v.height > best.height then
            if v.width >  best.width then
                best.width  = v.width
                best.height = v.height
                best.url    = v.url
                best.role   = k
            end
        end
    end
--    print('choose_best:', best.url, best.role, best.width, best.height)
    return best
end
 
function choose_default(t)
    local url
    for k,v in pairs(t) do
        if v.default == 'true' then
            url = v.url
            break
        end
    end
--    print('choose_default:', url)
    return url
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
