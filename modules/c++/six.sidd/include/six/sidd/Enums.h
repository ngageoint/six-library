/* =========================================================================
 * This file is part of six.sidd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_SIDD_ENUMS_H__
#define __SIX_SIDD_ENUMS_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>

// -----------------------------------------------------------------------------
// This file is auto-generated - please do NOT edit directly
// -----------------------------------------------------------------------------

namespace six
{
namespace sidd
{

const int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()


/*!
 *  \struct Version 
 *
 *  Enumeration used to represent Versions
 */
struct Version
{
    //! The enumerations allowed
    enum
    {
        SIDD_0_5_0 = 0,
        SIDD_1_0_0 = 1,
        SIDD_NOT_SET = six::sidd::NOT_SET_VALUE
    };

    //! Default constructor
    Version(){ value = SIDD_1_0_0; }

    //! string constructor
    Version(std::string s)
    {
        if (s == "SIDD_0_5_0")
            value = SIDD_0_5_0;
        else if (s == "0_5_0")
            value = SIDD_0_5_0;
        else if (s == "SIDD:0_5_0")
            value = SIDD_0_5_0;
        else if (s == "SIDD_0.5.0")
            value = SIDD_0_5_0;
        else if (s == "0.5.0")
            value = SIDD_0_5_0;
        else if (s == "SIDD:0.5.0")
            value = SIDD_0_5_0;
        else if (s == "SIDD_1_0_0")
            value = SIDD_1_0_0;
        else if (s == "1_0_0")
            value = SIDD_1_0_0;
        else if (s == "SIDD:1_0_0")
            value = SIDD_1_0_0;
        else if (s == "SIDD_1.0.0")
            value = SIDD_1_0_0;
        else if (s == "1.0.0")
            value = SIDD_1_0_0;
        else if (s == "SIDD:1.0.0")
            value = SIDD_1_0_0;
        else if (s == "SIDD_NOT_SET")
            value = SIDD_NOT_SET;
        else if (s == "NOT_SET")
            value = SIDD_NOT_SET;
        else if (s == "SIDD:NOT_SET")
            value = SIDD_NOT_SET;
        else
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
    }

    //! int constructor
    Version(int i)
    {
        switch(i)
        {
        case 0:
            value = SIDD_0_5_0;
            break;
        case 1:
            value = SIDD_1_0_0;
            break;
        case six::sidd::NOT_SET_VALUE:
            value = SIDD_NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }
    }

    //! destructor
    ~Version(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return std::string("0.5.0");
        case 1:
            return std::string("1.0.0");
        case six::sidd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    //! assignment operator
    Version& operator=(const Version& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const Version& o) const { return value == o.value; }
    bool operator!=(const Version& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    Version& operator=(const int& o) { value = o; return *this; }
    bool operator<(const Version& o) const { return value < o.value; }
    bool operator>(const Version& o) const { return value > o.value; }
    bool operator<=(const Version& o) const { return value <= o.value; }
    bool operator>=(const Version& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    static size_t size() { return 3; }

    int value;

};

const Version SIDD_0_5_0(Version::SIDD_0_5_0);
const Version SIDD_1_0_0(Version::SIDD_1_0_0);
const Version SIDD_NOT_SET(Version::SIDD_NOT_SET);


// code auto-generated 2012-08-03 15:24:13.304756

}
}

#endif
