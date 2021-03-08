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

#include "xml/lite/NamespaceStack.h"

void xml::lite::NamespaceStack::push()
{
    mRefStack.push(0);
}

void xml::lite::NamespaceStack::pop()
{
    int numToRemove = mRefStack.top();

    for (int i = 0; i < numToRemove; i++)
    {
        // Remove this namespace mapping
        mMappingStack.pop_back();
    }

    mRefStack.pop();
}


void xml::lite::NamespaceStack::newMapping(const std::string& prefix,
        const std::string& uri)
{
    ++mRefStack.top();
    mMappingStack.push_back(xml::lite::NamespaceEntity(prefix, uri));
}

std::string
xml::lite::NamespaceStack::getMapping(const std::string& prefix) const
{
    for (int i = (int)mMappingStack.size() - 1; i >= 0; --i)
    {
        if (mMappingStack[i].first == prefix)
        {
            return mMappingStack[i].second;
        }
    }
    return std::string("");
}

void xml::lite::NamespaceStack::
getAllPrefixes(std::vector<std::string>& allPrefixes) const
{
    for (unsigned int i = 0; i < mMappingStack.size(); i++)
    {
        allPrefixes.push_back(mMappingStack[i].first);
    }
}
