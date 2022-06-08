/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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
#ifndef SIX_six_Enum_h_INCLUDED_
#define SIX_six_Enum_h_INCLUDED_
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

constexpr int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace details
{
    struct EnumBase
    {
        int value = NOT_SET_VALUE;
        operator int() const { return value; }
    };

    template<typename T>
    class EnumDetails final
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
    public:
        static size_t size() { return int_to_string().size(); }

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
    };

    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum : public EnumBase
    {
        static bool eq_(const Enum& e, const std::string& o)
        {
            return default_eq(e, o);
        }
        static bool lt_(const Enum& lhs, const Enum& rhs)
        {
            return default_lt(lhs, rhs);
        }

        virtual std::string toString_(bool throw_if_not_set) const
        {
            return default_toString(throw_if_not_set);
        }

    protected:
        Enum() = default;
        Enum(const Enum&) = default;
        Enum(Enum&&) = default;
        Enum& operator=(const Enum&) = default;
        Enum& operator=(Enum&&) = default;
        virtual ~Enum() = default;

        //! string constructor
        Enum(const std::string& s)
        {
            // Go though T::toType() to account for OTHER.* in SIDD 3.0/SICD 1.3
            *this = std::move(T::toType(s));
        }

        //! int constructor
        Enum(int i)
        {
            (void)EnumDetails<T>::index(i);
            value = i;
        }

        static std::optional<T> default_toType(const std::string& v, std::nothrow_t)
        {
            return EnumDetails<T>::default_toType(v, std::nothrow);
        }
        static T default_toType(const std::string& v)
        {
            return EnumDetails<T>::default_toType(v);
        }

        std::string default_toString(bool throw_if_not_set) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }
            return EnumDetails<T>::index(value);
        }

        static bool default_eq(const Enum& e, const std::string& o)
        {
            return e.toString() == o;
        }
        static bool default_lt(const Enum& lhs, const Enum& rhs)
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

        operator std::string() const { return toString(); }
        static size_t size() { return EnumDetails<T>::size(); }

        // needed for SWIG
        bool operator<(const int& o) const { return value < o; }
        bool operator<(const Enum& o) const { return lt(o); }
        bool operator>=(const int& o) const { return value >= o; }
        bool operator>=(const Enum& o) const { return !(*this < o); }
        bool operator==(const int& o) const { return value == o; }
        bool operator==(const Enum& o) const { return !(*this < o) && !(o < *this); }
        bool operator!=(const int& o) const { return value != o; }
        bool operator!=(const Enum& o) const { return !(*this == o); }
        bool operator<=(const int& o) const { return value <= o; }
        bool operator<=(const Enum& o) const { return (*this < o) || (*this == o); }
        bool operator>(const int& o) const { return value > o; }
        bool operator>(const Enum& o) const { return !(*this <= o); }

        bool eq(const std::string& o) const { return T::eq_(*this, o); }
        bool lt(const Enum& o) const { return T::lt_(*this, o); }
    };
    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const Enum<T>& e)
    {
        os << e.toString();
        return os;
    }
    template<typename T>
    inline bool operator==(const Enum<T>& e, const std::string& o) { return e.eq(o); } // for unittests, not SWIG
    template<typename T>
    inline bool operator!=(const Enum<T>& e, const std::string& o) { return !(e == o); } // for unittests, not SWIG


    #define SIX_Enum_map_entry_(n) { #n, n }
    #define SIX_Enum_map_entry_NOT_SET SIX_Enum_map_entry_(NOT_SET), { "NOT SET", NOT_SET }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
    #define SIX_Enum_constructors_(name) name() = default; name(const std::string& s) : Enum(s) {} name(int i) : Enum(i) {} \
            name& operator=(int v) {  *this = name(v); return *this; } ~name() = default; \
            name(const name&) = default; name(name&&) = default; name& operator=(const name&) = default; name& operator=(name&&) = default; 
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
#endif // SIX_six_Enum_h_INCLUDED_
