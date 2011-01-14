
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
function ident (self)
    package.path = self.script_dir .. '/?.lua'
    local C      = require 'quvi/const'
    local r      = {}
    r.domain     = "funnyhub.com"
    r.formats    = "default"
    r.categories = C.proto_http
    r.handles    =
        (self.page_url ~= nil and self.page_url:find(r.domain) ~= nil)
    return r
end

-- Parse video URL.
function parse (self)
    self.host_id = "funnyhub"
    local page   = quvi.fetch(self.page_url)

    local _,_,s = page:find("<title>(.-)%s+-")
    self.title  = s or error ("no match: video title")

    local _,_,s = page:find("/videofiles/(.-)_")
    self.id     = s or error ("no match: video id")

    local _,_,s = page:find('"flv", "(.-)"')
    self.url    = {s or error ("no match: flv")}

    return self
end

-- vim: set ts=4 sw=4 tw=72 expandtab:
