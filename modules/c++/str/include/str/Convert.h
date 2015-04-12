/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "except/Exception.h"
#include <typeinfo>

namespace str
{

template <typename T> int setPrecision(const T& type);

template <typename T> std::string toString(const T& value)
{
    std::ostringstream buf;

    buf.precision(str::setPrecision(value));
    buf << std::boolalpha << value;
    return buf.str();
}

template <typename T> std::string toString(const std::complex<T>& value)
{
    std::ostringstream buf;

    buf.precision(str::setPrecision(value.real()));
    buf << value;
    return buf.str();
}

template <typename T> std::string toString(const T& real,
        const T& imag)
{
    std::complex<T> cpx(real, imag);
    std::ostringstream buf;

    buf.precision(str::setPrecision(real));
    buf << cpx;
    return buf.str();
}

template <typename T> T toType(const std::string& s)
{
    if (s.empty())
        return T();

    std::istringstream buf(s);
    T value;
    buf >> std::boolalpha >> value;

    if (buf.fail())
    {
        throw
        except::BadCastException(std::string("Conversion failed: '") +
                                 buf.str() + std::string("' -> ") +
                                 typeid(T).name());
    }

    return value;
}

template <> std::string toType<std::string>(const std::string& s);
/**
 *  Determine the precision required for the data type.
 *
 *  @param type A variable of the type whose precision argument is desired.
 *  @return The integer argument required by ios::precision() to represent
 *  this type.
 */
template <typename T> int setPrecision(const T& type)
{
    int precision(0);

    if (typeid(type) == typeid(float))
        precision = std::numeric_limits<float>::digits10;
    else if (typeid(type) == typeid(double))
        precision = std::numeric_limits<double>::digits10;
    else if (typeid(type) == typeid(long double))
        precision = std::numeric_limits<long double>::digits10;

    return precision;
}


/** Generic casting routine; used by explicitly overloaded
conversion operators.

@param value A variable of the type being cast to.
@return The internal representation of GenericType, converted
to the desired type, if possible.
@throw BadCastException thrown if cast cannot be performed.
*/
template <typename T>
T generic_cast(const std::string& value)
throw (except::BadCastException)
{

    if (value.empty())
        return T();

    std::istringstream buf(value);
    T val;
    buf >> val;

    if (buf.fail())
    {
        throw
        except::BadCastException(std::string("Cast failed: '") +
                                 buf.str() +
                                 std::string("' -> ") +
                                 typeid(T).name());
    }

    return val;
}
}

#endif
