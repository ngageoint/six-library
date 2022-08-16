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
//SIX_Enum_ENUM_2(BooleanType,
//    IS_FALSE, 0,
//    IS_TRUE, 1
//);
SIX_Enum_BEGIN_DEFINE(BooleanType)
    SIX_Enum_BEGIN_enum(BooleanType) IS_FALSE = 0, IS_TRUE = 1, SIX_Enum_END_enum
    SIX_Enum_map_2_(BooleanType, IS_FALSE, IS_TRUE);
    BooleanType& operator=(bool b)
    {
        *this = BooleanType(b ? IS_TRUE : IS_FALSE);
        return *this;
    }
SIX_Enum_END_DEFINE(BooleanType);

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
    SIX_Enum_BEGIN_enum(PixelType)
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

    #undef SIX_Enum_map_entry_name
    #define SIX_Enum_map_entry_name PixelType
    SIX_Enum_BEGIN_string_to_value(PixelType)
            SIX_Enum_map_entry(RE32F_IM32F),
            SIX_Enum_map_entry(RE16I_IM16I),
            SIX_Enum_map_entry(AMP8I_PHS8I),
            SIX_Enum_map_entry(MONO8I),
            SIX_Enum_map_entry(MONO8LU),
            SIX_Enum_map_entry(MONO16I),
            SIX_Enum_map_entry(RGB8LU),
            SIX_Enum_map_entry(RGB24I),
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(PixelType);

/*!
 *  \struct PolarizationSequenceType
 *
 *  Enumeration used to represent PolarizationSequenceTypes
 * 
 * From Table 3.7, Transmit polarization type.
 *  Allowed values: "V", "H", "X", "Y", "S", "E", "RHC", "LHC", "UNKNOWN", "SEQUENCE", "OTHER*"
 */
SIX_Enum_BEGIN_DEFINE(Polarization1Type)
    std::string other_; // value of OTHER.* for SIDD 3.0/SICD 1.3

    //! The enumerations allowed
    SIX_Enum_BEGIN_enum(Polarization1Type)
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

    #undef SIX_Enum_map_entry_name
    #define SIX_Enum_map_entry_name Polarization1Type
    SIX_Enum_BEGIN_string_to_value(Polarization1Type)
            SIX_Enum_map_entry(OTHER),
            SIX_Enum_map_entry(V),
            SIX_Enum_map_entry(H),
            SIX_Enum_map_entry(X),
            SIX_Enum_map_entry(Y),
            SIX_Enum_map_entry(S),
            SIX_Enum_map_entry(E),
            SIX_Enum_map_entry(RHC),
            SIX_Enum_map_entry(LHC),
            SIX_Enum_map_entry(UNKNOWN),
            SIX_Enum_map_entry(SEQUENCE),
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(Polarization1Type);
using PolarizationSequenceType = Polarization1Type;

/*!
 *  \struct PolarizationType
 *
 *  Enumeration used to represent PolarizationTypes (no SEQUENCE)
 * 
 * From Table 3.7, Transmit signal polarization for this step:
 *  Allowed values: "V", "H", "X", "Y", "S", "E", "RHC", "LHC", "UNKNOWN", "OTHER*"
 */
SIX_Enum_BEGIN_DEFINE(Polarization2Type)
    std::string other_; // value of OTHER.* for SIDD 3.0/SICD 1.3

    //! The enumerations allowed
    SIX_Enum_BEGIN_enum(Polarization2Type)
        OTHER = 1,
        V = 2,
        H = 3,
        X = 7, // SIDD 3.0, SICD 1.3
        Y, // SIDD 3.0, SICD 1.3
        S, // SIDD 3.0, SICD 1.3
        E, // SIDD 3.0, SICD 1.3
        RHC = 4,
        LHC = 5,
        UNKNOWN = 6,
    SIX_Enum_END_enum

    #undef SIX_Enum_map_entry_name
    #define SIX_Enum_map_entry_name Polarization2Type
    SIX_Enum_BEGIN_string_to_value(Polarization2Type)
            SIX_Enum_map_entry(OTHER),
            SIX_Enum_map_entry(V),
            SIX_Enum_map_entry(H),
            SIX_Enum_map_entry(X),
            SIX_Enum_map_entry(Y),
            SIX_Enum_map_entry(S),
            SIX_Enum_map_entry(E),
            SIX_Enum_map_entry(RHC),
            SIX_Enum_map_entry(LHC),
            SIX_Enum_map_entry(UNKNOWN),
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(Polarization2Type);
using PolarizationType = Polarization2Type;

/*!
 *  \struct DualPolarizationType
 *
 *  Enumeration used to represent DualPolarizationTypes
 * 
 * From Table 3.7, Indicates the combined transmit and receive polarization for the channel.
 *  Allowed values include the form TX:RCV that is formed from one TX value and one RCV value.
 *  Allowed TX values:  "V", "H", "X", "Y", "S", "E", "RHC", "LHC", "OTHER*"
 *  Allowed RCV values: "V", "H", "X", "Y", "S", "E", "RHC", "LHC", "OTHER*"
 *  Allowed values also include: "OTHER", "UNKNOWN"
 */
