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
    T result;
    if (six::Enum::toType<T>(result, s, std::nothrow))
    {
        auto retval = nitf::details::value(std::make_optional<T>(result), ex);
        if (retval != T::NOT_SET)
        {
            return retval;
        }
    }
    throw ex;
}
template <typename T>
inline T toType(const std::string& s)
{
    if (is_OTHER_(s))
    {
        T retval = T::OTHER;
        retval.other_ = s; // save away original value for toString()
        return retval;
    }

    const except::Exception ex(Ctxt("Unsupported polarization type '" + s + "'"));
    return toType_<T>(s, ex);
}

template<typename T>
inline std::string toString_(const T& t, const except::Exception& ex)
{
    if (t == T::NOT_SET)
    {
        throw ex;
    }

    const auto result = six::Enum::toString(t, std::nothrow);
    return nitf::details::value(result, ex);
}
template <typename T>
inline std::string toString(const T& t)
{
    // use the OTHER.* string, if we have one
    if ((t == T::OTHER) && is_OTHER_(t.other_))
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

namespace six
{

template <>
PolarizationSequenceType toType<PolarizationSequenceType>(
    const std::string& s)
{
    return ::toType<PolarizationSequenceType>(s);
}
template <>
std::string toString(const PolarizationSequenceType& t)
{
    return ::toString(t);
}

template <>
PolarizationType toType<PolarizationType>(const std::string& s)
{
    return ::toType<PolarizationType>(s);
}
template <>
std::string toString(const PolarizationType& t)
{
    return ::toString(t);
}

template <>
DualPolarizationType toType<DualPolarizationType>(const std::string& s)
{
    const except::Exception ex(Ctxt("Unsupported conversion to dual polarization type '" + s + "'"));
    const auto splits = str::split(s, ":");
    if (splits.size() != 2)
    {
        // Note there is no "top level" OTHER.*; rather it's OTHER.*:OTHER.*
        return toType_<DualPolarizationType>(s, ex); // No ":", assume it is another allowed value.
    }

    // "Allowed values include the form TX:RCV that is formed from one TX value and one RCV value."
    auto&& tx = splits[0];
    auto&& rcv = splits[1];

    // "V:OTHER.*" or "OTHER.*:V" or "OTHER.*:OTHER.*"
    static const PolarizationType other = PolarizationType::OTHER;
    const auto strTx = is_OTHER_(tx) ? six::Enum::toString(other) : tx;  // get rid of .* from OTHER strings
    const auto strRcv = is_OTHER_(rcv) ? six::Enum::toString(other) : rcv;  // get rid of .* from OTHER strings
    const auto type = strTx + "_" + strRcv; // "OTHER_abc:V" -> "OTHER_V"

    auto retval = toType_<DualPolarizationType>(type, ex);
    if (is_OTHER_(tx) || is_OTHER_(rcv))
    {
        retval.other_ = s;  // save away original value for toString()
    }
    return retval;
}
template <>
std::string toString(const DualPolarizationType& t)
{
    // use the OTHER.* string, if we have one
    const auto splits = str::split(t.other_, ":");
    if ((splits.size() == 2) && (is_OTHER_(splits[0]) || is_OTHER_(splits[1])))
    {
        // Looking good ... convert to type to be sure other_ wasn't set
        // to a string representing a different type.  This corresponds to 
        // checking against PolarizationType::OTHER above; but since there
        // a more combinations (OTHER_V, H_OTHER, etc.), this is easier.
        const auto otherType = six::toType<DualPolarizationType>(t.other_);
        if (t == otherType)
        {
            return t.other_;
        }
    }

    const except::Exception ex(Ctxt("Unsupported dual polarization type to string"));
    if (!t.other_.empty())
    {
        // other_ got set to some non-OTHER.* string ... or an OTHER.*
        // string of a different type.  Note there is no "top level" OTHER.*
        throw ex;
    }

    auto retval = toString_(t, ex);
    str::replace(retval, "_", ":"); // "V_V" -> "V:V"
    return retval;
}

}
