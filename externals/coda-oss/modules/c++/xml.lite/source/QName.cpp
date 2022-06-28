/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#include <stdexcept>

#include "xml/lite/QName.h"
#include "str/Manip.h"

std::string xml::lite::QName::getName() const
{
    return mLocalName;
}

//! \return The fully qualifed qname (e.g., soap-env:SOAP-BODY)
std::string xml::lite::QName::toString() const
{
    std::string fullName;
    if (mPrefix.length()) fullName = mPrefix + std::string(":");
    fullName += mLocalName;
    return fullName;
}

void xml::lite::QName::setName( const std::string& str)
{
    mLocalName = str;
}

void xml::lite::QName::setPrefix( const std::string& prefix )
{
    mPrefix = prefix;
}

std::string xml::lite::QName::getPrefix() const
{
    return mPrefix;
}


void xml::lite::QName::setQName(const std::string& str)
{

    size_t x = str.find_first_of(':');
    // Either we have a namespace prefix
    if (x != std::string::npos)
    {
        setPrefix(str.substr(0, x));
        setName(str.substr(x + 1));
    }
    else
    {
        setName(str);
    }
}

void xml::lite::QName::setAssociatedUri(const Uri& v)
{
    mAssocUri = v;
}

void xml::lite::QName::getAssociatedUri(Uri& v) const
{
    v = getUri();
}
std::string xml::lite::QName::getAssociatedUri() const
{
    return getUri().value;
}
const xml::lite::Uri& xml::lite::QName::getUri() const
{
    return mAssocUri;
}

static std::string flatten(const std::vector<std::string>& strs, size_t start = 0)
{
    std::string retval;
    for (size_t i = start; i < strs.size(); i++)
    {
        retval += strs[i];
    }
    return retval;
}

xml::lite::Uri::Uri(const std::string& uri)
{
    // Do some very simple sanity-checking on a URI; this could be (much?) more sophisticated.
    if (!uri.empty())
    {
        // https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
        if (uri.length() <= 6) // "ab:CDEF"
        {
            // There's nothing that says we can't have short URIs, but does it
            // make sense in actual use cases?
            throw std::invalid_argument("string value '" + uri + "' is (too?) short.");
        }

        const auto r = str::split(uri, ":");
        if (r.size() < 2)
        {
            throw std::invalid_argument("string value '" + uri + "' is not a URI.");
        }

        if (r[0].length() <= 1)
        {
            // Is "a:" a real-world scheme?
            throw std::invalid_argument("string value '" + r[0] + "' is not a URI scheme.");
        }

        const auto path = flatten(r, 1); // don't care about other ':'s
        if (path.length() <= 6)
        {
            // does it make sense to have a really short path?
            // in SIX we have "urn:us:gov"
            throw std::invalid_argument("string value '" +  path + "' is (too?) short for a URI path.");        
        }
    }
    value = uri;
}
