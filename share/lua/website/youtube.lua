
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

-- Formats.
local lookup = {
    mobile   = "17", --   3gp
    sd_270p  = "18", --   480x270
    sd_360p  = "34", --   640x360
    hq_480p  = "35", --   854x480
    hd_720p  = "22", --  1280x720
    hd_1080p = "37"  -- 1920x1080
}

-- Identify the script.
function ident (page_url)
    local t   = {}
    t.domain  = "youtube.com"
    t.formats = ""
    for k,_ in pairs (lookup) do
        t.formats = t.formats .."|".. k
    end
    t.formats = "default|best" .. t.formats
    if (page_url ~= nil) then
        page_url = youtubify(page_url)
    end
    t.handles = (page_url ~= nil and page_url:find(t.domain) ~= nil)
    return t
end

function parse (video)
    video.host_id  = "youtube"
    local page_url = youtubify(video.page_url)

    local _,_,s = page_url:find("v=([%w-_]+)")
    video.id    = s or error ("no match: video id")

    local t,best = get_video_info (video)
    if (t == nil) then
        t,best = old_faithful (page_url, video)
    end

    local video_url = 
        string.format(
            "http://youtube.com/get_video?video_id=%s&t=%s&asv=2",
                video.id, quvi.unescape(t))

    if (best == nil and video.requested_format == "best") then
        print ("libquvi: Warning: Unable to find `best' format. Use `default'.")
    end

    local fmt_id = nil

    if (video.requested_format == "best") then
        fmt_id = best or fmt_id
    else
        for k,v in pairs (lookup) do
            if (k == video.requested_format) then
                fmt_id = v
                break
            end
        end
    end

    if (fmt_id ~= nil) then
        video_url = video_url .."&fmt=".. fmt_id
    end

    video.url = {video_url}

    return video
end

-- Youtubify the URL.
function youtubify (url)
    url = url:gsub("-nocookie", "")    -- youtube-nocookie.com
    url = url:gsub("/v/", "/watch?v=") -- embedded
    return url
end

-- Should work around at least some of the videos that require
-- signing in first. Requires less bandwidth than the "old faithful".
-- This may, however, fail with some (older?) videos.
function get_video_info (video, result)

    local config_url = string.format(
        "http://www.youtube.com/get_video_info?&video_id=%s"
         .. "&el=detailpage&ps=default&eurl=&gl=US&hl=en", video.id)

    local config = quvi.unescape( quvi.fetch(config_url, "config") )

    local _,_,s = config:find("&reason=(.-)[?:&]?$")
    if (s ~= nil) then
        local reason   = s:gsub("+"," ")
        local _,_,code = config:find("&errorcode=(.-)[?:&?$]")
        if (code == "150") then error (reason) end
        print ("libquvi: Warning: get_video_info: " .. reason)
        print ("libquvi: Warning: Fetch video page instead.")
        return nil -- This one's for the Old Faithful.
    end

    local _,_,s = config:find("&title=(.-)&")
    video.title = s or error ("no match: video title")
    video.title = video.title:gsub("+"," ")

    local _,_,s = config:find("&token=(.-)&")
    local t     = s or error ("no match: token parameter")

    local _,_,best = config:find("&fmt_map=(%d+)")

    return t, best
end

-- As long as video is not otherwise retricted (e.g. age check), this function
-- should work with most videos. Page fetches, however, typically require
-- a lot more bandwidth compared to the config fetch (above).
function old_faithful (page_url, video)
    local page = quvi.fetch(page_url)

    local _,_,s = page:find('<meta name="title" content="(.-)"')
    video.title = s or error ("no match: video title")

    local _,_,s = page:find('&t=(.-)&')
    local t     = s or error ("no match: t param")

    local _,_,best = page:find("&fmt_map=(%d+)")

    return t, best
end


