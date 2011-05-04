
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

-- <http://en.wikipedia.org/wiki/YouTube#Quality_and_codecs>
-- $container_$maxwidth = '$fmt_id'

local formats = {
    -- flv
    'default',  -- flv_240p
    'flv_360p',
    'flv_480p',
    -- mp4
    'mp4_360p',
    'mp4_720p',
    'mp4_1080p',
    'mp4_3072p'
    -- 'best' is determined by an algorithm.
}

local format_lookup = {
    -- flv
    default =  '5', -- flv_240p
    flv_360p = '34',
    flv_480p = '35',
    -- mp4
     mp4_360p = '18',
     mp4_720p = '22',
    mp4_1080p = '37',
    mp4_3072p = '38'
}

-- Identify the script.
function ident(self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "youtube.com"
    r.formats    = table.concat(formats, "|") .. "|best"
    r.categories  = C.proto_http
    self.page_url = youtubify(self.page_url)
    local U       = require 'quvi/util'
    r.handles     = U.handles(self.page_url,
                        {r.domain}, {"/watch"}, {"v=[%w-_]+"})
    return r
end

function parse(self)
    self.host_id  = "youtube"
    local page_url = youtubify(self.page_url)

    local _,_,s = page_url:find("v=([%w-_]+)")
    self.id    = s or error("no match: video id")

    local _,_,s = page_url:find('#a?t=(.+)')
    self.start_time = s or ''

    return get_video_info(self)
end

-- Youtubify the URL.
function youtubify(url)
    if not url then return url end
    url = url:gsub("-nocookie", "")        -- youtube-nocookie.com
    url = url:gsub("/v/", "/watch?v=")     -- embedded
    url = url:gsub("/embed/", "/watch?v=") -- embedded
    url = url:gsub("/e/", "/watch?v=") -- embedded
    url = url:gsub("youtu%.be/","youtube.com/watch?v=") -- youtu.be
    return url
end

function get_video_info(self, result)

    local _,_,s  = self.page_url:find('^(%w+)://')
    local scheme = s or error("no match: scheme")

    local config_url = string.format(
        "%s://www.youtube.com/get_video_info?&video_id=%s"
        .. "&el=detailpage&ps=default&eurl=&gl=US&hl=en",
            scheme, self.id)

    local opts   = { fetch_type = 'config' }
    local U      = require 'quvi/util'
    local config = U.decode(quvi.fetch(config_url, opts))

    if config['reason'] then
        local reason = U.unescape(config['reason'])
        local code   = config['errorcode']
        error(reason..' (code='..code..')')
    end

    self.title = config['title'] or error('no match: video title')
    self.title = U.unescape(self.title)

    local fmt_url_map =
        config['fmt_url_map'] or error("no match: fmt_url_map")

    fmt_url_map = U.unescape(fmt_url_map) .. ','

    -- Assume first found URL to be the 'best'.
    local best
    local t = {}

    for fmt,url in fmt_url_map:gfind('(%d+)%|(.-),') do
        if not best then best = url end
        t[fmt] = url
    end

    -- Choose URL.
    local url

    if self.requested_format == 'best' then
        url = best
    else
        local r_fmt = self.requested_format
        r_fmt = (not format_lookup[r_fmt]) and 'default' or r_fmt
        url   = t[format_lookup[r_fmt]]
    end

    if url and #self.start_time > 0 then
        local msec
        local min, sec = self.start_time:match("^(%d+)m(%d+)s$")
        min = tonumber(min) or 0
        sec = tonumber(sec) or 0
        msec = (min * 60000) + (sec * 1000)
        if msec > 0 then
            url = url .. "&begin=" .. msec
        end
    end

    self.url = {url or error("no match: video url")}

    return self
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
