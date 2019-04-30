/* =========================================================================
 * This file is part of net-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "net/NetUtils.h"
#include "net/NetExceptions.h"
#include <import/str.h>
#include <import/re.h>
#include <iostream>

std::vector<std::string> net::urlSplit(std::string url)
{
    re::Regex regex;
    regex.compile(
                  "([A-Za-z]+)://([^/?#:]+)(?::(\\d+))?(/[^?#:]+)?(?:[?]([^&#/]+(?:[&;][^&;#/]+)*)?)?(?:[#](.*))?");

    re::RegexMatch match;
    if (regex.match(url, match))
    {
        size_t matchLen = match.size();
        std::vector < std::string > parts(6, "");
        for (size_t i = 1; i <= 6; ++i)
        {
            if (i < matchLen)
                parts[i - 1] = match[i];
        }

        //unquote the query string
        parts[4] = net::unquote(parts[4]);

        return parts;
    }
    else
    {
        throw net::MalformedURLException(url.c_str());
    }
}

std::string net::urlJoin(std::string scheme, std::string location, int port,
        std::string path, std::string query, std::string fragment)
{
    std::ostringstream url;
    url << scheme << "://" << location;
    if (port >= 0 && (net::getStandardPort(scheme) != port))
        url << ":" << port;
    if (!path.empty())
    {
        if (!str::startsWith(path, "/"))
            url << "/";
        url << path;
    }
    if (!query.empty())
        url << "?" << query;
    if (!fragment.empty())
        url << "#" << fragment;
    return url.str();
}

std::string net::urlJoin(const std::vector<std::string>& parts)
{
    size_t numParts = parts.size();
    if (numParts < 2)
        throw net::MalformedURLException("No URL provided");
    std::string scheme, location, path, query, fragment;
    int port = -1;
    size_t idx = 0;
    scheme = parts[idx++];
    location = parts[idx++];
    if (numParts > idx)
    {
        re::Regex regex;
        regex.compile("\\d+");
        re::RegexMatch match;
        if (regex.match(parts[idx], match))
            port = str::toType<int>(parts[idx++]);

        if (numParts > idx)
        {
            path = parts[idx++];
            if (numParts > idx)
            {
                query = parts[idx++];
                if (numParts > idx)
                    fragment = parts[idx++];
            }
        }
    }
    return urlJoin(scheme, location, port, path, query, fragment);
}

std::string net::quote(std::string s)
{
    std::ostringstream quoted;
    re::Regex regex;
    regex.compile("[^&\\s]");
    for (size_t i = 0, len = s.length(); i < len; ++i)
    {
        std::string c = s.substr(i, 1);
        if (regex.matches(c))
            quoted << c[0];
        else
            quoted << "%" << std::hex << ((int) c[0]) << std::dec;
    }
    return quoted.str();
}

std::string net::unquote(std::string s)
{
    std::ostringstream unquoted;
    std::vector < std::string > parts = str::split(s, "%");
    size_t numParts = parts.size();
    if (numParts > 0)
        unquoted << parts[0];
    for (size_t i = 1; i < numParts; ++i)
    {
        std::string part = parts[i];
        std::string hexStr = "0x" + part.substr(0, 2);
        long val = strtol(hexStr.c_str(), NULL, 16);
        unquoted << (char) val;
        if (part.length() > 2)
            unquoted << part.substr(2);
    }
    return unquoted.str();
}


int net::getStandardPort(std::string protocol)
{
    str::lower(protocol);
    if (str::startsWith(protocol, "https"))
        return 443;
    else if (str::startsWith(protocol, "http"))
        return 80;
    else if (str::startsWith(protocol, "ssh"))
        return 22;
    else if (str::startsWith(protocol, "sftp"))
        return 115;
    else if (str::startsWith(protocol, "ftp"))
        return 21;
    return -1;
}
