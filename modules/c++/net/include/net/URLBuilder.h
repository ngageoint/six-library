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

#ifndef __URL_URL_BUILDER_H__
#define __URL_URL_BUILDER_H__

#include "net/NetUtils.h"
#include "net/URL.h"

namespace net
{

class URLBuilder
{
public:

    URLBuilder(const std::string url = "") : mURL(url)
    {
    }

    virtual ~URLBuilder()
    {
    }

    URLBuilder& setProtocol(const std::string protocol)
    {
        mURL.mProtocol = protocol;
        return *this;
    }

    URLBuilder& addParam(const std::string key, const std::string value)
    {
        mURL.getParams().add(key, value);
        return *this;
    }

    URLBuilder& setHost(const std::string host)
    {
        mURL.mHost = host;
        return *this;
    }

    URLBuilder& setPort(int port)
    {
        mURL.mPort = port;
        return *this;
    }

    URLBuilder& setFragment(const std::string fragment)
    {
        mURL.mFragment = fragment;
        return *this;
    }

    URLBuilder& setPath(const std::string path)
    {
        mURL.mPath = path;
        return *this;
    }

    net::URL getURL()
    {
        return mURL;
    }

    std::string toString()
    {
        return mURL.toString();
    }

protected:
    net::URL mURL;
};

}
#endif
