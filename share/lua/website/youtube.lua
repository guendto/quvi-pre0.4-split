--[[
/* 
* Copyright (C) 2010 Toni Gundogdu.
*
* This file is part of quvi, see http://quvi.googlecode.com/
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

]]--

-- If you make improvements to this script, drop a line. Thanks.
--   <http://quvi.googlecode.com/>

-- These are my formats.
local lookup = {
    mobile_3gp = "17",
    hq_480p    = "18",
    hd_720p    = "22",
    hd_1080p   = "37"
}

-- Returns script details.
function ident (page_url)
    
    -- This is what I return.
    local t = {}

    -- This is my domain.
    t.domain = "youtube.com"

    -- This is my formats-string.
    t.formats = ""
    for k,_ in pairs (lookup) do
        t.formats = t.formats .."|".. k
    end
    t.formats = "default|best" .. t.formats

    -- This is my response to "Will you handle this URL?"
    -- Note that page_url may be nil.
    if (page_url ~= nil) then
        page_url = youtubify(page_url)
    end
    t.will_handle = (page_url ~= nil and page_url:find(t.domain) ~= nil)

    -- Here are my details.
    return t

end

-- Fetches video page and parses the video URL.
function parse (video)

    -- This is my "host ID".
    video.host_id = "youtube"

    -- Fetch video page.
    local page = quvi.fetch( youtubify(video.page_url) )

    -- This is my video title.
    local _,_,s = page:find('<meta name="title" content="(.-)"')
    video.title = s or error ("no match: video title")

    -- This is my video ID.
    local _,_,s = page:find('&video_id=(.-)&')
    video.id    = s or error ("no match: video id")

    -- This is my t param used to construct the video URL.
    local _,_,s = page:find('&t=(.-)&')
    local t     = s or error ("no match: t param")
    t = quvi.unescape(t)

    -- Construct the video URL.
    local video_url = 
        string.format(
            "http://youtube.com/get_video?video_id=%s&t=%s",
                video.id, t)

    -- Grab "best" available video format id.
    -- Note that nil should not result in error.
    local _,_,best_format = page:find('&fmt_map=(%d+)')

    if (best_format == nil) then
        print ("quvi: warning: no match: best format, using default")
    end

    -- Choose correct fmt ID. Default to nil.
    local fmt_id = nil

    if (video.requested_format == "best") then
        fmt_id = best_format or fmt_id
    else
        for k,v in pairs (lookup) do
            if (k == video.requested_format) then
                fmt_id = v
                break
            end
        end
    end

    -- And append it.
    if (fmt_id ~= nil) then
        video_url = video_url .. "&fmt=" .. fmt_id
    end

    -- Set my video URL.
    video.url = {video_url}

    -- Return the updated video properties.
    return video

end

-- Converts various youtube-like links to "regular" video page links.
function youtubify (url)
    url = url:gsub("-nocookie", "")    -- youtube-nocookie.com
    url = url:gsub("/v/", "/watch?v=") -- embedded
    return url
end


