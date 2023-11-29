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
#pragma once
#ifndef SIX_six_Parameter_h_INCLUDED_
#define SIX_six_Parameter_h_INCLUDED_

#include <iostream>

#include <import/str.h>

#include "six/Types.h"
#include "six/Exports.h"

namespace six
{
/*!
 *  \struct Parameter
 *  \brief Free parameter using overloaded streams
 *
 *  Any parameter type (some exclusions).  This type is intended
 *  for use with the Options object and allows the developer to set
 *  and get parameters directly from native types without string
 *  conversion.
 */
struct SIX_SIX_API Parameter final
{
    Parameter() = default;
    ~Parameter() = default;

    Parameter(const Parameter&) = default;
    Parameter& operator=(const Parameter&) = default;
    Parameter(Parameter&&) = default;
    Parameter& operator=(Parameter&&) = default;

    //!  Templated constructor, constructs from given value
    template<typename T>
    Parameter(const T& value)
    {
        mValue = str::toString<T>(value);
    }
    Parameter(const char* value) : Parameter(std::string(value))
    {
    }

    template<typename T>
    Parameter(const std::complex<T>& value)
    {
        mValue = str::toString<std::complex<T> >(mValue);
    }
    #if SIX_six_unique_ComplexInteger
    template<typename T>
    Parameter(const six::ComplexInteger<T>& value)
    {
        mValue = str::toString<six::ComplexInteger<T> >(mValue);
    }
    #endif

     /*!
     * \tparam T Desired (presumably numeric) type to convert to
     *
     * \return Value as a T type
     */
    template<typename T>
    operator T() const
    {
        return str::toType<T>(mValue);
    }

    //!  Get a string as a string
    std::string str() const
    {
        return mValue;
    }
    //!  Get the parameter's name
    std::string getName() const
    {
        return mName;
    }

    //! Get complex parameter
    template<typename T>
    std::complex<T> getComplex() const
    {
        return str::toType<std::complex<T> >(mValue);
    }
    template<typename T>
    void getComplex(std::complex<T>& result) const
    {
        result = str::toType<std::complex<T> >(mValue);
    }
    #if SIX_six_unique_ComplexInteger
    template<typename T>
    void getComplex(six::ComplexInteger<T>& result) const
    {
        result = str::toType<six::ComplexInteger<T> >(mValue);
    }
    #endif

    //!  Set the parameters' name
    void setName(std::string name)
    {
        mName = name;
    }

    //!  Set the parameters' value
    template<typename T>
    void setValue(const T& value)
    {
        mValue = str::toString<T>(value);
    }
    void setValue(const char* value)
    {
        setValue(std::string(value));
    }

    //! Overload templated setValue function
    template<typename T>
    void setValue(const std::complex<T>& value)
    {
        mValue = str::toString<std::complex<T> >(value);
    }
    #if SIX_six_unique_ComplexInteger 
    template<typename T>
    void setValue(const six::ComplexInteger<T>& value)
    {
        mValue = str::toString<six::ComplexInteger<T> >(value);
    }
    #endif

    //!  Get back const char*
    operator const char*() const
    {
        return mValue.c_str();
    }

    // Must be member functions for existing SWIG bindings
    bool operator==(const Parameter& rhs) const
    {
        return (getName() == rhs.getName()) && (str() == rhs.str());
    }
    bool operator!=(const Parameter& rhs) const
    {
        return !(*this == rhs);
    }

private:
    std::string mValue;
    std::string mName;
};

inline std::ostream& operator<<(std::ostream& os, const Parameter& p)
{
    os << p.str();
    return os;
}

inline std::ostream& out(std::ostream& os, const Parameter& p)
{
    os << "    Parameter Name   : " << p.getName() << "\n"
        << "    Parameter Value  : " << p << "\n";
    return os;
}

}

#endif // SIX_six_Parameter_h_INCLUDED_