
-- quvi
-- Copyright (C) 2010-2011  Lionel Elie Mamane <lionel@mamane.lu>
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
function ident (self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "collegehumor.com"
    r.formats    = "default|best"
    r.categories = C.proto_http
    local U      = require 'quvi/util'
    r.handles    = U.handles(self.page_url,
                    {r.domain}, {"/video[:/]%d+/?"})
    return r
end

-- Parse video URL.
function parse (self)
    self.host_id = "collegehumor"

    self.id = get_media_id(self.page_url)
    self.id = self.id or error("no match: video id")

    -- quvi normally checks the page URL for a redirection to another
    -- URL. Disabling this check (QUVIOPT_NORESOLVE) breaks the support
    -- which is why we do this manually here.
    local r = quvi.resolve(self.page_url)

    -- Make a note of the use of the quvi.resolve returned string.
    local config_url =
        string.format("http://www.collegehumor.com/moogaloop/video%s%s",
            (#r > 0) and ':' or '/', self.id)

    local config     = quvi.fetch(config_url, {fetch_type='config'})
    local _,_,sd_url = config:find('<file><!%[%w+%[(.-)%]')
    local _,_,hq_url = config:find('<hq><!%[%w+%[(.-)%]')
    local hq_avail   = (hq_url and #hq_url > 0) and 1 or 0

    -- default=sd, best=hq
    local s = (self.requested_format == 'best' and hq_avail == 1)
                and hq_url or sd_url

    self.url = {s or error('no match: media url')}

    local _,_,s = config:find('<caption>(.-)<')
    self.title  = s or error("no match: video title")

    local _,_,s = config:find('<thumbnail><!%[%w+%[(.-)%]')
    self.thumbnail_url = s or ""

    return self
end

function get_media_id(url)
    if not url then return url end
    local _,_,s = url:find('collegehumor%.com/video[/:](%d+)')
    return s
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
