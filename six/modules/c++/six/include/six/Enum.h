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
#include <new>

#include <scene/sys_Conf.h>
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>
#include <nitf/Enum.hpp>

 // This is to help developers ensure details::Enum<T> is the same as "enum class"
#define SIX_Enum_as_class_ 1 // using six::details::Enum<T>
// There can be up to THREE types in use:
// * T, the type of the "enum"; e.g., six::MyEnum
// * TEnum, the details::Enum<T> wrapper
// * TValues, the actual enum; e.g., six::MyEnum::values { one, two, three }
// 
// When using "enum class" (SIX_Enum_as_class_=0), TEnum is not used (no wrapper)
// and T and TValues are the same: enum class six::MyEnum { one, two, three }

namespace six
{

constexpr int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace details
{
    template<typename TEnum, typename TValues = typename TEnum::values>
    inline TValues cast(int i)
    {
        return static_cast<TValues>(i);
    }
}

namespace Enum
{
    template<typename TEnum, typename TValues = typename TEnum::values>
    inline TEnum cast(int i)
    {
        return TEnum(details::cast<TEnum, TValues>(i));
    }
}

namespace details
{
    template<typename T>
    inline std::map<std::string, int> to_string_to_int(const std::map<std::string, T>& map)
    {
        std::map<std::string, int> retval;
        for (auto&& kv : map)
        {
            retval[kv.first] = kv.second;
        }
        return retval;
    }

    template<typename T>
    inline T index(const std::map<std::string, T>& map, const std::string& v)
    {
        const auto result = nitf::details::index(map, v);
        const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %s", v.c_str())));
        return nitf::details::value(result, ex);
    }
    template<typename T>
    inline std::string index(const std::map<T, std::string>& map, T v)
    {
        const auto result = nitf::details::index(map, v);
        const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %d", v)));
        return nitf::details::value(result, ex);
    }

    template<typename TEnum, typename TValues = typename TEnum::values>
    const std::map<std::string, TValues>& string_to_value()
    {
        return six_Enum_string_to_value_(TEnum());
    }
} // namespace details

namespace Enum
{
    template<typename T>
    inline std::optional<T> toType(const std::string& v, std::nothrow_t)
    {
        const auto result = nitf::details::index(details::string_to_value<T>(), v);
        if (!result.has_value())
        {
            return std::optional<T>();
        }
        return std::optional<T>(*result);
    }
    template<typename T>
    inline T toType(const std::string& v)
    {
        const auto result = toType<T>(v, std::nothrow);
        const except::Exception ex(Ctxt("Unknown type '" + v + "'"));
        return nitf::details::value(result, ex);
    }

    //! Returns string representation of the value
    template<typename TEnum, typename TValues = typename TEnum::values>
    inline std::optional<std::string> toString_(TValues value, std::nothrow_t)
    {
        static const auto value_to_string = nitf::details::swap_key_value(details::string_to_value<TEnum, TValues>());
        return nitf::details::index(value_to_string, value);
    }
    template<typename TEnum, typename TValues = typename TEnum::values>
    inline std::string toString_(TValues value, bool throw_if_not_set = false)
    {
        if (throw_if_not_set && (value == NOT_SET_VALUE))
        {
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
        static const auto value_to_string = nitf::details::swap_key_value(details::string_to_value<TEnum, TValues>());
        return details::index(value_to_string, value);
    }

    template<typename TEnum>
    inline std::optional<std::string> toString(const TEnum& value_, std::nothrow_t)
    {
        const auto value = details::cast<TEnum>(value_); // get  the "enum" value
        return toString_<TEnum>(value, std::nothrow);
    }
    template<typename TEnum>
    inline std::string toString(const TEnum& value_, bool throw_if_not_set = false)
    {
        const auto value = details::cast<TEnum>(value_); // get  the "enum" value
        return toString_<TEnum>(value, throw_if_not_set);
    }
} // namespace Enum

namespace details
{
    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum
    {
        static const std::map<int, std::string>& int_to_string()
        {
            static const auto string_to_int = details::to_string_to_int(details::string_to_value<T>());
            static const auto retval = nitf::details::swap_key_value(string_to_int);
            return retval;
        }

    protected:
        Enum() = default;

        //! int constructor
        explicit Enum(int i)
        {
            (void)details::index(int_to_string(), i); // validate "i"
            value = i;
        }

#ifdef SWIGPYTHON
    public:
#endif
        int value = NOT_SET_VALUE; // existing SWIG code uses "value", regenerating is a huge nusiance

    public:
        void set_value_(int v) { value = v; } // for unit-testing
        operator int() const { return value; }

        std::string toString(bool throw_if_not_set = false) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }
            return details::index(int_to_string(), value);
        }

