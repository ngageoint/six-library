/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <map>
#include <functional>
#include <stdexcept>
#include <cstddef>
#include <type_traits>

#include "nitf/TextSubheader.hpp"

using namespace nitf;

TextSubheader::TextSubheader(const TextSubheader & x)
{
    *this = x;
}

TextSubheader & TextSubheader::operator=(const TextSubheader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

TextSubheader::TextSubheader(nitf_TextSubheader * x)
{
    setNative(x);
    getNativeOrThrow();
}

TextSubheader::TextSubheader() noexcept(false) : TextSubheader(nitf_TextSubheader_construct(&error))
{
    setManaged(false);
}

nitf::TextSubheader TextSubheader::clone() const
{
    nitf::TextSubheader dolly(nitf_TextSubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

//nitf::Field TextSubheader::get(nitf_TextSubheaderFields_enum v) const
//{
//    using get_t = nitf::Field(TextSubheader::*)() const;
//    static const std::map<nitf_TextSubheaderFields_enum, get_t> getFieldMap = {
//        { filePartType, &TextSubheader::getFilePartType }
//    };
//
//    auto it = getFieldMap.find(v);
//    if (it != getFieldMap.end())
//    {
//        auto&& p_mf = it->second;
//        return (this->*p_mf)();
//    }
//
//    throw std::invalid_argument("member-function not in map.");
//}

//nitf::Field TextSubheader::get(nitf_TextSubheaderFields_enum v) const
//{
//    static const std::map<nitf_TextSubheaderFields_enum, size_t> offsetMap = {
//        { filePartType, offsetof(nitf_TextSubheader, filePartType) }
//    };
//
//    auto it = offsetMap.find(v);
//    if (it != offsetMap.end())
//    {
//        const auto fieldOffset = it->second;
//        return Field::fromNativeOffset(getNativeOrThrow(), fieldOffset);
//    }
//
//    throw std::invalid_argument("member-function not in map.");
//}

//#define getField_(name) nitf::Field(getNativeOrThrow()->name)
//#define getField_(name) getField(*this, offsetof(native_t, name));

nitf::Field TextSubheader::getFilePartType() const
{
    //return getField(*this, offsetof(native_t, filePartType));
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field TextSubheader::getTextID() const
{
    return nitf::Field(getNativeOrThrow()->textID);
}

nitf::Field TextSubheader::getAttachmentLevel() const
{
    return nitf::Field(getNativeOrThrow()->attachmentLevel);
}

nitf::Field TextSubheader::getDateTime() const
{
    return nitf::Field(getNativeOrThrow()->dateTime);
}

nitf::Field TextSubheader::getTitle() const
{
    return nitf::Field(getNativeOrThrow()->title);
}

nitf::Field TextSubheader::getSecurityClass() const
{
    return nitf::Field(getNativeOrThrow()->securityClass);
}

nitf::FileSecurity TextSubheader::getSecurityGroup() const
{
    return nitf::FileSecurity(getNativeOrThrow()->securityGroup);
}

void TextSubheader::setSecurityGroup(nitf::FileSecurity value)
{
    //release the owned security group
    nitf::FileSecurity fs = nitf::FileSecurity(getNativeOrThrow()->securityGroup);
    fs.setManaged(false);

    //have the library manage the new securitygroup
    getNativeOrThrow()->securityGroup = value.getNative();
    value.setManaged(true);
}

nitf::Field TextSubheader::getEncrypted() const
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field TextSubheader::getFormat() const
{
    return nitf::Field(getNativeOrThrow()->format);
}

nitf::Field TextSubheader::getExtendedHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field TextSubheader::getExtendedHeaderOverflow() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions TextSubheader::getExtendedSection() const
{
    return nitf::Extensions(getNativeOrThrow()->extendedSection);
}

void TextSubheader::setExtendedSection(nitf::Extensions value)
{
    if (getNativeOrThrow()->extendedSection)
    {
        //release the one currently "owned", if different
        nitf::Extensions exts = nitf::Extensions(getNativeOrThrow()->extendedSection);
        if (exts != value)
            exts.setManaged(false);
    }

    //have the library manage the "new" one
    getNativeOrThrow()->extendedSection = value.getNative();
    value.setManaged(true);
}
