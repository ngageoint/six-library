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
#ifndef SIX_six_Init_h_INCLUDED_
#define SIX_six_Init_h_INCLUDED_

#include <assert.h>

#include <limits>
#include <stdexcept>

#include <std/optional>

#include <six/Parameter.h>
#include <types/RgAz.h>
#include <import/except.h>
#include <except/Exception.h>
#include <six/Exports.h>

namespace six
{

DECLARE_EXCEPTION(UninitializedValue);

/*!
 *  \struct Init
 *  \brief Helper functions to tell if objects are undefined
 *
 *  To minimize the model, we check values against known 'undefined'
 *  values.  This allows us to specialize those calls for each type.
 *  Sometimes, the init value is the same as the default constructor
 *  (e.g., std::string), in which case this doesn't need to be used
 */
struct SIX_SIX_API Init final
{
    template<typename T> static bool isUndefined(T value)
    {
        return (value == Init::undefined<T>());
    }

    template<typename T> static bool isDefined(T value)
    {
        return !isUndefined<T>(value);
    }

    template<typename T> static T undefined()
    {
        // This will handle integer types
        // It won't compile for other types that aren't specialized below
        return std::numeric_limits<T>::max();
    }
};

// This is intentionally undefined in order to generate a linker error
template<> bool Init::undefined<bool>();

#define SIX_Init_undefined_(T) template<> SIX_SIX_API T Init::undefined<T>()

SIX_Init_undefined_(std::string);
SIX_Init_undefined_(float);
SIX_Init_undefined_(double);
SIX_Init_undefined_(types::RgAz<double>);
SIX_Init_undefined_(RowColInt);
SIX_Init_undefined_(RowColDouble);
SIX_Init_undefined_(DecorrType);
SIX_Init_undefined_(AngleMagnitude);
SIX_Init_undefined_(DateTime);
SIX_Init_undefined_(Vector3);
SIX_Init_undefined_(LatLon);
SIX_Init_undefined_(LatLonAlt);
SIX_Init_undefined_(ReferencePoint);
SIX_Init_undefined_(Poly1D);
SIX_Init_undefined_(Poly2D);
SIX_Init_undefined_(PolyXYZ);
SIX_Init_undefined_(Parameter);

// enums
SIX_Init_undefined_(AppliedType);
SIX_Init_undefined_(AutofocusType);
SIX_Init_undefined_(BooleanType);
SIX_Init_undefined_(ByteSwapping);
SIX_Init_undefined_(CollectType);
SIX_Init_undefined_(ComplexImageGridType);
SIX_Init_undefined_(ComplexImagePlaneType);
SIX_Init_undefined_(LatLonCorners);
SIX_Init_undefined_(LatLonAltCorners);
SIX_Init_undefined_(DataType);
SIX_Init_undefined_(DecimationMethod);
SIX_Init_undefined_(DemodType);
SIX_Init_undefined_(DisplayType);
SIX_Init_undefined_(DualPolarizationType);
SIX_Init_undefined_(EarthModelType);
SIX_Init_undefined_(FFTSign);
SIX_Init_undefined_(FrameType);
SIX_Init_undefined_(ImageBeamCompensationType);
SIX_Init_undefined_(ImageFormationType);
SIX_Init_undefined_(MagnificationMethod);
SIX_Init_undefined_(OrientationType);
SIX_Init_undefined_(PixelType);
SIX_Init_undefined_(PolarizationSequenceType);
SIX_Init_undefined_(PolarizationType);
SIX_Init_undefined_(ProjectionType);
SIX_Init_undefined_(RMAlgoType);
SIX_Init_undefined_(RadarModeType);
SIX_Init_undefined_(RegionType);
SIX_Init_undefined_(RowColEnum);
SIX_Init_undefined_(SCPType);
SIX_Init_undefined_(SideOfTrackType);
SIX_Init_undefined_(SlowTimeBeamCompensationType);
SIX_Init_undefined_(XYZEnum);

#undef SIX_Init_undefined_

template<> SIX_SIX_API bool Init::isUndefined<float>(float);
template<> SIX_SIX_API bool Init::isUndefined<double>(double);

// These allow switching between `double` and `std::optional<double>` w/o changing a lot of other code.
template<typename T>
inline bool has_value(const T& v)
{
    return Init::isDefined(v);
}
template<typename T>
inline bool has_value(const std::optional<T>& v)
{
    return v.has_value() && has_value(*v);
}

namespace details
{
    inline void throw_undefined_value()
    {
        coda_oss::details::throw_bad_optional_access();
    }
}

template<typename T>
inline const T& value(const T& v)
{
    if (!has_value(v))
    {
        details::throw_undefined_value();
    }
    return v;
}
template<typename T>
inline const T& value(const std::optional<T>& v)
{
    return value(v.value());
}

}

#endif // SIX_six_Init_h_INCLUDED_