SIX_Enum_BEGIN_DEFINE(DualPolarizationType)
    std::string other_; // value of OTHER.* for SIDD 3.0/SICD 1.3

    //! The enumerations allowed
    SIX_Enum_BEGIN_enum(DualPolarizationType)
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

    #undef SIX_Enum_map_entry_name
    #define SIX_Enum_map_entry_name DualPolarizationType
    SIX_Enum_BEGIN_string_to_value(DualPolarizationType)
        SIX_Enum_map_entry(OTHER),

        SIX_Enum_map_entry(V_V),
        SIX_Enum_map_entry(V_H),
        SIX_Enum_map_entry(V_X),
        SIX_Enum_map_entry(V_Y),
        SIX_Enum_map_entry(V_S),
        SIX_Enum_map_entry(V_E),
        SIX_Enum_map_entry(V_RHC),
        SIX_Enum_map_entry(V_LHC),
        SIX_Enum_map_entry(V_OTHER),

        SIX_Enum_map_entry(H_V),
        SIX_Enum_map_entry(H_H),
        SIX_Enum_map_entry(H_X),
        SIX_Enum_map_entry(H_Y),
        SIX_Enum_map_entry(H_S),
        SIX_Enum_map_entry(H_E),
        SIX_Enum_map_entry(H_RHC),
        SIX_Enum_map_entry(H_LHC),
        SIX_Enum_map_entry(H_OTHER),

        SIX_Enum_map_entry(X_V),
        SIX_Enum_map_entry(X_H),
        SIX_Enum_map_entry(X_X),
        SIX_Enum_map_entry(X_Y),
        SIX_Enum_map_entry(X_S),
        SIX_Enum_map_entry(X_E),
        SIX_Enum_map_entry(X_RHC),
        SIX_Enum_map_entry(X_LHC),
        SIX_Enum_map_entry(X_OTHER),

        SIX_Enum_map_entry(Y_V),
        SIX_Enum_map_entry(Y_H),
        SIX_Enum_map_entry(Y_X),
        SIX_Enum_map_entry(Y_Y),
        SIX_Enum_map_entry(Y_S),
        SIX_Enum_map_entry(Y_E),
        SIX_Enum_map_entry(Y_RHC),
        SIX_Enum_map_entry(Y_LHC),
        SIX_Enum_map_entry(Y_OTHER),

        SIX_Enum_map_entry(S_V),
        SIX_Enum_map_entry(S_H),
        SIX_Enum_map_entry(S_X),
        SIX_Enum_map_entry(S_Y),
        SIX_Enum_map_entry(S_S),
        SIX_Enum_map_entry(S_E),
        SIX_Enum_map_entry(S_RHC),
        SIX_Enum_map_entry(S_LHC),
        SIX_Enum_map_entry(S_OTHER),

        SIX_Enum_map_entry(E_V),
        SIX_Enum_map_entry(E_H),
        SIX_Enum_map_entry(E_X),
        SIX_Enum_map_entry(E_Y),
        SIX_Enum_map_entry(E_S),
        SIX_Enum_map_entry(E_E),
        SIX_Enum_map_entry(E_RHC),
        SIX_Enum_map_entry(E_LHC),
        SIX_Enum_map_entry(E_OTHER),

        SIX_Enum_map_entry(RHC_V),
        SIX_Enum_map_entry(RHC_H),
        SIX_Enum_map_entry(RHC_X),
        SIX_Enum_map_entry(RHC_Y),
        SIX_Enum_map_entry(RHC_S),
        SIX_Enum_map_entry(RHC_E),
        SIX_Enum_map_entry(RHC_RHC),
        SIX_Enum_map_entry(RHC_LHC),
        SIX_Enum_map_entry(RHC_OTHER),

        SIX_Enum_map_entry(LHC_V),
        SIX_Enum_map_entry(LHC_H),
        SIX_Enum_map_entry(LHC_X),
        SIX_Enum_map_entry(LHC_Y),
        SIX_Enum_map_entry(LHC_S),
        SIX_Enum_map_entry(LHC_E),
        SIX_Enum_map_entry(LHC_RHC),
        SIX_Enum_map_entry(LHC_LHC),
        SIX_Enum_map_entry(LHC_OTHER),

        SIX_Enum_map_entry(OTHER_V),
        SIX_Enum_map_entry(OTHER_H),
        SIX_Enum_map_entry(OTHER_X),
        SIX_Enum_map_entry(OTHER_Y),
        SIX_Enum_map_entry(OTHER_S),
        SIX_Enum_map_entry(OTHER_E),
        SIX_Enum_map_entry(OTHER_RHC),
        SIX_Enum_map_entry(OTHER_LHC),
        SIX_Enum_map_entry(OTHER_OTHER),

        SIX_Enum_map_entry(UNKNOWN),
    SIX_Enum_END_string_to_value
SIX_Enum_END_DEFINE(DualPolarizationType);

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
