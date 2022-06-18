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
#include "six/Enums.h"

#include <std/optional>

#include "six/Utilities.h"

static bool is_OTHER_(const std::string& v)
{
    // OTHER.* for  SIDD 3.0/SICD 1.3, not "OTHER"
    if (str::starts_with(v, "OTHER") && (v != "OTHER")) // i.e., "OTHER_foo"
    {
        // "where * = 0 or more characters that does not contain “:” (0x3A)."
        return v.find(':') == std::string::npos; // "OTHER:foo" is invalid
    }
    return false; // "OTHER" or "<something else>"
}

template<typename T>
inline T toType_(const std::string& s, const except::Exception& ex)
{
    const auto result = T::toType(s, std::nothrow);
    auto retval = nitf::details::value(result, ex);
    if (retval == T::NOT_SET)
    {
        throw ex;
    }
    return retval;
}

template<typename T>
inline std::string toString_(const T& t, const except::Exception& ex)
{
    if (t == T::NOT_SET)
    {
        throw ex;
    }

    const auto result = t.toString(std::nothrow);
    return nitf::details::value(result, ex);
}

namespace six
{
    template <>
    six::PolarizationSequenceType six::toType<six::PolarizationSequenceType>(
        const std::string& s)
    {
        if (is_OTHER_(s))
        {
            PolarizationSequenceType retval = PolarizationSequenceType::OTHER;
            retval.other_ = s;
            return retval;
        }

        const except::Exception ex(Ctxt("Unsupported polarization type '" + s + "'"));
        return toType_<PolarizationSequenceType>(s, ex);
    }
    template <>
    std::string six::toString(const PolarizationSequenceType& t)
    {
        // use the OTHER.* string, if we have one
        if ((t == PolarizationSequenceType::OTHER) && is_OTHER_(t.other_))
        {
            return t.other_;
        }

        const except::Exception ex(Ctxt("Unsupported conversion from polarization type"));
        if (!t.other_.empty())
        {
            // other_ got set to some non-OTHER.* string
            throw ex;
        }
        return toString_(t, ex);
    }

    template <>
    PolarizationType six::toType<PolarizationType>(const std::string& s)
    {
        if (is_OTHER_(s))
        {
            PolarizationType retval = PolarizationType::OTHER;
            retval.other_ = s;
            return retval;
        }

        const except::Exception ex(Ctxt("Unsupported polarization type '" + s + "'"));
        return toType_<PolarizationType>(s, ex);
    }
    template <>
    std::string six::toString(const PolarizationType& t)
    {
        // use the OTHER.* string, if we have one
        if ((t == PolarizationType::OTHER) && is_OTHER_(t.other_))
        {
            return t.other_;
        }

        const except::Exception ex(Ctxt("Unsupported conversion from polarization type"));
        if (!t.other_.empty())
        {
            // other_ got set to some non-OTHER.* string
            throw ex;
        }
        return toString_(t, ex);
    }

    template <>
    DualPolarizationType six::toType<DualPolarizationType>(const std::string& s)
    {
        // Note there is no "top level" OTHER.*; rather it's OTHER.*:OTHER.*

        const except::Exception ex(Ctxt("Unsupported conversion to dual polarization type '" + s + "'"));

        std::string type(s);
        str::replace(type, ":", "_"); // "V:V" -> "V_V"
        return toType_<DualPolarizationType>(type, ex);
    }
    template <>
    std::string six::toString(const DualPolarizationType& t)
    {
        const except::Exception ex(Ctxt("Unsupported dual polarization type to string"));
        auto retval = toString_(t, ex);
        str::replace(retval, "_", ":"); // "V_V" -> "V:V"
        return retval;
    }
}

#if 0
// See https://stackoverflow.com/questions/13358672/how-to-convert-a-lambda-to-an-stdfunction-using-templates for
// some interesting reading regarding std::function<> and lambdas.
template<typename T>
inline std::optional<T> toType_imp(const std::string& v, const except::Exception* pEx, std::function<void(T&)> set_other)
{
    // Handle OTHER.* for  SIDD 3.0/SICD 1.3
    if (is_OTHER_(v)) // handle "OTHER" with default_toType_()
    {
        T result = T::OTHER;
        set_other(result); // know "v" is a valid OTHER.* 
        return result;
    }

    const auto result = T::default_toType_(v, pEx); // let default_toType_() throw the exception for "OTHER:foo"
    if ((pEx == nullptr) && !result.has_value())
    {
        return std::optional<T>();
    }
    return result;
}

