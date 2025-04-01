/* =========================================================================
 * This file is part of coda-oss.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
 */

#pragma once
#ifndef CODA_OSS_json_mem_h_INCLUDED_
#define CODA_OSS_json_mem_h_INCLUDED_

#include <mem/ScopedPtr.h>

namespace mem
{
    template<typename T, typename TCopyIsClone, typename BasicJsonType>
    void to_json(BasicJsonType& j, const ScopedPtr<T, TCopyIsClone>& p)
    {
        // If 'p' is not `nullptr`, serialize a type T
        if (p) j = *p;
    }
    template<typename T, typename TCopyIsClone, typename BasicJsonType>
    void from_json(const BasicJsonType& j, ScopedPtr<T, TCopyIsClone>& p)
    {
        // If the json is `null` reset with a`nullptr`
        // otherwise, deserialize a 'T'
        p.reset(j.is_null() ? nullptr : new T(j.template get<T>()));
    }
} // namespace mem

#endif