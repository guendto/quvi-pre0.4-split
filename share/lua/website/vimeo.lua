
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

-- w/ HD: <http://vimeo.com/1485507>
-- no HD: <http://vimeo.com/10772672>

-- Identify the script.
function ident(self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "vimeo.com"
    r.formats    = "default|best" -- default=sd, best=hd
    r.categories = C.proto_http
    local U      = require 'quvi/util'
    r.handles    = U.handles(self.page_url, {r.domain}, {"/%d+$"})
    return r
end

-- Parse video URL.
function parse(self)
    self.host_id  = "vimeo"
    self.page_url = vimeofy(self.page_url)

    local _,_,s = self.page_url:find('vimeo.com/(%d+)')
    self.id = s or error("no match: video id")

    local config_url = "http://vimeo.com/moogaloop/load/clip:" .. self.id
    local config = quvi.fetch(config_url, {fetch_type = 'config'})

    if config:find('<error>') then
        local _,_,s = config:find('<message>(.-)\n')
        error( (s == nil) and "no match: error message" or s )
    end

    local _,_,s = config:find("<caption>(.-)</")
    self.title  = s or error("no match: video title")

    local _,_,s = config:find("<request_signature>(.-)</")
    local sign  = s or error("no match: request signature")

    local _,_,s = config:find("<request_signature_expires>(.-)</")
    local exp   = s or error("no match: request signature expires")

    local _,_,s    = config:find('<isHD>(%d)')
    local hd_avail = (s and s == '1') and 1 or 0

    -- default=sd, best=hd
    local r_fmt = self.requested_format
    r_fmt = (r_fmt == 'best' and hd_avail == 1) and 'hd' or 'sd'

    self.url = {
        string.format("http://vimeo.com/moogaloop/play/clip:%s/%s/%s/?q=%s",
            self.id, sign, exp, r_fmt)
    }

    return self
end

-- vimeofy the URL.
function vimeofy(url)
    url = url:gsub("player.", "") -- player.vimeo.com
    return url
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