template<typename TFunc>
inline std::string toString_imp(const std::string& other, TFunc default_toString)
{
    // Handle OTHER.* for  SIDD 3.0/SICD 1.3
    if (is_OTHER_(other))
    {
        return other;
    }
    if (!other.empty())
    {
        // other_ got set to something other than an OTHER string
        except::InvalidFormatException(Ctxt("Invalid enum value: " + other));
    }
    return default_toString();
}

template<typename T, typename TFunc>
bool eq_imp(const T& e, const std::string& o, TFunc default_eq)
{
    using enum_t = typename T::enum_t;
    if (is_OTHER_(o))
    {
        return e == enum_t::OTHER;
    }
    if (e == enum_t::OTHER)
    {
        static const auto strOther = enum_t(enum_t::OTHER).toString();
        return is_OTHER_(o) || (o == strOther);
    }

    return default_eq();
}

template<typename T>
bool less_imp(const T& lhs, const six::details::EnumBase& rhs)
{
    // handle OTHER.* for SIDD 3.0/SICD 1.3
    const auto strLhs = lhs.toString();
    const auto strRhs = rhs.toString();
    if (is_OTHER_(strLhs) && is_OTHER_(strRhs))
    {
        // order OTHER.* < OTHER.* based on the strings
        return strLhs < strRhs;
    }

    // no OTHER.*, use the default
    return lhs.default_less_(rhs);
}

std::optional<six::PolarizationType> six::PolarizationType::toType_(const std::string& v, const except::Exception* pEx) const
{
    // Need something more than C++11 to avoid mentioning the type twice; in C++14, the lambda could be "auto"
    return toType_imp<PolarizationType>(v, pEx, [&](PolarizationType& t) { t.other_ = v; });
}
std::string six::PolarizationType::toString_(bool throw_if_not_set) const
{
    return toString_imp(other_, [&]() { return default_toString(throw_if_not_set); });
}
bool six::PolarizationType::equals_(const std::string& rhs) const
{
    return eq_imp(*this, rhs, [&]() { return this->default_equals(rhs); });
}
bool six::PolarizationType::less_(const EnumBase& rhs) const
{
    return less_imp(*this, rhs);
}

std::optional<six::PolarizationSequenceType> six::PolarizationSequenceType::toType_(const std::string& v, const except::Exception* pEx) const
{
    // Need something more than C++11 to avoid mentioning the type twice; in C++14, the lambda could be "auto"
    return toType_imp<PolarizationSequenceType>(v, pEx, [&](PolarizationSequenceType& t) { t.other_ = v; });
}
std::string six::PolarizationSequenceType::toString_(bool throw_if_not_set) const
{
    return toString_imp(other_, [&]() { return default_toString(throw_if_not_set); });
}
bool six::PolarizationSequenceType::equals_(const std::string& rhs) const
{
    return eq_imp(*this, rhs, [&]() { return this->default_equals(rhs); });
}
bool six::PolarizationSequenceType::less_(const EnumBase& rhs) const
{
    return less_imp(*this, rhs);
}

