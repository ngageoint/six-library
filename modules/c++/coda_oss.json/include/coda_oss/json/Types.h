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
 *
 */

#pragma once
#ifndef CODA_OSS_json_types_h_INCLUDED_
#define CODA_OSS_json_types_h_INCLUDED_

#include <nlohmann/json.hpp>
#include <import/types.h>
#include <types/Complex.h>

// Json definitions for the 'types' module.
namespace types
{
    template<typename T, typename BasicJsonType>
    void to_json(BasicJsonType& j, const Complex<T>& z)
    {
        j["real"] = z.real();
        j["imag"] = z.imag();
    }
    template<typename T, typename BasicJsonType>
    void from_json(const BasicJsonType& j, Complex<T>& z)
    {
        z.real(j["real"].template get<T>());
        z.imag(j["imag"].template get<T>());
    }

    template<typename T, typename BasicJsonType>
    void to_json(BasicJsonType& j, const PageRowCol<T>& prc)
    {
        j["page"] = prc.page;
        j["row"] = prc.row;
        j["col"] = prc.col;
    }
    template<typename T, typename BasicJsonType>
    void from_json(const BasicJsonType& j, PageRowCol<T>& prc)
    {
        prc.page = j["page"].template get<T>();
        prc.row = j["row"].template get<T>();
        prc.col = j["col"].template get<T>();
    }

    template<typename BasicJsonType>
    void to_json(BasicJsonType& j, const RangeList& list)
    {
        j = list.getRanges();
    }
    template<typename BasicJsonType>
    void from_json(const BasicJsonType& j, RangeList& list)
    {  
        list.insert(j.template get<std::vector<Range>>());
    }

    template<typename T, typename BasicJsonType>
    void to_json(BasicJsonType& j, const RgAz<T>& ra)
    {
        j["rg"] = ra.rg;
        j["az"] = ra.az;
    }
    template<typename T, typename BasicJsonType>
    void from_json(const BasicJsonType& j, RgAz<T>& ra)
    {
        ra.rg = j["rg"].template get<T>();
        ra.az = j["az"].template get<T>();
    }

    template<typename T, typename BasicJsonType>
    void to_json(BasicJsonType& j, const RowCol<T>& rc)
    {
        j["row"] = rc.row;
        j["col"] = rc.col;
    }
    template<typename T, typename BasicJsonType>
    void from_json(const BasicJsonType& j, RowCol<T>& rc)
    {
        rc.row = j["row"].template get<T>();
        rc.col = j["col"].template get<T>();
    }

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Range, mStartElement, mNumElements)
} // namespace types


#endif