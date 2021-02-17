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
#ifndef __SIX_SET_H__
#define __SIX_SET_H__

#include <limits>

#include <sys/Optional.h>

#include <six/Parameter.h>
#include <types/RgAz.h>
#include <import/except.h>
#include <except/Exception.h>

namespace six
{

DECLARE_EXCEPTION(UninitializedValue)

/*!
 *  \struct Init
 *  \brief Helper functions to tell if objects are undefined
 *
 *  To minimize the model, we check values against known 'undefined'
 *  values.  This allows us to specialize those calls for each type.
 *  Sometimes, the init value is the same as the default constructor
 *  (e.g., std::string), in which case this doesn't need to be used
 */
struct Init
{
    template<typename T> static bool isUndefined(T value)
    {
        return (value == Init::undefined<T>());
    }

    template<typename T> static bool isDefined(T value)
    {
        return !isUndefined<T>(value);
    }

    template<typename T> static bool hasValue(const std::optional<T>& value)
    {
        return value.has_value();
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

template<> std::string Init::undefined<std::string>();
template<> float Init::undefined<float>();
template<> double Init::undefined<double>();
template<> types::RgAz<double> Init::undefined<types::RgAz<double> >();
template<> RowColInt Init::undefined<RowColInt>();
template<> RowColDouble Init::undefined<RowColDouble>();
template<> DecorrType Init::undefined<DecorrType>();
template<> AngleMagnitude Init::undefined<AngleMagnitude>();
template<> DateTime Init::undefined<DateTime>();
template<> Vector3 Init::undefined<Vector3>();
template<> LatLon Init::undefined<LatLon>();
template<> LatLonAlt Init::undefined<LatLonAlt>();
template<> ReferencePoint Init::undefined<ReferencePoint>();
template<> Poly1D Init::undefined<Poly1D>();
template<> Poly2D Init::undefined<Poly2D>();
template<> PolyXYZ Init::undefined<PolyXYZ>();
template<> Parameter Init::undefined<Parameter>();

// enums
template<> AppliedType Init::undefined<AppliedType>();
template<> AutofocusType Init::undefined<AutofocusType>();
template<> BooleanType Init::undefined<BooleanType>();
template<> ByteSwapping Init::undefined<ByteSwapping>();
template<> CollectType Init::undefined<CollectType>();
template<> ComplexImageGridType Init::undefined<ComplexImageGridType>();
template<> ComplexImagePlaneType Init::undefined<ComplexImagePlaneType>();
template<> LatLonCorners Init::undefined<LatLonCorners>();
template<> LatLonAltCorners Init::undefined<LatLonAltCorners>();
template<> DataType Init::undefined<DataType>();
template<> DecimationMethod Init::undefined<DecimationMethod>();
template<> DemodType Init::undefined<DemodType>();
template<> DisplayType Init::undefined<DisplayType>();
template<> DualPolarizationType Init::undefined<DualPolarizationType>();
template<> EarthModelType Init::undefined<EarthModelType>();
template<> FFTSign Init::undefined<FFTSign>();
template<> FrameType Init::undefined<FrameType>();
template<> ImageBeamCompensationType Init::undefined<ImageBeamCompensationType>();
template<> ImageFormationType Init::undefined<ImageFormationType>();
template<> MagnificationMethod Init::undefined<MagnificationMethod>();
template<> OrientationType Init::undefined<OrientationType>();
template<> PixelType Init::undefined<PixelType>();
template<> PolarizationSequenceType Init::undefined<PolarizationSequenceType>();
template<> PolarizationType Init::undefined<PolarizationType>();
template<> ProjectionType Init::undefined<ProjectionType>();
template<> RMAlgoType Init::undefined<RMAlgoType>();
template<> RadarModeType Init::undefined<RadarModeType>();
template<> RegionType Init::undefined<RegionType>();
template<> RowColEnum Init::undefined<RowColEnum>();
template<> SCPType Init::undefined<SCPType>();
template<> SideOfTrackType Init::undefined<SideOfTrackType>();
template<> SlowTimeBeamCompensationType Init::undefined<SlowTimeBeamCompensationType>();
template<> XYZEnum Init::undefined<XYZEnum>();




}

#endif