std::optional<six::DualPolarizationType> six::DualPolarizationType::toType_(const std::string& v, const except::Exception* pEx) const
{
    const auto splits = str::split(v, ":");
    if (splits.size() != 2)
    {
        // It's not possible to determine whether a string like "OTHER_V" should be DualPolarizationType::OTHER (OTHER.*)
        // or DualPolarizationType::OTHER_V; try the "old way" (pre SIDD 3.0/SICD 1.3) first.  Note this is really only a 
        // problem for the default enums, in the XML ":" instead of "_" is the seperator.
        auto result = DualPolarizationType::default_toType_(v, nullptr /*pEx*/);
        if (result.has_value())
        {
            return *result;
        }

        // Need something more than C++11 to avoid mentioning the type twice; in C++14, the lambda could be "auto"
        return toType_imp<DualPolarizationType>(v, pEx, [&](DualPolarizationType& t) { t.other_ = v; });
    }

    // Handle OTHER.* for  SIDD 3.0/SICD 1.3
    // The "dual" type is really two `PolarizationType`s next to each other
    const auto left = PolarizationType::toType(splits[0], std::nothrow);
    if (left.has_value())
    {
        const auto right = PolarizationType::toType(splits[1], std::nothrow);
        if (right.has_value())
        {
            static const PolarizationType other = PolarizationType::OTHER;
            const auto strLeft = *left == PolarizationType::OTHER ? other.toString() : left->toString();
            const auto strRight = *right == PolarizationType::OTHER ? other.toString() : right->toString();

            // Get the right enum value set
            const auto str = strLeft + "_" + strRight; // can't do "A:B" in C++, so the enum/string is A_B
            auto result = DualPolarizationType::default_toType_(str, pEx);
            if (result.has_value())
            {
                // save away the exact strings for OTHER.*
                result->left_ =*left;
                result->right_ = *right;
                return *result;
            }
        }
    }

    // Need something more than C++11 to avoid mentioning the type twice; in C++14, the lambda could be "auto"
    return toType_imp<DualPolarizationType>(v, pEx, [&](DualPolarizationType& t) { t.other_ = v; });
}
std::string six::DualPolarizationType::toString_(bool throw_if_not_set) const
{
    if ((left_ != PolarizationType::NOT_SET) && (right_ != PolarizationType::NOT_SET))
    {
        // Handle OTHER.* for  SIDD 3.0/SICD 1.3
        const auto strLeft = left_.toString(throw_if_not_set);
        const auto strRight = right_.toString(throw_if_not_set);
        if (is_OTHER_(strLeft) || is_OTHER_(strRight))
        {
            return strLeft + ":" + strRight; // use ":" not "_" so the string can be split apart
        }

        // Using "_" instead of ":" matches pre- SIDD 3.0/SICD 1.3 behavior; yes, it makes "OTHER_V" ambiguous.
        return strLeft + "_" + strRight; // retval could interpreted as OTHER.* or OTHER:V.
    }

    if (other_.empty() || is_OTHER_(other_))  // Handle OTHER.* for  SIDD 3.0/SICD 1.3
    {
        return toString_imp(other_, [&]() { return default_toString(throw_if_not_set); });
    }

    if ((left_ == PolarizationType::NOT_SET) && (right_ == PolarizationType::NOT_SET))
    {
        default_toString(throw_if_not_set);
    }

    throw except::InvalidFormatException(Ctxt("Invalid enum value: " + other_));
}

bool six::DualPolarizationType::equals_(const std::string& rhs) const
{
    const auto& e = *this;
    const auto& o = rhs;

    if ((e.left_ == PolarizationType::OTHER) || (e.right_ == PolarizationType::OTHER))
    {
        // For OTHER.*_V or V_OTHER.* (i.e. "OTHER_abc:V" or "V:OTHER_xyz"), split
        // the string to compare the parts.
        const auto splits = str::split(o, ":");
        if (splits.size() == 2) // str is, e.g., "OTHER_abc:V"
        {
            // Can split the string, try to compare using PolarizationType
            const auto left = PolarizationType::toType(splits[0], std::nothrow);
            if (left.has_value())
            {
                const auto right = PolarizationType::toType(splits[1], std::nothrow);
                if (right.has_value())
                {
                    return (e.left_ == left) && (e.right_ == right);
                }
            }
        }
    }

    // If we can make a DualPolarizationType from the string; try that.
    const auto o_type = DualPolarizationType::toType(o, std::nothrow);
    if (o_type.has_value())
    {
        return e == *o_type;
    }

    // special processing needed only when comparing OTHERs
    return eq_imp(e, o, [&]() { return e.default_equals(o); });
}
bool six::DualPolarizationType::less_(const EnumBase& rhs) const
{
    return less_imp(*this, rhs);
}
#endif