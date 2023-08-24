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
#include <std/string>
#include <utility>

#include "str/Manip.h"
#include "str/Convert.h"
#include "str/Encoding.h"
#include "sys/OS.h"

#include "xml/lite/MinidomHandler.h"

void xml::lite::MinidomHandler::setDocument(Document *newDocument, bool own)
{
    if (mDocument != nullptr && mOwnDocument)
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
std::unique_ptr<xml::lite::Document>& xml::lite::MinidomHandler::getDocument(std::unique_ptr<Document>& pDocument)
{
    pDocument.reset(getDocument(true /*steal*/));
    return pDocument;
}

void xml::lite::MinidomHandler::clear()
{
    mDocument->destroy();
    currentCharacterData.clear();
    assert(bytesForElement.empty());
    assert(nodeStack.empty());
}

void xml::lite::MinidomHandler::characters(std::u8string&& s)
{
    // Append number of bytes added to this node's stack value
    assert(bytesForElement.size());
    bytesForElement.top() += static_cast<int>(s.length());

    // Append new data
    currentCharacterData += std::move(s);
}
void xml::lite::MinidomHandler::characters(const char *value, int length)
{
    // If we're still here despite use_char() being "false" then the
    // wide-character routine "failed."  On Windows, that means the char* value
    // is encoded as Windows-1252 (more-or-less ISO8859-1).
    characters(str::to_u8string(value, length));
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

    characters(str::to_u8string(pChars16, length));
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
std::u8string xml::lite::MinidomHandler::adjustCharacterData()
{
    // Edit the string with regard to this node's char data
    // Get rid of what we take on char data accumulator

    int diff = (int) (currentCharacterData.length()) - bytesForElement.top();

    auto newCharacterData(currentCharacterData.substr(
                                 diff,
                                 currentCharacterData.length())
                );
    assert(diff >= 0);
    currentCharacterData.erase(diff, currentCharacterData.length());
    if (!mPreserveCharData && !newCharacterData.empty())
        trim(newCharacterData);

    return newCharacterData;
}

void xml::lite::MinidomHandler::trim(std::u8string& s)
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

    current->setCharacterData(adjustCharacterData());

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

