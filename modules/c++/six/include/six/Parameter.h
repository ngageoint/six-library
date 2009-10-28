/* =========================================================================
 * This file is part of six-c++ 
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
#ifndef __SIX_PARAMETER_H__
#define __SIX_PARAMETER_H__

#include "six/Types.h"
#include <import/str.h>

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
class Parameter
{
public:
    //!  Constructor
    Parameter()
    {
    }
    //!  Destructor
    ~Parameter()
    {
    }
    
    //!  Templated constructor, constructs from given value
    template<typename T>
    Parameter(T value)
    {
        mData = str::toString<T>(value);
    }
    //!  Return a double
    inline operator double() const
    {
        return str::toType<double>(mData);
    }
    //!  Return a uchar
    inline operator sys::Uint8_T() const
    {
        return str::toType<sys::Uint8_T>(mData);
    }
    //!  Return a char
    inline operator sys::Int8_T() const
    {
        return str::toType<sys::Uint8_T>(mData);
    }
    //!  Return a ushort
    inline operator sys::Uint16_T() const
    {
        return str::toType<sys::Uint16_T>(mData);
    }
    //!  Return a short
    inline operator sys::Int16_T() const
    {
        return str::toType<sys::Uint16_T>(mData);
    }
    //!  Return a uint
    inline operator sys::Uint32_T() const
    {
        return str::toType<sys::Uint32_T>(mData);
    }
    //!  Return an int
    inline operator sys::Int32_T() const
    {
        return str::toType<sys::Uint32_T>(mData);
    }
    //!  64-bit uint
    inline operator sys::Uint64_T() const
    {
        return str::toType<sys::Uint64_T>(mData);
    }
    //!  64-bit int
    inline operator sys::Int64_T() const
    {
        return str::toType<sys::Int64_T>(mData);
    }

    /*         inline operator bool() const */
    /*         { */
    /*             std::string other = mData; */
    /*             str::lower(other); */
    /*             str::trim(other); */

    /*             if (other == "true" || other == "1") */
    /*                 return true; */
    /*             return false; */
    /*         } */
/*     inline operator std::string() const */
/*     { */
/*         return str(); */
/*     } */
    //!  Get a string as a string
    inline std::string str() const
    {
        return mData;
    }
    //!  Get the parameter's name
    inline std::string getName() const
    {
        return mName;
    }
    //!  Set the parameters' name
    void setName(std::string name)
    {
        mName = name;
    }

    //!  Get back const char*
    operator const char*() const
    {
        return mData.c_str();
    }
protected:
    std::string mData;
    std::string mName;
//private:
    // Don't allow this cast, ever.
   

};

}

#endif
