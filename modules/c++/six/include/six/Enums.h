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
#ifndef __SIX_ENUMS_H__
#define __SIX_ENUMS_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>

// -----------------------------------------------------------------------------
// This file is auto-generated - please do NOT edit directly
// -----------------------------------------------------------------------------

namespace six
{


/*!
 *  \struct AppliedType 
 *
 *  Enumeration used to represent AppliedTypes
 */
struct AppliedType
{
    //! The enumerations allowed
    enum
    {
        APPLIED_NOT_SET = -1,
        APPLIED_TRUE = 0,
        APPLIED_FALSE = 1
    };

    //! Default constructor
    AppliedType(){ value = APPLIED_NOT_SET; }

    //! string constructor
    AppliedType(std::string s)
    {
        if (s == "APPLIED_NOT_SET")
            value = APPLIED_NOT_SET;
        else if (s == "APPLIED_TRUE")
            value = APPLIED_TRUE;
        else if (s == "APPLIED_FALSE")
            value = APPLIED_FALSE;
        else
            value = APPLIED_NOT_SET;
    }

    //! int constructor
    AppliedType(int i)
    {
        switch(i)
        {
        case -1:
            value = APPLIED_NOT_SET;
            break;
        case 0:
            value = APPLIED_TRUE;
            break;
        case 1:
            value = APPLIED_FALSE;
            break;
        default:
            value = APPLIED_NOT_SET;
        }
    }

