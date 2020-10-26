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

#include "xml/lite/Element.h"
#include <import/str.h>

xml::lite::Element::Element(const xml::lite::Element& node)
{
    *this = node;
}

xml::lite::Element& xml::lite::Element::operator=(const xml::lite::Element& node)
{
    if (this !=&node)
    {
        mName = node.mName;
        mCharacterData = node.mCharacterData;
        mpEncoding = node.mpEncoding;
        mAttributes = node.mAttributes;
        mChildren = node.mChildren;
        mParent = node.mParent;
    }
    return *this;
}

void xml::lite::Element::clone(const xml::lite::Element& node)
{
    mName = node.mName;
    mCharacterData = node.mCharacterData;
    mpEncoding = node.mpEncoding;
    mAttributes = node.mAttributes;
    mParent = NULL;

    std::vector<xml::lite::Element *>::const_iterator iter;
    iter = node.getChildren().begin();
    for (; iter != node.getChildren().end(); ++iter)
    {
        xml::lite::Element *child = new xml::lite::Element();
        child->clone(**iter);
        this->addChild(child);
    }
}

bool xml::lite::Element::hasElement(const std::string& uri,
                                    const std::string& localName) const
{

    for (unsigned int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i]->getUri() == uri&& mChildren[i]->getLocalName()
                == localName)
            return true;
    }
    return false;
}

bool xml::lite::Element::hasElement(const std::string& localName) const
{

    for (unsigned int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i]->getLocalName() == localName)
            return true;
    }
    return false;
}

void xml::lite::Element::getElementsByTagName(const std::string& uri,
                                              const std::string& localName,
                                              std::vector<Element*>& elements,
                                              bool recurse) const
{
    for (unsigned int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i]->getUri() == uri && mChildren[i]->getLocalName()
                == localName)
            elements.push_back(mChildren[i]);
        if (recurse)
            mChildren[i]->getElementsByTagName(uri, localName, elements, recurse);
    }
}

void xml::lite::Element::getElementsByTagName(const std::string& localName,
                                              std::vector<Element*>& elements,
                                              bool recurse) const 
{
    for (unsigned int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i]->getLocalName() == localName)
            elements.push_back(mChildren[i]);
        if (recurse)
            mChildren[i]->getElementsByTagName(localName, elements, recurse);
    }
}

void xml::lite::Element::getElementsByTagNameNS(const std::string& qname,
                                                std::vector<Element*>& elements,
                                                bool recurse) const
{
    for (unsigned int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i]->mName.toString() == qname)
            elements.push_back(mChildren[i]);
        if (recurse)
            mChildren[i]->getElementsByTagNameNS(qname, elements, recurse);
    }
}

void xml::lite::Element::destroyChildren()
{
    // While something is in vector
    while (mChildren.size())
    {
        // Get the last thing out
        xml::lite::Element * childAtBack = mChildren.back();
        // Pop it off
        mChildren.pop_back();
        // Delete it
        delete childAtBack;
    }
}

void xml::lite::Element::print(io::OutputStream& stream) const
{
    depthPrint(stream, 0, "");
}
void xml::lite::Element::print(io::OutputStream& stream, string_encoding encoding) const
{
    depthPrint(stream, encoding, 0, "");
}

void xml::lite::Element::prettyPrint(io::OutputStream& stream,
                                     const std::string& formatter) const
{
    depthPrint(stream, 0, formatter);
    stream.writeln("");
}
void xml::lite::Element::prettyPrint(io::OutputStream& stream, string_encoding encoding,
                                     const std::string& formatter) const
{
    depthPrint(stream, encoding, 0, formatter);
    stream.writeln("");
}

static void writeCharacterData(io::OutputStream& stream,
    const std::string& characterData, const xml::lite::string_encoding* pCharacterEncoding)
{
    const std::string* pStringToWrite = &characterData; // already in UTF-8

    sys::u8string u8CharacterData; // keep result in-scope
    if (*pCharacterEncoding != xml::lite::string_encoding::utf_8)
    {
        u8CharacterData = str::toUtf8(characterData);
        pStringToWrite = reinterpret_cast<std::string*>(&u8CharacterData);
    }

    stream.write(*pStringToWrite);
}

