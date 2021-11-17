/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
#include "nitf/Handle.hpp"

#include <std/memory>

class nitf::Handle::Impl
{
    std::mutex mutex;
    int refCount{ 0 };

public:
    Impl() = default;
    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = delete;

    //! Get the ref count
    int getRef() const noexcept
    {
        return refCount;
    }

    //! Increment the ref count
    int incRef()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (refCount < 0) refCount = 0;
        refCount++;
        return refCount;
    }

    //! Decrement the ref count
    int decRef()
    {
        std::lock_guard<std::mutex> lock(mutex);
        refCount--;
        if (refCount < 0) refCount = 0;
        return refCount;
    }
};

nitf::Handle::Handle() noexcept(false) : mPimpl(std::make_unique<Impl>())
{
}
nitf::Handle::~Handle() noexcept(false) { }
nitf::Handle::Handle(Handle&&) noexcept = default;
nitf::Handle& nitf::Handle::operator=(Handle&&) noexcept = default;

int nitf::Handle::getRef() const noexcept
{
    return mPimpl->getRef();
}
int nitf::Handle::incRef()
{
    return mPimpl->incRef();
}
int nitf::Handle::decRef()
{
    return mPimpl->decRef();
}
