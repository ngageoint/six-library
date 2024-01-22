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

#include "nitf/Extensions.hpp"

using namespace nitf;

bool ExtensionsIterator::equals(const nitf::ExtensionsIterator& it2) const noexcept
{
    auto& it2_ = const_cast<nitf::ExtensionsIterator&>(it2);
    const NITF_BOOL x = nitf_ExtensionsIterator_equals(&handle, &it2_.getHandle());
    if (!x) return false;
    return true;
}

bool ExtensionsIterator::notEqualTo(const nitf::ExtensionsIterator& it2) const noexcept
{
    auto& it2_ = const_cast<nitf::ExtensionsIterator&>(it2);
    const NITF_BOOL x = nitf_ExtensionsIterator_notEqualTo(&handle, &it2_.getHandle());
    if (!x) return false;
    return true;
}

void ExtensionsIterator::increment() noexcept
{
    nitf_ExtensionsIterator_increment(&handle);
}

/*!
 *  Get the TRE from the iterator
 */
nitf::TRE ExtensionsIterator::get() const
{
    nitf_TRE* x = nitf_ExtensionsIterator_get(&handle);
    return nitf::TRE(x);
}

Extensions::Iterator Extensions::begin() const noexcept
{
    const nitf_ExtensionsIterator x = nitf_Extensions_begin(getNative());
    return nitf::ExtensionsIterator(x);
}

Extensions::Iterator Extensions::end() const noexcept
{
    const nitf_ExtensionsIterator x = nitf_Extensions_end(getNative());
    return nitf::ExtensionsIterator(x);
}