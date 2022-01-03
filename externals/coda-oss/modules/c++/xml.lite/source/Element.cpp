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
#include <tuple>

#include "xml/lite/Element.h"
#include <import/str.h>
#include <import/mem.h>
#include <sys/OS.h>

constexpr auto PlatformEncoding = sys::Platform == sys::PlatformType::Windows
        ? xml::lite::StringEncoding::Windows1252
        : xml::lite::StringEncoding::Utf8;

std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const std::string& qname, const std::string& uri, const std::string& characterData)
{
    return mem::make::unique<Element>(qname, uri, characterData, PlatformEncoding);
}
std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const std::string& qname, const Uri& uri, const std::string& characterData)
{
    return create(qname, uri.value, characterData);
}
std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const QName& qname, const std::string& characterData)
{
    return create(qname.getName(), qname.getUri(), characterData);
}
std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const QName& qname, const sys::U8string& characterData)
{
    return mem::make::unique<Element>(qname.getName(), qname.getUri().value,  characterData);
}
std::unique_ptr<xml::lite::Element> xml::lite::Element::createU8(const QName& qname, const std::string& characterData)
{
    return create(qname,  str::to_u8string(characterData));
}

xml::lite::Element::Element(const xml::lite::Element& node)
{
    *this = node;
}
xml::lite::Element& xml::lite::Element::operator=(const xml::lite::Element& node)
{
    if (this != &node)
    {
        mName = node.mName;
        mCharacterData = node.mCharacterData;
        mEncoding = node.mEncoding;
        mAttributes = node.mAttributes;
        mChildren = node.mChildren;
        mParent = node.mParent;
        mEncoding = node.mEncoding;
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

bool xml::lite::Element::hasElement(const QName& qname) const
{
    const auto uri = qname.getUri().value;
    const auto localName = qname.getName();

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

void xml::lite::Element::getElementsByTagName(const QName& n, std::vector<Element*>& elements, bool recurse) const
{
    const auto uri = n.getUri().value;
    const auto localName = n.getName();

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
std::tuple<xml::lite::Element*, std::string> getElement(TGetElements getElements)
{
    auto elements = getElements();
    if (elements.size() == 1)
    {
        return std::make_tuple(elements[0], "");
    }
    return std::make_tuple(nullptr, std::to_string(elements.size()));
}
template <typename TGetElements, typename TMakeContext>
xml::lite::Element& getElement(TGetElements getElements, TMakeContext makeContext)
{
    auto result = getElement(getElements);
    auto pElement = std::get<0>(result);
    if (pElement  == nullptr)
    {
        const auto ctxt = makeContext(std::get<1>(result));
        throw xml::lite::XMLException(ctxt);
    }
    return *pElement;
}

xml::lite::Element* xml::lite::Element::getElementByTagName(std::nothrow_t, const QName& n, bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagName(n, recurse); };
    return std::get<0>(getElement(getElements));
}
xml::lite::Element& xml::lite::Element::getElementByTagName(const QName& n, bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagName(n, recurse); };
    auto makeContext = [&](const std::string& sz) {
        const auto uri = n.getUri().value;
        const auto localName = n.getName();
       return Ctxt("Expected exactly one '" + localName + "' (uri=" + uri + "); but got " + sz); };
    return getElement(getElements, makeContext);
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
    return std::get<0>(getElement(getElements));
}
xml::lite::Element& xml::lite::Element::getElementByTagName(
    const std::string& localName, bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagName(localName, recurse); };
    auto makeContext = [&](const std::string& sz) {
       return Ctxt("Expected exactly one '" + localName + "'; but got " + sz); };
    return getElement(getElements, makeContext);
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
    return std::get<0>(getElement(getElements));
}
xml::lite::Element& xml::lite::Element::getElementByTagNameNS(
    const std::string& qname, bool recurse) const
{
    auto getElements = [&]() { return getElementsByTagNameNS(qname, recurse); };
    auto makeContext = [&](const std::string& sz) {
        return Ctxt("Expected exactly one '" + qname + "'; but got " + sz); };
    return getElement(getElements, makeContext);
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
void xml::lite::Element::print(io::OutputStream& stream, StringEncoding encoding) const
{
    depthPrint(stream, encoding, 0, "");
}

void xml::lite::Element::prettyPrint(io::OutputStream& stream,
                                     const std::string& formatter) const
{
    depthPrint(stream, 0, formatter);
    stream.writeln("");
}
void xml::lite::Element::prettyPrint(io::OutputStream& stream, StringEncoding encoding,
                                     const std::string& formatter) const
{
    depthPrint(stream, encoding, 0, formatter);
    stream.writeln("");
}

static xml::lite::StringEncoding getEncoding_(const sys::Optional<xml::lite::StringEncoding>& encoding)
{
    if (encoding.has_value())
    {
        if (encoding == xml::lite::StringEncoding::Utf8) { }
        else if (encoding == xml::lite::StringEncoding::Windows1252) { }
        else
        {
            throw std::logic_error("Unknown encoding.");
        }
        return *encoding;
    }

    // don't know the encoding ... assume a default based on the platform
    return PlatformEncoding;
}

void xml::lite::Element::getCharacterData(sys::U8string& result) const
{
    const auto encoding = ::getEncoding_(this->getEncoding());

    if (encoding == xml::lite::StringEncoding::Utf8)
    {
        // already in UTF-8, no converstion necessary
        result = str::c_str<sys::U8string::const_pointer>(mCharacterData); // copy
    }
    else if (encoding == xml::lite::StringEncoding::Windows1252)
    {
        result = str::fromWindows1252(mCharacterData);
    }
    else
    {
        throw std::logic_error("getCharacterData(): unknown encoding");
    }
}

static void writeCharacterData(io::OutputStream& stream,
    const std::string& characterData, const sys::Optional<xml::lite::StringEncoding>& encoding_)
{
    const auto encoding = getEncoding_(encoding_);
    if (encoding == xml::lite::StringEncoding::Windows1252)
    {
        // need to convert before writing
        const auto utf8 = str::fromWindows1252(characterData);
        stream.write(utf8);
    }
    else if (encoding == xml::lite::StringEncoding::Utf8)
    {
        // already in UTF-8, no converstion necessary
        auto pUtf8 = str::c_str<sys::U8string::const_pointer>(characterData);
        stream.write(pUtf8, characterData.length()); // call UTF-8 overload
    }
    else
    {
        throw std::logic_error("writeCharacterData(): unknown encoding");
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
void xml::lite::Element::depthPrint(io::OutputStream& stream, StringEncoding encoding,
                                    int depth,
                                    const std::string& formatter) const
{
    if (encoding != StringEncoding::Utf8)
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

xml::lite::Element& xml::lite::Element::addChild(std::unique_ptr<xml::lite::Element>&& node)
{
    auto retval = node.get();
    addChild(node.release());
    return *retval;
}
#if CODA_OSS_autoptr_is_std  // std::auto_ptr removed in C++17
xml::lite::Element& xml::lite::Element::addChild(mem::auto_ptr<xml::lite::Element> node)
{
    return addChild(std::unique_ptr<xml::lite::Element>(node.release()));
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

#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)  // unreachable code
#endif
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

    // the "i++" is unreachable because of the "break"
    const auto s = element->mChildren.size();
    for (size_t i = 0; i < s; i++)
    {
        changeURI(element->mChildren[i], prefix, uri);
        break;
    }
}
#if _MSC_VER
#pragma warning(pop)
#endif

void xml::lite::Element::setNamespacePrefix(
    std::string prefix, const Uri& uri_)
{
    str::trim(prefix);
    auto uri = uri_.value;
    changePrefix(this, prefix, uri);

    // Add namespace definition
    ::xml::lite::Attributes& attr = getAttributes();

    std::string p("xmlns");
    if (!prefix.empty())
        p += std::string(":") + prefix;
    attr[p] = uri;
}

void xml::lite::Element::setNamespaceURI(
    std::string prefix,  const Uri& uri_)
{
    str::trim(prefix);
    auto uri = uri_.value;
    changeURI(this, prefix, uri);

    // Add namespace definition
    ::xml::lite::Attributes& attr = getAttributes();

    std::string p("xmlns");
    if (!prefix.empty())
        p += std::string(":") + prefix;
    attr[p] = uri;

    attr[std::string("xmlns:") + prefix] = uri;
}

void xml::lite::Element::setCharacterData_(const std::string& characters, const StringEncoding* pEncoding)
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
void xml::lite::Element::setCharacterData(const std::string& characters, StringEncoding encoding)
{
    setCharacterData_(characters, &encoding);
}
void xml::lite::Element::setCharacterData(const sys::U8string& characters)
{
    setCharacterData(str::c_str<std::string::const_pointer>(characters), StringEncoding::Utf8);
}

xml::lite::Element& xml::lite::add(const QName& qname,
                                   const std::string& value,
                                   Element& parent)
{
    auto elem = Element::create(qname, value);
    return parent.addChild(std::move(elem));
}

