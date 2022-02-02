/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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
#include <xml/lite/ValidatorInterface.h>

#include <algorithm>
#include <iterator>
#include <std/filesystem>
#include <std/memory>

#include <sys/OS.h>
#include <io/StringStream.h>
#include <mem/ScopedArray.h>
#include <str/EncodedStringView.h>

namespace fs = std::filesystem;

#include <xml/lite/xml_lite_config.h>

template<typename TStringStream>
bool vallidate_(const xml::lite::ValidatorInterface& validator,
                io::InputStream& xml, TStringStream&& oss, 
                const std::string& xmlID, std::vector<xml::lite::ValidationInfo>& errors)
{
    xml.streamTo(oss);
    return validator.validate(oss.stream().str(), xmlID, errors);
}
bool xml::lite::ValidatorInterface::validate(
        io::InputStream& xml, StringEncoding encoding,
        const std::string& xmlID,
        std::vector<ValidationInfo>& errors) const
{
    // convert to the correcrt std::basic_string<T> based on "encoding"
    if (encoding == StringEncoding::Utf8)
    {
        return vallidate_(*this, xml, io::U8StringStream(), xmlID, errors);
    }
    if (encoding == StringEncoding::Windows1252)
    {
        return vallidate_(*this, xml, io::W1252StringStream(), xmlID, errors);
    }
    
    // this really shouldn't happen
    return validate(xml, xmlID, errors);
}
