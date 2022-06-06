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
#ifndef __SIX_ENUMS_H__
#define __SIX_ENUMS_H__
#pragma once

#include "Enum.h"

namespace six
{

/*!
 *  \struct AppliedType
 *
 *  Enumeration used to represent AppliedTypes
 */
SIX_Enum_ENUM_2(AppliedType,
    IS_FALSE, 0,
    IS_TRUE, 1
);

/*!
 *  \struct AutofocusType
 *
 *  Enumeration used to represent AutofocusTypes
 */
SIX_Enum_ENUM_3(AutofocusType,
    NO, 0,
    GLOBAL, 1,
    SV, 2
);


/*!
 *  \struct BooleanType
 *
 *  Enumeration used to represent BooleanTypes
 */
SIX_Enum_ENUM_2(BooleanType,
    IS_FALSE, 0,
    IS_TRUE, 1
);

/*!
 *  \struct ByteSwapping
 *
 *  Enumeration used to represent ByteSwappings
 */
SIX_Enum_ENUM_3(ByteSwapping,
    SWAP_OFF, 0,
    SWAP_ON, 1,
    SWAP_AUTO, 2
);


/*!
 *  \struct CollectType
 *
 *  Enumeration used to represent CollectTypes
 */
SIX_Enum_ENUM_2(CollectType,
    MONOSTATIC, 1,
    BISTATIC, 2
);

/*!
 *  \struct ComplexImageGridType
 *
 *  Enumeration used to represent ComplexImageGridTypes
 */
SIX_Enum_ENUM_5(ComplexImageGridType,
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
SIX_Enum_ENUM_3(ComplexImagePlaneType,
    OTHER, 0,
    SLANT, 1,
    GROUND, 2
);


/*!
 *  \struct DataType
 *
 *  Enumeration used to represent DataTypes
 */
SIX_Enum_ENUM_2(DataType,
    COMPLEX, 1,
    DERIVED, 2
);

/*!
 *  \struct DecimationMethod
 *
 *  Enumeration used to represent DecimationMethods
 */
SIX_Enum_ENUM_4(DecimationMethod,
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
SIX_Enum_ENUM_2(DemodType,
    STRETCH, 1,
    CHIRP, 2
);

/*!
 *  \struct DisplayType
 *
 *  Enumeration used to represent DisplayTypes
 */
SIX_Enum_ENUM_2(DisplayType,
    COLOR, 1,
    MONO, 2
);

/*!
 *  \struct EarthModelType
 *
 *  Enumeration used to represent EarthModelTypes
 */
SIX_Enum_ENUM_1(EarthModelType,
    WGS84, 1
);

/*!
 *  \struct FFTSign
 *
 *  Enumeration used to represent FFTSigns
 */
SIX_Enum_ENUM_2(FFTSign,
    NEG, -1,
    POS, 1
);

/*!
 *  \struct ImageBeamCompensationType
 *
 *  Enumeration used to represent ImageBeamCompensationTypes
 */
SIX_Enum_ENUM_2(ImageBeamCompensationType,
    NO, 0,
    SV, 1
);

/*!
 *  \struct ImageFormationType
 *
 *  Enumeration used to represent ImageFormationTypes
 */
SIX_Enum_ENUM_4(ImageFormationType,
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
SIX_Enum_ENUM_3(MagnificationMethod,
    NEAREST_NEIGHBOR, 1,
    BILINEAR, 2,
    LAGRANGE, 3
);

/*!
 *  \struct OrientationType
 *
 *  Enumeration used to represent OrientationTypes
 */
SIX_Enum_ENUM_5(OrientationType,
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
SIX_Enum_BEGIN_DEFINE(PixelType)
    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        // Each pixel is stored as a pair of numbers that represent the realand imaginary
        // components. Each component is stored in a 32-bit IEEE floating point format (4
        // bytes per component, 8 bytes per pixel).
        RE32F_IM32F = 1,

        // Each pixel is stored as a pair of numbers that represent the real and imaginary 
        // components. Each component is stored in a 16-bit signed integer in 2’s 
        // complement format (2 bytes per component, 4 bytes per pixel). 
        RE16I_IM16I = 2,

        // Each pixel is stored as a pair of numbers that represent the amplitude and phase
        // components. Each component is stored in an 8-bit unsigned integer (1 byte per 
        // component, 2 bytes per pixel). 
        AMP8I_PHS8I = 3,

        MONO8I = 4,
        MONO8LU = 5,
        MONO16I = 6,
        RGB8LU = 7,
        RGB24I = 8,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_int
            SIX_Enum_map_entry_(RE32F_IM32F),
            SIX_Enum_map_entry_(RE16I_IM16I),
            SIX_Enum_map_entry_(AMP8I_PHS8I),
            SIX_Enum_map_entry_(MONO8I),
            SIX_Enum_map_entry_(MONO8LU),
            SIX_Enum_map_entry_(MONO16I),
            SIX_Enum_map_entry_(RGB8LU),
            SIX_Enum_map_entry_(RGB24I),
    SIX_Enum_END_string_to_int
SIX_Enum_END_DEFINE(PixelType);

/*!
 *  \struct PolarizationSequenceType
 *
 *  Enumeration used to represent PolarizationSequenceTypes
 */
SIX_Enum_BEGIN_DEFINE(PolarizationSequenceType)
private:
    std::string other_; // valid of OTHER.* for SIDD 3.0/SICD 1.3
    std::string toString_(bool throw_if_not_set) const override; // handle OTHER.* for SIDD 3.0/SICD 1.3
public:
    static PolarizationSequenceType toType_imp_(const std::string&); // handle OTHER.* for SIDD 3.0/SICD 1.3

    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        OTHER = 1,
        V = 2,
        H = 3,
        X = 8, // SIDD 3.0, SICD 1.3
        Y, // SIDD 3.0, SICD 1.3
        S, // SIDD 3.0, SICD 1.3
        E, // SIDD 3.0, SICD 1.3
        RHC = 4,
        LHC = 5,
        UNKNOWN = 6,
        SEQUENCE = 7,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_int
            SIX_Enum_map_entry_(OTHER),
            SIX_Enum_map_entry_(V),
            SIX_Enum_map_entry_(H),
            SIX_Enum_map_entry_(X),
            SIX_Enum_map_entry_(Y),
            SIX_Enum_map_entry_(S),
            SIX_Enum_map_entry_(E),
            SIX_Enum_map_entry_(RHC),
            SIX_Enum_map_entry_(LHC),
            SIX_Enum_map_entry_(UNKNOWN),
            SIX_Enum_map_entry_(SEQUENCE),
    SIX_Enum_END_string_to_int
SIX_Enum_END_DEFINE(PolarizationSequenceType);


/*!
 *  \struct PolarizationType
 *
 *  Enumeration used to represent PolarizationTypes
 */
SIX_Enum_BEGIN_DEFINE(PolarizationType)
private:
    std::string other_; // valid of OTHER.* for SIDD 3.0/SICD 1.3
    std::string toString_(bool throw_if_not_set) const override; // handle OTHER.* for SIDD 3.0/SICD 1.3
public:
    static PolarizationType toType_imp_(const std::string&); // handle OTHER.* for SIDD 3.0/SICD 1.3

    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        OTHER = 1,
        V = 2,
        H = 3,
        X = 7, // SICD 1.3
        Y, // SICD 1.3
        S, // SICD 1.3
        E, // SICD 1.3
        RHC = 4,
        LHC = 5,
        UNKNOWN = 6,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_int
        SIX_Enum_map_entry_(OTHER),
        SIX_Enum_map_entry_(V),
        SIX_Enum_map_entry_(H),
        SIX_Enum_map_entry_(X),
        SIX_Enum_map_entry_(Y),
        SIX_Enum_map_entry_(S),
        SIX_Enum_map_entry_(E),
        SIX_Enum_map_entry_(RHC),
        SIX_Enum_map_entry_(LHC),
        SIX_Enum_map_entry_(UNKNOWN),
    SIX_Enum_END_string_to_int
SIX_Enum_END_DEFINE(PolarizationType);

/*!
 *  \struct DualPolarizationType
 *
 *  Enumeration used to represent DualPolarizationTypes
 */
SIX_Enum_BEGIN_DEFINE(DualPolarizationType)
private:
    PolarizationType left_,right_; // other than OTHER/NOT_SET/UNKNOWN, this is two `PolarizationType`s
    std::string other_; // handle OTHER.* for SIDD 3.0/SICD 1.3
    std::string toString_(bool throw_if_not_set) const override; // handle OTHER.* for SIDD 3.0/SICD 1.3
public:
    static DualPolarizationType toType_imp_(const std::string&); // handle OTHER.* for SIDD 3.0/SICD 1.3

    //! The enumerations allowed
    SIX_Enum_BEGIN_enum
        // SICD 1.3 add a few more allowable values, which means the number of combinations explodes.
        // Keeping integer values the same in case there's some code somewhere that depends on it.
        OTHER = 1,

        V_V = 2,
        V_H = 3,
        V_X = 19, // SICD 1.3
        V_Y, // SICD 1.3
        V_S, // SICD 1.3
        V_E, // SICD 1.3
        V_RHC = 4,
        V_LHC = 5,
        V_OTHER = V_E+1, // SICD 1.3

        H_V = 6,
        H_H = 7,
        H_X = V_OTHER +1, // SICD 1.3
        H_Y, // SICD 1.3
        H_S, // SICD 1.3
        H_E, // SICD 1.3
        H_RHC = 8,
        H_LHC = 9,
        H_OTHER = H_E + 1, // SICD 1.3

        X_V = H_OTHER +1, // SICD 1.3
        X_H,
        X_X,
        X_Y,
        X_S,
        X_E,
        X_RHC,
        X_LHC,
        X_OTHER,

        Y_V = X_OTHER+1, // SICD 1.3
        Y_H,
        Y_X,
        Y_Y,
        Y_S,
        Y_E,
        Y_RHC,
        Y_LHC,
        Y_OTHER,

        S_V = Y_OTHER +1, // SICD 1.3
        S_H,
        S_X,
        S_Y,
        S_S,
        S_E,
        S_RHC,
        S_LHC,
        S_OTHER,

        E_V = S_OTHER +1, // SICD 1.3
        E_H,
        E_X,
        E_Y,
        E_S,
        E_E,
        E_RHC,
        E_LHC,
        E_OTHER,

        RHC_V = 12,
        RHC_H = 13,
        RHC_X = E_OTHER +1, // SICD 1.3
        RHC_Y, // SICD 1.3
        RHC_S, // SICD 1.3
        RHC_E, // SICD 1.3
        RHC_RHC = 10,
        RHC_LHC = 11,
        RHC_OTHER = RHC_E + 1, // SICD 1.3

        LHC_V = 16,
        LHC_H = 17,
        LHC_X = RHC_OTHER + 1, // SICD 1.3
        LHC_Y, // SICD 1.3
        LHC_S, // SICD 1.3
        LHC_E, // SICD 1.3
        LHC_RHC = 14,
        LHC_LHC = 15,
        LHC_OTHER = LHC_E + 1,

        OTHER_V = LHC_OTHER + 1, // SICD 1.3
        OTHER_H,
        OTHER_X,
        OTHER_Y,
        OTHER_S,
        OTHER_E,
        OTHER_RHC,
        OTHER_LHC,
        OTHER_OTHER,

        UNKNOWN = 18,
    SIX_Enum_END_enum

    SIX_Enum_BEGIN_string_to_int
            SIX_Enum_map_entry_(OTHER),

            SIX_Enum_map_entry_(V_V),
            SIX_Enum_map_entry_(V_H),
            SIX_Enum_map_entry_(V_X),
            SIX_Enum_map_entry_(V_Y),
            SIX_Enum_map_entry_(V_S),
            SIX_Enum_map_entry_(V_E),
            SIX_Enum_map_entry_(V_RHC),
            SIX_Enum_map_entry_(V_LHC),
            SIX_Enum_map_entry_(V_OTHER),

            SIX_Enum_map_entry_(H_V),
            SIX_Enum_map_entry_(H_H),
            SIX_Enum_map_entry_(H_X),
            SIX_Enum_map_entry_(H_Y),
            SIX_Enum_map_entry_(H_S),
            SIX_Enum_map_entry_(H_E),
            SIX_Enum_map_entry_(H_RHC),
            SIX_Enum_map_entry_(H_LHC),
            SIX_Enum_map_entry_(H_OTHER),

            SIX_Enum_map_entry_(X_V),
            SIX_Enum_map_entry_(X_H),
            SIX_Enum_map_entry_(X_X),
            SIX_Enum_map_entry_(X_Y),
            SIX_Enum_map_entry_(X_S),
            SIX_Enum_map_entry_(X_E),
            SIX_Enum_map_entry_(X_RHC),
            SIX_Enum_map_entry_(X_LHC),
            SIX_Enum_map_entry_(X_OTHER),

            SIX_Enum_map_entry_(Y_V),
            SIX_Enum_map_entry_(Y_H),
            SIX_Enum_map_entry_(Y_X),
            SIX_Enum_map_entry_(Y_Y),
            SIX_Enum_map_entry_(Y_S),
            SIX_Enum_map_entry_(Y_E),
            SIX_Enum_map_entry_(Y_RHC),
            SIX_Enum_map_entry_(Y_LHC),
            SIX_Enum_map_entry_(Y_OTHER),

            SIX_Enum_map_entry_(S_V),
            SIX_Enum_map_entry_(S_H),
            SIX_Enum_map_entry_(S_X),
            SIX_Enum_map_entry_(S_Y),
            SIX_Enum_map_entry_(S_S),
            SIX_Enum_map_entry_(S_E),
            SIX_Enum_map_entry_(S_RHC),
            SIX_Enum_map_entry_(S_LHC),
            SIX_Enum_map_entry_(S_OTHER),

            SIX_Enum_map_entry_(E_V),
            SIX_Enum_map_entry_(E_H),
            SIX_Enum_map_entry_(E_X),
            SIX_Enum_map_entry_(E_Y),
            SIX_Enum_map_entry_(E_S),
            SIX_Enum_map_entry_(E_E),
            SIX_Enum_map_entry_(E_RHC),
            SIX_Enum_map_entry_(E_LHC),
            SIX_Enum_map_entry_(E_OTHER),

            SIX_Enum_map_entry_(RHC_V),
            SIX_Enum_map_entry_(RHC_H),
            SIX_Enum_map_entry_(RHC_X),
            SIX_Enum_map_entry_(RHC_Y),
            SIX_Enum_map_entry_(RHC_S),
            SIX_Enum_map_entry_(RHC_E),
            SIX_Enum_map_entry_(RHC_RHC),
            SIX_Enum_map_entry_(RHC_LHC),
            SIX_Enum_map_entry_(RHC_OTHER),

            SIX_Enum_map_entry_(LHC_V),
            SIX_Enum_map_entry_(LHC_H),
            SIX_Enum_map_entry_(LHC_X),
            SIX_Enum_map_entry_(LHC_Y),
            SIX_Enum_map_entry_(LHC_S),
            SIX_Enum_map_entry_(LHC_E),
            SIX_Enum_map_entry_(LHC_RHC),
            SIX_Enum_map_entry_(LHC_LHC),
            SIX_Enum_map_entry_(LHC_OTHER),

            SIX_Enum_map_entry_(OTHER_V),
            SIX_Enum_map_entry_(OTHER_H),
            SIX_Enum_map_entry_(OTHER_X),
            SIX_Enum_map_entry_(OTHER_Y),
            SIX_Enum_map_entry_(OTHER_S),
            SIX_Enum_map_entry_(OTHER_E),
            SIX_Enum_map_entry_(OTHER_RHC),
            SIX_Enum_map_entry_(OTHER_LHC),
            SIX_Enum_map_entry_(OTHER_OTHER),

            SIX_Enum_map_entry_(UNKNOWN),
    SIX_Enum_END_string_to_int
SIX_Enum_END_DEFINE(DualPolarizationType);
static_assert(six::DualPolarizationType::V_V == 2, "six::DualPolarizationType is wrong!");
static_assert(six::DualPolarizationType::UNKNOWN == 18, "six::DualPolarizationType is wrong!");
static_assert(six::DualPolarizationType::V_X == 19, "six::DualPolarizationType is wrong!"); // SICD 1.3
static_assert(six::DualPolarizationType::OTHER_OTHER == 83, "six::DualPolarizationType is wrong!");

/*!
 *  \struct ProjectionType
 *
 *  Enumeration used to represent ProjectionTypes
 */
SIX_Enum_ENUM_4(ProjectionType,
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
SIX_Enum_ENUM_3(RMAlgoType,
    OMEGA_K, 1,
    CSA, 2,
    RG_DOP, 3
);

/*!
 *  \struct RadarModeType
 *
 *  Enumeration used to represent RadarModeTypes
 */
SIX_Enum_ENUM_4(RadarModeType,
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
SIX_Enum_ENUM_2(RegionType,
    SUB_REGION, 1,
    GEOGRAPHIC_INFO, 2
);

/*!
 *  \struct RowColEnum
 *
 *  Enumeration used to represent RowColEnums
 */
SIX_Enum_ENUM_2(RowColEnum,
    ROW, 0,
    COL, 1
);

/*!
 *  \struct SCPType
 *
 *  Enumeration used to represent SCPTypes
 */
SIX_Enum_ENUM_2(SCPType,
    SCP_ROW_COL, 0,
    SCP_RG_AZ, 1
);

/*!
 *  \struct SideOfTrackType
 *
 *  Enumeration used to represent SideOfTrackTypes
 */
SIX_Enum_ENUM_2(SideOfTrackType,
    LEFT, scene::TRACK_LEFT,
    RIGHT, scene::TRACK_RIGHT
);

/*!
 *  \struct SlowTimeBeamCompensationType
 *
 *  Enumeration used to represent SlowTimeBeamCompensationTypes
 */
SIX_Enum_ENUM_3(SlowTimeBeamCompensationType,
    NO, 0,
    GLOBAL, 1,
    SV, 2
);

/*!
 *  \struct XYZEnum
 *
 *  Enumeration used to represent XYZEnums
 */
SIX_Enum_ENUM_3(XYZEnum,
    X, 0,
    Y, 1,
    Z, 2
);

}

#endif
