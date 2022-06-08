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

    protected:
        EnumBase() = default;
        EnumBase(const EnumBase&) = default;
        EnumBase(EnumBase&&) = default;
        EnumBase& operator=(const EnumBase&) = default;
        EnumBase& operator=(EnumBase&&) = default;
        /*virtual*/ ~EnumBase() = default; // don't want "delete pEnumBase"

        virtual std::string default_toString(bool throw_if_not_set) const = 0;
        virtual std::string toString_(bool throw_if_not_set) const
        {
            return default_toString(throw_if_not_set);
        }

        bool default_less(const EnumBase& rhs) const
        {
            return value < rhs.value;
        }
        virtual bool less_(const EnumBase& rhs) const
        {
            return default_less(rhs);
        }

        bool default_equals(const std::string& rhs) const
        {
            return toString() == rhs;
        }
        virtual bool equals_(const std::string& rhs) const // equals(), not less(); order based on ints, not strings
        {
            return default_equals(rhs);
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

    template<typename T>
    class EnumDetails final
    {
        const std::map<std::string, int>& string_to_int() const
        {
            return T::string_to_int_();
        }
        const std::map<int, std::string>& int_to_string() const
        {
            static const auto retval = nitf::details::swap_key_value(string_to_int());
            return retval;
        }

        std::optional<T> default_toType(const std::string& v, const except::Exception* pEx) const
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

    public:
        //size_t size() const { return int_to_string().size(); }

        int index(const std::string& v) const
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %s", v.c_str())));
            return nitf::details::index(string_to_int(), v, ex);
        }
        std::string index(int v) const
        {
            const except::InvalidFormatException ex(Ctxt(FmtX("Invalid enum value: %d", v)));
            return nitf::details::index(int_to_string(), v, ex);
        }

        std::optional<T> default_toType(const std::string& v, std::nothrow_t) const
        {
            return default_toType(v, nullptr /*pEx*/);
        }
        T default_toType(const std::string& v) const
        {
            const except::Exception ex(Ctxt("Unknown type '" + v + "'"));
            const auto result = default_toType(v, &ex);
            assert(result.has_value()); // nitf::details::index() should have already thrown, if necessary
            return *result;
        }

        std::string default_toString(int value, bool throw_if_not_set) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }
            return index(value);
        }
    };

    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum : public EnumBase
    {
        std::string default_toString(bool throw_if_not_set) const override
        {
            return details().default_toString(value, throw_if_not_set);
        }

    protected:
        static const EnumDetails<T>& details()
        {
            static const EnumDetails<T> details_;
            return details_;
        }

    public:
        using enum_t = T;
        virtual ~Enum() = default;

        static T toType(const std::string& v)
        {
            return details().default_toType(v);
        }
        static std::optional<T> toType(const std::string& v, std::nothrow_t)
        {
            return details().default_toType(v, std::nothrow);
        }

        // needed for SWIG
        bool operator<(const int& o) const { return value < o; }
        bool operator<(const Enum& o) const { return less(o); }
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
    };
    template<typename T>
    inline bool operator==(const Enum<T>& e, const std::string& o) { return e.equals(o); } // for unittests, not SWIG
    template<typename T>
    inline bool operator!=(const Enum<T>& e, const std::string& o) { return !(e == o); } // for unittests, not SWIG


    #define SIX_Enum_map_entry_(n) { #n, n }
    #define SIX_Enum_map_entry_NOT_SET SIX_Enum_map_entry_(NOT_SET), { "NOT SET", NOT_SET }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
    #define SIX_Enum_constructors_(name) name() = default; \
            name(const std::string& s) { *this = std::move(name::toType(s)); } \
            name(int i) { (void)details().index(i); value = i; } \
            name& operator=(int v) {  *this = name(v); return *this; } virtual ~name() = default; \
            name(const name&) = default; name(name&&) = default; name& operator=(const name&) = default; name& operator=(name&&) = default; 
    #define SIX_Enum_BEGIN_enum enum {
    #define SIX_Enum_BEGIN_DEFINE(name) struct name final : public six::details::Enum<name> { 
    /*
    #define SIX_Enum_BEGIN_DEFINE(name) struct name final : public six::details::EnumBase { \
        std::string default_toString(bool throw_if_not_set) const override { return details().default_toString(value, throw_if_not_set); } \
        protected: static const six::details::EnumDetails<name>& details() { static const six::details::EnumDetails<name> details_; return details_;  } \
        public: using enum_t = name; \
        static name toType(const std::string& v) { return details().default_toType(v); } \
        static std::optional<name> toType(const std::string& v, std::nothrow_t) { return details().default_toType(v, std::nothrow); } \
        bool operator<(const int& o) const { return value < o; } \
        bool operator<(const name& o) const { return less(o); } \
        bool operator>=(const int& o) const { return value >= o; } \
        bool operator>=(const name& o) const { return !(*this < o); } \
        bool operator==(const int& o) const { return value == o; } \
        bool operator==(const name& o) const { return !(*this < o) && !(o < *this); } \
        bool operator!=(const int& o) const { return value != o; } \
        bool operator!=(const name& o) const { return !(*this == o); } \
        bool operator<=(const int& o) const { return value <= o; } \
        bool operator<=(const name& o) const { return (*this < o) || (*this == o); } \
        bool operator>(const int& o) const { return value > o; } \
        bool operator>(const name& o) const { return !(*this <= o); }
     */
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
