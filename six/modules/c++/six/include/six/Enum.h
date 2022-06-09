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
    // This class is provides a base class for all of our custom enums.  It's intentionally
    // not templated and just deals with `int`s and `std::string`s.
    struct EnumBase
    {
        int value = NOT_SET_VALUE;
        operator int() const { return value; }
        bool less(const EnumBase& rhs) const
        {
            return less_(rhs);
        }

        //! Returns string representation of the value
        std::string toString(bool throw_if_not_set = false) const
        {
            return toString_(throw_if_not_set);
        }
        operator std::string() const { return toString(); }
        bool equals(const std::string& rhs) const // equals(), not less(); order based on ints, not strings
        {
            return equals_(rhs);
        }

        bool default_less_(const EnumBase& rhs) const
        {
            return value < rhs.value;
        }

    protected:
        EnumBase() = default;
        EnumBase(const EnumBase&) = default;
        EnumBase(EnumBase&&) = default;
        EnumBase& operator=(const EnumBase&) = default;
        EnumBase& operator=(EnumBase&&) = default;
        /*virtual*/ ~EnumBase() = default; // don't want "delete pEnumBase_"

        std::string default_toString(bool throw_if_not_set) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }
            return index(value);
        }
        virtual std::string toString_(bool throw_if_not_set) const // for OTHER.* support in SIDD 3.0/SICD 1.3
        {
            return default_toString(throw_if_not_set);
        }

        virtual bool less_(const EnumBase& rhs) const // for OTHER.* support in SIDD 3.0/SICD 1.3
        {
            return default_less_(rhs);
        }

        bool default_equals(const std::string& rhs) const
        {
            return toString() == rhs;
        }
        // equals(), not less(); order based on ints, not strings;
        virtual bool equals_(const std::string& rhs) const // for OTHER.* support in SIDD 3.0/SICD 1.3
        {
            return default_equals(rhs);
        }

        std::string index(int v) const
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %d", v)));
            return nitf::details::index(int_to_string(), v, ex);
        }
        virtual const std::map<std::string, int>& string_to_int() const = 0;

        std::optional<int> toInt(const std::string& v, const except::Exception* pEx) const
        {
            std::string type(v);
            str::trim(type);
            auto&& map = string_to_int();
            if (pEx == nullptr)
            {
                const auto it = map.find(type);
                return it == map.end() ? std::optional<int>() : std::optional<int>(it->second);
            }
            return std::optional<int>(nitf::details::index(map, type, *pEx));
        }

    private:
        int index(const std::string& v) const
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %s", v.c_str())));
            return nitf::details::index(string_to_int(), v, ex);
        }

        //size_t size() const { return int_to_string_.size(); }
        mutable std::map<int, std::string> int_to_string_;
        const std::map<int, std::string>& int_to_string() const
        {
            if (int_to_string_.empty())
            {
                int_to_string_ = nitf::details::swap_key_value(string_to_int());
            }
            return int_to_string_;
        }
    };
    inline bool operator<(const EnumBase& lhs, const EnumBase& rhs)
    {
        return lhs.less(rhs);
    }
    inline bool operator==(const EnumBase& lhs, const std::string& rhs) // for unittests, not SWIG
    {
        return lhs.equals(rhs);
    }
    inline bool operator!=(const EnumBase& lhs, const std::string& rhs) // for unittests, not SWIG
    {
        return !(lhs == rhs);
    }
    inline std::ostream& operator<<(std::ostream& os, const EnumBase& e)
    {
        os << e.toString();
        return os;
    }

    // A wrapper around EnumBase_ to add of "type" support
    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum : public EnumBase
    {
        static const T& instance()
        {
            static const T instance_;
            return instance_;
        }

    public:
        using enum_t = T;
        virtual ~Enum() = default;

        static std::optional<T> toType(const std::string& v, std::nothrow_t)
        {
            return instance().toType_(v, nullptr /*pEx*/);
        }
        static T toType(const std::string& v)
        {
            const except::Exception ex(Ctxt("Unknown type '" + v + "'"));
            const auto result = instance().toType_(v, &ex);
            assert(result.has_value()); // nitf::details::index() should have already thrown, if necessary
            return *result;
        }
        static std::optional<T> default_toType_(const std::string& v, const except::Exception* pEx)
        {
            return instance().default_toType(v, pEx);
        }

        // needed for SWIG
        bool operator<(const int& o) const { return this->value < o; }
        bool operator<(const Enum& o) const { return this->less(o); }
        bool operator>=(const int& o) const { return  this->value >= o; }
        bool operator>=(const Enum& o) const { return !(*this < o); }
        bool operator==(const int& o) const { return  this->value == o; }
        bool operator==(const Enum& o) const { return !(*this < o) && !(o < *this); }
        bool operator!=(const int& o) const { return  this->value != o; }
        bool operator!=(const Enum& o) const { return !(*this == o); }
        bool operator<=(const int& o) const { return  this->value <= o; }
        bool operator<=(const Enum& o) const { return (*this < o) || (*this == o); }
        bool operator>(const int& o) const { return  this->value > o; }
        bool operator>(const Enum& o) const { return !(*this <= o); }

    protected:
        Enum() = default;
        Enum(const Enum&) = default;
        Enum(Enum&&) = default;
        Enum& operator=(const Enum&) = default;
        Enum& operator=(Enum&&) = default;

        std::optional<T> default_toType(const std::string& v, const except::Exception* pEx) const
        {
            auto result = toInt(v, pEx);
            return result.has_value() ? std::optional<T>(*result) : std::optional<T>();
        }
        virtual std::optional<T> toType_(const std::string& v, const except::Exception* pEx) const // for OTHER.* support in SIDD 3.0/SICD 1.3
        {
            return default_toType(v, pEx);
        }
    };

    #define SIX_Enum_map_entry_(n) { #n, n }
    #define SIX_Enum_map_entry_NOT_SET SIX_Enum_map_entry_(NOT_SET), { "NOT SET", NOT_SET }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
   #define SIX_Enum_default_ctor_assign_(name) name() = default; virtual ~name() = default; \
            name(const name&) = default; name(name&&) = default; name& operator=(const name&) = default; name& operator=(name&&) = default; 
    #define SIX_Enum_constructors_(name) SIX_Enum_default_ctor_assign_(name); \
            name(const std::string& s) { *this = std::move(name::toType(s)); } \
            name(int i) { (void)index(i); value = i; } name& operator=(int v) {  *this = name(v); return *this; } 
    #define SIX_Enum_BEGIN_enum enum {
    #define SIX_Enum_BEGIN_DEFINE(name) struct name final : public six::details::Enum<name> { 
    #define SIX_Enum_END_DEFINE(name)  SIX_Enum_constructors_(name); }
    #define SIX_Enum_BEGIN_string_to_int const std::map<std::string, int>& string_to_int() const override { return string_to_int_(); } static const std::map<std::string, int>& string_to_int_() { static const std::map<std::string, int> retval {
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
