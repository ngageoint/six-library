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
#include <import/mem.h>

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
        mEncoding = node.mEncoding;
        mAttributes = node.mAttributes;
        mChildren = node.mChildren;
        mParent = node.mParent;
    }
    return *this;
}

void xml::lite::Element::clone(const xml::lite::Element& node)
{
    *this = node;
    clearChildren();
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

bool xml::lite::Element::hasElement(const std::string& uri, const std::string& localName) const
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

void xml::lite::Element::getElementsByTagName(const std::string& uri, const std::string& localName,
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

template <typename TGetElements>
xml::lite::Element* getElement(TGetElements getElements, const except::Context& ctxt)
{
    auto elements = getElements();
    if (elements.empty())
    {
        return nullptr;
    }
    if (elements.size() > 1)
    {
        // Yes, this is "nothrow" ... that's for found/non-found status.  We
        // asked for an ELEMENT, not "elements".
        throw xml::lite::XMLException(ctxt);
    }
    return elements[0];
}

xml::lite::Element* xml::lite::Element::getElementByTagName(std::nothrow_t,
    const std::string& uri, const std::string& localName,
    bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagName(uri, localName, recurse); };
    const auto ctxt(Ctxt("Multiple elements returned for '" + localName + "' (uri=" + uri + ")."));
    return getElement(getElements, ctxt);
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

xml::lite::Element* xml::lite::Element::getElementByTagName(std::nothrow_t,
    const std::string& localName, bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagName(localName, recurse); };
    const auto ctxt(Ctxt("Multiple elements returned for '" + localName + "'."));
    return getElement(getElements, ctxt);
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

xml::lite::Element* xml::lite::Element::getElementByTagNameNS(std::nothrow_t,
    const std::string& qname, bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagNameNS(qname, recurse); };
    const auto ctxt(Ctxt("Multiple elements returned for '" + qname + "'."));
    return getElement(getElements, ctxt);
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

static xml::lite::string_encoding getEncoding_(const sys::Optional<xml::lite::string_encoding>& encoding)
{
    if (encoding.has_value())
    {
        if (encoding == xml::lite::string_encoding::utf_8) { }
        else if (encoding == xml::lite::string_encoding::windows_1252) { }
        else
        {
            throw std::logic_error("Unknown encoding.");
        }
        return *encoding;
    }

    // don't know the encoding ... assume a default based on the platform
    #ifdef _WIN32
    return xml::lite::string_encoding::windows_1252;
    #else
    return xml::lite::string_encoding::utf_8;
    #endif
}

void xml::lite::Element::getCharacterData(sys::U8string& result) const
{
    const auto encoding = ::getEncoding_(this->getEncoding());

    if (encoding == xml::lite::string_encoding::utf_8)
    {
        // already in UTF-8, no converstion necessary
        result = str::castToU8string(mCharacterData);
    }
    else if (encoding == xml::lite::string_encoding::windows_1252)
    {
        str::fromWindows1252(mCharacterData, result);
    }
}

static void writeCharacterData(io::OutputStream& stream,
    const std::string& characterData, const sys::Optional<xml::lite::string_encoding>& encoding)
{
    if (getEncoding_(encoding) != xml::lite::string_encoding::utf_8)
    {
        std::string utf8; // need to convert before writing
        str::fromWindows1252(characterData, utf8);
        stream.write(utf8);
    }
    else
    {
        // already UTF-8
        stream.write(characterData);    
    }
}

void xml::lite::Element::depthPrint(io::OutputStream& stream,
                                    int depth,
                                    const std::string& formatter) const
{
    // XML must be stored in UTF-8 (or UTF-16/32), in particular, not
    // Windows-1252. However, existing code did this, so preserve current behavior.
    depthPrint(stream, false /*utf8*/, depth, formatter);
}
void xml::lite::Element::depthPrint(io::OutputStream& stream, string_encoding encoding,
                                    int depth,
                                    const std::string& formatter) const
{
    if (encoding != string_encoding::utf_8)
    {
        throw std::invalid_argument("'encoding' must be UTF-8");
    }
    // THIS IS CORRECT, but may break existing code; so it must be explicitly requested.
    depthPrint(stream, true /*utf8*/, depth, formatter);
}
void xml::lite::Element::depthPrint(io::OutputStream& stream, bool utf8,
                                    int depth,
                                    const std::string& formatter) const
{
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
        if (utf8)
        {
            // Correct behavior, but may break existing code.
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
            mChildren[i]->depthPrint(stream, utf8, depth + 1, formatter);
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
    addChild(node.release());
}
#if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
void xml::lite::Element::addChild(std::auto_ptr<xml::lite::Element> node)
{
    addChild(std::unique_ptr<xml::lite::Element>(node.release()));
}
#endif

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

    for (size_t i = 0, s = element->mChildren.size(); i < s; i++)
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

    for (size_t i = 0, s = element->mChildren.size(); i < s; i++)
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

void xml::lite::Element::setCharacterData_(const std::string& characters, const string_encoding* pEncoding)
{
    mCharacterData = characters;
    if (pEncoding != nullptr)
    {
        mEncoding = *pEncoding;
    }
    else
    {
        mEncoding.reset();
    }
}
void xml::lite::Element::setCharacterData(const std::string& characters)
{
    setCharacterData_(characters, nullptr /*pEncoding*/);
}
void xml::lite::Element::setCharacterData(const std::string& characters, string_encoding encoding)
{
    setCharacterData_(characters, &encoding);
}
void xml::lite::Element::setCharacterData(const sys::U8string& characters)
{
    setCharacterData(str::toString(characters), string_encoding::utf_8);
}