        static T toType(const std::string& v)
        {
            return six::Enum::toType<T>(v);
        }

        #ifdef SWIGPYTHON
        static size_t size() { return int_to_string().size(); }
        bool operator<(const int& o) const { return value < o; }
        bool operator<(const Enum& o) const { return *this < o.value; }
        bool operator==(const int& o) const { return value == o; }
        bool operator==(const Enum& o) const { return *this == o.value; }
        bool operator!=(const int& o) const { return !(*this == o); }
        bool operator!=(const Enum& o) const { return !(*this == o); }
        bool operator<=(const int& o) const { return (*this < o) || (*this == o); }
        bool operator<=(const Enum& o) const { return (*this < o) || (*this == o); }
        bool operator>(const int& o) const { return !(*this <= o); }
        bool operator>(const Enum& o) const { return !(*this <= o); }
        bool operator>=(const int& o) const { return !(*this < o); }
        bool operator>=(const Enum& o) const { return !(*this < o); }
	    #endif // SWIGPYTHON
    };
    template<typename T>
    inline bool operator==(const Enum<T>& lhs, typename T::values rhs)
    {
        return details::cast<T>(lhs) == rhs;
    }
    template<typename T>
    inline bool operator==(const Enum<T>& lhs, const Enum<T>& rhs)
    {
        return lhs == details::cast<T>(rhs);
    }
    template<typename T>
    inline bool operator!=(const Enum<T>& lhs, typename T::values rhs)
    {
        return !(lhs == rhs);
    }
    template<typename T>
    inline bool operator!=(const Enum<T>& lhs, const Enum<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename T>
    inline std::optional<std::string> toString(const Enum<T>& e_, std::nothrow_t)
    {
        const T e(details::cast<T>(e_)); // get rid of wrapper class: Enum<T> -> T
        return six::Enum::toString(e, std::nothrow);
    }
    template<typename T>
    inline std::string toString(const Enum<T>& e_, bool throw_if_not_set = false)
    {
        const T e(details::cast<T>(e_)); // get rid of wrapper class: Enum<T> -> T
        return six::Enum::toString(e, throw_if_not_set);
    }

    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const Enum<T>& e)
    {
        os << toString(e);
        return os;
    }

} // namespace details

    #define SIX_Enum_map_entry_values_(name, n) { #n, n }
    #define SIX_Enum_map_entry_class_(name, n) { #n, name::n }
    #define SIX_Enum_map_entry_(name, n) SIX_Enum_map_entry_class_(name, n)
    #define SIX_Enum_map_entry(n) SIX_Enum_map_entry_(SIX_Enum_map_entry_name, n)
    #define SIX_Enum_map_entry_NOT_SET_(name) SIX_Enum_map_entry_(name, NOT_SET), { "NOT SET", name::NOT_SET }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
    #define SIX_Enum_default_ctor_assign_(name) name() = default; name(const name&) = default; name(name&&) = default; \
            name& operator=(const name&) = default; name& operator=(name&&) = default
    #ifdef SWIGPYTHON
       // Normal C++ code shouldn't use these constructors; use toType() or cast() instead
       #define SIX_Enum_constructors_SWIGPYTHON_(name) explicit name(int i) : name(static_cast<values>(i)) { } \
           explicit name(const std::string& s) { *this = std::move(toType(s)); }
    #else
       #define SIX_Enum_constructors_SWIGPYTHON_(name)
    #endif
    #define SIX_Enum_constructors_(name) SIX_Enum_default_ctor_assign_(name); SIX_Enum_constructors_SWIGPYTHON_(name); \
        name(values v) : Enum(static_cast<int>(v)) {} name& operator=(values v) { *this = std::move(name(v)); return *this; }
    #define SIX_Enum_BEGIN_enum_values_(name) enum values {
    #define SIX_Enum_BEGIN_enum_class_(name) enum class name {
    #define SIX_Enum_BEGIN_enum(name) SIX_Enum_BEGIN_enum_values_(name)
    #define SIX_Enum_END_enum NOT_SET = six::NOT_SET_VALUE };
    #define SIX_Enum_BEGIN_DEFINE(name) struct name final : public six::details::Enum<name> { 
    #define SIX_Enum_END_DEFINE(name)  SIX_Enum_constructors_(name); }
    #define SIX_Enum_BEGIN_string_to_value_(name, values) inline const std::map<std::string, values>& six_Enum_string_to_value_(const name&) { \
        static const std::map<std::string, values> retval { SIX_Enum_map_entry_NOT_SET_(name), 
    #define SIX_Enum_BEGIN_string_to_value(name) SIX_Enum_BEGIN_string_to_value_(name, name::values)
    #define SIX_Enum_END_string_to_value }; return retval; }

    #define SIX_Enum_map_entry_1_(name, n) SIX_Enum_map_entry_(name, n)
    #define SIX_Enum_map_entry_2_(name, n1, n2)  SIX_Enum_map_entry_(name, n1), SIX_Enum_map_entry_1_(name, n2)
    #define SIX_Enum_map_entry_3_(name, n1, n2, n3)  SIX_Enum_map_entry_(name, n1), SIX_Enum_map_entry_2_(name, n2, n3)
    #define SIX_Enum_map_entry_4_(name, n1, n2, n3, n4)  SIX_Enum_map_entry_(name, n1), SIX_Enum_map_entry_3_(name, n2, n3, n4)
    #define SIX_Enum_map_entry_5_(name, n1, n2, n3, n4, n5)  SIX_Enum_map_entry_(name, n1), SIX_Enum_map_entry_4_(name, n2, n3, n4, n5)

    #define SIX_Enum_map_1_(name, n) SIX_Enum_BEGIN_string_to_value(name)  SIX_Enum_map_entry_1_(name, n), SIX_Enum_END_string_to_value
    #define SIX_Enum_map_2_(name, n1, n2) SIX_Enum_BEGIN_string_to_value(name)  SIX_Enum_map_entry_2_(name, n1, n2), SIX_Enum_END_string_to_value
    #define SIX_Enum_map_3_(name, n1, n2, n3) SIX_Enum_BEGIN_string_to_value(name)  SIX_Enum_map_entry_3_(name, n1, n2, n3), SIX_Enum_END_string_to_value
    #define SIX_Enum_map_4_(name, n1, n2, n3, n4) SIX_Enum_BEGIN_string_to_value(name)  SIX_Enum_map_entry_4_(name, n1, n2, n3, n4), SIX_Enum_END_string_to_value
    #define SIX_Enum_map_5_(name, n1, n2, n3, n4, n5) SIX_Enum_BEGIN_string_to_value(name)  SIX_Enum_map_entry_5_(name, n1, n2, n3, n4, n5), SIX_Enum_END_string_to_value

    #if SIX_Enum_as_class_
    #define SIX_Enum_ENUM_1(name, n, v) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n = v,  SIX_Enum_END_enum SIX_Enum_END_DEFINE(name); SIX_Enum_map_1_(name, n)
    #define SIX_Enum_ENUM_2(name, n1, v1, n2, v2) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_2_(name, n1, n2) 
    #define SIX_Enum_ENUM_3(name, n1, v1, n2, v2, n3, v3) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  n3 = v3, SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_3_(name, n1, n2, n3) 
    #define SIX_Enum_ENUM_4(name, n1, v1, n2, v2, n3, v3, n4, v4) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  n3 = v3, n4 = v4, SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_4_(name, n1, n2, n3, n4) 
    #define SIX_Enum_ENUM_5(name, n1, v1, n2, v2, n3, v3, n4, v4, n5, v5) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  n3 = v3, n4 = v4, n5 = v5, SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_5_(name, n1, n2, n3, n4, n5) 

    #else
    #define SIX_six_Enum_map_entry_1_(name, n) SIX_Enum_map_entry_class_(name, n)
    #define SIX_six_Enum_map_entry_2_(name, n1, n2)  SIX_six_Enum_map_entry_1_(name, n1), SIX_six_Enum_map_entry_1_(name, n2)
 
    #define SIX_six_Enum_map_2_(name, n1, n2) SIX_Enum_BEGIN_string_to_value_(name, name)  SIX_six_Enum_map_entry_2_(name, n1, n2), SIX_Enum_END_string_to_value
    #define SIX_Enum_ENUM_2(name, n1, v1, n2, v2) SIX_Enum_BEGIN_enum_class_(name) n1 = v1, n2 = v2,  SIX_Enum_END_enum \
            SIX_six_Enum_map_2_(name, n1, n2) 

    #endif // SIX_Enum_as_class_
}
#endif // SIX_six_Enum_h_INCLUDED_
