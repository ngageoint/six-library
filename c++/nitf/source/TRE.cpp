/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include <string.h>
#include "nitf/TRE.hpp"

using namespace nitf;

TRE::TRE(const TRE & x)
{
    setNative(x.getNative());
}

TRE & TRE::operator=(const TRE & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

TRE::TRE(nitf_TRE * x)
{
    setNative(x);
    getNativeOrThrow();
}

TRE::TRE(NITF_DATA * x)
{
    setNative((nitf_TRE*)x);
    getNativeOrThrow();
}

TRE & TRE::operator=(NITF_DATA * x)
{
    setNative((nitf_TRE*)x);
    getNativeOrThrow();
    return *this;
}

TRE::TRE(const char* tag) throw(nitf::NITFException)
{
	setNative(nitf_TRE_construct(tag, NULL, &error));
	getNativeOrThrow();
	setManaged(false);
}

TRE::TRE(const char* tag, const char* id) throw(nitf::NITFException)
{
	setNative(nitf_TRE_construct(tag, (::strlen(id) > 0) ? id : NULL, &error));
	getNativeOrThrow();
	setManaged(false);
}

TRE::TRE(const std::string& tag) throw(nitf::NITFException)
{
	setNative(nitf_TRE_construct(tag.c_str(), NULL, &error));
	getNativeOrThrow();
	setManaged(false);
}

TRE::TRE(const std::string& tag, const std::string& id)
    throw(nitf::NITFException)
{
    setNative(nitf_TRE_construct(tag.c_str(),
    		                     id.empty() ? NULL : id.c_str(),
    		                     &error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::TRE TRE::clone() throw(nitf::NITFException)
{
    nitf::TRE dolly(nitf_TRE_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

TRE::~TRE(){}

TRE::Iterator TRE::begin()
{
    return TRE::Iterator(nitf_TRE_begin(getNativeOrThrow(), &error));
}

TRE::Iterator TRE::end() throw (nitf::NITFException)
{
    return TRE::Iterator();
}

nitf::Field TRE::getField(const std::string& key)
    throw(except::NoSuchKeyException)
{
    nitf_Field* field = nitf_TRE_getField(getNativeOrThrow(), key.c_str());
    if (!field)
        throw except::NoSuchKeyException(Ctxt(FmtX(
                "Field does not exist in TRE: %s", key.c_str())));
    return nitf::Field(field);
}

nitf::Field TRE::operator[] (const std::string& key)
    throw(except::NoSuchKeyException)
{
    return getField(key);
}

bool TRE::exists(const std::string& key)
{
    return nitf_TRE_exists(getNativeOrThrow(), key.c_str()) == NITF_SUCCESS;
}

size_t TRE::getCurrentSize()
{
    int size = nitf_TRE_getCurrentSize(getNativeOrThrow(), &error);
    if (size < 0)
        throw nitf::NITFException(&error);
    return (size_t)size;
}

std::string TRE::getTag() const
{
    return std::string(getNativeOrThrow()->tag);
}

void TRE::setTag(const std::string & value)
{
    memset(getNativeOrThrow()->tag, 0, 7);
    memcpy(getNativeOrThrow()->tag, value.c_str(), 7);
}

nitf::List TRE::find(const std::string& pattern)
{
    nitf_List* list = nitf_TRE_find(getNative(), pattern.c_str(), &error);
    if (!list)
        throw except::NoSuchKeyException();
    return nitf::List(list);
}

std::string TRE::getID() const
{
    const char* id = nitf_TRE_getID(getNativeOrThrow());
    return id ? std::string(id) : "";
}
