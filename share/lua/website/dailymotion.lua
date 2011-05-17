
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

-- Identify the script.
function ident(self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "dailymotion."
    --
    -- 'best' is determined by parsing the available formats, picking
    -- the one with the highest height quality. 'default' to the lowest
    -- quality.
    --
    r.formats    = "default|best|h264_480p"
    r.categories = C.proto_http
    local U      = require 'quvi/util'
-- "http://dai.ly/cityofscars",
-- "http://www.dailymotion.com/video/xdpig1_city-of-scars_shortfilms",
    r.handles    =
        U.handles(self.page_url, {r.domain, "dai.ly"}, {"/video/","/%w+$"})
    return r
end

-- Parse video URL.
function parse(self)
    self.host_id  = "dailymotion"
    self.page_url = normalize(self.page_url)

    local U = require 'quvi/util'

    local _,_,s = self.page_url:find('/family_filter%?urlback=(.+)')
    if s then -- Set family_filter arbitrary cookie below.
        self.page_url = 'http://dailymotion.com' .. U.unescape(s)
    end

    local opts = {arbitrary_cookie = 'family_filter=off'}
    local page = quvi.fetch(self.page_url, opts)

    local _,_,s = page:find('title="(.-)"')
    self.title  = s or error("no match: video title")

    local _,_,s = page:find("video/(.-)_")
    self.id     = s or error("no match: video id")

    local _,_,s = page:find('"og:image" content="(.-)"')
    self.thumbnail_url = s or ''

    local found = iter_media(page, U)
    local r_fmt = self.requested_format

    local url = (r_fmt == 'default' or not found[r_fmt])
                    and choose_default(found).url
                    or  found[r_fmt].url

    url = (r_fmt == 'best')
            and choose_best(found).url
            or  url

    self.url = {url or error('no match: media url')}

    return self
end

function normalize(page_url) -- embedded URL to "regular".
    if page_url:find("/swf/") then
        page_url = page_url:gsub("/video/", "/")
        page_url = page_url:gsub("/swf/", "/video/")
    end
    return page_url
end

function iter_media(page, U)
    local _,_,seq = page:find('"sequence",%s+"(.-)"')
    if not seq then
        local e = "no match: sequence"
        if page:find("_partnerplayer") then
            e = e .. ": looks like a partner video which we do not support"
        end
        error(e)
    end
    seq = U.unescape(seq)

    local _,_,msg = seq:find('"message":"(.-)"')
    if msg then
        msg = msg:gsub('+',' ')
        error(msg)
    end

    local _,_,vpp = seq:find('"videoPluginParameters":{(.-)}')
    if not vpp then
        local _,_,s = page:find('"video", "(.-)"')
        if not s then
            error("no match: video plugin params")
        else
            -- some videos (that require setting family_filter cookie)
            -- may list only one link which is not found under
            -- "videoPluginParameters". See also:
            -- http://sourceforge.net/apps/trac/clive/ticket/4
            return {s}
        end
    end

    local t = {}
    for id,url in vpp:gfind('(%w+)URL":"(.-)"') do
        url = url:gsub("\\/", "/")
        local _,_,c,w,h = url:find('(%w+)%-(%d+)x(%d+)')
        if not c or not w or not h then
            error('no match: container, width, height')
        end
        c = string.lower(c)
--        print(c,w,h)
        local id = c .. '_' .. h .. 'p'
        t[id] = {width=tonumber(w), height=tonumber(h), url=url}
    end
    return t
end

function choose_default(t)
    -- Go with the lowest resolution.
    local r = {width=0xffff, height=0xffff, url=nil}
    for k,v in pairs(t) do
        if v.height < r.height then
            r.width  = v.width
            r.height = v.height
            r.url    = v.url
        end
    end
    return r
end

function choose_best(t)
    local r = {width=0, height=0, url=nil}
    for k,v in pairs(t) do
        if v.height > r.height then
            r.width  = v.width
            r.height = v.height
            r.url    = v.url
        end
    end
    return r
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
