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
#include "xml/lite/Element.h"

#include <assert.h>

#include <stdexcept>
#include <tuple>
#include <std/string>

#include <import/str.h>
#include <import/mem.h>
#include <sys/OS.h>
#include <str/Encoding.h>
#include "xml/lite/Attributes.h"

std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const std::string& qname, const std::string& uri, const std::string& characterData)
{
    return std::make_unique<Element>(qname, uri, characterData);
}
std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const QName& qname, const std::string& characterData)
{
    return create(qname.getName(), qname.getUri().value, characterData);
}
std::unique_ptr<xml::lite::Element> xml::lite::Element::create(const QName& qname, const coda_oss::u8string& characterData)
{
    return std::make_unique<Element>(qname,  characterData);
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
    mParent = nullptr;

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

    elements.reserve(mChildren.size());
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
    return std::make_tuple(nullptr, str::toString(elements.size()));
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
    elements.reserve(mChildren.size());
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
    elements.reserve(mChildren.size());
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

void xml::lite::Element::prettyPrint(io::OutputStream& stream,
                                     const std::string& formatter) const
{
    depthPrint(stream, 0, formatter);
    stream.writeln("");
}

void xml::lite::Element::consoleOutput_(io::OutputStream& stream) const
{
    depthPrint(stream, 0, "", true /*isConsoleOutput*/);
}
void xml::lite::Element::prettyConsoleOutput_(io::OutputStream& stream,
                                     const std::string& formatter) const
{
    depthPrint(stream, 0, formatter, true /*isConsoleOutput*/);
    stream.writeln("");
}


std::string xml::lite::Element::getCharacterData() const
{
    return str::toString(mCharacterData);
}
coda_oss::u8string& xml::lite::Element::getCharacterData(coda_oss::u8string& result) const
{
    result = mCharacterData;
    return result;
}
coda_oss::u8string xml::lite::getCharacterData(const Element& e)
{
    coda_oss::u8string retval;
    return e.getCharacterData(retval);
}

static void writeCharacterData_utf8(io::OutputStream& stream, const std::u8string& characterData)
{
    stream.write(characterData);  // call UTF-8 overload
}
static void writeCharacterData_native(io::OutputStream& stream, const std::u8string& characterData)
{
    stream.write(str::toString(characterData));
}

static void depthPrint_(const xml::lite::Element& element,
    io::OutputStream& stream, int depth, const std::string& formatter,
    void(*writeCharacterData)(io::OutputStream&, const std::u8string&))
{
    // XML must be stored in UTF-8 (or UTF-16/32), in particular, not Windows-1252. 
    //
    // Except for a special exception for writing to the console: UTF-8 won't display well on Windows
    // and Windows-1252 won't display nicely on Linux.  Of course, "console output" is a bit
    // iffy since both Windows and Linux support redirection ... so the user could still generate
    // a bad XML file.

    std::string prefix = "";
    for (int i = 0; i < depth; ++i)
        prefix += formatter;

    // Printing in XML form, recursively
    std::string lBrack = "<";
    static const std::string rBrack = ">";

    const auto name = element.getQName();
    std::string acc = prefix + lBrack + name;

    auto&& attributes = element.getAttributes();
    for (int i = 0; i < attributes.getLength(); i++)
    {
        acc += std::string(" ");
        acc += attributes.getQName(i);
        acc += std::string("=\"");
        acc += attributes.getValue(i);
        acc += std::string("\"");
    }

    const auto characterData = getCharacterData(element);
    auto&& children = element.getChildren();
    if (characterData.empty() && children.empty())
    {
        //simple type - just end it here
        stream.write(acc + "/" + rBrack);
    }
    else
    {
        stream.write(acc + rBrack);            
        writeCharacterData(stream, characterData);

        for (auto&& child: children)
        {
            if (!formatter.empty())
                stream.write("\n");
            depthPrint_(*child, stream, depth + 1, formatter, writeCharacterData);
        }

        if (!children.empty() && !formatter.empty())
        {
            stream.write("\n" + prefix);
        }

        lBrack += "/";
        stream.write(lBrack + name + rBrack);
    }
}
void xml::lite::Element::depthPrint(io::OutputStream& stream, int depth, const std::string& formatter, bool isConsoleOutput) const
{
    const auto f = isConsoleOutput ? writeCharacterData_native // write to the console using the platform native encoding
        : writeCharacterData_utf8;
    depthPrint_(*this, stream, depth, formatter, f);
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

void xml::lite::Element::setCharacterData(const std::string& characters)
{
    mCharacterData = str::to_u8string(characters);
}
xml::lite::Element& xml::lite::Element::operator=(const std::string& characterData)
{
    setCharacterData(characterData);
    return *this;
}
xml::lite::Element& xml::lite::Element::operator=(const char* characterData)
{
    *this = std::string(characterData);
    return *this;
}

xml::lite::Element& xml::lite::add(const QName& qname,
                                   const std::string& value,
                                   Element& parent)
{
    auto elem = Element::create(qname, value);
    return parent.addChild(std::move(elem));
}

void xml::lite::operator+=(Element& e, std::unique_ptr<xml::lite::Element>&& child)
{
    std::ignore = e.addChild(std::move(child));
}

xml::lite::Element& xml::lite::addChild(Element& e, const std::string& qname, const Uri& uri, const coda_oss::u8string& characterData)
{
    return e.addChild(Element::create(QName(qname, uri), characterData));
}
xml::lite::Element& xml::lite::addChild(Element& e, const std::string& qname, const Uri& uri)
{
    return e.addChild(Element::create(QName(qname, uri)));
}

xml::lite::Element& xml::lite::addChild(Element& e, const QName& qname, const coda_oss::u8string& characterData)
{
    return e.addChild(Element::create(qname, characterData));
}
xml::lite::Element& xml::lite::addChild(Element& e, const QName& qname, const std::string& characterData)
{
    return addChild(e, qname, str::to_u8string(characterData));
}
xml::lite::Element& xml::lite::addChild(Element& e, const QName& qname)
{
    return e.addChild(Element::create(qname));
}
void xml::lite::operator+=(Element& e, const QName& qname)
{
    std::ignore = addChild(e, qname);
}

xml::lite::Element& xml::lite::addChild(Element& e, const std::string& qname, const coda_oss::u8string& characterData)
{
    return addChild(e, QName(qname), characterData);
}
xml::lite::Element& xml::lite::addChild(Element& e, const std::string& qname)
{
    return addChild(e, QName(qname));
}
void xml::lite::operator+=(Element& e, const std::string& qname)
{
    std::ignore = addChild(e, qname);
}

xml::lite::Element& xml::lite::setChild(Element& e, std::unique_ptr<Element>&& child)
{
    e.destroyChildren();
    return e.addChild(std::move(child));
}
xml::lite::Element& xml::lite::Element::operator=(std::unique_ptr<Element>&& child)
{
    std::ignore = setChild(*this, std::move(child));
    return *this;
}

xml::lite::AttributeNode& xml::lite::addAttribute(Element& e, const AttributeNode& a)
{
    return e.getAttributes().add(a);
}
void xml::lite::operator+=(Element& e, const AttributeNode& a)
{
    std::ignore = addAttribute(e, a);
}
xml::lite::AttributeNode& xml::lite::addAttribute(Element& e, const QName& name, const std::string& value)
{
    return addAttribute(e, AttributeNode(name, value));
}
xml::lite::AttributeNode& xml::lite::addAttribute(Element& e, const QName& name)
{
    return addAttribute(e, AttributeNode(name));
}
xml::lite::AttributeNode& xml::lite::addAttribute(Element& e, const std::string& name)
{
    return addAttribute(e, QName(name));
}