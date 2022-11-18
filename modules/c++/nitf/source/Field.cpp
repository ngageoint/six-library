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

#include "nitf/Field.hpp"

nitf::Field::Field(size_t length, FieldType type)
    : Field(nitf_Field_construct(length, type, &error))
{    
}

void nitf::Field::setU_(uint32_t data)
{
    const NITF_BOOL x = nitf_Field_setUint32(getNativeOrThrow(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}
void nitf::Field::set_(int32_t data)
{
    if (!nitf_Field_setInt32(getNativeOrThrow(), data, &error))
        throw nitf::NITFException(&error);
}
void nitf::Field::set_(double data)
{
    if (!nitf_Field_setReal(getNativeOrThrow(), "f", false, data, &error))
        throw nitf::NITFException(&error);
}
void nitf::Field::set(uint64_t data)
{
    const NITF_BOOL x = nitf_Field_setUint64(getNativeOrThrow(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}
void nitf::Field::set(int64_t data)
{
    if (!nitf_Field_setInt64(getNativeOrThrow(), data, &error))
        throw nitf::NITFException(&error);
}
void nitf::Field::set(const char* data)
{
    const NITF_BOOL x = nitf_Field_setString(getNativeOrThrow(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}
void nitf::Field::set(const nitf::DateTime& dateTime, const std::string& format)
{
    const NITF_BOOL x = nitf_Field_setDateTime(getNativeOrThrow(), dateTime.getNative(), format.c_str(), &error);
    if (!x)
        throw nitf::NITFException(&error);
}
void nitf::Field::set(NITF_DATA* inval, size_t length)
{
    const NITF_BOOL x = nitf_Field_setRawData(getNativeOrThrow(), inval, length, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

nitf::DateTime nitf::Field::asDateTime(const std::string& format) const
{
    nitf_DateTime* const dateTime = nitf_Field_asDateTime(getNativeOrThrow(), format.c_str(), &error);
    if (!dateTime)
    {
        throw nitf::NITFException(&error);
    }
    return nitf::DateTime(dateTime);
}

void nitf::Field::get_(NITF_DATA* outval, nitf_ConvType vtype, size_t length) const
{
    nitf_Error e;
    const NITF_BOOL x = nitf_Field_get(getNativeOrThrow(), outval, vtype, length, &e);
    if (!x)
        throw nitf::NITFException(&e);
}

void nitf::Field::resize(size_t length)
{
    nitf_Field* field = getNativeOrThrow();
    const NITF_BOOL resizable = field->resizable;
    field->resizable = 1;

    if (!nitf_Field_resizeField(field, length, &error))
        throw nitf::NITFException(&error);
    field->resizable = resizable;
}