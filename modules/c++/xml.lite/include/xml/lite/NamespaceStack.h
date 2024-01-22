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

#ifndef CODA_OSS_xml_lite_NamespaceStack_h_INCLUDED_
#define CODA_OSS_xml_lite_NamespaceStack_h_INCLUDED_

#pragma once

/*!
 *  \file NamespaceStack.h
 *  \brief An implementation of a scope-based namespace mapper
 *
 *  Namespaces arent very useful if there is no way to resolve them
 *  using their respective components.  Unfortunately, the solution
 *  is more complex than would be expected.  Here are the rules:
 *  1)  We are out of scope if the xmlns mapping takes place inside a
 *  sub-element (it must be declared in this element or a parent.
 *
 *  2)  Prefixes MUST be unique at least in scope.
 *  For example, xmlns:x = "blah" and then
 *  another mapping over it xmlns:x="blah2" is ok.  blah2 is in local scope,
 *  and blah is in global scope, which means, when we pop x off the first
 *  time, the initial x remains.
 *
 *  3)  URI's MAY be duplicate.  For example:
 *  xmlns:x="a-uri" and xmlns:y="a-uri" are both valid at the same time
 *
 *  We guarantee this with a namespace stack.
 *
 */

#include <stack>
#include <map>
#include <vector>
#include <string>
#include <utility>

#include "except/Exception.h"
#include "xml/lite/QName.h"

namespace xml
{
namespace lite
{
typedef std::stack<int> NamespaceRefStack;
typedef std::pair<std::string, std::string> NamespaceEntity;
typedef std::vector<NamespaceEntity> NamespaceEntityMap;

/*!
 * \class NamespaceStack
 * \brief A namespace mapping object
 * This object uses scope declarator functions push and pop to
 * determine when to add and throw away entities.  Internally,
 * it uses a stack of range-marker integers, which represent
 * how many elements where added for each new scope.
 *
 *
 */
struct NamespaceStack final
{
    NamespaceStack() = default;
    ~NamespaceStack() = default;

    /*!
     *  Push a new scope on the stack.  This will add a zero-
     *  initialized integer to the reference stack.
     *  The XMLReader caller will use at the beginning of the
     *  startElement() method.
     */
    void push();

    /*!
     *  Pop the scope off the stack.  This will pop the integer
     *  off the reference stack and delete the value number of
     *  namespaces off of the namespace mapping stack.
     *  It should be called in the XMLReader in the endElement()
     *  method.
     */
    void pop();

    /*!
     *  Create a scope-based association between the prefix
     *  and the uri in question.  The top of the reference stack
     *  will be incremented, and the mapping will be established
     *  in the namespace mapping stack
     *  \param prefix  The unique prefix to associate
     *  \param uri     The uri to associate
     */
    void newMapping(const std::string & prefix, const Uri&);
    void newMapping(const std::string& prefix, const std::string& uri)
    {
        newMapping(prefix, Uri(uri));
    }

    /*!
     *  Of course, we also wish to retrieve the
     *  mapping from the namespace stack.
     *  \param  prefix The unique prefix to retrieve our uri for
     *  \return The associated uri or "" if none is available
     */
    void getMapping(const std::string& prefix, Uri&) const;
    std::string getMapping(const std::string& prefix) const
    {
        Uri result;
        getMapping(prefix, result);
        return result.value;
    }

    /*!
     *  Get back an array of all of the prefixes in our namespace
     *  stack
     *  \param allPrefixes All of the prefixes
     *
     */
    void getAllPrefixes(std::vector<std::string> &allPrefixes) const;

private:
    NamespaceEntityMap mMappingStack;
    NamespaceRefStack mRefStack;
};
}
}

#endif  // CODA_OSS_xml_lite_NamespaceStack_h_INCLUDED_
