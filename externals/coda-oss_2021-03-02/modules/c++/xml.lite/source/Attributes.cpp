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

#include "xml/lite/Attributes.h"
#include "gsl/gsl.h"

xml::lite::AttributeNode::AttributeNode(const xml::lite::AttributeNode& node)
{
    mName = node.mName;
    mValue = node.mValue;
}

xml::lite::AttributeNode&
xml::lite::AttributeNode::operator=(const xml::lite::AttributeNode& node)
{
    if (&node != this)
    {
        mName = node.mName;
        mValue = node.mValue;
    }
    return *this;
}

int xml::lite::Attributes::getIndex(int i) const
{
    const size_t i_ = i;
    if ((i >= 0) && (i_ < mAttributes.size()))
    {
        return i;
    }
    return -1;
}

int xml::lite::Attributes::getIndex(const std::string& qname) const
{
    for (size_t i = 0; i < mAttributes.size(); i++)
    {
        if (qname == mAttributes[i].getQName())
            return gsl::narrow<int>(i);
    }
    return -1;
}

int xml::lite::Attributes::getIndex(const std::string& uri,
                                    const std::string& localName) const
{
    for (size_t i = 0; i < mAttributes.size(); i++)
    {
        if ((uri == mAttributes[i].getUri()) && (localName
                == mAttributes[i].getLocalName()))
            return gsl::narrow<int>(i);
    }
    return -1;
}

std::string xml::lite::Attributes::getValue(int i) const
{
    try
    {
        return mAttributes.at(i).getValue();
    }
    catch (const std::out_of_range& ex)
    {
        throw except::NoSuchKeyException(Ctxt(FmtX("attributes[%d] not found, %s", i, ex.what())));
    }
}
bool xml::lite::Attributes::getValue(int i, std::string& result) const
{
    const size_t i_ = i;
    if ((i >= 0) && (i_ < mAttributes.size()))
    {
        result = mAttributes[i].getValue();
        return true; // index in range
    }
    return false; // index out-of-range
}

std::string xml::lite::Attributes::getUri(int i) const
{
    return mAttributes.at(i).getUri();
}

std::string xml::lite::Attributes::getLocalName(int i) const
{
    return mAttributes.at(i).getLocalName();
}

std::string xml::lite::Attributes::getQName(int i) const
{
    return mAttributes.at(i).getQName();
}

std::string xml::lite::Attributes::getValue(const std::string& qname) const
{
    std::string retval;
    if (!getValue(qname, retval))
    {
        throw except::NoSuchKeyException(Ctxt(FmtX("QName '%s' could not be found",
                                                   qname.c_str())));
    }
    
    return retval;  
}
bool xml::lite::Attributes::getValue(const std::string& qname, std::string& result) const
{
    for (size_t i = 0; i < mAttributes.size(); i++)
    {
        if (qname == mAttributes[i].getQName())
        {
            result = mAttributes[i].getValue();
            return true; // found
        }
    }

    return false; // not found
}

std::string xml::lite::Attributes::getValue(
    const std::string& uri,
    const std::string& localName) const
{
    std::string retval;
    if (!getValue(uri, localName, retval))
    {
        throw except::NoSuchKeyException(Ctxt(FmtX("(uri: %s, localName: %s",
                                                   uri.c_str(), localName.c_str())));
    }
    return retval;
}
bool xml::lite::Attributes::getValue(
    const std::string& uri,
    const std::string& localName,
    std::string& result) const
{
    for (size_t i = 0; i < mAttributes.size(); i++)
    {
        if ((uri == mAttributes[i].getUri()) && (localName
                == mAttributes[i].getLocalName()))
        {
            result = mAttributes[i].getValue();
            return true; // found
        }
    }
    return false; // not found
}

xml::lite::AttributeNode& xml::lite::Attributes::add(const AttributeNode& attribute)
{
    mAttributes.push_back(attribute);
    return mAttributes.back();
}

void xml::lite::AttributeNode::setQName(const std::string& qname)
{
    mName.setQName(qname);
}

void xml::lite::AttributeNode::setLocalName(const std::string& lname)
{
    mName.setName(lname);
}

void xml::lite::AttributeNode::setPrefix(const std::string& prefix)
{
    mName.setPrefix(prefix);
}

void xml::lite::AttributeNode::setUri(const std::string& uri)
{
    mName.setAssociatedUri(uri);
}

void xml::lite::AttributeNode::setValue(const std::string& value)
{
    mValue = value;
}

std::string xml::lite::AttributeNode::getUri() const
{
    return mName.getAssociatedUri();
}

std::string xml::lite::AttributeNode::getLocalName() const
{
    return mName.getName();
}

std::string xml::lite::AttributeNode::getPrefix() const
{
    return mName.getPrefix();
}

std::string xml::lite::AttributeNode::getValue() const
{
    return mValue;
}

std::string xml::lite::AttributeNode::getQName() const
{
    return mName.toString();
}

xml::lite::Attributes::Attributes(const xml::lite::Attributes& attributes)
{
    mAttributes = attributes.mAttributes;
}

xml::lite::Attributes&
xml::lite::Attributes::operator=(const xml::lite::Attributes& attributes)
{
    if (this != &attributes)
    {
        mAttributes = attributes.mAttributes;
    }
    return *this;
}

const xml::lite::AttributeNode& xml::lite::Attributes::getNode(int i) const
{
    return mAttributes.at(i);
}

xml::lite::AttributeNode& xml::lite::Attributes::getNode(int i)
{
    return mAttributes.at(i);
}
