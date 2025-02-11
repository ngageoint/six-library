/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
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

#ifndef NITF_Enum_hpp_INCLUDED_
#define NITF_Enum_hpp_INCLUDED_
#pragma once

#include <string>
#include <map>
#include <stdexcept>
#include <ostream>
#include <std/optional>
#include <new> // std::nothrow

#include "str/Manip.h"
#include "str/EncodedStringView.h"

namespace nitf
{
    namespace details
    {
        template<typename TKey, typename TValue>
        inline std::map<TValue, TKey> swap_key_value(const std::map<TKey, TValue>& map)
        {
            std::map<TValue, TKey> retval;
            for (const auto& p : map)
            {
                retval[p.second] = p.first;
            }
            return retval;
        }

        template<typename TKey, typename TValue>
        inline std::optional<TValue> index(const std::map<TKey, TValue>& map, const TKey& key)
        {
            const auto it = map.find(key);
            return it == map.end() ? std::optional<TValue>() : std::optional<TValue>(it->second);
        }

        template<typename T>
        inline std::optional<std::string> to_string(T v, const std::map<T, std::string>& enum_to_string)
        {
            return index(enum_to_string, v);
        }

        template<typename T>
        inline std::optional<T> from_string(std::string v, const std::map<std::string, T>& string_to_enum)
        {
            str::trim(v);
            return index(string_to_enum, v);
        }

        template<typename T, typename TException>
        inline T value(const std::optional<T>& v, const TException& ex)
        {
            if (!v.has_value())
            {
                throw ex;
            }
            return *v;
        }
        template<typename T>
        inline T value(const std::optional<T>& v)
        {
            return value(v, std::invalid_argument("key not found in map."));
        }
    }

#define NITF_ENUM_define_enum_(name, ...) enum class name { __VA_ARGS__ }

#define NITF_ENUM_map_entry(name, n) { #n, name::n }
#define NITF_ENUM_map_entry_2(name, n1, n2) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry(name, n2)
#define NITF_ENUM_map_entry_3(name, n1, n2, n3) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_2(name, n2, n3)
#define NITF_ENUM_map_entry_4(name, n1, n2, n3, n4)  NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_3(name, n2, n3, n4)
#define NITF_ENUM_map_entry_5(name, n1, n2, n3, n4, n5)  NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_4(name, n2, n3, n4, n5)
#define NITF_ENUM_map_entry_6(name, n1, n2, n3, n4, n5, n6)  NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_5(name, n2, n3, n4, n5, n6)
#define NITF_ENUM_map_entry_7(name, n1, n2, n3, n4, n5, n6, n7) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_6(name, n2, n3, n4, n5, n6, n7)
#define NITF_ENUM_map_entry_8(name, n1, n2, n3, n4, n5, n6, n7, n8) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_7(name, n2, n3, n4, n5, n6, n7, n8)
#define NITF_ENUM_map_entry_9(name, n1, n2, n3, n4, n5, n6, n7, n8, n9) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_8(name, n2, n3, n4, n5, n6, n7, n8, n9)
#define NITF_ENUM_map_entry_10(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_9(name, n2, n3, n4, n5, n6, n7, n8, n9, n10)
#define NITF_ENUM_map_entry_11(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_10(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11)
#define NITF_ENUM_map_entry_12(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_11(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12)
#define NITF_ENUM_map_entry_13(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_12(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13)
#define NITF_ENUM_map_entry_14(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_13(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14)
#define NITF_ENUM_map_entry_15(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15) NITF_ENUM_map_entry(name, n1), NITF_ENUM_map_entry_14(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15)

#define NITF_ENUM_define_string_to_enum_ostream_(name) inline std::ostream& operator<<(std::ostream& os, name e) { os << to_string(e); return os; }
#define NITF_ENUM_define_string_to_enum_begin(name)  NITF_ENUM_define_string_to_enum_ostream_(name) \
   inline const std::map<std::string, name>& string_to_enum(name) { static const std::map<std::string, name> retval {
#define NITF_ENUM_define_string_to_end }; return retval; }
#define NITF_ENUM_define_string_to_enum_(name, ...) NITF_ENUM_define_string_to_enum_begin(name) __VA_ARGS__  \
    NITF_ENUM_define_string_to_end

#define NITF_ENUM(n, name, ...) NITF_ENUM_define_enum_(name, __VA_ARGS__); \
        NITF_ENUM_define_string_to_enum_(name, NITF_ENUM_map_entry_##n(name, __VA_ARGS__))

    template<typename T>
    inline std::optional<std::string> to_string(T v, const std::map<std::string, T>& string_to_enum, std::nothrow_t)
    {
        static const auto enum_to_string = details::swap_key_value(string_to_enum);
        return details::to_string(v, enum_to_string);
    }
    template<typename T>
    inline std::string to_string(T v, const std::map<std::string, T>& string_to_enum)
    {
        const auto result = to_string(v, string_to_enum, std::nothrow);
        return details::value(result);
    }

    template<typename T>
    inline std::optional<T> from_string(const std::string& v, const std::map<std::string, T>& string_to_enum, std::nothrow_t)
    {
        return details::from_string(v, string_to_enum);
    }
    template<typename T>
    inline T from_string(const std::string& v, const std::map<std::string, T>& string_to_enum)
    {
        const auto result = from_string(v, string_to_enum, std::nothrow);
        return details::value(result);
    }

    template<typename T>
    inline std::optional<std::string> to_string(T v, std::nothrow_t)
    {
        return to_string(v, string_to_enum(T()), std::nothrow);
    }
    template<typename T>
    inline std::string to_string(T v)
    {
        return to_string(v, string_to_enum(T()));
    }

    template<typename T>
    inline std::optional<T> from_string(const std::string& v, std::nothrow_t)
    {
        return from_string<T>(v, string_to_enum(T()), std::nothrow);
    }
    template<typename T>
    inline T from_string(const std::string& v)
    {
        return from_string<T>(v, string_to_enum(T()));
    }

}
#endif // NITF_Enum_hpp_INCLUDED_