void xml::lite::Element::depthPrint(io::OutputStream& stream,
                                    int depth,
                                    const std::string& formatter) const
{
    depthPrint(stream, nullptr /*pEncoding*/, depth, formatter);
}
void xml::lite::Element::depthPrint(io::OutputStream& stream, string_encoding encoding,
                                    int depth,
                                    const std::string& formatter) const
{
    depthPrint(stream, &encoding, depth, formatter);
}
void xml::lite::Element::depthPrint(io::OutputStream& stream, const string_encoding* pEncoding,
                                    int depth,
                                    const std::string& formatter) const
{
    // XML must be stored in UTF-8 (or UTF-16/32), in particular, not Windows-1252.
    // However, existing code did this, so preserve current behavior.
    if ((pEncoding != nullptr) && (*pEncoding != string_encoding::utf_8))
    {
        throw std::invalid_argument("'encoding' must be UTF-8");
    }

    std::string prefix = "";
    for (int i = 0; i < depth; ++i)
        prefix += formatter;

    // Printing in XML form, recursively
    std::string lBrack = "<";
    std::string rBrack = ">";

    std::string acc = prefix + lBrack + mName.toString();

    for (int i = 0; i < mAttributes.getLength(); i++)
    {
        acc += std::string(" ");
        acc += mAttributes.getQName(i);
        acc += std::string("=\"");
        acc += mAttributes.getValue(i);
        acc += std::string("\"");
    }

    if (mCharacterData.empty()&& mChildren.empty())
    {
        //simple type - just end it here
        stream.write(acc + "/" + rBrack);
    }
    else
    {
        stream.write(acc + rBrack);
        if (pEncoding != nullptr)
        {
            writeCharacterData(stream, mCharacterData, getEncoding());
        }
        else
        {
            // Legacy behavior, will generate incorrect XML output if there are western European
            // characters in "mCharacterData".
            stream.write(mCharacterData);
        }

        for (unsigned int i = 0; i < mChildren.size(); i++)
        {
            if (!formatter.empty())
                stream.write("\n");
            mChildren[i]->depthPrint(stream, pEncoding, depth + 1, formatter);
        }

        if (!mChildren.empty() && !formatter.empty())
        {
            stream.write("\n" + prefix);
        }

        lBrack += "/";
        stream.write(lBrack + mName.toString() + rBrack);
    }
}

void xml::lite::Element::addChild(xml::lite::Element * node)
{
    mChildren.push_back(node);
    node->setParent(this);
}

void xml::lite::Element::addChild(std::unique_ptr<xml::lite::Element>&& node)
{
    // Always take ownership
    addChild(node.release());
}

void xml::lite::Element::changePrefix(Element* element,
    const std::string& prefix, const std::string& uri)
{
    if (element->mName.getAssociatedUri() == uri)
    {
        element->mName.setPrefix(prefix);
    }

    // Traverse backward to support removing nodes
    for (int i = element->mAttributes.getLength() - 1; i >= 0; i--)
    {
        if (element->mAttributes[i].getPrefix() == "xmlns" &&
            element->mAttributes[i].getValue() == uri)
        {
            // Remove all definitions of namespace
            element->mAttributes.remove(i);
        }
        else if (element->mAttributes[i].getUri() == uri)
        {
            element->mAttributes[i].setPrefix(prefix);
        }
    }

    for (int i = 0, s = element->mChildren.size(); i < s; i++)
    {
        changePrefix(element->mChildren[i], prefix, uri);
    }
}

void xml::lite::Element::changeURI(Element* element,
    const std::string& prefix, const std::string& uri)
{
    if (element->mName.getPrefix() == prefix)
    {
        element->mName.setAssociatedUri(uri);
    }

    // Traverse backward to support removing nodes
    for (int i = element->mAttributes.getLength() - 1; i >= 0; i--)
    {
        if (element->mAttributes[i].getPrefix() == "xmlns" &&
            element->mAttributes[i].getLocalName() == prefix)
        {
            // Remove all definitions of namespace
            element->mAttributes.remove(i);
        }
        else if (element->mAttributes[i].getPrefix() == prefix)
        {
            element->mAttributes[i].setUri(uri);
        }
    }

    for (int i = 0, s = element->mChildren.size(); i < s; i++)
    {
        changeURI(element->mChildren[i], prefix, uri);
        break;
    }
}

void xml::lite::Element::setNamespacePrefix(
    std::string prefix, std::string uri)
{
    str::trim(prefix);
    changePrefix(this, prefix, uri);

    // Add namespace definition
    ::xml::lite::Attributes& attr = getAttributes();

    std::string p("xmlns");
    if (!prefix.empty())
        p += std::string(":") + prefix;
    attr[p] = uri;
}

void xml::lite::Element::setNamespaceURI(
    std::string prefix, std::string uri)
{
    str::trim(prefix);
    changeURI(this, prefix, uri);

    // Add namespace definition
    ::xml::lite::Attributes& attr = getAttributes();

    std::string p("xmlns");
    if (!prefix.empty())
        p += std::string(":") + prefix;
    attr[p] = uri;

    attr[std::string("xmlns:") + prefix] = uri;
}
