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
#ifndef __CPHD_ENUMS_H__
#define __CPHD_ENUMS_H__

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>

namespace cphd
{
const int NOT_SET_VALUE = std::numeric_limits<int>::max();


/*!
 *  \struct SampleType
 *
 *  Enumeration used to represent SampleTypes
 */
struct SampleType
{
    //! The enumerations allowed
    enum
    {
        RE32F_IM32F = 1,
        RE16I_IM16I = 2,
        RE08I_IM08I = 3,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    SampleType() :
        value(NOT_SET)
    {
    }

    //! string constructor
    SampleType(const std::string& s)
    {
        if (s == "RE32F_IM32F")
            value = RE32F_IM32F;
        else if (s == "RE16I_IM16I")
            value = RE16I_IM16I;
        else if (s == "RE08I_IM08I")
            value = RE08I_IM08I;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    SampleType(int i)
    {
        switch(i)
        {
        case 1:
            value = RE32F_IM32F;
            break;
        case 2:
            value = RE16I_IM16I;
            break;
        case 3:
            value = RE08I_IM08I;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("RE32F_IM32F");
        case 2:
            return std::string("RE16I_IM16I");
        case 3:
            return std::string("RE08I_IM08I");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
    }

    bool operator==(const SampleType& o) const { return value == o.value; }
    bool operator!=(const SampleType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    SampleType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const SampleType& o) const { return value < o.value; }
    bool operator>(const SampleType& o) const { return value > o.value; }
    bool operator<=(const SampleType& o) const { return value <= o.value; }
    bool operator>=(const SampleType& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    int value;
};

/*!
 *  \struct DomainType
 *
 *  Enumeration used to represent DomainTypes
 */
struct DomainType
{
    //! The enumerations allowed
    enum
    {
        FX = 1,
        TOA = 2,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    DomainType() :
        value(NOT_SET)
    {
    }

    //! string constructor
    DomainType(const std::string& s)
    {
        if (s == "FX")
            value = FX;
        else if (s == "TOA")
            value = TOA;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    DomainType(int i)
    {
        switch(i)
        {
        case 1:
            value = FX;
            break;
        case 2:
            value = TOA;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("FX");
        case 2:
            return std::string("TOA");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }
    }

    bool operator==(const DomainType& o) const { return value == o.value; }
    bool operator!=(const DomainType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    DomainType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const DomainType& o) const { return value < o.value; }
    bool operator>(const DomainType& o) const { return value > o.value; }
    bool operator<=(const DomainType& o) const { return value <= o.value; }
    bool operator>=(const DomainType& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    int value;
};


/*!
 *  \struct PhaseSGN
 *
 *  Enumeration used to represent PhaseSGNs
 */
struct PhaseSGN
{
    //! The enumerations allowed
    enum
    {
        MINUS_1 = -1,
        PLUS_1 = 1,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    PhaseSGN() :
        value(NOT_SET)
    {
    }

    //! string constructor
    PhaseSGN(const std::string& s)
    {
        if (s == "-1")
            value = MINUS_1;
        else if (s == "+1")
            value = PLUS_1;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    PhaseSGN(int i)
    {
        switch(i)
        {
        case -1:
            value = MINUS_1;
            break;
        case 1:
            value = PLUS_1;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case -1:
            return std::string("-1");
        case 1:
            return std::string("+1");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
    }

    bool operator==(const PhaseSGN& o) const { return value == o.value; }
    bool operator!=(const PhaseSGN& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    PhaseSGN& operator=(const int& o) { value = o; return *this; }
    bool operator<(const PhaseSGN& o) const { return value < o.value; }
    bool operator>(const PhaseSGN& o) const { return value > o.value; }
    bool operator<=(const PhaseSGN& o) const { return value <= o.value; }
    bool operator>=(const PhaseSGN& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    int value;
};

/*!
 *  \struct RefHeight
 *
 *  Enumeration used to represent RefHeights
 */
struct RefHeight
{
    //! The enumerations allowed
    enum
    {
        IARP = 1,
        ZERO = 2,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    RefHeight() :
        value(NOT_SET)
    {
    }

    //! string constructor
    RefHeight(const std::string& s)
    {
        if (s == "IARP")
            value = IARP;
        else if (s == "ZERO")
            value = ZERO;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    RefHeight(int i)
    {
        switch(i)
        {
        case 1:
            value = IARP;
            break;
        case 2:
            value = ZERO;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("IARP");
        case 2:
            return std::string("ZERO");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
    }

    bool operator==(const RefHeight& o) const { return value == o.value; }
    bool operator!=(const RefHeight& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    RefHeight& operator=(const int& o) { value = o; return *this; }
    bool operator<(const RefHeight& o) const { return value < o.value; }
    bool operator>(const RefHeight& o) const { return value > o.value; }
    bool operator<=(const RefHeight& o) const { return value <= o.value; }
    bool operator>=(const RefHeight& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    int value;
};

/*!
 *  \struct EarthModelType
 *
 *  Enumeration used to represent EarthModelType
 *  Distict from SICD's EarthModelType
 *  (underscore vs. no underscore)
 */
struct EarthModelType
{
    //! The enumerations allowed
    enum
    {
        WGS_84= 1,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    EarthModelType() :
        value(NOT_SET)
    {
    }

    //! string constructor
    EarthModelType(const std::string& s)
    {
        if (s == "WGS_84")
            value = WGS_84;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    EarthModelType(int i)
    {
        switch(i)
        {
        case 1:
            value = WGS_84;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("WGS_84");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
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
    operator std::string() const { return toString(); }

    int value;
};

/*!
 * \struct SignalArrayFormat
 *
 * Enumeration used to represent signal array format
 */
struct SignalArrayFormat
{
    //! The enumerations allowed
    enum
    {
        CI2 = 1,
        CI4 = 2,
        CF8 = 3,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    SignalArrayFormat() :
        value(NOT_SET)
    {
    }

    //! string constructor
    SignalArrayFormat(const std::string& s)
    {
        if (s == "CI2")
            value = CI2;
        else if (s == "CI4")
            value = CI4;
        else if (s == "CF8")
            value = CF8;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    SignalArrayFormat(int i)
    {
        switch(i)
        {
        case 1:
            value = CI2;
            break;
        case 2:
            value = CI4;
            break;
        case 3:
            value = CF8;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("CI2");
        case 2:
            return std::string("CI4");
        case 3:
            return std::string("CF8");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
    }

    bool operator==(const SignalArrayFormat& o) const { return value == o.value; }
    bool operator!=(const SignalArrayFormat& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    SignalArrayFormat& operator=(const int& o) { value = o; return *this; }
    bool operator<(const SignalArrayFormat& o) const { return value < o.value; }
    bool operator>(const SignalArrayFormat& o) const { return value > o.value; }
    bool operator<=(const SignalArrayFormat& o) const { return value <= o.value; }
    bool operator>=(const SignalArrayFormat& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

    int value;
};
/*!
 *  \struct SRPType
 *
 *  Enumeration used to represent SRPTypes
 */
struct SRPType
{
    //! The enumerations allowed
    enum
    {
        FIXEDPT = 1,
        PVTPOLY = 2,
        PVVPOLY = 3,
        STEPPED = 4,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    SRPType() :
        value(NOT_SET)
    {
    }

    //! string constructor
    SRPType(const std::string& s)
    {
        if (s == "FIXEDPT")
            value = FIXEDPT;
        else if (s == "PVTPOLY")
            value = PVTPOLY;
        else if (s == "PVVPOLY")
            value = PVVPOLY;
        else if (s == "STEPPED")
            value = STEPPED;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    SRPType(int i)
    {
        switch(i)
        {
        case 1:
            value = FIXEDPT;
            break;
        case 2:
            value = PVTPOLY;
            break;
        case 3:
            value = PVVPOLY;
            break;
        case 4:
            value = STEPPED;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("FIXEDPT");
        case 2:
            return std::string("PVTPOLY");
        case 3:
            return std::string("PVVPOLY");
        case 4:
            return std::string("STEPPED");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
    }

    bool operator==(const SRPType& o) const { return value == o.value; }
    bool operator!=(const SRPType& o) const { return value != o.value; }
    bool operator==(const int& o) const { return value == o; }
    bool operator!=(const int& o) const { return value != o; }
    SRPType& operator=(const int& o) { value = o; return *this; }
    bool operator<(const SRPType& o) const { return value < o.value; }
    bool operator>(const SRPType& o) const { return value > o.value; }
    bool operator<=(const SRPType& o) const { return value <= o.value; }
    bool operator>=(const SRPType& o) const { return value >= o.value; }
    operator int() const { return value; }
    operator std::string() const { return toString(); }

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
        X = 1,
        Y = 2,
        V = 3,
        H = 4,
        RHC = 5,
        LHC = 6,
        UNSPECIFIED = 7,
        NOT_SET = cphd::NOT_SET_VALUE
    };

    //! Default constructor
    PolarizationType() :
        value(NOT_SET)
    {
    }

    //! string constructor
    PolarizationType(const std::string& s)
    {
        if (s == "X")
            value = X;
        else if (s == "Y")
            value = Y;
        else if (s == "V")
            value = V;
        else if (s == "H")
            value = H;
        else if (s == "RHC")
            value = RHC;
        else if (s == "LHC")
            value = LHC;
        else if (s == "UNSPECIFIED")
            value = UNSPECIFIED;
        else if (s == "NOT_SET")
            value = NOT_SET;
        else
        {
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + s));
        }
    }

    //! int constructor
    PolarizationType(int i)
    {
        switch(i)
        {
        case 1:
            value = X;
            break;
        case 2:
            value = Y;
            break;
        case 3:
            value = V;
            break;
        case 4:
            value = H;
            break;
        case 5:
            value = RHC;
            break;
        case 6:
            value = LHC;
            break;
        case 7:
            value = UNSPECIFIED;
            break;
        case cphd::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(i)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch(value)
        {
        case 1:
            return std::string("X");
        case 2:
            return std::string("Y");
        case 3:
            return std::string("V");
        case 4:
            return std::string("H");
        case 5:
            return std::string("RHC");
        case 6:
            return std::string("LHC");
        case 7:
            return std::string("UNSPECIFIED");
        case cphd::NOT_SET_VALUE:
            return std::string("NOT_SET");
        default:
            throw except::InvalidFormatException(Ctxt(
                    "Invalid enum value: " + str::toString(value)));
        }
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
    operator std::string() const { return toString(); }

    int value;
};


}

#endif
