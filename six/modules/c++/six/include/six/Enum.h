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
#include <tuple> // std::ignore
#include <type_traits>

#include <scene/sys_Conf.h>
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>
#include <nitf/Enum.hpp>

// This is messy file with a bunch of ugly macros ... it's all about making "strongly typed" enums
// (i.e., "enum class" in C++11) and converting to/from strings. But, there are some complicatons ...
//
// First, pre-C++11 infrastructure (i.e., `struct Enum { enum values { one } };`) was already in place
// and "widely used" (well, at least in SWIG bindings), changing it can break things.
// 
// Furthermore, SIDD 3.0 and SICD 1.3 introduced "OTHER*" strings for polarization types which is
// exposed in C++ enums as having different strings for an enum value.  (Yes, other options are possible
// but "OTHER*" strings are more of a one-off; the preference is for strongly-typed enums.)
// This means that even if it were possible to switch everything to "enum class", there would still
// need to be a way to handle "OTHER*"; this is fairly easy to do using a class.
//
// Unsurprisingly, the conversion to/from strings involves a bunch of boilerplate code; this would
// be easier if there weren't a handful of special cases such as wanting spaces in the string:
//    MyEnum::My_Value <-> "My Value"
// 
// And it would be nice to have `struct Enum { ... };` behave like `enum class Enum { ... };`
// as much as possible.
 
 // This is to help developers ensure details::Enum<T> is the same as "enum class"
#define SIX_Enum_as_class_ 1 // using six::details::Enum<T>
//#define SIX_Enum_as_class_ 0 // most enums are "enum class", not six::details::Enum<T>
// There can be up to THREE types in use:
// * T, the type of the "enum"; e.g., six::MyEnum
// * the details::Enum<T> wrapper, sometimes TEnum
// * TValues, the actual enum; e.g., six::MyEnum::values { one, two, three }
// 
// When using "enum class" (SIX_Enum_as_class_=0), TEnum is not used (no wrapper)
// and T and TValues are the same: enum class six::MyEnum { one, two, three }

namespace six
{

constexpr int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace Enum
{
    // see https://en.cppreference.com/w/cpp/utility/to_underlying
    template<typename T, typename TUnderlying, typename TValues = typename T::values>
    inline T from_underlying(TUnderlying v)
    {
        const auto value = static_cast<TValues>(v);
        return T(value); // T() might throw if value is out-of-range
    }
    template<typename T>
    inline T cast(int i)
    {
        return from_underlying<T>(i);
    }
}

namespace details
{
#if CODA_OSS_cpp14
    template<bool B, typename T = void>
    using enable_if_t = std::enable_if_t<B, T>;
#else
    template<bool B, typename T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;
#endif

    // all of these routines take a value->string or string->value map
    template<typename T>
    inline T string_to_value(const std::map<std::string, T>& map, const std::string& v)
    {
        const auto result = nitf::details::index(map, v);
        const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %s", v.c_str())));
        return nitf::details::value(result, ex);
    }

    template<typename T>
    inline std::string value_to_string(const std::map<T, std::string>& map, T v)
    {
        const auto result = nitf::details::index(map, v);
        const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %d", v)));
        return nitf::details::value(result, ex);
    }
    template<typename T>
    inline std::string value_to_string(const std::map<T, std::string>& map, T v, bool throw_if_not_set)
    {
        if (throw_if_not_set && (v == NOT_SET_VALUE))
        {
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", v)));
        }
        return value_to_string(map, v);
    }

    // Having a hard time getting overloading/specialization/etc. to work, so just
    // use different names ... <shrug>
    template<typename TValues, enable_if_t<std::is_enum<TValues>::value, bool> = true>
    inline const std::map<std::string, TValues>& strings_to_values_(const TValues& v)
    {
        return six_Enum_strings_to_values_(v);
    }
    template<typename TValues, enable_if_t<std::is_enum<TValues>::value, bool> = true>
    inline const std::map<TValues, std::string>& values_to_strings_(const TValues& v)
    {
        static const auto retval = nitf::details::swap_key_value(strings_to_values_(v));
        return retval;
    }
} // namespace details

namespace Enum
{
    template<typename TValues, details::enable_if_t<std::is_enum<TValues>::value, bool> = true>
    inline std::optional<std::string> toString(TValues v, std::nothrow_t)
    {
        return nitf::details::index(details::values_to_strings_(v), v);
    }
    template<typename TValues, details::enable_if_t<std::is_enum<TValues>::value, bool> = true>
    inline std::string toString(TValues v, bool throw_if_not_set = false)
    {
        return details::value_to_string(details::values_to_strings_(v), v, throw_if_not_set);
    }
    template<typename TValues, details::enable_if_t<std::is_enum<TValues>::value, bool> = true>
    inline bool toType(TValues& result, const std::string& s, std::nothrow_t)
    {
        const auto value = nitf::details::index(details::strings_to_values_(result), s);
        if (!value.has_value())
        {
            return false;
        }
        result = *value;
        return true;
    }
    template<typename TValues, details::enable_if_t<std::is_enum<TValues>::value, bool> = true>
    inline void toType(TValues& result, const std::string& s)
    {
        result = details::string_to_value(details::strings_to_values_(result), s);
    }
} // namespace Enum


namespace details
{
    // Base type for all "class enums" (NOT C++11's "enum class"); avoids code duplication
    // Want to call these routines from Enum class (below) while overloading on Enum<T>
    template<typename T> class Enum; // forward
    template<typename T, typename TValues = typename T::values>
    inline const std::map<std::string, TValues>& Enum_strings_to_values_(const Enum<T>&)
    {
        return six_Enum_strings_to_values_(T());
    }
    template<typename T, typename TValues = typename T::values>
    inline const std::map<TValues, std::string>& Enum_values_to_strings_(const Enum<T>& e)
    {
        static const auto retval = nitf::details::swap_key_value(Enum_strings_to_values_(e));
        return retval;
    }

