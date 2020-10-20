/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <string>
#include <stdexcept>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <io/OutputStream.h>

// Convert a single ISO8859-1 character to UTF-8
// // https://en.wikipedia.org/wiki/ISO/IEC_8859-1
static std::string to_utf8_(std::string::value_type ch)
{
    if ((ch >= '\x00') && (ch <= '\x7f'))  // ASCII
    {
        return std::string{ch};
    }

    if ((ch >= '\xC0' /*À*/) && (ch <= '\xFF' /*y*/))  // ISO8859-1 letters
    {
        std::string retval{'\xC3'};
        ch -= 0x40;  // 0xC0 -> 0x80
        retval.push_back(ch);
        return retval;
    }
    
    return std::string{ch}; // ???
}

static std::string to_utf8(const std::string& str)
{
    std::string retval;
    // Assume the input string is ISO8859-1 (western European) and convert to UTF-8
    for (const auto& ch : str)
    {
        retval += to_utf8_(ch);
    }
    return retval;
}

static std::string convert(const std::string& str, io::TextEncoding encoding)
{
    if (encoding == io::TextEncoding::Utf8)
    {
        return to_utf8(str);    
    }

    throw std::invalid_argument("Unexpected 'encoding' value.");
}

void io::OutputStream::write(const std::string& str_, const io::TextEncoding* pEncoding /*= nullptr*/)
{
    const std::string* pStr = &str_;
    std::string str; // keep any result from convert() in-scope so we use its address
    if (pEncoding != nullptr)
    {
        str = convert(str_, *pEncoding);
        pStr = &str; // stays in-scope, above
    }
     
    auto buffer = reinterpret_cast<const sys::byte*>(pStr->c_str());
    const sys::Size_T len{pStr->length()};
    write(buffer, len);
}
