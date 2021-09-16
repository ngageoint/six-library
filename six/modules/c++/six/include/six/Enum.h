/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef SIX_SIX_Enum_h_INCLUDED_
#define SIX_SIX_Enum_h_INCLUDED_
#pragma once

#include <string>
#include <map>
#include <ostream>

#include <scene/sys_Conf.h>
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>
#include <nitf/Enum.hpp>

namespace six
{

constexpr int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace details
{
    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum
    {
        static const std::map<std::string, int>& string_to_int()
        {
            return T::string_to_int_();
        }
        static const std::map<int, std::string>& int_to_string()
        {
            static const auto retval = nitf::details::swap_key_value(string_to_int());
            return retval;
        }

        template<typename TKey, typename TValue>
        static TValue index(const std::map<TKey, TValue>& map, const TKey& k, const except::Exception& ex)
        {
            const auto it = map.find(k);
            if (it == map.end())
            {
                throw ex;
            }
            return it->second;
        }
        static int index(const std::string& v)
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %s", v.c_str())));
            return index(string_to_int(), v, ex);
        }
        static std::string index(int v)
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %d", v)));
            return index(int_to_string(), v, ex);
        }

    protected:
        Enum() = default;

        //! string constructor
        Enum(const std::string& s)
        {
            value = index(s);
        }

        //! int constructor
        Enum(int i)
        {
            (void) index(i);
            value = i;
        }

    public:
        //! Returns string representation of the value
        std::string toString(bool throw_if_not_set = false) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }
            return index(value);
        }

        static T toType(const std::string& v)
        {
            std::string type(v);
            str::trim(type);
            const except::Exception ex(Ctxt("Unknown type '" + v + "'"));
            return index(string_to_int(), type, ex);
        }

        operator int() const { return value; }
        operator std::string() const { return toString(); }
        static size_t size() { return int_to_string().size(); }

        // needed for SWIG
        bool operator==(const int& o) const { return value == o; }
        bool operator==(const Enum& o) const { return value == o.value; }
        bool operator==(const std::string& o) const { return toString() == o; } // for unittests, not SWIG
        bool operator!=(const int& o) const { return value != o; }
        bool operator!=(const Enum& o) const { return value != o.value; }
        bool operator!=(const std::string& o) const { return !(*this == o); } // for unittests, not SWIG
        bool operator<(const int& o) const { return value < o; }
        bool operator<(const Enum& o) const { return value < o.value; }
        bool operator<=(const int& o) const { return value <= o; }
        bool operator<=(const Enum& o) const { return value <= o.value; }
        bool operator>(const int& o) const { return value > o; }
        bool operator>(const Enum& o) const { return value > o.value; }
        bool operator>=(const int& o) const { return value >= o; }
        bool operator>=(const Enum& o) const { return value >= o.value; }

        int value = NOT_SET_VALUE;
    };
    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const Enum<T>& e)
    {
        os << e.toString();
        return os;
    }


    #define SIX_Enum_map_entry_(n) { #n, n }
    #define SIX_Enum_map_entry_NOT_SET SIX_Enum_map_entry_(NOT_SET), { "NOT SET", NOT_SET }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
    #define SIX_Enum_constructors_(name) name() = default; name(const std::string& s) : Enum(s) {} name(int i) : Enum(i) {} \
            name& operator=(int v) {  *this = name(v); return *this; }
    #define SIX_Enum_BEGIN_enum enum {
    #define SIX_Enum_BEGIN_DEFINE(name) struct name final : public six::details::Enum<name> { 
    #define SIX_Enum_END_DEFINE(name)  SIX_Enum_constructors_(name); }
    #define SIX_Enum_BEGIN_string_to_int static const std::map<std::string, int>& string_to_int_() { static const std::map<std::string, int> retval {
    #define SIX_Enum_END_enum NOT_SET = six::NOT_SET_VALUE };
    #define SIX_Enum_END_string_to_int SIX_Enum_map_entry_NOT_SET }; return retval; }

    #define SIX_Enum_ENUM_begin_(name) SIX_Enum_BEGIN_DEFINE(name) SIX_Enum_constructors_(name); SIX_Enum_BEGIN_enum
    #define SIX_Enum_ENUM_1_ SIX_Enum_END_enum SIX_Enum_BEGIN_string_to_int
    #define SIX_Enum_ENUM_end_ SIX_Enum_END_string_to_int }

    #define SIX_Enum_map_entry_1_(n) SIX_Enum_map_entry_(n)
    #define SIX_Enum_map_entry_2_(n1, n2)  SIX_Enum_map_entry_(n1), SIX_Enum_map_entry_1_(n2)
    #define SIX_Enum_map_entry_3_(n1, n2, n3)  SIX_Enum_map_entry_(n1), SIX_Enum_map_entry_2_(n2, n3)
    #define SIX_Enum_map_entry_4_(n1, n2, n3, n4)  SIX_Enum_map_entry_(n1), SIX_Enum_map_entry_3_(n2, n3, n4)
    #define SIX_Enum_map_entry_5_(n1, n2, n3, n4, n5)  SIX_Enum_map_entry_(n1), SIX_Enum_map_entry_4_(n2, n3, n4, n5)

    #define SIX_Enum_ENUM_(map_entry) SIX_Enum_ENUM_1_ map_entry, SIX_Enum_ENUM_end_
    #define SIX_Enum_ENUM_1(name, n, v) SIX_Enum_ENUM_begin_(name) \
        n = v, SIX_Enum_ENUM_(SIX_Enum_map_entry_1_(n))
    #define SIX_Enum_ENUM_2(name, n1, v1, n2, v2) SIX_Enum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, SIX_Enum_ENUM_(SIX_Enum_map_entry_2_(n1, n2))
    #define SIX_Enum_ENUM_3(name, n1, v1, n2, v2, n3, v3) SIX_Enum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, n3 = v3, SIX_Enum_ENUM_(SIX_Enum_map_entry_3_(n1, n2, n3))
    #define SIX_Enum_ENUM_4(name, n1, v1, n2, v2, n3, v3, n4, v4) SIX_Enum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, SIX_Enum_ENUM_(SIX_Enum_map_entry_4_(n1, n2, n3, n4))
    #define SIX_Enum_ENUM_5(name, n1, v1, n2, v2, n3, v3, n4, v4, n5, v5) SIX_Enum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, n5 = v5, SIX_Enum_ENUM_(SIX_Enum_map_entry_5_(n1, n2, n3, n4, n5))
} // namespace details
}
#endif // SIX_SIX_Enum_h_INCLUDED_
