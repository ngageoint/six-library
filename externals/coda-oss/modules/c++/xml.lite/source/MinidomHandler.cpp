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

#include "str/Manip.h"
#include "str/Convert.h"
#include "str/Encoding.h"
#include "sys/OS.h"

#include "xml/lite/MinidomHandler.h"

void xml::lite::MinidomHandler::setDocument(Document *newDocument, bool own)
{
    if (mDocument != NULL && mOwnDocument)
    {
        if (newDocument != mDocument)
            delete mDocument;
    }
    mDocument = newDocument;
    mOwnDocument = own;
}
void xml::lite::MinidomHandler::setDocument(std::unique_ptr<Document>&& newDocument)
{
    setDocument(newDocument.release(), true /*own*/);
}
void  xml::lite::MinidomHandler::getDocument(std::unique_ptr<Document>& pDocument)
{
    pDocument.reset(getDocument(true /*steal*/));
}

void xml::lite::MinidomHandler::clear()
{
    mDocument->destroy();
    currentCharacterData = "";
    assert(bytesForElement.empty());
    assert(nodeStack.empty());
}

void xml::lite::MinidomHandler::characters(const char* value, int length, const StringEncoding* pEncoding)
{
    if (pEncoding != nullptr)
    {
        if (mpEncoding != nullptr)
        {
            // be sure the given encoding matches any encoding already set
            if (*pEncoding != *mpEncoding)
            {
                throw std::invalid_argument("New 'encoding' is different than value already set.");
            }
        }
        else if (storeEncoding())
        {
            mpEncoding = std::make_shared<const StringEncoding>(*pEncoding);
        }
    }

    // Append new data
    if (length)
        currentCharacterData += std::string(value, length);

    // Append number of bytes added to this node's stack value
    assert(bytesForElement.size());
    bytesForElement.top() += length;
}
void xml::lite::MinidomHandler::characters(const char *value, int length)
{
    const StringEncoding* pEncoding = nullptr;
    if ((sys::Platform == sys::PlatformType::Windows) && call_vcharacters())
    {
        // If we're still here despite use_char() being "false" then the wide-character
        // routine "failed."  On Windows, that means the char* value is encoded
        // as Windows-1252 (more-or-less ISO8859-1).
        static const auto encoding = StringEncoding::Windows1252;
        pEncoding = &encoding;
    }
    characters(value, length, pEncoding);
}

void xml::lite::MinidomHandler::call_characters(const std::string& s, StringEncoding encoding)
{
    const auto length = static_cast<int>(s.length());
    characters(s.c_str(), length, &encoding);
}

bool xml::lite::MinidomHandler::call_vcharacters() const
{
    // if we're storing the encoding, get wchar_t so that we can convert
    return storeEncoding();
}

bool xml::lite::MinidomHandler::vcharacters(const void /*XMLCh*/* chars_, size_t length)
{
    if (chars_ == nullptr)
    {
        throw std::invalid_argument("chars_ is NULL.");
    }
    if (length == 0)
    {
        throw std::invalid_argument("length is 0.");
    }

    static_assert(sizeof(XMLCh) == sizeof(char16_t), "XMLCh should be 16-bits.");
    auto pChars16 = static_cast<const char16_t*>(chars_);

    std::string chars;
    auto platformEncoding = xml::lite::PlatformEncoding;  // "conditional expression is constant"
    if (platformEncoding == xml::lite::StringEncoding::Utf8)
    {
        str::details::to_u8string(pChars16, length, chars);
    }
    else if (platformEncoding == xml::lite::StringEncoding::Windows1252)
    {
        // On Windows, we want std::string encoded as Windows-1252 so that
        // western European characters will be displayed.  We can't convert
        // to UTF-8 (as above on Linux), because Windows doesn't have good
        // support for displaying such strings.  Using UTF-16 would be preferred
        // on Windows, but all existing code uses std::string instead of std::wstring.
        assert(pChars16 != nullptr);  // XMLCh == wchar_t == char16_t on Windows
        auto pChars = static_cast<const XMLCh*>(chars_);
        chars = xml::lite::XercesLocalString(pChars).str();
    }
    else
    {
        throw std::logic_error("Unknown xml::lite::StringEncoding");
    }

    call_characters(chars, platformEncoding);
    return true; // vcharacters() processed
}

void xml::lite::MinidomHandler::startElement(const std::string & uri,
                                             const std::string & /*localName*/,
                                             const std::string & qname,
                                             const xml::lite::Attributes & atts)
{
    // Assign what we can now, and push rest on stack for later
    xml::lite::Element * current = mDocument->createElement(qname, uri);

    current->setAttributes(atts);
    // Push this onto the node stack
    nodeStack.push(current);
    // Push a size of zero bytes on stack for this node's char data
    bytesForElement.push(0);
}

// This function subtracts off the char place from the push
std::string xml::lite::MinidomHandler::adjustCharacterData()
{
    // Edit the string with regard to this node's char data
    // Get rid of what we take on char data accumulator

    int diff = (int) (currentCharacterData.length()) - bytesForElement.top();

    std::string newCharacterData(currentCharacterData.substr(
                                 diff,
                                 currentCharacterData.length())
                );
    assert(diff >= 0);
    currentCharacterData.erase(diff, currentCharacterData.length());
    if (!mPreserveCharData && !newCharacterData.empty())
        trim(newCharacterData);

    return newCharacterData;
}

void xml::lite::MinidomHandler::trim(std::string & s)
{
    str::trim(s);
}

void xml::lite::MinidomHandler::endElement(const std::string & /*uri*/,
                                           const std::string & /*localName*/,
                                           const std::string & /*qname*/)
{
    // Pop current off top
    xml::lite::Element * current = nodeStack.top();
    nodeStack.pop();

    current->setCharacterData_(adjustCharacterData(), mpEncoding.get());

    // Remove corresponding int on bytes stack
    bytesForElement.pop();
    // Something is left on the stack
    // (We dont have not top-level node)
    if (nodeStack.size())
    {
        // Add current to child of parent
        xml::lite::Element * parent = nodeStack.top();
        parent->addChild(current);
    }
    // This is the top-level node, and we are done
    // Just Assign
    else
    {
        mDocument->setRootElement(current);
    }
}

void xml::lite::MinidomHandler::preserveCharacterData(bool preserve)
{
    mPreserveCharData = preserve;
}

void xml::lite::MinidomHandler::storeEncoding(bool value)
{
    mStoreEncoding = value;
}

bool xml::lite::MinidomHandler::storeEncoding() const
{
    return mStoreEncoding;
}
