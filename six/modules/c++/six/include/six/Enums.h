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

#include <string>
#include <map>

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>

namespace six
{

const int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

namespace details
{
    // Base type for all enums; avoids code duplication
    template<typename T>
    class Enum
    {
        static std::map<std::string, int> to_string_to_int(const std::map<int, std::string>& int_to_string)
        {
            std::map<std::string, int> retval;
            for (const auto& p : int_to_string)
            {
                retval[p.second] = p.first;
            }
            return retval;
        }
        static const std::map<int, std::string>& int_to_string()
        {
            return T::int_to_string_();
        }
        static const std::map<std::string, int>& string_to_int()
        {
            static const std::map<std::string, int> retval = to_string_to_int(int_to_string());
            return retval;
        }

    protected:
        Enum() = default;

        //! string constructor
        Enum(const std::string& s)
        {
            const auto it = string_to_int().find(s);
            if (it != string_to_int().end())
            {
                value = it->second;
            }
            else
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));
        }

        //! int constructor
        Enum(int i)
        {
            const auto it = int_to_string().find(i);
            if (it != int_to_string().end())
            {
                value = it->first;
            }
            else
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));
        }

    public:
        //! Returns string representation of the value
        std::string toString(bool throw_if_not_set = false) const
        {
            if (throw_if_not_set && (value == NOT_SET_VALUE))
            {
                throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
            }

            const auto it = int_to_string().find(value);
            if (it != int_to_string().end())
            {
                return it->second;
            }

            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));
        }

        operator int() const { return value; }
        operator std::string() const { return toString(); }
        static size_t size() { return int_to_string().size(); }

        // needed for SWIG
        bool operator==(const int& o) const { return value == o; }
        bool operator==(const Enum& o) const { return value == o.value; }
        bool operator!=(const int& o) const { return value != o; }
        bool operator!=(const Enum& o) const { return value != o.value; }
        bool operator<(const int& o) const { return value < o; }
        bool operator<(const Enum& o) const { return value < o.value; }
        bool operator<=(const int& o) const { return value <= o; }
        bool operator<=(const Enum& o) const { return value <= o.value; }
        bool operator>(const int& o) const { return value > o; }
        bool operator>(const Enum& o) const { return value > o.value; }
        bool operator>=(const int& o) const { return value >= o; }
        bool operator>=(const Enum& o) const { return value >= o.value; }

        int value = NOT_SET_VALUE;
    };
 
    #define SIX_Enum_map_entry_(n) { n, #n }

    // Generate an enum class derived from details::Enum
    // There are a few examples of expanded code below.
    #define SIX_Enum_struct_1_(name) struct name final : public details::Enum<name> { \
            name() = default; name(const std::string& s) : Enum(s) {} name(int i) : Enum(i) {} \
            name& operator=(const int& o) { value = o; return *this; } enum {
    #define SIX_Enum_struct_2_ NOT_SET = six::NOT_SET_VALUE }; \
            static const std::map<int, std::string>& int_to_string_() { \
	      static const std::map<int, std::string> retval{ 
    #define SIX_Enum_struct_3_ SIX_Enum_map_entry_(NOT_SET) }; return retval; } }

    #define SIX_Enum_DECLARE_ENUM_1(name, n, v) SIX_Enum_struct_1_(name) \
        n = v, SIX_Enum_struct_2_ \
        { n, #n }, SIX_Enum_struct_3_
    #define SIX_Enum_DECLARE_ENUM_2(name, n1, v1, n2, v2) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, SIX_Enum_struct_2_ \
        { n1, #n1 }, {n2, #n2}, SIX_Enum_struct_3_
    #define SIX_Enum_DECLARE_ENUM_3(name, n1, v1, n2, v2, n3, v3) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, n3 = v3, SIX_Enum_struct_2_ \
        { n1, #n1 }, {n2, #n2}, {n3, #n3}, SIX_Enum_struct_3_
    #define SIX_Enum_DECLARE_ENUM_4(name, n1, v1, n2, v2, n3, v3, n4, v4) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, SIX_Enum_struct_2_ \
        { n1, #n1 }, {n2, #n2}, {n3, #n3}, {n4, #n4}, SIX_Enum_struct_3_
    #define SIX_Enum_DECLARE_ENUM_5(name, n1, v1, n2, v2, n3, v3, n4, v4, n5, v5) SIX_Enum_struct_1_(name) \
        n1 = v1, n2 = v2, n3 = v3, n4 = v4, n5 = v5, SIX_Enum_struct_2_ \
        { n1, #n1 }, {n2, #n2}, {n3, #n3}, {n4, #n4}, {n5, #n5}, SIX_Enum_struct_3_
} // namespace details

/*!
 *  \struct AppliedType
 *
 *  Enumeration used to represent AppliedTypes
 */
SIX_Enum_DECLARE_ENUM_2(AppliedType,
    IS_FALSE, 0,
    IS_TRUE, 1
);

/*!
 *  \struct AutofocusType
 *
 *  Enumeration used to represent AutofocusTypes
 */
SIX_Enum_DECLARE_ENUM_3(AutofocusType,
    NO, 0,
    GLOBAL, 1,
    SV, 2
);


/*!
 *  \struct BooleanType
 *
 *  Enumeration used to represent BooleanTypes
 */
SIX_Enum_DECLARE_ENUM_2(BooleanType,
    IS_FALSE, 0,
    IS_TRUE, 1
);

/*!
 *  \struct ByteSwapping
 *
 *  Enumeration used to represent ByteSwappings
 */
SIX_Enum_DECLARE_ENUM_3(ByteSwapping,
    SWAP_OFF, 0,
    SWAP_ON, 1,
    SWAP_AUTO, 2
);


/*!
 *  \struct CollectType
 *
 *  Enumeration used to represent CollectTypes
 */
SIX_Enum_DECLARE_ENUM_2(CollectType,
    MONOSTATIC, 1,
    BISTATIC, 2
);

/*!
 *  \struct ComplexImageGridType
 *
 *  Enumeration used to represent ComplexImageGridTypes
 */
SIX_Enum_DECLARE_ENUM_5(ComplexImageGridType,
    RGAZIM, 0,
    RGZERO, 1,
    XRGYCR, 2,
    XCTYAT, 3,
    PLANE, 4
);

/*!
 *  \struct ComplexImagePlaneType
 *
 *  Enumeration used to represent ComplexImagePlaneTypes
 */
SIX_Enum_DECLARE_ENUM_3(ComplexImagePlaneType,
    OTHER, 0,
    SLANT, 1,
    GROUND, 2
);


/*!
 *  \struct DataType
 *
 *  Enumeration used to represent DataTypes
 */
SIX_Enum_DECLARE_ENUM_2(DataType,
    COMPLEX, 1,
    DERIVED, 2
);

/*!
 *  \struct DecimationMethod
 *
 *  Enumeration used to represent DecimationMethods
 */
SIX_Enum_DECLARE_ENUM_4(DecimationMethod,
    NEAREST_NEIGHBOR, 1,
    BILINEAR, 2,
    BRIGHTEST_PIXEL, 3,
    LAGRANGE, 4
);

/*!
 *  \struct DemodType
 *
 *  Enumeration used to represent DemodTypes
 */
SIX_Enum_DECLARE_ENUM_2(DemodType,
    STRETCH, 1,
    CHIRP, 2
);

/*!
 *  \struct DisplayType
 *
 *  Enumeration used to represent DisplayTypes
 */
SIX_Enum_DECLARE_ENUM_2(DisplayType,
    COLOR, 1,
    MONO, 2
);

/*!
 *  \struct DualPolarizationType
 *
 *  Enumeration used to represent DualPolarizationTypes
 */
struct DualPolarizationType final : public details::Enum<DualPolarizationType>
{
    //! The enumerations allowed
    enum
    {
        OTHER = 1,
        V_V = 2,
        V_H = 3,
        V_RHC = 4,
        V_LHC = 5,
        H_V = 6,
        H_H = 7,
        H_RHC = 8,
        H_LHC = 9,
        RHC_RHC = 10,
        RHC_LHC = 11,
        RHC_V = 12,
        RHC_H = 13,
        LHC_RHC = 14,
        LHC_LHC = 15,
        LHC_V = 16,
        LHC_H = 17,
        UNKNOWN = 18,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const std::map<int, std::string>& int_to_string_()
    {
        static const std::map<int, std::string> retval
        {
            SIX_Enum_map_entry_(OTHER),
            SIX_Enum_map_entry_(V_V),
            SIX_Enum_map_entry_(V_H),
            SIX_Enum_map_entry_(V_RHC),
            SIX_Enum_map_entry_(V_LHC),
            SIX_Enum_map_entry_(H_V),
            SIX_Enum_map_entry_(H_H),
            SIX_Enum_map_entry_(H_RHC),
            SIX_Enum_map_entry_(H_LHC),
            SIX_Enum_map_entry_(RHC_RHC),
            SIX_Enum_map_entry_(RHC_LHC),
            SIX_Enum_map_entry_(RHC_V),
            SIX_Enum_map_entry_(RHC_H),
            SIX_Enum_map_entry_(LHC_RHC),
            SIX_Enum_map_entry_(LHC_LHC),
            SIX_Enum_map_entry_(LHC_V),
            SIX_Enum_map_entry_(LHC_H),
            SIX_Enum_map_entry_(UNKNOWN),
            SIX_Enum_map_entry_(NOT_SET)
        };
        return retval;
    }

    DualPolarizationType() = default;
    DualPolarizationType(const std::string& s) : Enum(s) {}
    DualPolarizationType(int i) : Enum(i) {}
    DualPolarizationType& operator=(const int& o) { value = o; return *this; }
};

/*!
 *  \struct EarthModelType
 *
 *  Enumeration used to represent EarthModelTypes
 */
SIX_Enum_DECLARE_ENUM_1(EarthModelType,
    WGS84, 1
);

/*!
 *  \struct FFTSign
 *
 *  Enumeration used to represent FFTSigns
 */
SIX_Enum_DECLARE_ENUM_2(FFTSign,
    NEG, -1,
    POS, 1
);

/*!
 *  \struct ImageBeamCompensationType
 *
 *  Enumeration used to represent ImageBeamCompensationTypes
 */
SIX_Enum_DECLARE_ENUM_2(ImageBeamCompensationType,
    NO, 0,
    SV, 1
);

/*!
 *  \struct ImageFormationType
 *
 *  Enumeration used to represent ImageFormationTypes
 */
SIX_Enum_DECLARE_ENUM_4(ImageFormationType,
    OTHER, 0,
    PFA, 1,
    RMA, 2,
    RGAZCOMP, 3
);

/*!
 *  \struct MagnificationMethod
 *
 *  Enumeration used to represent MagnificationMethods
 */
SIX_Enum_DECLARE_ENUM_3(MagnificationMethod,
    NEAREST_NEIGHBOR, 1,
    BILINEAR, 2,
    LAGRANGE, 3
);

/*!
 *  \struct OrientationType
 *
 *  Enumeration used to represent OrientationTypes
 */
SIX_Enum_DECLARE_ENUM_5(OrientationType,
    UP, 1,
    DOWN, 2,
    LEFT, 3,
    RIGHT, 4,
    ARBITRARY, 5
);

/*!
 *  \struct PixelType
 *
 *  Enumeration used to represent PixelTypes
 */
struct PixelType final : public details::Enum<PixelType>
{
    //! The enumerations allowed
    enum
    {
        RE32F_IM32F = 1,
        RE16I_IM16I = 2,
        AMP8I_PHS8I = 3,
        MONO8I = 4,
        MONO8LU = 5,
        MONO16I = 6,
        RGB8LU = 7,
        RGB24I = 8,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const std::map<int, std::string>& int_to_string_()
    {
        static const std::map<int, std::string> retval
        {
            SIX_Enum_map_entry_(RE32F_IM32F),
            SIX_Enum_map_entry_(RE16I_IM16I),
            SIX_Enum_map_entry_(AMP8I_PHS8I),
            SIX_Enum_map_entry_(MONO8I),
            SIX_Enum_map_entry_(MONO8LU),
            SIX_Enum_map_entry_(MONO16I),
            SIX_Enum_map_entry_(RGB8LU),
            SIX_Enum_map_entry_(RGB24I),
            SIX_Enum_map_entry_(NOT_SET)
        };
        return retval;
    }

    PixelType() = default;
    PixelType(const std::string& s) : Enum(s) {}
    PixelType(int i) : Enum(i) {}
    PixelType& operator=(const int& o) { value = o; return *this; }
};

/*!
 *  \struct PolarizationSequenceType
 *
 *  Enumeration used to represent PolarizationSequenceTypes
 */
struct PolarizationSequenceType final : public details::Enum<PolarizationSequenceType>
{
    //! The enumerations allowed
    enum
    {
        OTHER = 1,
        V = 2,
        H = 3,
        RHC = 4,
        LHC = 5,
        UNKNOWN = 6,
        SEQUENCE = 7,
        NOT_SET = six::NOT_SET_VALUE
    };

    static const std::map<int, std::string>& int_to_string_()
    {
        static const std::map<int, std::string> retval
        {
            SIX_Enum_map_entry_(OTHER),
            SIX_Enum_map_entry_(V),
            SIX_Enum_map_entry_(H),
            SIX_Enum_map_entry_(RHC),
            SIX_Enum_map_entry_(LHC),
            SIX_Enum_map_entry_(UNKNOWN),
            SIX_Enum_map_entry_(SEQUENCE),
            SIX_Enum_map_entry_(NOT_SET)
        };
        return retval;
    }

    PolarizationSequenceType() = default;
    PolarizationSequenceType(const std::string & s) : Enum(s) {}
    PolarizationSequenceType(int i) : Enum(i) {}
    PolarizationSequenceType& operator=(const int& o) { value = o; return *this; }
};


/*!
 *  \struct PolarizationType
 *
 *  Enumeration used to represent PolarizationTypes
 */
SIX_Enum_DECLARE_ENUM_5(PolarizationType,
    OTHER, 1,
    V, 2,
    H, 3,
    RHC, 4,
    LHC, 5
);

/*!
 *  \struct ProjectionType
 *
 *  Enumeration used to represent ProjectionTypes
 */
SIX_Enum_DECLARE_ENUM_4(ProjectionType,
    PLANE, 1,
    GEOGRAPHIC, 2,
    CYLINDRICAL, 3,
    POLYNOMIAL, 4
);

/*!
 *  \struct RMAlgoType
 *
 *  Enumeration used to represent RMAlgoTypes
 */
SIX_Enum_DECLARE_ENUM_3(RMAlgoType,
    OMEGA_K, 1,
    CSA, 2,
    RG_DOP, 3
);

/*!
 *  \struct RadarModeType
 *
 *  Enumeration used to represent RadarModeTypes
 */
SIX_Enum_DECLARE_ENUM_4(RadarModeType,
    SPOTLIGHT, 1,
    STRIPMAP, 2,
    DYNAMIC_STRIPMAP, 3,
    SCANSAR, 4
);

/*!
 *  \struct RegionType
 *
 *  Enumeration used to represent RegionTypes
 */
SIX_Enum_DECLARE_ENUM_2(RegionType,
    SUB_REGION, 1,
    GEOGRAPHIC_INFO, 2
);

/*!
 *  \struct RowColEnum
 *
 *  Enumeration used to represent RowColEnums
 */
SIX_Enum_DECLARE_ENUM_2(RowColEnum,
    ROW, 0,
    COL, 1
);

/*!
 *  \struct SCPType
 *
 *  Enumeration used to represent SCPTypes
 */
SIX_Enum_DECLARE_ENUM_2(SCPType,
    SCP_ROW_COL, 0,
    SCP_RG_AZ, 1
);

/*!
 *  \struct SideOfTrackType
 *
 *  Enumeration used to represent SideOfTrackTypes
 */
SIX_Enum_DECLARE_ENUM_2(SideOfTrackType,
    LEFT, scene::TRACK_LEFT,
    RIGHT, scene::TRACK_RIGHT
);

/*!
 *  \struct SlowTimeBeamCompensationType
 *
 *  Enumeration used to represent SlowTimeBeamCompensationTypes
 */
SIX_Enum_DECLARE_ENUM_3(SlowTimeBeamCompensationType,
    NO, 0,
    GLOBAL, 1,
    SV, 2
);

/*!
 *  \struct XYZEnum
 *
 *  Enumeration used to represent XYZEnums
 */
SIX_Enum_DECLARE_ENUM_3(XYZEnum,
    X, 0,
    Y, 1,
    Z, 2
);

}

