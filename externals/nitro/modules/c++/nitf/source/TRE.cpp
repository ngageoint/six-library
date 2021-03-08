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

#include "nitf/TRE.hpp"
#include <string.h>
#include "nitf/TREUtils.h"

#include "gsl/gsl.h"

using namespace nitf;

TRE::TRE(const TRE& x)
{
    *this = x;
}

TRE& TRE::operator=(const TRE& x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

TRE::TRE(nitf_TRE* x)
{
    setNative(x);
    getNativeOrThrow();
}

TRE::TRE(NITF_DATA* x) : TRE(static_cast<nitf_TRE*>(x))
{
}

TRE& TRE::operator=(NITF_DATA* x)
{
    setNative(static_cast<nitf_TRE*>(x));
    getNativeOrThrow();
    return *this;
}

TRE::TRE(const char* tag) : TRE(nitf_TRE_construct(tag, nullptr, &error))
{
    setManaged(false);
}

TRE::TRE(const char* tag, const char* id)
    : TRE(nitf_TRE_construct(tag, (::strlen(id) > 0) ? id : nullptr, & error))
{
    setManaged(false);
}

TRE::TRE(const std::string& tag) : TRE(tag.c_str())
{
}

TRE::TRE(const std::string& tag, const std::string& id)
    : TRE(nitf_TRE_construct(tag.c_str(), id.empty() ? nullptr : id.c_str(), & error))
{
    setManaged(false);
}

nitf::TRE TRE::clone() const
{
    nitf::TRE dolly(nitf_TRE_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

TRE::~TRE()
{
}

TRE::Iterator TRE::begin() const
{
    nitf_TREEnumerator* iter = nitf_TRE_begin(getNativeOrThrow(), &error);
    if (!iter)
        throw nitf::NITFException(Ctxt("Invalid TRE: " + getTag()));
    return TRE::Iterator(iter);
}

TRE::Iterator TRE::end() const noexcept
{
    return TRE::Iterator();
}

nitf::Field TRE::getField(const std::string& key) const
{
    nitf_Field* field = nitf_TRE_getField(getNativeOrThrow(), key.c_str());
    if (!field)
        throw except::NoSuchKeyException(
                Ctxt(FmtX("Field does not exist in TRE: %s", key.c_str())));
    return nitf::Field(field);
}

void TRE::updateFields()
{
    if (!nitf_TREUtils_fillData(
                getNative(),
                ((nitf_TREPrivateData*)getNative()->priv)->description,
                &error))
    {
        throw NITFException(&error);
    }
}

nitf::Field TRE::operator[](const std::string& key)
{
    return getField(key);
}

bool TRE::exists(const std::string& key) const
{
    return nitf_TRE_exists(getNativeOrThrow(), key.c_str()) == NITF_SUCCESS;
}

size_t TRE::getCurrentSize() const
{
    const int size = nitf_TRE_getCurrentSize(getNativeOrThrow(), &error);
    if (size < 0)
        throw nitf::NITFException(&error);
    return gsl::narrow<size_t>(size >= 0 ? size : 0);
}

std::string TRE::getTag() const
{
    return getNativeOrThrow()->tag;
}

void TRE::setTag(const std::string& value)
{
    memset(getNativeOrThrow()->tag, 0, 7);
    memcpy(getNativeOrThrow()->tag, value.c_str(), 7);
}

nitf::List TRE::find(const std::string& pattern) const
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

static bool endsWith(const std::string& s, const std::string& match) noexcept
{
    const size_t mLen = match.length();
    const size_t sLen = s.length();
    for (size_t i = 0; i < sLen && i < mLen; ++i)
        if (!(s[sLen - i - 1] == match[mLen - i - 1]))
            return false;
    return sLen >= mLen;
}

std::string TRE::truncate(const std::string& value, size_t maxDigits) const
{
    const size_t decimalIndex = value.find('.');
    if (decimalIndex == std::string::npos)
    {
        return value;
    }

    const std::string truncated = value.substr(0, maxDigits);
    if (endsWith(truncated, "."))
    {
        return truncated.substr(0, truncated.size() - 1);
    }
    if (truncated.size() > decimalIndex)
    {
        return truncated;
    }
    return value;
}
