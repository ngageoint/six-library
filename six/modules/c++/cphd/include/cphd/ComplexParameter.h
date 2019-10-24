/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD_COMPLEX_PARAMETER_H__
#define __CPHD_COMPLEX_PARAMETER_H__

#include <complex>
#include <six/Types.h>
#include <six/Parameter.h>
#include <import/str.h>

namespace cphd
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
class ComplexParameter : public six::Parameter
{
public:
    //!  Constructor
    ComplexParameter()
    {
    }
    //!  Destructor
    ~ComplexParameter()
    {
    }

    //! Copy constructor
    ComplexParameter(const ComplexParameter& other)
    {
        mName = other.mName;
        mValue = other.mValue;
    }

    //! Custom constructor
    template<typename T>
    ComplexParameter(std::complex<T> value)
    {
        mValue = str::toString<std::complex<T> >(value.real(), value.imag());
    }

    //! Get complex parameter
    template<typename T> inline std::complex<T> getComplex() const
    {
        return str::toType<std::complex<T> >(mValue);
    }

    //! Using setValue function
    using six::Parameter::setValue;

    //! Overload templated setValue function
    template<typename T>
    void setValue(std::complex<T> value)
    {
        mValue = str::toString<std::complex<T> >(value);
    }

    //! Equality operators
    bool operator==(const ComplexParameter& o) const
    {
        return mName == o.mName && mValue == o.mValue;
    }
    bool operator!=(const ComplexParameter& o) const
    {
        return !((*this) == o);
    }
};
}

#endif

