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
#include <type_traits>

#include "str/Manip.h"
#include "str/Convert.h"

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

void xml::lite::MinidomHandler::clear()
{
    mDocument->destroy();
    currentCharacterData = "";
    assert(bytesForElement.empty());
    assert(nodeStack.empty());
}

void xml::lite::MinidomHandler::characters(const char* value, int length, const string_encoding* pEncoding)
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
            mpEncoding = std::make_shared<const string_encoding>(*pEncoding);
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
    const string_encoding* pEncoding = nullptr;
    #ifdef _WIN32
    if (use_wchar_t())
    {
        // If we're still here despite use_char() being "false" then the wide-character
        // routine "failed."  On Windows, that means the char* value is encoded
        // as Windows-1252 (more-or-less ISO8859-1).
        static const auto encoding = string_encoding::windows_1252;
        pEncoding = &encoding;
    }
    #endif
    characters(value, length, pEncoding);
}

template<typename CharT>
inline std::string toUtf8_(const CharT* value,  size_t length)
{
    const std::basic_string<CharT> strValue(value, length);
    std::string utf8Value;
    str::toUtf8(strValue, utf8Value);
    return utf8Value;
}
inline std::string toUtf8(const uint16_t* value_, size_t length)
{
    const auto value = reinterpret_cast<std::u16string::const_pointer>(value_);
    return toUtf8_(value, length);
}
inline std::string toUtf8(const uint32_t* value_, size_t length)
{
    const auto value = reinterpret_cast<std::u32string::const_pointer>(value_);
    return toUtf8_(value, length);
}
#if CODA_OSS_wchar_t_is_type_
inline std::string toUtf8(const wchar_t* value_, size_t length)
{
    using wchar_t_type = std::conditional<sizeof(wchar_t) == sizeof(uint32_t), uint32_t, uint16_t>::type;
#ifdef _WIN32
    // if we somehow get here on Windows (shouldn't, see below), wchar_t is UTF-16 not UTF-32
    static_assert(sizeof(wchar_t) == sizeof(wchar_t_type), "wchar_t should be 16-bits on Windows.");
#endif
    const auto value = reinterpret_cast<const wchar_t_type*>(value_);
    return toUtf8(value, length);
}
#endif

bool xml::lite::MinidomHandler::call_characters(const std::string& utf8Value)
{
    const auto length = static_cast<int>(utf8Value.length());
    static const auto encoding = xml::lite::string_encoding::utf_8;
    characters(utf8Value.c_str(), length, &encoding);
    return true;  // all done, characters(char*) already called, above
}

template <typename T>
bool xml::lite::MinidomHandler::characters_(const T* value, size_t length)
{
    #ifndef _WIN32
    const auto utf8Value = toUtf8(value, length);
    return call_characters(utf8Value);  // all done, characters(char*) already called, above
    #else
    UNREFERENCED_PARAMETER(value);
    UNREFERENCED_PARAMETER(length);
    // On Windows, we want std::string encoded as Windows-1252 (ISO8859-1)
    // so that western European characters will be displayed.  We can't convert
    // to UTF-8 (as above on Linux), because Windows doesn't have good support
    // for displaying such strings.  Using UTF-16 would be preferred on Windows, but
    // all existing code uses std::string instead of std::wstring.
    return false; // call characters(char*) to get a Windows-1252 string
    #endif
}
#if CODA_OSS_wchar_t_is_type_
bool xml::lite::MinidomHandler::characters(const wchar_t* value, size_t length)
{
    return characters_(value, length);
}
#endif
bool xml::lite::MinidomHandler::characters(const uint32_t* value, size_t length)
{
    return characters_(value, length);
}
bool xml::lite::MinidomHandler::characters(const uint16_t* value, size_t length)
{
    return characters_(value, length);
}

bool xml::lite::MinidomHandler::use_wchar_t() const
{
    // if we're storing the encoding, get wchar_t so that we can convert
    return storeEncoding();
}

void xml::lite::MinidomHandler::startElement(const std::string & uri,
                                             const std::string & /*localName*/,
                                             const std::string & qname,
                                             const xml::lite::Attributes & atts)
{
    // Assign what we can now, and push rest on stack
    // for later

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
    // Without mPreserveCharData=true, we gets asserts when parsing text containing
    // non-ASCII characters.  Given that, don't bother storing an encoding w/o 
    // mPreserveCharData also set.  This also further preserves existing behavior.
    // Also note that much code leaves mPreserveCharData as it's default of false.
    if (mStoreEncoding)
    {
        if (!mPreserveCharData)
        {
            throw std::logic_error("preserveCharacterData() must be set with storeEncoding()");
        }
    }
    return mStoreEncoding && mPreserveCharData;
}
