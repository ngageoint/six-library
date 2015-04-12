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

#include "xml/lite/QName.h"



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

void xml::lite::QName::setAssociatedUri( const std::string& str )
{
    mAssocUri = str;
}

std::string xml::lite::QName::getAssociatedUri() const
{
    return mAssocUri;
}
