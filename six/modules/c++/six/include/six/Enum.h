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

#include <six/sys_Conf.h>
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>

namespace six
{

const int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace details
{
    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum
    {
    protected:
        using map_t = std::map<std::string, int>;
    private:
        using reverse_map_t = std::map<int, std::string>;
        static reverse_map_t to_int_to_string(const map_t& string_to_int)
        {
            reverse_map_t retval;
            for (const auto& p : string_to_int)
            {
                retval[p.second] = p.first;
            }
            return retval;
        }
        static const map_t& string_to_int()
        {
            return T::string_to_int_();
        }
        static const reverse_map_t& int_to_string()
        {
            static reverse_map_t retval = to_int_to_string(string_to_int());
            return retval;
        }
    protected:
        Enum() = default;

        //! string constructor
        Enum(const std::string& s)
        {
            const auto it = string_to_int().find(s);
            if (it != string_to_int().end())
            {
                value = it->second;
            }
            else
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
        }

        //! int constructor
        Enum(int i)
        {
            const auto it = int_to_string().find(i);
            if (it != int_to_string().end())
            {
                value = it->first;
            }
            else
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }

    public:
        //! Returns string representation of the value
        std::string toString(bool throw_if_not_set = false) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }

            const auto it = int_to_string().find(value);
            if (it != int_to_string().end())
            {
                return it->second;
            }

            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }

        static T toType(const std::string& s)
        {
            std::string type(s);
            str::trim(type);
            const auto it = string_to_int().find(type);
            if (it != string_to_int().end())
            {
                return it->second;
            }
            throw except::Exception(Ctxt("Unknown type '" + s + "'"));
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
    #define SIX_Enum_map_entry_NOT_SET { "NOT SET", NOT_SET }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
    #define SIX_Enum_struct_1_(name) struct name final : public six::details::Enum<name> { \
            name() = default; name(const std::string& s) : Enum(s) {} name(int i) : Enum(i) {} \
            name& operator=(const int& o) { value = o; return *this; } enum {
    #define SIX_Enum_struct_2_ NOT_SET = six::NOT_SET_VALUE }; \
            static const map_t& string_to_int_() { \
	      static const map_t retval{ 
    #define SIX_Enum_struct_3_ SIX_Enum_map_entry_NOT_SET }; return retval; } }


    #define SIX_Enum_ENUM_STR_1(name, n, s, v) SIX_Enum_struct_1_(name) \
        n = v, SIX_Enum_struct_2_ \
        {s, n}, SIX_Enum_struct_3_
    #define SIX_Enum_ENUM_1(name, n, v) SIX_Enum_ENUM_STR_1(name, n, #n, v)
    #define SIX_Enum_ENUM_STR_2(name, n1, s1, v1, n2, s2, v2) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, SIX_Enum_struct_2_ \
        {s1, n1}, {s2, n2}, SIX_Enum_struct_3_
    #define SIX_Enum_ENUM_2(name, n1, v1, n2, v2) \
        SIX_Enum_ENUM_STR_2(name, n1, #n1, v1, n2, #n2, v2)
    #define SIX_Enum_ENUM_3(name, n1, v1, n2, v2, n3, v3) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, n3 = v3, SIX_Enum_struct_2_ \
        SIX_Enum_map_entry_(n1), SIX_Enum_map_entry_(n2), SIX_Enum_map_entry_(n3), SIX_Enum_struct_3_
    #define SIX_Enum_ENUM_STR_4(name, n1, s1, v1, n2, s2, v2, n3, s3, v3, n4, s4, v4) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, SIX_Enum_struct_2_ \
        {s1, n1}, {s2, n2}, {s3, n3}, {s4, n4}, SIX_Enum_struct_3_
    #define SIX_Enum_ENUM_4(name, n1, v1, n2, v2, n3, v3, n4, v4) \
        SIX_Enum_ENUM_STR_4(name, n1, #n1, v1, n2, #n2, v2, n3, #n3, v3, n4, #n4, v4)
    #define SIX_Enum_ENUM_5(name, n1, v1, n2, v2, n3, v3, n4, v4, n5, v5) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, n5 = v5, SIX_Enum_struct_2_ \
        SIX_Enum_map_entry_(n1), SIX_Enum_map_entry_(n2), SIX_Enum_map_entry_(n3), SIX_Enum_map_entry_(n4), SIX_Enum_map_entry_(n5), \
        SIX_Enum_struct_3_
} // namespace details
}
#endif // SIX_SIX_Enum_h_INCLUDED_
