/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

xml::lite::AttributeNode::AttributeNode(const xml::lite::AttributeNode & node)
{
    mName = node.mName;
    mValue = node.mValue;
}

xml::lite::AttributeNode &
xml::lite::AttributeNode::operator=(const xml::lite::AttributeNode & node)
{
    if (&node != this)
    {
        mName = node.mName;
        mValue = node.mValue;
    }
    return *this;
}

int xml::lite::Attributes::getIndex(const std::string& qname) const
{

    for (int i = 0; i < (int) mAttributes.size(); i++)
    {
        if (qname == mAttributes[i].getQName())
            return i;
    }
    return -1;
}

int xml::lite::Attributes::getIndex(const std::string & uri,
                                    const std::string & localName) const
{
    for (int i = 0; i < (int) mAttributes.size(); i++)
    {
        if ((uri == mAttributes[i].getUri()) && (localName
                == mAttributes[i].getLocalName()))
            return i;
    }
    return -1;

}

std::string xml::lite::Attributes::getValue(int i) const
{
    return mAttributes[i].getValue();
}

std::string xml::lite::Attributes::getUri(int i) const
{
    return mAttributes[i].getUri();
}
std::string xml::lite::Attributes::getLocalName(int i) const
{
    return mAttributes[i].getLocalName();
}

std::string xml::lite::Attributes::getQName(int i) const
{
    return mAttributes[i].getQName();
}

std::string xml::lite::Attributes::getValue(const std::string & qname)
{
    for (int i = 0; i < (int) mAttributes.size(); i++)
    {
        if (qname == mAttributes[i].getQName())
            return mAttributes[i].getValue();
    }
    throw except::NoSuchKeyException(Ctxt(FmtX("QName '%s' could not be found",
                                               qname.c_str())));

    // We don't ever reach this but it keeps the compiler from complaining...
    return mAttributes[(int) mAttributes.size() - 1].getValue();
}

std::string xml::lite::Attributes::getValue(const std::string & uri,
                                            const std::string & localName)
{
    for (int i = 0; i < (int) mAttributes.size(); i++)
    {
        if ((uri == mAttributes[i].getUri()) && (localName
                == mAttributes[i].getLocalName()))
            return mAttributes[i].getValue();
    }
    throw except::NoSuchKeyException(Ctxt(FmtX("(uri: %s, localName: %s",
                                               uri.c_str(), localName.c_str())));

    // We don't ever reach this but it keeps the compiler from complaining...
    return mAttributes[(int) mAttributes.size() - 1].getValue();
}

void xml::lite::Attributes::add(const AttributeNode& attribute)
{
    mAttributes.push_back(attribute);
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

xml::lite::Attributes::Attributes(const xml::lite::Attributes & attributes)
{
    mAttributes = attributes.mAttributes;
}

xml::lite::Attributes&
xml::lite::Attributes::operator=(const xml::lite::Attributes & attributes)
{
    if (this != &attributes)
    {
        mAttributes = attributes.mAttributes;
    }
    return *this;
}

const xml::lite::AttributeNode& xml::lite::Attributes::getNode(int i) const
{
    return mAttributes[i];
}

xml::lite::AttributeNode& xml::lite::Attributes::getNode(int i)
{
    return mAttributes[i];
}