    template<typename T>
    class Enum
    {
        template<typename TValues>
        static std::map<std::string, int> to_string_to_int(const std::map<std::string, TValues>& map)
        {
            std::map<std::string, int> retval;
            for (auto&& kv : map)
            {
                retval[kv.first] = kv.second;
            }
            return retval;
        }

        static const std::map<int, std::string>& int_to_string()
        {
            static const auto map = to_string_to_int(details::Enum_strings_to_values_(Enum<T>()));
            static const auto retval = nitf::details::swap_key_value(map);
            return retval;
        }

    protected:
        Enum() = default;

        //! int constructor
        explicit Enum(int i)
        {
            std::ignore = details::value_to_string(int_to_string(), i); // validate "i"
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
            return details::value_to_string(int_to_string(), value, throw_if_not_set);
        }

        static T toType(const std::string& s)
        {
            return details::string_to_value(details::Enum_strings_to_values_(Enum<T>()), s);
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

    // see https ://en.cppreference.com/w/cpp/utility/to_underlying
    template<typename T, typename TValues = typename T::values>
    inline TValues to_underlying(const Enum<T>& e) noexcept
    {
      // assume Enum doesn't allow a bad value to get set
      const auto value = static_cast<int>(e); //  call Enum::operator int() 
      return static_cast<TValues>(value);  // get  the "enum" value
    }

    template<typename T>
    inline bool operator==(const Enum<T>& lhs, typename T::values rhs)
    {
        return to_underlying(lhs) == rhs;
    }
    template<typename T>
    inline bool operator==(const Enum<T>& lhs, const Enum<T>& rhs)
    {
        return lhs == to_underlying(rhs);
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
    inline std::ostream& operator<<(std::ostream& os, const Enum<T>& e)
    {
        os << e.toString();
        return os;
    }
} // namespace details

namespace Enum
{
    template<typename T>
    inline std::optional<std::string> toString(const details::Enum<T>& e, std::nothrow_t)
    {
        return nitf::details::index(details::Enum_values_to_strings_(e), details::to_underlying(e));
    }
    template<typename T>
    inline std::string toString(const details::Enum<T>& e, bool throw_if_not_set = false)
    {
        // This is more strongly-typed than e.toString()
        return details::value_to_string(details::Enum_values_to_strings_(e), details::to_underlying(e), throw_if_not_set);
    }
    template<typename T>
    inline bool toType(details::Enum<T>& result, const std::string& s, std::nothrow_t)
    {
        const auto value = nitf::details::index(details::Enum_strings_to_values_(result), s);
        if (!value.has_value())
        {
            return false;
        }
        result = T(*value); // no details::Enum::operator=(); it's on T
        return true;
    }
    template<typename T>
    inline void toType(details::Enum<T>& result, const std::string& s)
    {
        result = details::Enum<T>::toType(s);
    }
} // namespace Enum


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
    #define SIX_Enum_BEGIN_string_to_value_(name, values) inline const std::map<std::string, values>& six_Enum_strings_to_values_(const name&) { \
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

    #define SIX_Enum_ENUM_1(name, n, v) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n = v,  SIX_Enum_END_enum SIX_Enum_END_DEFINE(name); SIX_Enum_map_1_(name, n)
    #if SIX_Enum_as_class_
    #define SIX_Enum_ENUM_2(name, n1, v1, n2, v2) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_2_(name, n1, n2) 
    #endif // SIX_Enum_as_class_
    #define SIX_Enum_ENUM_3(name, n1, v1, n2, v2, n3, v3) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  n3 = v3, SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_3_(name, n1, n2, n3) 
    #define SIX_Enum_ENUM_4(name, n1, v1, n2, v2, n3, v3, n4, v4) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  n3 = v3, n4 = v4, SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_4_(name, n1, n2, n3, n4) 
    #define SIX_Enum_ENUM_5(name, n1, v1, n2, v2, n3, v3, n4, v4, n5, v5) SIX_Enum_BEGIN_DEFINE(name) \
        SIX_Enum_BEGIN_enum_values_(name) n1 = v1, n2 = v2,  n3 = v3, n4 = v4, n5 = v5, SIX_Enum_END_enum \
        SIX_Enum_END_DEFINE(name); SIX_Enum_map_5_(name, n1, n2, n3, n4, n5) 

    #if !SIX_Enum_as_class_
    #define SIX_six_Enum_map_entry_1_(name, n) SIX_Enum_map_entry_class_(name, n)
    #define SIX_six_Enum_map_entry_2_(name, n1, n2)  SIX_six_Enum_map_entry_1_(name, n1), SIX_six_Enum_map_entry_1_(name, n2)
 
    #define SIX_six_Enum_map_2_(name, n1, n2) SIX_Enum_BEGIN_string_to_value_(name, name)  SIX_six_Enum_map_entry_2_(name, n1, n2), SIX_Enum_END_string_to_value
    #define SIX_Enum_ENUM_2(name, n1, v1, n2, v2) SIX_Enum_BEGIN_enum_class_(name) n1 = v1, n2 = v2,  SIX_Enum_END_enum \
            SIX_six_Enum_map_2_(name, n1, n2) 
    #endif // SIX_Enum_as_class_
}
#endif // SIX_six_Enum_h_INCLUDED_
