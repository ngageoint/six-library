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

#ifndef __NET_URL_H__
#define __NET_URL_H__

#include <string>
#include <map>
#include <list>
#include "net/NetExceptions.h"
#include "net/NetUtils.h"

/*! \file URL.h
 *  \brief Class for abstract locations
 *
 *  Allows several ways of specifying location.
 *
 */

namespace net
{


class URLParams
{
public:
    typedef std::list<std::string> ParamValues;
    typedef std::map<std::string, ParamValues> Params;

    URLParams(const std::string paramString = "");

    bool contains(std::string key) const;
    ParamValues& get(std::string key);
    const ParamValues& get(std::string key) const;
    Params& get() { return mParams; }
    const Params& get() const { return mParams; }
    std::string getFirst(std::string key) const;
    void add(std::string key, std::string value = "");
    void remove(std::string key);

    std::string toString() const;

protected:
    Params mParams;
};


class URL
{
public:

    URL(const std::string url = "");

    /*!
     *  Copy constructor.
     *  \param url A right-hand-side URL
     */
    URL(const URL& url);

    virtual ~URL()
    {
    }

    void set(std::string url);

    std::string getProtocol() const;
    std::string getHost() const;
    int getPort() const;
    std::string getPath() const;
    std::string getFragment() const;
    std::string getQuery() const;
    std::string getServer() const;
    std::string getDocument() const;
    URLParams& getParams() { return mParams; }
    const URLParams& getParams() const { return mParams; }
    std::string toString() const;
    void setPort(int port);
    bool hasStandardPort() const;

    /*!
     * Are these URLs equal
     * \param url A URL to compare
     */
    bool operator==(const URL& url) const;

protected:
    friend class URLBuilder;
    std::string mProtocol;
    std::string mHost;
    int mPort;
    std::string mPath;
    URLParams mParams;
    std::string mFragment;

};
}
#endif