    //! destructor
    ~AppliedType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case -1:
            return "APPLIED_NOT_SET";
        case 0:
            return "APPLIED_TRUE";
        case 1:
            return "APPLIED_FALSE";
        default:
            return "APPLIED_NOT_SET";
        }
    }

    //! assignment operator
    AppliedType& operator=(const AppliedType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const AppliedType& o) const { return value == o.value; }
    bool operator!=(const AppliedType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    AppliedType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const AppliedType& o) const { return value < o.value; }
    bool operator>(const AppliedType& o) const { return value > o.value; }
    bool operator<=(const AppliedType& o) const { return value <= o.value; }
    bool operator>=(const AppliedType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct AutofocusType 
 *
 *  Enumeration used to represent AutofocusTypes
 */
struct AutofocusType
{
    //! The enumerations allowed
    enum
    {
        AUTOFOCUS_NO = 0,
        AUTOFOCUS_GLOBAL = 1,
        AUTOFOCUS_SV = 2
    };

    //! Default constructor
    AutofocusType(){ value = AUTOFOCUS_NO; }

    //! string constructor
    AutofocusType(std::string s)
    {
        if (s == "AUTOFOCUS_NO")
            value = AUTOFOCUS_NO;
        else if (s == "AUTOFOCUS_GLOBAL")
            value = AUTOFOCUS_GLOBAL;
        else if (s == "AUTOFOCUS_SV")
            value = AUTOFOCUS_SV;
        else
            value = AUTOFOCUS_NO;
    }

    //! int constructor
    AutofocusType(int i)
    {
        switch(i)
        {
        case 0:
            value = AUTOFOCUS_NO;
            break;
        case 1:
            value = AUTOFOCUS_GLOBAL;
            break;
        case 2:
            value = AUTOFOCUS_SV;
            break;
        default:
            value = AUTOFOCUS_NO;
        }
    }

    //! destructor
    ~AutofocusType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "AUTOFOCUS_NO";
        case 1:
            return "AUTOFOCUS_GLOBAL";
        case 2:
            return "AUTOFOCUS_SV";
        default:
            return "AUTOFOCUS_NO";
        }
    }

    //! assignment operator
    AutofocusType& operator=(const AutofocusType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const AutofocusType& o) const { return value == o.value; }
    bool operator!=(const AutofocusType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    AutofocusType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const AutofocusType& o) const { return value < o.value; }
    bool operator>(const AutofocusType& o) const { return value > o.value; }
    bool operator<=(const AutofocusType& o) const { return value <= o.value; }
    bool operator>=(const AutofocusType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct BooleanType 
 *
 *  Enumeration used to represent BooleanTypes
 */
struct BooleanType
{
    //! The enumerations allowed
    enum
    {
        BOOL_NOT_SET = -1,
        BOOL_FALSE = 0,
        BOOL_TRUE = 1
    };

    //! Default constructor
    BooleanType(){ value = BOOL_NOT_SET; }

    //! string constructor
    BooleanType(std::string s)
    {
        if (s == "BOOL_NOT_SET")
            value = BOOL_NOT_SET;
        else if (s == "BOOL_FALSE")
            value = BOOL_FALSE;
        else if (s == "BOOL_TRUE")
            value = BOOL_TRUE;
        else
            value = BOOL_NOT_SET;
    }

    //! int constructor
    BooleanType(int i)
    {
        switch(i)
        {
        case -1:
            value = BOOL_NOT_SET;
            break;
        case 0:
            value = BOOL_FALSE;
            break;
        case 1:
            value = BOOL_TRUE;
            break;
        default:
            value = BOOL_NOT_SET;
        }
    }

    //! destructor
    ~BooleanType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case -1:
            return "BOOL_NOT_SET";
        case 0:
            return "BOOL_FALSE";
        case 1:
            return "BOOL_TRUE";
        default:
            return "BOOL_NOT_SET";
        }
    }

    //! assignment operator
    BooleanType& operator=(const BooleanType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const BooleanType& o) const { return value == o.value; }
    bool operator!=(const BooleanType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    BooleanType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const BooleanType& o) const { return value < o.value; }
    bool operator>(const BooleanType& o) const { return value > o.value; }
    bool operator<=(const BooleanType& o) const { return value <= o.value; }
    bool operator>=(const BooleanType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct ByteSwapping 
 *
 *  Enumeration used to represent ByteSwappings
 */
struct ByteSwapping
{
    //! The enumerations allowed
    enum
    {
        BYTE_SWAP_OFF = 0,
        BYTE_SWAP_ON = 1,
        BYTE_SWAP_AUTO = 2
    };

    //! Default constructor
    ByteSwapping(){ value = BYTE_SWAP_OFF; }

    //! string constructor
    ByteSwapping(std::string s)
    {
        if (s == "BYTE_SWAP_OFF")
            value = BYTE_SWAP_OFF;
        else if (s == "BYTE_SWAP_ON")
            value = BYTE_SWAP_ON;
        else if (s == "BYTE_SWAP_AUTO")
            value = BYTE_SWAP_AUTO;
        else
            value = BYTE_SWAP_OFF;
    }

    //! int constructor
    ByteSwapping(int i)
    {
        switch(i)
        {
        case 0:
            value = BYTE_SWAP_OFF;
            break;
        case 1:
            value = BYTE_SWAP_ON;
            break;
        case 2:
            value = BYTE_SWAP_AUTO;
            break;
        default:
            value = BYTE_SWAP_OFF;
        }
    }

    //! destructor
    ~ByteSwapping(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "BYTE_SWAP_OFF";
        case 1:
            return "BYTE_SWAP_ON";
        case 2:
            return "BYTE_SWAP_AUTO";
        default:
            return "BYTE_SWAP_OFF";
        }
    }

    //! assignment operator
    ByteSwapping& operator=(const ByteSwapping& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ByteSwapping& o) const { return value == o.value; }
    bool operator!=(const ByteSwapping& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ByteSwapping& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ByteSwapping& o) const { return value < o.value; }
    bool operator>(const ByteSwapping& o) const { return value > o.value; }
    bool operator<=(const ByteSwapping& o) const { return value <= o.value; }
    bool operator>=(const ByteSwapping& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct CollectType 
 *
 *  Enumeration used to represent CollectTypes
 */
struct CollectType
{
    //! The enumerations allowed
    enum
    {
        COLLECT_NOT_SET = 0,
        COLLECT_MONOSTATIC = 1,
        COLLECT_BISTATIC = 2
    };

    //! Default constructor
    CollectType(){ value = COLLECT_NOT_SET; }

    //! string constructor
    CollectType(std::string s)
    {
        if (s == "COLLECT_NOT_SET")
            value = COLLECT_NOT_SET;
        else if (s == "COLLECT_MONOSTATIC")
            value = COLLECT_MONOSTATIC;
        else if (s == "COLLECT_BISTATIC")
            value = COLLECT_BISTATIC;
        else
            value = COLLECT_NOT_SET;
    }

    //! int constructor
    CollectType(int i)
    {
        switch(i)
        {
        case 0:
            value = COLLECT_NOT_SET;
            break;
        case 1:
            value = COLLECT_MONOSTATIC;
            break;
        case 2:
            value = COLLECT_BISTATIC;
            break;
        default:
            value = COLLECT_NOT_SET;
        }
    }

    //! destructor
    ~CollectType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "COLLECT_NOT_SET";
        case 1:
            return "COLLECT_MONOSTATIC";
        case 2:
            return "COLLECT_BISTATIC";
        default:
            return "COLLECT_NOT_SET";
        }
    }

    //! assignment operator
    CollectType& operator=(const CollectType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const CollectType& o) const { return value == o.value; }
    bool operator!=(const CollectType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    CollectType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const CollectType& o) const { return value < o.value; }
    bool operator>(const CollectType& o) const { return value > o.value; }
    bool operator<=(const CollectType& o) const { return value <= o.value; }
    bool operator>=(const CollectType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct ComplexImageGridType 
 *
 *  Enumeration used to represent ComplexImageGridTypes
 */
struct ComplexImageGridType
{
    //! The enumerations allowed
    enum
    {
        GRID_RGAZIM = 0,
        GRID_RGZERO = 1,
        GRID_XRGYCR = 2,
        GRID_XCTYAT = 3,
        GRID_PLANE = 4
    };

    //! Default constructor
    ComplexImageGridType(){ value = GRID_RGAZIM; }

    //! string constructor
    ComplexImageGridType(std::string s)
    {
        if (s == "GRID_RGAZIM")
            value = GRID_RGAZIM;
        else if (s == "GRID_RGZERO")
            value = GRID_RGZERO;
        else if (s == "GRID_XRGYCR")
            value = GRID_XRGYCR;
        else if (s == "GRID_XCTYAT")
            value = GRID_XCTYAT;
        else if (s == "GRID_PLANE")
            value = GRID_PLANE;
        else
            value = GRID_RGAZIM;
    }

    //! int constructor
    ComplexImageGridType(int i)
    {
        switch(i)
        {
        case 0:
            value = GRID_RGAZIM;
            break;
        case 1:
            value = GRID_RGZERO;
            break;
        case 2:
            value = GRID_XRGYCR;
            break;
        case 3:
            value = GRID_XCTYAT;
            break;
        case 4:
            value = GRID_PLANE;
            break;
        default:
            value = GRID_RGAZIM;
        }
    }

    //! destructor
    ~ComplexImageGridType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "GRID_RGAZIM";
        case 1:
            return "GRID_RGZERO";
        case 2:
            return "GRID_XRGYCR";
        case 3:
            return "GRID_XCTYAT";
        case 4:
            return "GRID_PLANE";
        default:
            return "GRID_RGAZIM";
        }
    }

    //! assignment operator
    ComplexImageGridType& operator=(const ComplexImageGridType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ComplexImageGridType& o) const { return value == o.value; }
    bool operator!=(const ComplexImageGridType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ComplexImageGridType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ComplexImageGridType& o) const { return value < o.value; }
    bool operator>(const ComplexImageGridType& o) const { return value > o.value; }
    bool operator<=(const ComplexImageGridType& o) const { return value <= o.value; }
    bool operator>=(const ComplexImageGridType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct ComplexImagePlaneType 
 *
 *  Enumeration used to represent ComplexImagePlaneTypes
 */
struct ComplexImagePlaneType
{
    //! The enumerations allowed
    enum
    {
        PLANE_OTHER = 0,
        PLANE_SLANT = 1,
        PLANE_GROUND = 2
    };

    //! Default constructor
    ComplexImagePlaneType(){ value = PLANE_OTHER; }

    //! string constructor
    ComplexImagePlaneType(std::string s)
    {
        if (s == "PLANE_OTHER")
            value = PLANE_OTHER;
        else if (s == "PLANE_SLANT")
            value = PLANE_SLANT;
        else if (s == "PLANE_GROUND")
            value = PLANE_GROUND;
        else
            value = PLANE_OTHER;
    }

    //! int constructor
    ComplexImagePlaneType(int i)
    {
        switch(i)
        {
        case 0:
            value = PLANE_OTHER;
            break;
        case 1:
            value = PLANE_SLANT;
            break;
        case 2:
            value = PLANE_GROUND;
            break;
        default:
            value = PLANE_OTHER;
        }
    }

    //! destructor
    ~ComplexImagePlaneType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "PLANE_OTHER";
        case 1:
            return "PLANE_SLANT";
        case 2:
            return "PLANE_GROUND";
        default:
            return "PLANE_OTHER";
        }
    }

    //! assignment operator
    ComplexImagePlaneType& operator=(const ComplexImagePlaneType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ComplexImagePlaneType& o) const { return value == o.value; }
    bool operator!=(const ComplexImagePlaneType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ComplexImagePlaneType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ComplexImagePlaneType& o) const { return value < o.value; }
    bool operator>(const ComplexImagePlaneType& o) const { return value > o.value; }
    bool operator<=(const ComplexImagePlaneType& o) const { return value <= o.value; }
    bool operator>=(const ComplexImagePlaneType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct CornerIndex 
 *
 *  Enumeration used to represent CornerIndexs
 */
struct CornerIndex
{
    //! The enumerations allowed
    enum
    {
        FIRST_ROW_FIRST_COL = 0,
        FIRST_ROW_LAST_COL = 1,
        LAST_ROW_LAST_COL = 2,
        LAST_ROW_FIRST_COL = 3
    };

    //! Default constructor
    CornerIndex(){ value = FIRST_ROW_FIRST_COL; }

    //! string constructor
    CornerIndex(std::string s)
    {
        if (s == "FIRST_ROW_FIRST_COL")
            value = FIRST_ROW_FIRST_COL;
        else if (s == "FIRST_ROW_LAST_COL")
            value = FIRST_ROW_LAST_COL;
        else if (s == "LAST_ROW_LAST_COL")
            value = LAST_ROW_LAST_COL;
        else if (s == "LAST_ROW_FIRST_COL")
            value = LAST_ROW_FIRST_COL;
        else
            value = FIRST_ROW_FIRST_COL;
    }

    //! int constructor
    CornerIndex(int i)
    {
        switch(i)
        {
        case 0:
            value = FIRST_ROW_FIRST_COL;
            break;
        case 1:
            value = FIRST_ROW_LAST_COL;
            break;
        case 2:
            value = LAST_ROW_LAST_COL;
            break;
        case 3:
            value = LAST_ROW_FIRST_COL;
            break;
        default:
            value = FIRST_ROW_FIRST_COL;
        }
    }

    //! destructor
    ~CornerIndex(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "FIRST_ROW_FIRST_COL";
        case 1:
            return "FIRST_ROW_LAST_COL";
        case 2:
            return "LAST_ROW_LAST_COL";
        case 3:
            return "LAST_ROW_FIRST_COL";
        default:
            return "FIRST_ROW_FIRST_COL";
        }
    }

    //! assignment operator
    CornerIndex& operator=(const CornerIndex& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const CornerIndex& o) const { return value == o.value; }
    bool operator!=(const CornerIndex& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    CornerIndex& operator=(const int& o) { value = o; return *this; }
    bool operator<(const CornerIndex& o) const { return value < o.value; }
    bool operator>(const CornerIndex& o) const { return value > o.value; }
    bool operator<=(const CornerIndex& o) const { return value <= o.value; }
    bool operator>=(const CornerIndex& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct DataClass 
 *
 *  Enumeration used to represent DataClasss
 */
struct DataClass
{
    //! The enumerations allowed
    enum
    {
        DATA_UNKNOWN = 0,
        DATA_COMPLEX = 1,
        DATA_DERIVED = 2
    };

    //! Default constructor
    DataClass(){ value = DATA_UNKNOWN; }

    //! string constructor
    DataClass(std::string s)
    {
        if (s == "DATA_UNKNOWN")
            value = DATA_UNKNOWN;
        else if (s == "DATA_COMPLEX")
            value = DATA_COMPLEX;
        else if (s == "DATA_DERIVED")
            value = DATA_DERIVED;
        else
            value = DATA_UNKNOWN;
    }

    //! int constructor
    DataClass(int i)
    {
        switch(i)
        {
        case 0:
            value = DATA_UNKNOWN;
            break;
        case 1:
            value = DATA_COMPLEX;
            break;
        case 2:
            value = DATA_DERIVED;
            break;
        default:
            value = DATA_UNKNOWN;
        }
    }

    //! destructor
    ~DataClass(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "DATA_UNKNOWN";
        case 1:
            return "DATA_COMPLEX";
        case 2:
            return "DATA_DERIVED";
        default:
            return "DATA_UNKNOWN";
        }
    }

    //! assignment operator
    DataClass& operator=(const DataClass& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DataClass& o) const { return value == o.value; }
    bool operator!=(const DataClass& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DataClass& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DataClass& o) const { return value < o.value; }
    bool operator>(const DataClass& o) const { return value > o.value; }
    bool operator<=(const DataClass& o) const { return value <= o.value; }
    bool operator>=(const DataClass& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct DataType 
 *
 *  Enumeration used to represent DataTypes
 */
struct DataType
{
    //! The enumerations allowed
    enum
    {
        TYPE_UNKNOWN = 0,
        TYPE_COMPLEX = 1,
        TYPE_DERIVED = 2
    };

    //! Default constructor
    DataType(){ value = TYPE_UNKNOWN; }

    //! string constructor
    DataType(std::string s)
    {
        if (s == "TYPE_UNKNOWN")
            value = TYPE_UNKNOWN;
        else if (s == "TYPE_COMPLEX")
            value = TYPE_COMPLEX;
        else if (s == "TYPE_DERIVED")
            value = TYPE_DERIVED;
        else
            value = TYPE_UNKNOWN;
    }

    //! int constructor
    DataType(int i)
    {
        switch(i)
        {
        case 0:
            value = TYPE_UNKNOWN;
            break;
        case 1:
            value = TYPE_COMPLEX;
            break;
        case 2:
            value = TYPE_DERIVED;
            break;
        default:
            value = TYPE_UNKNOWN;
        }
    }

    //! destructor
    ~DataType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "TYPE_UNKNOWN";
        case 1:
            return "TYPE_COMPLEX";
        case 2:
            return "TYPE_DERIVED";
        default:
            return "TYPE_UNKNOWN";
        }
    }

    //! assignment operator
    DataType& operator=(const DataType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DataType& o) const { return value == o.value; }
    bool operator!=(const DataType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DataType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DataType& o) const { return value < o.value; }
    bool operator>(const DataType& o) const { return value > o.value; }
    bool operator<=(const DataType& o) const { return value <= o.value; }
    bool operator>=(const DataType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct DecimationMethod 
 *
 *  Enumeration used to represent DecimationMethods
 */
struct DecimationMethod
{
    //! The enumerations allowed
    enum
    {
        DEC_NOT_SET = 0,
        DEC_NEAREST_NEIGHBOR = 1,
        DEC_BILINEAR = 2,
        DEC_BRIGHTEST_PIXEL = 3,
        DEC_LAGRANGE = 4
    };

    //! Default constructor
    DecimationMethod(){ value = DEC_NOT_SET; }

    //! string constructor
    DecimationMethod(std::string s)
    {
        if (s == "DEC_NOT_SET")
            value = DEC_NOT_SET;
        else if (s == "DEC_NEAREST_NEIGHBOR")
            value = DEC_NEAREST_NEIGHBOR;
        else if (s == "DEC_BILINEAR")
            value = DEC_BILINEAR;
        else if (s == "DEC_BRIGHTEST_PIXEL")
            value = DEC_BRIGHTEST_PIXEL;
        else if (s == "DEC_LAGRANGE")
            value = DEC_LAGRANGE;
        else
            value = DEC_NOT_SET;
    }

    //! int constructor
    DecimationMethod(int i)
    {
        switch(i)
        {
        case 0:
            value = DEC_NOT_SET;
            break;
        case 1:
            value = DEC_NEAREST_NEIGHBOR;
            break;
        case 2:
            value = DEC_BILINEAR;
            break;
        case 3:
            value = DEC_BRIGHTEST_PIXEL;
            break;
        case 4:
            value = DEC_LAGRANGE;
            break;
        default:
            value = DEC_NOT_SET;
        }
    }

    //! destructor
    ~DecimationMethod(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "DEC_NOT_SET";
        case 1:
            return "DEC_NEAREST_NEIGHBOR";
        case 2:
            return "DEC_BILINEAR";
        case 3:
            return "DEC_BRIGHTEST_PIXEL";
        case 4:
            return "DEC_LAGRANGE";
        default:
            return "DEC_NOT_SET";
        }
    }

    //! assignment operator
    DecimationMethod& operator=(const DecimationMethod& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DecimationMethod& o) const { return value == o.value; }
    bool operator!=(const DecimationMethod& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DecimationMethod& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DecimationMethod& o) const { return value < o.value; }
    bool operator>(const DecimationMethod& o) const { return value > o.value; }
    bool operator<=(const DecimationMethod& o) const { return value <= o.value; }
    bool operator>=(const DecimationMethod& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct DemodType 
 *
 *  Enumeration used to represent DemodTypes
 */
struct DemodType
{
    //! The enumerations allowed
    enum
    {
        DEMOD_NOT_SET = 0,
        DEMOD_STRETCH = 1,
        DEMOD_CHIRP = 2
    };

    //! Default constructor
    DemodType(){ value = DEMOD_NOT_SET; }

    //! string constructor
    DemodType(std::string s)
    {
        if (s == "DEMOD_NOT_SET")
            value = DEMOD_NOT_SET;
        else if (s == "DEMOD_STRETCH")
            value = DEMOD_STRETCH;
        else if (s == "DEMOD_CHIRP")
            value = DEMOD_CHIRP;
        else
            value = DEMOD_NOT_SET;
    }

    //! int constructor
    DemodType(int i)
    {
        switch(i)
        {
        case 0:
            value = DEMOD_NOT_SET;
            break;
        case 1:
            value = DEMOD_STRETCH;
            break;
        case 2:
            value = DEMOD_CHIRP;
            break;
        default:
            value = DEMOD_NOT_SET;
        }
    }

    //! destructor
    ~DemodType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "DEMOD_NOT_SET";
        case 1:
            return "DEMOD_STRETCH";
        case 2:
            return "DEMOD_CHIRP";
        default:
            return "DEMOD_NOT_SET";
        }
    }

    //! assignment operator
    DemodType& operator=(const DemodType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DemodType& o) const { return value == o.value; }
    bool operator!=(const DemodType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DemodType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DemodType& o) const { return value < o.value; }
    bool operator>(const DemodType& o) const { return value > o.value; }
    bool operator<=(const DemodType& o) const { return value <= o.value; }
    bool operator>=(const DemodType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct DisplayType 
 *
 *  Enumeration used to represent DisplayTypes
 */
struct DisplayType
{
    //! The enumerations allowed
    enum
    {
        DISPLAY_NOT_SET = 0,
        DISPLAY_COLOR = 1,
        DISPLAY_MONO = 2
    };

    //! Default constructor
    DisplayType(){ value = DISPLAY_NOT_SET; }

    //! string constructor
    DisplayType(std::string s)
    {
        if (s == "DISPLAY_NOT_SET")
            value = DISPLAY_NOT_SET;
        else if (s == "DISPLAY_COLOR")
            value = DISPLAY_COLOR;
        else if (s == "DISPLAY_MONO")
            value = DISPLAY_MONO;
        else
            value = DISPLAY_NOT_SET;
    }

    //! int constructor
    DisplayType(int i)
    {
        switch(i)
        {
        case 0:
            value = DISPLAY_NOT_SET;
            break;
        case 1:
            value = DISPLAY_COLOR;
            break;
        case 2:
            value = DISPLAY_MONO;
            break;
        default:
            value = DISPLAY_NOT_SET;
        }
    }

    //! destructor
    ~DisplayType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "DISPLAY_NOT_SET";
        case 1:
            return "DISPLAY_COLOR";
        case 2:
            return "DISPLAY_MONO";
        default:
            return "DISPLAY_NOT_SET";
        }
    }

    //! assignment operator
    DisplayType& operator=(const DisplayType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DisplayType& o) const { return value == o.value; }
    bool operator!=(const DisplayType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DisplayType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DisplayType& o) const { return value < o.value; }
    bool operator>(const DisplayType& o) const { return value > o.value; }
    bool operator<=(const DisplayType& o) const { return value <= o.value; }
    bool operator>=(const DisplayType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct DualPolarizationType 
 *
 *  Enumeration used to represent DualPolarizationTypes
 */
struct DualPolarizationType
{
    //! The enumerations allowed
    enum
    {
        DUAL_POL_NOT_SET = 0,
        DUAL_POL_OTHER = 1,
        DUAL_POL_V_V = 2,
        DUAL_POL_V_H = 3,
        DUAL_POL_H_V = 4,
        DUAL_POL_H_H = 5,
        DUAL_POL_RHC_RHC = 6,
        DUAL_POL_RHC_LHC = 7,
        DUAL_POL_LHC_LHC = 8
    };

    //! Default constructor
    DualPolarizationType(){ value = DUAL_POL_NOT_SET; }

    //! string constructor
    DualPolarizationType(std::string s)
    {
        if (s == "DUAL_POL_NOT_SET")
            value = DUAL_POL_NOT_SET;
        else if (s == "DUAL_POL_OTHER")
            value = DUAL_POL_OTHER;
        else if (s == "DUAL_POL_V_V")
            value = DUAL_POL_V_V;
        else if (s == "DUAL_POL_V_H")
            value = DUAL_POL_V_H;
        else if (s == "DUAL_POL_H_V")
            value = DUAL_POL_H_V;
        else if (s == "DUAL_POL_H_H")
            value = DUAL_POL_H_H;
        else if (s == "DUAL_POL_RHC_RHC")
            value = DUAL_POL_RHC_RHC;
        else if (s == "DUAL_POL_RHC_LHC")
            value = DUAL_POL_RHC_LHC;
        else if (s == "DUAL_POL_LHC_LHC")
            value = DUAL_POL_LHC_LHC;
        else
            value = DUAL_POL_NOT_SET;
    }

    //! int constructor
    DualPolarizationType(int i)
    {
        switch(i)
        {
        case 0:
            value = DUAL_POL_NOT_SET;
            break;
        case 1:
            value = DUAL_POL_OTHER;
            break;
        case 2:
            value = DUAL_POL_V_V;
            break;
        case 3:
            value = DUAL_POL_V_H;
            break;
        case 4:
            value = DUAL_POL_H_V;
            break;
        case 5:
            value = DUAL_POL_H_H;
            break;
        case 6:
            value = DUAL_POL_RHC_RHC;
            break;
        case 7:
            value = DUAL_POL_RHC_LHC;
            break;
        case 8:
            value = DUAL_POL_LHC_LHC;
            break;
        default:
            value = DUAL_POL_NOT_SET;
        }
    }

    //! destructor
    ~DualPolarizationType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "DUAL_POL_NOT_SET";
        case 1:
            return "DUAL_POL_OTHER";
        case 2:
            return "DUAL_POL_V_V";
        case 3:
            return "DUAL_POL_V_H";
        case 4:
            return "DUAL_POL_H_V";
        case 5:
            return "DUAL_POL_H_H";
        case 6:
            return "DUAL_POL_RHC_RHC";
        case 7:
            return "DUAL_POL_RHC_LHC";
        case 8:
            return "DUAL_POL_LHC_LHC";
        default:
            return "DUAL_POL_NOT_SET";
        }
    }

    //! assignment operator
    DualPolarizationType& operator=(const DualPolarizationType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const DualPolarizationType& o) const { return value == o.value; }
    bool operator!=(const DualPolarizationType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DualPolarizationType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DualPolarizationType& o) const { return value < o.value; }
    bool operator>(const DualPolarizationType& o) const { return value > o.value; }
    bool operator<=(const DualPolarizationType& o) const { return value <= o.value; }
    bool operator>=(const DualPolarizationType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct EarthModelType 
 *
 *  Enumeration used to represent EarthModelTypes
 */
struct EarthModelType
{
    //! The enumerations allowed
    enum
    {
        EARTH_NOT_SET = 0,
        EARTH_WGS84 = 1
    };

    //! Default constructor
    EarthModelType(){ value = EARTH_NOT_SET; }

    //! string constructor
    EarthModelType(std::string s)
    {
        if (s == "EARTH_NOT_SET")
            value = EARTH_NOT_SET;
        else if (s == "EARTH_WGS84")
            value = EARTH_WGS84;
        else
            value = EARTH_NOT_SET;
    }

    //! int constructor
    EarthModelType(int i)
    {
        switch(i)
        {
        case 0:
            value = EARTH_NOT_SET;
            break;
        case 1:
            value = EARTH_WGS84;
            break;
        default:
            value = EARTH_NOT_SET;
        }
    }

    //! destructor
    ~EarthModelType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "EARTH_NOT_SET";
        case 1:
            return "EARTH_WGS84";
        default:
            return "EARTH_NOT_SET";
        }
    }

    //! assignment operator
    EarthModelType& operator=(const EarthModelType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const EarthModelType& o) const { return value == o.value; }
    bool operator!=(const EarthModelType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    EarthModelType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const EarthModelType& o) const { return value < o.value; }
    bool operator>(const EarthModelType& o) const { return value > o.value; }
    bool operator<=(const EarthModelType& o) const { return value <= o.value; }
    bool operator>=(const EarthModelType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct FFTSign 
 *
 *  Enumeration used to represent FFTSigns
 */
struct FFTSign
{
    //! The enumerations allowed
    enum
    {
        FFT_SIGN_NEG = -1,
        FFT_SIGN_NOT_SET = 0,
        FFT_SIGN_POS = 1
    };

    //! Default constructor
    FFTSign(){ value = FFT_SIGN_NOT_SET; }

    //! string constructor
    FFTSign(std::string s)
    {
        if (s == "FFT_SIGN_NEG")
            value = FFT_SIGN_NEG;
        else if (s == "FFT_SIGN_NOT_SET")
            value = FFT_SIGN_NOT_SET;
        else if (s == "FFT_SIGN_POS")
            value = FFT_SIGN_POS;
        else
            value = FFT_SIGN_NOT_SET;
    }

    //! int constructor
    FFTSign(int i)
    {
        switch(i)
        {
        case -1:
            value = FFT_SIGN_NEG;
            break;
        case 0:
            value = FFT_SIGN_NOT_SET;
            break;
        case 1:
            value = FFT_SIGN_POS;
            break;
        default:
            value = FFT_SIGN_NOT_SET;
        }
    }

    //! destructor
    ~FFTSign(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case -1:
            return "FFT_SIGN_NEG";
        case 0:
            return "FFT_SIGN_NOT_SET";
        case 1:
            return "FFT_SIGN_POS";
        default:
            return "FFT_SIGN_NOT_SET";
        }
    }

    //! assignment operator
    FFTSign& operator=(const FFTSign& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const FFTSign& o) const { return value == o.value; }
    bool operator!=(const FFTSign& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    FFTSign& operator=(const int& o) { value = o; return *this; }
    bool operator<(const FFTSign& o) const { return value < o.value; }
    bool operator>(const FFTSign& o) const { return value > o.value; }
    bool operator<=(const FFTSign& o) const { return value <= o.value; }
    bool operator>=(const FFTSign& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct FrameType 
 *
 *  Enumeration used to represent FrameTypes
 */
struct FrameType
{
    //! The enumerations allowed
    enum
    {
        FRAME_ECF = 0,
        FRAME_RIC_ECF = 1,
        FRAME_RIC_ECI = 2
    };

    //! Default constructor
    FrameType(){ value = FRAME_ECF; }

    //! string constructor
    FrameType(std::string s)
    {
        if (s == "FRAME_ECF")
            value = FRAME_ECF;
        else if (s == "FRAME_RIC_ECF")
            value = FRAME_RIC_ECF;
        else if (s == "FRAME_RIC_ECI")
            value = FRAME_RIC_ECI;
        else
            value = FRAME_ECF;
    }

    //! int constructor
    FrameType(int i)
    {
        switch(i)
        {
        case 0:
            value = FRAME_ECF;
            break;
        case 1:
            value = FRAME_RIC_ECF;
            break;
        case 2:
            value = FRAME_RIC_ECI;
            break;
        default:
            value = FRAME_ECF;
        }
    }

    //! destructor
    ~FrameType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "FRAME_ECF";
        case 1:
            return "FRAME_RIC_ECF";
        case 2:
            return "FRAME_RIC_ECI";
        default:
            return "FRAME_ECF";
        }
    }

    //! assignment operator
    FrameType& operator=(const FrameType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const FrameType& o) const { return value == o.value; }
    bool operator!=(const FrameType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    FrameType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const FrameType& o) const { return value < o.value; }
    bool operator>(const FrameType& o) const { return value > o.value; }
    bool operator<=(const FrameType& o) const { return value <= o.value; }
    bool operator>=(const FrameType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct ImageBeamCompensationType 
 *
 *  Enumeration used to represent ImageBeamCompensationTypes
 */
struct ImageBeamCompensationType
{
    //! The enumerations allowed
    enum
    {
        IMAGE_BEAM_NO = 0,
        IMAGE_BEAM_SV = 1
    };

    //! Default constructor
    ImageBeamCompensationType(){ value = IMAGE_BEAM_NO; }

    //! string constructor
    ImageBeamCompensationType(std::string s)
    {
        if (s == "IMAGE_BEAM_NO")
            value = IMAGE_BEAM_NO;
        else if (s == "IMAGE_BEAM_SV")
            value = IMAGE_BEAM_SV;
        else
            value = IMAGE_BEAM_NO;
    }

    //! int constructor
    ImageBeamCompensationType(int i)
    {
        switch(i)
        {
        case 0:
            value = IMAGE_BEAM_NO;
            break;
        case 1:
            value = IMAGE_BEAM_SV;
            break;
        default:
            value = IMAGE_BEAM_NO;
        }
    }

    //! destructor
    ~ImageBeamCompensationType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "IMAGE_BEAM_NO";
        case 1:
            return "IMAGE_BEAM_SV";
        default:
            return "IMAGE_BEAM_NO";
        }
    }

    //! assignment operator
    ImageBeamCompensationType& operator=(const ImageBeamCompensationType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ImageBeamCompensationType& o) const { return value == o.value; }
    bool operator!=(const ImageBeamCompensationType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ImageBeamCompensationType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ImageBeamCompensationType& o) const { return value < o.value; }
    bool operator>(const ImageBeamCompensationType& o) const { return value > o.value; }
    bool operator<=(const ImageBeamCompensationType& o) const { return value <= o.value; }
    bool operator>=(const ImageBeamCompensationType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct ImageFormationType 
 *
 *  Enumeration used to represent ImageFormationTypes
 */
struct ImageFormationType
{
    //! The enumerations allowed
    enum
    {
        IF_OTHER = 0,
        IF_PFA = 1,
        IF_RMA = 2,
        IF_RGAZCOMP = 3
    };

    //! Default constructor
    ImageFormationType(){ value = IF_OTHER; }

    //! string constructor
    ImageFormationType(std::string s)
    {
        if (s == "IF_OTHER")
            value = IF_OTHER;
        else if (s == "IF_PFA")
            value = IF_PFA;
        else if (s == "IF_RMA")
            value = IF_RMA;
        else if (s == "IF_RGAZCOMP")
            value = IF_RGAZCOMP;
        else
            value = IF_OTHER;
    }

    //! int constructor
    ImageFormationType(int i)
    {
        switch(i)
        {
        case 0:
            value = IF_OTHER;
            break;
        case 1:
            value = IF_PFA;
            break;
        case 2:
            value = IF_RMA;
            break;
        case 3:
            value = IF_RGAZCOMP;
            break;
        default:
            value = IF_OTHER;
        }
    }

    //! destructor
    ~ImageFormationType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "IF_OTHER";
        case 1:
            return "IF_PFA";
        case 2:
            return "IF_RMA";
        case 3:
            return "IF_RGAZCOMP";
        default:
            return "IF_OTHER";
        }
    }

    //! assignment operator
    ImageFormationType& operator=(const ImageFormationType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ImageFormationType& o) const { return value == o.value; }
    bool operator!=(const ImageFormationType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ImageFormationType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ImageFormationType& o) const { return value < o.value; }
    bool operator>(const ImageFormationType& o) const { return value > o.value; }
    bool operator<=(const ImageFormationType& o) const { return value <= o.value; }
    bool operator>=(const ImageFormationType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct MagnificationMethod 
 *
 *  Enumeration used to represent MagnificationMethods
 */
struct MagnificationMethod
{
    //! The enumerations allowed
    enum
    {
        MAG_NOT_SET = 0,
        MAG_NEAREST_NEIGHBOR = 1,
        MAG_BILINEAR = 2
    };

    //! Default constructor
    MagnificationMethod(){ value = MAG_NOT_SET; }

    //! string constructor
    MagnificationMethod(std::string s)
    {
        if (s == "MAG_NOT_SET")
            value = MAG_NOT_SET;
        else if (s == "MAG_NEAREST_NEIGHBOR")
            value = MAG_NEAREST_NEIGHBOR;
        else if (s == "MAG_BILINEAR")
            value = MAG_BILINEAR;
        else
            value = MAG_NOT_SET;
    }

    //! int constructor
    MagnificationMethod(int i)
    {
        switch(i)
        {
        case 0:
            value = MAG_NOT_SET;
            break;
        case 1:
            value = MAG_NEAREST_NEIGHBOR;
            break;
        case 2:
            value = MAG_BILINEAR;
            break;
        default:
            value = MAG_NOT_SET;
        }
    }

    //! destructor
    ~MagnificationMethod(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "MAG_NOT_SET";
        case 1:
            return "MAG_NEAREST_NEIGHBOR";
        case 2:
            return "MAG_BILINEAR";
        default:
            return "MAG_NOT_SET";
        }
    }

    //! assignment operator
    MagnificationMethod& operator=(const MagnificationMethod& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const MagnificationMethod& o) const { return value == o.value; }
    bool operator!=(const MagnificationMethod& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    MagnificationMethod& operator=(const int& o) { value = o; return *this; }
    bool operator<(const MagnificationMethod& o) const { return value < o.value; }
    bool operator>(const MagnificationMethod& o) const { return value > o.value; }
    bool operator<=(const MagnificationMethod& o) const { return value <= o.value; }
    bool operator>=(const MagnificationMethod& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct OrientationType 
 *
 *  Enumeration used to represent OrientationTypes
 */
struct OrientationType
{
    //! The enumerations allowed
    enum
    {
        ORIENT_NOT_SET = 0,
        ORIENT_UP = 1,
        ORIENT_DOWN = 2,
        ORIENT_LEFT = 3,
        ORIENT_RIGHT = 4,
        ORIENT_ARBITRARY = 5
    };

    //! Default constructor
    OrientationType(){ value = ORIENT_NOT_SET; }

    //! string constructor
    OrientationType(std::string s)
    {
        if (s == "ORIENT_NOT_SET")
            value = ORIENT_NOT_SET;
        else if (s == "ORIENT_UP")
            value = ORIENT_UP;
        else if (s == "ORIENT_DOWN")
            value = ORIENT_DOWN;
        else if (s == "ORIENT_LEFT")
            value = ORIENT_LEFT;
        else if (s == "ORIENT_RIGHT")
            value = ORIENT_RIGHT;
        else if (s == "ORIENT_ARBITRARY")
            value = ORIENT_ARBITRARY;
        else
            value = ORIENT_NOT_SET;
    }

    //! int constructor
    OrientationType(int i)
    {
        switch(i)
        {
        case 0:
            value = ORIENT_NOT_SET;
            break;
        case 1:
            value = ORIENT_UP;
            break;
        case 2:
            value = ORIENT_DOWN;
            break;
        case 3:
            value = ORIENT_LEFT;
            break;
        case 4:
            value = ORIENT_RIGHT;
            break;
        case 5:
            value = ORIENT_ARBITRARY;
            break;
        default:
            value = ORIENT_NOT_SET;
        }
    }

    //! destructor
    ~OrientationType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "ORIENT_NOT_SET";
        case 1:
            return "ORIENT_UP";
        case 2:
            return "ORIENT_DOWN";
        case 3:
            return "ORIENT_LEFT";
        case 4:
            return "ORIENT_RIGHT";
        case 5:
            return "ORIENT_ARBITRARY";
        default:
            return "ORIENT_NOT_SET";
        }
    }

    //! assignment operator
    OrientationType& operator=(const OrientationType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const OrientationType& o) const { return value == o.value; }
    bool operator!=(const OrientationType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    OrientationType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const OrientationType& o) const { return value < o.value; }
    bool operator>(const OrientationType& o) const { return value > o.value; }
    bool operator<=(const OrientationType& o) const { return value <= o.value; }
    bool operator>=(const OrientationType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct PixelType 
 *
 *  Enumeration used to represent PixelTypes
 */
struct PixelType
{
    //! The enumerations allowed
    enum
    {
        PIXEL_TYPE_NOT_SET = 0,
        RE32F_IM32F = 1,
        RE16I_IM16I = 2,
        MONO8I = 3,
        MONO16I = 4,
        MONO8LU = 5,
        RGB8LU = 6,
        RGB24I = 7
    };

    //! Default constructor
    PixelType(){ value = PIXEL_TYPE_NOT_SET; }

    //! string constructor
    PixelType(std::string s)
    {
        if (s == "PIXEL_TYPE_NOT_SET")
            value = PIXEL_TYPE_NOT_SET;
        else if (s == "RE32F_IM32F")
            value = RE32F_IM32F;
        else if (s == "RE16I_IM16I")
            value = RE16I_IM16I;
        else if (s == "MONO8I")
            value = MONO8I;
        else if (s == "MONO16I")
            value = MONO16I;
        else if (s == "MONO8LU")
            value = MONO8LU;
        else if (s == "RGB8LU")
            value = RGB8LU;
        else if (s == "RGB24I")
            value = RGB24I;
        else
            value = PIXEL_TYPE_NOT_SET;
    }

    //! int constructor
    PixelType(int i)
    {
        switch(i)
        {
        case 0:
            value = PIXEL_TYPE_NOT_SET;
            break;
        case 1:
            value = RE32F_IM32F;
            break;
        case 2:
            value = RE16I_IM16I;
            break;
        case 3:
            value = MONO8I;
            break;
        case 4:
            value = MONO16I;
            break;
        case 5:
            value = MONO8LU;
            break;
        case 6:
            value = RGB8LU;
            break;
        case 7:
            value = RGB24I;
            break;
        default:
            value = PIXEL_TYPE_NOT_SET;
        }
    }

    //! destructor
    ~PixelType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "PIXEL_TYPE_NOT_SET";
        case 1:
            return "RE32F_IM32F";
        case 2:
            return "RE16I_IM16I";
        case 3:
            return "MONO8I";
        case 4:
            return "MONO16I";
        case 5:
            return "MONO8LU";
        case 6:
            return "RGB8LU";
        case 7:
            return "RGB24I";
        default:
            return "PIXEL_TYPE_NOT_SET";
        }
    }

    //! assignment operator
    PixelType& operator=(const PixelType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const PixelType& o) const { return value == o.value; }
    bool operator!=(const PixelType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    PixelType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const PixelType& o) const { return value < o.value; }
    bool operator>(const PixelType& o) const { return value > o.value; }
    bool operator<=(const PixelType& o) const { return value <= o.value; }
    bool operator>=(const PixelType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct PolarizationType 
 *
 *  Enumeration used to represent PolarizationTypes
 */
struct PolarizationType
{
    //! The enumerations allowed
    enum
    {
        POL_NOT_SET = 0,
        POL_OTHER = 1,
        POL_V = 2,
        POL_H = 3,
        POL_RHC = 4,
        POL_LHC = 5
    };

    //! Default constructor
    PolarizationType(){ value = POL_NOT_SET; }

    //! string constructor
    PolarizationType(std::string s)
    {
        if (s == "POL_NOT_SET")
            value = POL_NOT_SET;
        else if (s == "POL_OTHER")
            value = POL_OTHER;
        else if (s == "POL_V")
            value = POL_V;
        else if (s == "POL_H")
            value = POL_H;
        else if (s == "POL_RHC")
            value = POL_RHC;
        else if (s == "POL_LHC")
            value = POL_LHC;
        else
            value = POL_NOT_SET;
    }

    //! int constructor
    PolarizationType(int i)
    {
        switch(i)
        {
        case 0:
            value = POL_NOT_SET;
            break;
        case 1:
            value = POL_OTHER;
            break;
        case 2:
            value = POL_V;
            break;
        case 3:
            value = POL_H;
            break;
        case 4:
            value = POL_RHC;
            break;
        case 5:
            value = POL_LHC;
            break;
        default:
            value = POL_NOT_SET;
        }
    }

    //! destructor
    ~PolarizationType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "POL_NOT_SET";
        case 1:
            return "POL_OTHER";
        case 2:
            return "POL_V";
        case 3:
            return "POL_H";
        case 4:
            return "POL_RHC";
        case 5:
            return "POL_LHC";
        default:
            return "POL_NOT_SET";
        }
    }

    //! assignment operator
    PolarizationType& operator=(const PolarizationType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const PolarizationType& o) const { return value == o.value; }
    bool operator!=(const PolarizationType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    PolarizationType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const PolarizationType& o) const { return value < o.value; }
    bool operator>(const PolarizationType& o) const { return value > o.value; }
    bool operator<=(const PolarizationType& o) const { return value <= o.value; }
    bool operator>=(const PolarizationType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct ProjectionType 
 *
 *  Enumeration used to represent ProjectionTypes
 */
struct ProjectionType
{
    //! The enumerations allowed
    enum
    {
        PROJECTION_NOT_SET = 0,
        PROJECTION_PLANE = 1,
        PROJECTION_GEOGRAPHIC = 2,
        PROJECTION_CYLINDRICAL = 3,
        PROJECTION_POLYNOMIAL = 4
    };

    //! Default constructor
    ProjectionType(){ value = PROJECTION_NOT_SET; }

    //! string constructor
    ProjectionType(std::string s)
    {
        if (s == "PROJECTION_NOT_SET")
            value = PROJECTION_NOT_SET;
        else if (s == "PROJECTION_PLANE")
            value = PROJECTION_PLANE;
        else if (s == "PROJECTION_GEOGRAPHIC")
            value = PROJECTION_GEOGRAPHIC;
        else if (s == "PROJECTION_CYLINDRICAL")
            value = PROJECTION_CYLINDRICAL;
        else if (s == "PROJECTION_POLYNOMIAL")
            value = PROJECTION_POLYNOMIAL;
        else
            value = PROJECTION_NOT_SET;
    }

    //! int constructor
    ProjectionType(int i)
    {
        switch(i)
        {
        case 0:
            value = PROJECTION_NOT_SET;
            break;
        case 1:
            value = PROJECTION_PLANE;
            break;
        case 2:
            value = PROJECTION_GEOGRAPHIC;
            break;
        case 3:
            value = PROJECTION_CYLINDRICAL;
            break;
        case 4:
            value = PROJECTION_POLYNOMIAL;
            break;
        default:
            value = PROJECTION_NOT_SET;
        }
    }

    //! destructor
    ~ProjectionType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "PROJECTION_NOT_SET";
        case 1:
            return "PROJECTION_PLANE";
        case 2:
            return "PROJECTION_GEOGRAPHIC";
        case 3:
            return "PROJECTION_CYLINDRICAL";
        case 4:
            return "PROJECTION_POLYNOMIAL";
        default:
            return "PROJECTION_NOT_SET";
        }
    }

    //! assignment operator
    ProjectionType& operator=(const ProjectionType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const ProjectionType& o) const { return value == o.value; }
    bool operator!=(const ProjectionType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    ProjectionType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const ProjectionType& o) const { return value < o.value; }
    bool operator>(const ProjectionType& o) const { return value > o.value; }
    bool operator<=(const ProjectionType& o) const { return value <= o.value; }
    bool operator>=(const ProjectionType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct RMAlgoType 
 *
 *  Enumeration used to represent RMAlgoTypes
 */
struct RMAlgoType
{
    //! The enumerations allowed
    enum
    {
        RMA_NOT_SET = 0,
        RMA_OMEGA_K = 1,
        RMA_CSA = 2,
        RMA_RG_DOP = 3
    };

    //! Default constructor
    RMAlgoType(){ value = RMA_NOT_SET; }

    //! string constructor
    RMAlgoType(std::string s)
    {
        if (s == "RMA_NOT_SET")
            value = RMA_NOT_SET;
        else if (s == "RMA_OMEGA_K")
            value = RMA_OMEGA_K;
        else if (s == "RMA_CSA")
            value = RMA_CSA;
        else if (s == "RMA_RG_DOP")
            value = RMA_RG_DOP;
        else
            value = RMA_NOT_SET;
    }

    //! int constructor
    RMAlgoType(int i)
    {
        switch(i)
        {
        case 0:
            value = RMA_NOT_SET;
            break;
        case 1:
            value = RMA_OMEGA_K;
            break;
        case 2:
            value = RMA_CSA;
            break;
        case 3:
            value = RMA_RG_DOP;
            break;
        default:
            value = RMA_NOT_SET;
        }
    }

    //! destructor
    ~RMAlgoType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "RMA_NOT_SET";
        case 1:
            return "RMA_OMEGA_K";
        case 2:
            return "RMA_CSA";
        case 3:
            return "RMA_RG_DOP";
        default:
            return "RMA_NOT_SET";
        }
    }

    //! assignment operator
    RMAlgoType& operator=(const RMAlgoType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const RMAlgoType& o) const { return value == o.value; }
    bool operator!=(const RMAlgoType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    RMAlgoType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const RMAlgoType& o) const { return value < o.value; }
    bool operator>(const RMAlgoType& o) const { return value > o.value; }
    bool operator<=(const RMAlgoType& o) const { return value <= o.value; }
    bool operator>=(const RMAlgoType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct RadarModeType 
 *
 *  Enumeration used to represent RadarModeTypes
 */
struct RadarModeType
{
    //! The enumerations allowed
    enum
    {
        MODE_INVALID = 0,
        MODE_NOT_SET = 1,
        MODE_SPOTLIGHT = 2,
        MODE_STRIPMAP = 3,
        MODE_DYNAMIC_STRIPMAP = 4
    };

    //! Default constructor
    RadarModeType(){ value = MODE_INVALID; }

    //! string constructor
    RadarModeType(std::string s)
    {
        if (s == "MODE_INVALID")
            value = MODE_INVALID;
        else if (s == "MODE_NOT_SET")
            value = MODE_NOT_SET;
        else if (s == "MODE_SPOTLIGHT")
            value = MODE_SPOTLIGHT;
        else if (s == "MODE_STRIPMAP")
            value = MODE_STRIPMAP;
        else if (s == "MODE_DYNAMIC_STRIPMAP")
            value = MODE_DYNAMIC_STRIPMAP;
        else
            value = MODE_INVALID;
    }

    //! int constructor
    RadarModeType(int i)
    {
        switch(i)
        {
        case 0:
            value = MODE_INVALID;
            break;
        case 1:
            value = MODE_NOT_SET;
            break;
        case 2:
            value = MODE_SPOTLIGHT;
            break;
        case 3:
            value = MODE_STRIPMAP;
            break;
        case 4:
            value = MODE_DYNAMIC_STRIPMAP;
            break;
        default:
            value = MODE_INVALID;
        }
    }

    //! destructor
    ~RadarModeType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "MODE_INVALID";
        case 1:
            return "MODE_NOT_SET";
        case 2:
            return "MODE_SPOTLIGHT";
        case 3:
            return "MODE_STRIPMAP";
        case 4:
            return "MODE_DYNAMIC_STRIPMAP";
        default:
            return "MODE_INVALID";
        }
    }

    //! assignment operator
    RadarModeType& operator=(const RadarModeType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const RadarModeType& o) const { return value == o.value; }
    bool operator!=(const RadarModeType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    RadarModeType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const RadarModeType& o) const { return value < o.value; }
    bool operator>(const RadarModeType& o) const { return value > o.value; }
    bool operator<=(const RadarModeType& o) const { return value <= o.value; }
    bool operator>=(const RadarModeType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct RegionType 
 *
 *  Enumeration used to represent RegionTypes
 */
struct RegionType
{
    //! The enumerations allowed
    enum
    {
        REGION_NOT_SET = 0,
        REGION_SUB_REGION = 1,
        REGION_GEOGRAPHIC_INFO = 2
    };

    //! Default constructor
    RegionType(){ value = REGION_NOT_SET; }

    //! string constructor
    RegionType(std::string s)
    {
        if (s == "REGION_NOT_SET")
            value = REGION_NOT_SET;
        else if (s == "REGION_SUB_REGION")
            value = REGION_SUB_REGION;
        else if (s == "REGION_GEOGRAPHIC_INFO")
            value = REGION_GEOGRAPHIC_INFO;
        else
            value = REGION_NOT_SET;
    }

    //! int constructor
    RegionType(int i)
    {
        switch(i)
        {
        case 0:
            value = REGION_NOT_SET;
            break;
        case 1:
            value = REGION_SUB_REGION;
            break;
        case 2:
            value = REGION_GEOGRAPHIC_INFO;
            break;
        default:
            value = REGION_NOT_SET;
        }
    }

    //! destructor
    ~RegionType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "REGION_NOT_SET";
        case 1:
            return "REGION_SUB_REGION";
        case 2:
            return "REGION_GEOGRAPHIC_INFO";
        default:
            return "REGION_NOT_SET";
        }
    }

    //! assignment operator
    RegionType& operator=(const RegionType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const RegionType& o) const { return value == o.value; }
    bool operator!=(const RegionType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    RegionType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const RegionType& o) const { return value < o.value; }
    bool operator>(const RegionType& o) const { return value > o.value; }
    bool operator<=(const RegionType& o) const { return value <= o.value; }
    bool operator>=(const RegionType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct RowColEnum 
 *
 *  Enumeration used to represent RowColEnums
 */
struct RowColEnum
{
    //! The enumerations allowed
    enum
    {
        RC_ROW = 0,
        RC_COL = 1
    };

    //! Default constructor
    RowColEnum(){ value = RC_ROW; }

    //! string constructor
    RowColEnum(std::string s)
    {
        if (s == "RC_ROW")
            value = RC_ROW;
        else if (s == "RC_COL")
            value = RC_COL;
        else
            value = RC_ROW;
    }

    //! int constructor
    RowColEnum(int i)
    {
        switch(i)
        {
        case 0:
            value = RC_ROW;
            break;
        case 1:
            value = RC_COL;
            break;
        default:
            value = RC_ROW;
        }
    }

    //! destructor
    ~RowColEnum(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "RC_ROW";
        case 1:
            return "RC_COL";
        default:
            return "RC_ROW";
        }
    }

    //! assignment operator
    RowColEnum& operator=(const RowColEnum& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const RowColEnum& o) const { return value == o.value; }
    bool operator!=(const RowColEnum& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    RowColEnum& operator=(const int& o) { value = o; return *this; }
    bool operator<(const RowColEnum& o) const { return value < o.value; }
    bool operator>(const RowColEnum& o) const { return value > o.value; }
    bool operator<=(const RowColEnum& o) const { return value <= o.value; }
    bool operator>=(const RowColEnum& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct SCPType 
 *
 *  Enumeration used to represent SCPTypes
 */
struct SCPType
{
    //! The enumerations allowed
    enum
    {
        SCP_ROW_COL = 0,
        SCP_RG_AZ = 1
    };

    //! Default constructor
    SCPType(){ value = SCP_ROW_COL; }

    //! string constructor
    SCPType(std::string s)
    {
        if (s == "SCP_ROW_COL")
            value = SCP_ROW_COL;
        else if (s == "SCP_RG_AZ")
            value = SCP_RG_AZ;
        else
            value = SCP_ROW_COL;
    }

    //! int constructor
    SCPType(int i)
    {
        switch(i)
        {
        case 0:
            value = SCP_ROW_COL;
            break;
        case 1:
            value = SCP_RG_AZ;
            break;
        default:
            value = SCP_ROW_COL;
        }
    }

    //! destructor
    ~SCPType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "SCP_ROW_COL";
        case 1:
            return "SCP_RG_AZ";
        default:
            return "SCP_ROW_COL";
        }
    }

    //! assignment operator
    SCPType& operator=(const SCPType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const SCPType& o) const { return value == o.value; }
    bool operator!=(const SCPType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    SCPType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const SCPType& o) const { return value < o.value; }
    bool operator>(const SCPType& o) const { return value > o.value; }
    bool operator<=(const SCPType& o) const { return value <= o.value; }
    bool operator>=(const SCPType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct SideOfTrackType 
 *
 *  Enumeration used to represent SideOfTrackTypes
 */
struct SideOfTrackType
{
    //! The enumerations allowed
    enum
    {
        SIDE_LEFT = -1,
        SIDE_NOT_SET = 0,
        SIDE_RIGHT = 1
    };

    //! Default constructor
    SideOfTrackType(){ value = SIDE_NOT_SET; }

    //! string constructor
    SideOfTrackType(std::string s)
    {
        if (s == "SIDE_LEFT")
            value = SIDE_LEFT;
        else if (s == "SIDE_NOT_SET")
            value = SIDE_NOT_SET;
        else if (s == "SIDE_RIGHT")
            value = SIDE_RIGHT;
        else
            value = SIDE_NOT_SET;
    }

    //! int constructor
    SideOfTrackType(int i)
    {
        switch(i)
        {
        case -1:
            value = SIDE_LEFT;
            break;
        case 0:
            value = SIDE_NOT_SET;
            break;
        case 1:
            value = SIDE_RIGHT;
            break;
        default:
            value = SIDE_NOT_SET;
        }
    }

    //! destructor
    ~SideOfTrackType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case -1:
            return "SIDE_LEFT";
        case 0:
            return "SIDE_NOT_SET";
        case 1:
            return "SIDE_RIGHT";
        default:
            return "SIDE_NOT_SET";
        }
    }

    //! assignment operator
    SideOfTrackType& operator=(const SideOfTrackType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const SideOfTrackType& o) const { return value == o.value; }
    bool operator!=(const SideOfTrackType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    SideOfTrackType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const SideOfTrackType& o) const { return value < o.value; }
    bool operator>(const SideOfTrackType& o) const { return value > o.value; }
    bool operator<=(const SideOfTrackType& o) const { return value <= o.value; }
    bool operator>=(const SideOfTrackType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct SlowTimeBeamCompensationType 
 *
 *  Enumeration used to represent SlowTimeBeamCompensationTypes
 */
struct SlowTimeBeamCompensationType
{
    //! The enumerations allowed
    enum
    {
        SLOW_TIME_BEAM_NO = 0,
        SLOW_TIME_BEAM_GLOBAL = 1,
        SLOW_TIME_BEAM_SV = 2
    };

    //! Default constructor
    SlowTimeBeamCompensationType(){ value = SLOW_TIME_BEAM_NO; }

    //! string constructor
    SlowTimeBeamCompensationType(std::string s)
    {
        if (s == "SLOW_TIME_BEAM_NO")
            value = SLOW_TIME_BEAM_NO;
        else if (s == "SLOW_TIME_BEAM_GLOBAL")
            value = SLOW_TIME_BEAM_GLOBAL;
        else if (s == "SLOW_TIME_BEAM_SV")
            value = SLOW_TIME_BEAM_SV;
        else
            value = SLOW_TIME_BEAM_NO;
    }

    //! int constructor
    SlowTimeBeamCompensationType(int i)
    {
        switch(i)
        {
        case 0:
            value = SLOW_TIME_BEAM_NO;
            break;
        case 1:
            value = SLOW_TIME_BEAM_GLOBAL;
            break;
        case 2:
            value = SLOW_TIME_BEAM_SV;
            break;
        default:
            value = SLOW_TIME_BEAM_NO;
        }
    }

    //! destructor
    ~SlowTimeBeamCompensationType(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "SLOW_TIME_BEAM_NO";
        case 1:
            return "SLOW_TIME_BEAM_GLOBAL";
        case 2:
            return "SLOW_TIME_BEAM_SV";
        default:
            return "SLOW_TIME_BEAM_NO";
        }
    }

    //! assignment operator
    SlowTimeBeamCompensationType& operator=(const SlowTimeBeamCompensationType& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const SlowTimeBeamCompensationType& o) const { return value == o.value; }
    bool operator!=(const SlowTimeBeamCompensationType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    SlowTimeBeamCompensationType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const SlowTimeBeamCompensationType& o) const { return value < o.value; }
    bool operator>(const SlowTimeBeamCompensationType& o) const { return value > o.value; }
    bool operator<=(const SlowTimeBeamCompensationType& o) const { return value <= o.value; }
    bool operator>=(const SlowTimeBeamCompensationType& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

/*!
 *  \struct XYZEnum 
 *
 *  Enumeration used to represent XYZEnums
 */
struct XYZEnum
{
    //! The enumerations allowed
    enum
    {
        XYZ_X = 0,
        XYZ_Y = 1,
        XYZ_Z = 2
    };

    //! Default constructor
    XYZEnum(){ value = XYZ_X; }

    //! string constructor
    XYZEnum(std::string s)
    {
        if (s == "XYZ_X")
            value = XYZ_X;
        else if (s == "XYZ_Y")
            value = XYZ_Y;
        else if (s == "XYZ_Z")
            value = XYZ_Z;
        else
            value = XYZ_X;
    }

    //! int constructor
    XYZEnum(int i)
    {
        switch(i)
        {
        case 0:
            value = XYZ_X;
            break;
        case 1:
            value = XYZ_Y;
            break;
        case 2:
            value = XYZ_Z;
            break;
        default:
            value = XYZ_X;
        }
    }

    //! destructor
    ~XYZEnum(){}

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 0:
            return "XYZ_X";
        case 1:
            return "XYZ_Y";
        case 2:
            return "XYZ_Z";
        default:
            return "XYZ_X";
        }
    }

    //! assignment operator
    XYZEnum& operator=(const XYZEnum& o)
    {
        if (&o != this)
        {
            value = o.value;
        }
        return *this;
    }

    bool operator==(const XYZEnum& o) const { return value == o.value; }
    bool operator!=(const XYZEnum& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    XYZEnum& operator=(const int& o) { value = o; return *this; }
    bool operator<(const XYZEnum& o) const { return value < o.value; }
    bool operator>(const XYZEnum& o) const { return value > o.value; }
    bool operator<=(const XYZEnum& o) const { return value <= o.value; }
    bool operator>=(const XYZEnum& o) const { return value >= o.value; }
    operator int() const { return value; }

    int value;

};

// code auto-generated 2010-05-26 15:37:44.078483

}

#endif
