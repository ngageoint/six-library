/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * str-c++ is free software; you can redistribute it and/or modify
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

#ifndef __STR_CONVERT_H__
#define __STR_CONVERT_H__

#include <string>
#include <complex>
#include <limits>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <typeinfo>
#include <iostream>
#include <import/except.h>

namespace str
{

template<typename T> int getPrecision(const T& type);

template<typename T> std::string toString(const T& value)
{
    std::ostringstream buf;
    buf.precision(str::getPrecision(value));
    buf << std::boolalpha << value;
    return buf.str();
}

template<typename T> std::string toString(const std::complex<T>& value)
{
    std::ostringstream buf;

    buf.precision(str::getPrecision(value.real()));
    buf << value;
    return buf.str();
}

template<typename T> std::string toString(const T& real, const T& imag)
{
    std::complex<T> cpx(real, imag);
    std::ostringstream buf;

    buf.precision(str::getPrecision(real));
    buf << cpx;
    return buf.str();
}

template<typename T> T toType(const std::string& s)
{
    if (s.empty())
        throw except::BadCastException(except::Context(__FILE__, __LINE__,
            std::string(""), std::string(""), std::string("Empty string")));

    T value;

    std::stringstream buf(s);
    buf.precision(str::getPrecision(value));
    buf >> value;

    if (buf.fail())
    {
        throw except::BadCastException(except::Context(__FILE__, __LINE__,
            std::string(""), std::string(""),
            std::string("Conversion failed: '")
                + s + std::string("' -> ") + typeid(T).name()));
    }

    return value;
}

template<> bool toType<bool> (const std::string& s);
template<> std::string toType<std::string> (const std::string& s);

/**
 *  Determine the precision required for the data type.
 *
 *  @param type A variable of the type whose precision argument is desired.
 *  @return The integer argument required by ios::precision() to represent
 *  this type.
 */
template<typename T> int getPrecision(const T& type)
{
    return 0;
}

template<> int getPrecision(const float& type);
template<> int getPrecision(const double& type);
template<> int getPrecision(const long double& type);

/** Generic casting routine; used by explicitly overloaded
 conversion operators.

 @param value A variable of the type being cast to.
 @return The internal representation of GenericType, converted
 to the desired type, if possible.
 @throw BadCastException thrown if cast cannot be performed.
 */
template<typename T>
T generic_cast(const std::string& value) throw (except::BadCastException)
{
    return str::toType<T>(value);
}

}

#endif
