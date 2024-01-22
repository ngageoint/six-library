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

#include "net/URL.h"
#include <import/str.h>
#include <import/sys.h>

net::URL::URL(std::string url)
{
    if (!url.empty())
        set(url);
}

net::URL& net::URL::operator=(const URL& url)
{
  if (this != &url)
  {
    mProtocol = url.getProtocol();
    mHost = url.getHost();
    setPort(url.getPort());
    mPath = url.getPath();
    mFragment = url.getFragment();
    mParams = net::URLParams(url.getParams().toString());
  }
  return *this;
}
net::URL::URL(const URL& url)
{
    *this = url;
}

void net::URL::set(std::string url)
{
    std::vector<std::string> parts = net::urlSplit(url);
    mProtocol = parts[0];
    mHost = parts[1];
    setPort(parts[2].empty() ? -1 : str::toType<int>(parts[2]));
    mPath = parts[3];
    std::string params = parts[4];
    mFragment = parts[5];
    mParams = net::URLParams(params);
}

void net::URL::setPort(int port)
{
    mPort = port;
    if (mPort < 0)
        mPort = net::getStandardPort(getProtocol());
}

std::string net::URL::getProtocol() const
{
    return mProtocol;
}
std::string net::URL::getHost() const
{
    return mHost;
}
int net::URL::getPort() const
{
    return mPort;
}
std::string net::URL::getPath() const
{
    return mPath;
}
std::string net::URL::getFragment() const
{
    return mFragment;
}
std::string net::URL::getQuery() const
{
    return mParams.toString();
}

std::string net::URL::getDocument() const
{
    std::ostringstream doc;
    std::string path = getPath();
    if (!str::startsWith(path, "/"))
        doc << "/";
    doc << path;
    std::string query = getQuery();
    if (!query.empty())
        doc << "?" << query;
    std::string fragment = getFragment();
    if (!fragment.empty())
        doc << "#" << fragment;
    return doc.str();
}

std::string net::URL::getServer() const
{
    std::ostringstream server;
    server << getHost();
    int port = getPort();
    if (port >= 0)
        server << ":" << port;
    return server.str();
}

std::string net::URL::toString() const
{
    return net::urlJoin(getProtocol(), getHost(), getPort(), getPath(),
                        getQuery(), getFragment());
}

bool net::URL::operator==(const net::URL& url) const
{
    return toString() == url.toString();
}

net::URLParams::URLParams(std::string paramString)
{
    if (!paramString.empty())
    {
        str::Tokenizer tokenizer(paramString, "&;"); //can be & or ;
        str::Tokenizer::Tokens& paramParts =
                (str::Tokenizer::Tokens&) tokenizer;
        for (size_t i = 0, size = paramParts.size(); i < size; ++i)
        {
            std::string param = paramParts[i];
            std::string val = "";
            size_t pos = param.find("=");
            if (pos > 0)
            {
                std::string key = param.substr(0, pos);
                val = (pos < (param.length() - 1)) ? param.substr(pos + 1) : "";
                param = key;
            }
            add(param, val);
        }
    }
}

bool net::URLParams::contains(std::string key) const
{
    net::URLParams::Params::const_iterator it = mParams.find(key);
    return it != mParams.end() && it->second.size() > 0;
}

net::URLParams::ParamValues& net::URLParams::get(std::string key)
{
    net::URLParams::Params::iterator it = mParams.find(key);
    if (it == mParams.end() || it->second.empty())
        throw except::NoSuchKeyException(Ctxt(key));
    return it->second;
}

const net::URLParams::ParamValues& net::URLParams::get(std::string key) const
{
    net::URLParams::Params::const_iterator it = mParams.find(key);
    if (it == mParams.end() || it->second.empty())
        throw except::NoSuchKeyException(Ctxt(key));
    return it->second;
}
std::string net::URLParams::getFirst(std::string key) const
{
    net::URLParams::Params::const_iterator it = mParams.find(key);
    if (it == mParams.end() || it->second.empty())
        throw except::NoSuchKeyException(Ctxt(key));
    return it->second.front();
}
void net::URLParams::add(std::string key, std::string value)
{
    if (mParams.find(key) == mParams.end())
        mParams[key] = net::URLParams::ParamValues();
    mParams[key].push_back(value);
}
void net::URLParams::remove(std::string key)
{
    if (mParams.find(key) != mParams.end())
        mParams.erase(key);
}

std::string net::URLParams::toString() const
{
    std::ostringstream s;
    bool firstParam = true;
    for (net::URLParams::Params::const_iterator it = mParams.begin(); it
            != mParams.end(); ++it)
    {
        std::string key = it->first;
        const net::URLParams::ParamValues& vals = it->second;
        for (net::URLParams::ParamValues::const_iterator it2 = vals.begin(); it2
                != vals.end(); ++it2)
        {
            if (!firstParam)
                s << "&";
            auto quotedKey = net::quote(key);
            s << quotedKey;
            s << "=";
            quotedKey = net::quote(*it2);
            s << quotedKey;
            firstParam = false;
        }
    }
    return s.str();
}
