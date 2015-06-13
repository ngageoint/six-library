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

#ifndef __NET_NET_UTILS_H__
#define __NET_NET_UTILS_H__

#include <string>
#include <vector>

namespace net
{

enum
{
    DEFAULT_PORT_HTTP = 80, DEFAULT_PORT_HTTPS = 443
};

/**
 * Splits a URL string into 5 parts:
 * - scheme/protocol
 * - host/location
 * - path
 * - query
 * - fragment
 * \param url the string to parse
 */
std::vector<std::string> urlSplit(std::string url);

/**
 * Joins the given url components into a url string
 */
std::string urlJoin(std::string scheme, std::string location, int port = -1,
        std::string path = "", std::string query = "", std::string fragment =
                "");

/**
 * Joins the url components given in the vector into a url string
 */
std::string urlJoin(const std::vector<std::string>& parts);

/**
 * Quotes the given string - mainly for use in query strings
 */
std::string quote(std::string s);

/**
 * Unquotes the given string - inverse of quote()
 */
std::string unquote(std::string s);

/**
 * Returns the standard port for the given protocol (or url)
 */
int getStandardPort(std::string protocol);

}
#endif

