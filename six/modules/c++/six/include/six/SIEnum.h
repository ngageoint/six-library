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
#ifndef SIX_six_SIEnum_h_INCLUDED_
#define SIX_six_SIEnum_h_INCLUDED_
#pragma once

#include <assert.h>

#include <string>
#include <map>
#include <ostream>
#include <type_traits>
#include <new> // std::nothrow
#include <std/optional>

#include <scene/sys_Conf.h>
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>
#include <nitf/Enum.hpp>

namespace six
{

//constexpr int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace details
{
    // Base type for all enums; avoids code duplication
    template<typename T>
    class SIEnum
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

        static int index(const std::string& v)
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %s", v.c_str())));
            return nitf::details::index(string_to_int(), v, ex);
        }
        static std::string index(int v)
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %d", v)));
            return nitf::details::index(int_to_string(), v, ex);
        }
        
        static bool eq_(const SIEnum& e, const std::string& o)
        {
            return default_eq(e, o);
        }
        static bool lt_(const SIEnum& lhs, const SIEnum& rhs)
        {
            return default_lt(lhs, rhs);
        }

        virtual std::string toString_(bool throw_if_not_set) const
        {
            return default_toString(throw_if_not_set);
        }

        static std::optional<T> default_toType(const std::string& v, const except::Exception* pEx)
        {
            std::string type(v);
            str::trim(type);
            auto&& map = string_to_int();
            if (pEx == nullptr)
            {
                const auto it = map.find(type);
                return it == map.end() ? std::optional<T>() : std::optional<T>(it->second);
            }
            return std::optional<T>(nitf::details::index(map, type, *pEx));
        }

    protected:
        SIEnum() = default;
        SIEnum(const SIEnum&) = default;
        SIEnum(SIEnum&&) = default;
        SIEnum& operator=(const SIEnum&) = default;
        SIEnum& operator=(SIEnum&&) = default;
        virtual ~SIEnum() = default;

        //! string constructor
        SIEnum(const std::string& s)
        {
            // Go though T::toType() to account for OTHER.* in SIDD 3.0/SICD 1.3
            *this = std::move(T::toType(s));
        }

        //! int constructor
        SIEnum(int i)
        {
            (void) index(i);
            value = i;
        }

        static std::optional<T> default_toType(const std::string& v, std::nothrow_t)
        {
            return default_toType(v, nullptr /*pEx*/);
        }
        static T default_toType(const std::string& v)
        {
            const except::Exception ex(Ctxt("Unknown type '" + v + "'"));
            const auto result = default_toType(v, &ex);
            assert(result.has_value()); // nitf::details::index() should have already thrown, if necessary
            return *result; 
        }

        std::string default_toString(bool throw_if_not_set) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }
            return index(value);
        }

        static bool default_eq(const SIEnum& e, const std::string& o)
        {
            return e.toString() == o;
        }
        static bool default_lt(const SIEnum& lhs, const SIEnum& rhs)
        {
            return lhs.value < rhs.value;
        }

    public:
        using enum_t = T;

        //! Returns string representation of the value
        std::string toString(bool throw_if_not_set = false) const
        {
            return toString_(throw_if_not_set);
        }

        static T toType(const std::string& v)
        {
            return default_toType(v);
        }
        static std::optional<T> toType(const std::string& v, std::nothrow_t)
        {
            return default_toType(v, std::nothrow);
        }

        operator int() const { return value; }
        operator std::string() const { return toString(); }
        static size_t size() { return int_to_string().size(); }

        // needed for SWIG
        bool operator<(const int& o) const { return value < o; }
        bool operator<(const SIEnum& o) const { return lt(o); }
        bool operator>=(const int& o) const { return value >= o; }
        bool operator>=(const SIEnum& o) const { return !(*this < o); }
        bool operator==(const int& o) const { return value == o; }
        bool operator==(const SIEnum& o) const { return !(*this < o) && !(o < *this); }
        bool operator!=(const int& o) const { return value != o; }
        bool operator!=(const SIEnum& o) const { return !(*this == o); }
        bool operator<=(const int& o) const { return value <= o; }
        bool operator<=(const SIEnum& o) const { return (*this < o) || (*this == o); }
        bool operator>(const int& o) const { return value > o; }
        bool operator>(const SIEnum& o) const { return !(*this <= o); }

        bool eq(const std::string& o) const { return T::eq_(*this, o); }
        bool lt(const SIEnum& o) const { return T::lt_(*this, o); }

        int value = NOT_SET_VALUE;
    };
    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const SIEnum<T>& e)
    {
        os << e.toString();
        return os;
    }
    template<typename T>
    inline bool operator==(const SIEnum<T>& e, const std::string& o) { return e.eq(o); } // for unittests, not SWIG
    template<typename T>
    inline bool operator!=(const SIEnum<T>& e, const std::string& o) { return !(e == o); } // for unittests, not SWIG


    #define SIX_SIEnum_map_entry_(n) { #n, n }
    #define SIX_SIEnum_map_entry_NOT_SET SIX_SIEnum_map_entry_(NOT_SET), { "NOT SET", NOT_SET }

    // Generate an enum class derived from details::SIEnum
    // There are a few examples of expanded code below.
    #define SIX_SIEnum_constructors_(name) name() = default; name(const std::string& s) : SIEnum(s) {} name(int i) : SIEnum(i) {} \
            name& operator=(int v) {  *this = name(v); return *this; } ~name() = default; \
            name(const name&) = default; name(name&&) = default; name& operator=(const name&) = default; name& operator=(name&&) = default; 
    #define SIX_SIEnum_BEGIN_enum enum {
    #define SIX_SIEnum_BEGIN_DEFINE(name) struct name final : public six::details::SIEnum<name> { 
    #define SIX_SIEnum_END_DEFINE(name)  SIX_SIEnum_constructors_(name); }
    #define SIX_SIEnum_BEGIN_string_to_int static const std::map<std::string, int>& string_to_int_() { static const std::map<std::string, int> retval {
    #define SIX_SIEnum_END_enum NOT_SET = six::NOT_SET_VALUE };
    #define SIX_SIEnum_END_string_to_int SIX_SIEnum_map_entry_NOT_SET }; return retval; }

    #define SIX_SIEnum_ENUM_begin_(name) SIX_SIEnum_BEGIN_DEFINE(name) SIX_SIEnum_constructors_(name); SIX_SIEnum_BEGIN_enum
    #define SIX_SIEnum_ENUM_1_ SIX_SIEnum_END_enum SIX_SIEnum_BEGIN_string_to_int
    #define SIX_SIEnum_ENUM_end_ SIX_SIEnum_END_string_to_int }

    #define SIX_SIEnum_map_entry_1_(n) SIX_SIEnum_map_entry_(n)
    #define SIX_SIEnum_map_entry_2_(n1, n2)  SIX_SIEnum_map_entry_(n1), SIX_SIEnum_map_entry_1_(n2)
    #define SIX_SIEnum_map_entry_3_(n1, n2, n3)  SIX_SIEnum_map_entry_(n1), SIX_SIEnum_map_entry_2_(n2, n3)
    #define SIX_SIEnum_map_entry_4_(n1, n2, n3, n4)  SIX_SIEnum_map_entry_(n1), SIX_SIEnum_map_entry_3_(n2, n3, n4)
    #define SIX_SIEnum_map_entry_5_(n1, n2, n3, n4, n5)  SIX_SIEnum_map_entry_(n1), SIX_SIEnum_map_entry_4_(n2, n3, n4, n5)

    #define SIX_SIEnum_ENUM_(map_entry) SIX_SIEnum_ENUM_1_ map_entry, SIX_SIEnum_ENUM_end_
    #define SIX_SIEnum_ENUM_1(name, n, v) SIX_SIEnum_ENUM_begin_(name) \
        n = v, SIX_SIEnum_ENUM_(SIX_SIEnum_map_entry_1_(n))
    #define SIX_SIEnum_ENUM_2(name, n1, v1, n2, v2) SIX_SIEnum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, SIX_SIEnum_ENUM_(SIX_SIEnum_map_entry_2_(n1, n2))
    #define SIX_SIEnum_ENUM_3(name, n1, v1, n2, v2, n3, v3) SIX_SIEnum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, n3 = v3, SIX_SIEnum_ENUM_(SIX_SIEnum_map_entry_3_(n1, n2, n3))
    #define SIX_SIEnum_ENUM_4(name, n1, v1, n2, v2, n3, v3, n4, v4) SIX_SIEnum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, SIX_SIEnum_ENUM_(SIX_SIEnum_map_entry_4_(n1, n2, n3, n4))
    #define SIX_SIEnum_ENUM_5(name, n1, v1, n2, v2, n3, v3, n4, v4, n5, v5) SIX_SIEnum_ENUM_begin_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, n5 = v5, SIX_SIEnum_ENUM_(SIX_SIEnum_map_entry_5_(n1, n2, n3, n4, n5))
} // namespace details
}
#endif // SIX_six_SIEnum_h_INCLUDED_
