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

#include <cmath> // isnan()

#include "six/Init.h"

namespace six
{
template<> std::string Init::undefined<std::string>()
{
    return std::string("");
}

template<> float Init::undefined<float>()
{
    return std::numeric_limits<float>::infinity();
}

template<> double Init::undefined<double>()
{
    return std::numeric_limits<double>::infinity();
}

template<> types::RgAz<double>Init::undefined<types::RgAz<double> >()
{
    return types::RgAz<double>(Init::undefined<double>(),
                               Init::undefined<double>());
}

template<> RowColInt Init::undefined<RowColInt>()
{
    return RowColInt(Init::undefined<size_t>(), Init::undefined<size_t>());
}

template<> RowColDouble Init::undefined<RowColDouble>()
{
    return RowColDouble(Init::undefined<double>(), Init::undefined<double>());
}

template<> AngleMagnitude Init::undefined<AngleMagnitude>()
{
    return AngleMagnitude(Init::undefined<double>(), Init::undefined<double>());
}

template<> DecorrType Init::undefined<DecorrType>()
{
    return DecorrType(Init::undefined<double>(), Init::undefined<double>());
}

template<> DateTime Init::undefined<DateTime>()
{
    return DateTime(0.0);
}

template<> Vector3 Init::undefined<Vector3>()
{
    return Vector3(Init::undefined<double>());
}

template<> LatLon Init::undefined<LatLon>()
{
    return LatLon(Init::undefined<double>(), Init::undefined<double>());
}

template<> LatLonAlt Init::undefined<LatLonAlt>()
{
    return LatLonAlt(Init::undefined<double>(), Init::undefined<double>(),
                  Init::undefined<double>());
}

template<> LatLonCorners Init::undefined<LatLonCorners>()
{
    LatLonCorners corners;
    corners.upperLeft = Init::undefined<LatLon>();
    corners.upperRight = Init::undefined<LatLon>();
    corners.lowerRight = Init::undefined<LatLon>();
    corners.lowerLeft = Init::undefined<LatLon>();
    return corners;
}

template<> LatLonAltCorners Init::undefined<LatLonAltCorners>()
{
    LatLonAltCorners corners;
    corners.upperLeft = Init::undefined<LatLonAlt>();
    corners.upperRight = Init::undefined<LatLonAlt>();
    corners.lowerRight = Init::undefined<LatLonAlt>();
    corners.lowerLeft = Init::undefined<LatLonAlt>();
    return corners;
}

template<> ReferencePoint Init::undefined<ReferencePoint>()
{
    const double ud = Init::undefined<double>();
    return ReferencePoint(ud, ud, ud, ud, ud);
}

template<> Poly1D Init::undefined<Poly1D>()
{
    //TODO is this good enough, or should we init differently?
    return Poly1D();
}
template<> Poly2D Init::undefined<Poly2D>()
{
    //TODO is this good enough, or should we init differently?
    return Poly2D();
}
template<> PolyXYZ Init::undefined<PolyXYZ>()
{
    //TODO is this good enough, or should we init differently?
    return PolyXYZ();
}
template<> Parameter Init::undefined<Parameter>()
{
    return Parameter();
}


template<> AppliedType Init::undefined<AppliedType>()
{
    return AppliedType::NOT_SET;
}

template<> AutofocusType Init::undefined<AutofocusType>()
{
    return AutofocusType::NOT_SET;
}

template<> BooleanType Init::undefined<BooleanType>()
{
    return BooleanType::NOT_SET;
}

template<> ByteSwapping Init::undefined<ByteSwapping>()
{
    return ByteSwapping::NOT_SET;
}

template<> CollectType Init::undefined<CollectType>()
{
    return CollectType::NOT_SET;
}

template<> ComplexImageGridType Init::undefined<ComplexImageGridType>()
{
    return ComplexImageGridType::NOT_SET;
}

template<> ComplexImagePlaneType Init::undefined<ComplexImagePlaneType>()
{
    return ComplexImagePlaneType::NOT_SET;
}

template<> DataType Init::undefined<DataType>()
{
    return DataType::NOT_SET;
}

template<> DecimationMethod Init::undefined<DecimationMethod>()
{
    return DecimationMethod::NOT_SET;
}

template<> DemodType Init::undefined<DemodType>()
{
    return DemodType::NOT_SET;
}

template<> DisplayType Init::undefined<DisplayType>()
{
    return DisplayType::NOT_SET;
}

template<> DualPolarizationType Init::undefined<DualPolarizationType>()
{
    return DualPolarizationType::NOT_SET;
}

template<> EarthModelType Init::undefined<EarthModelType>()
{
    return EarthModelType::NOT_SET;
}

template<> FFTSign Init::undefined<FFTSign>()
{
    return FFTSign::NOT_SET;
}

template<> FrameType Init::undefined<FrameType>()
{
    return FrameType::NOT_SET;
}

template<> ImageBeamCompensationType Init::undefined<ImageBeamCompensationType>()
{
    return ImageBeamCompensationType::NOT_SET;
}

template<> ImageFormationType Init::undefined<ImageFormationType>()
{
    return ImageFormationType::NOT_SET;
}

template<> MagnificationMethod Init::undefined<MagnificationMethod>()
{
    return MagnificationMethod::NOT_SET;
}

template<> OrientationType Init::undefined<OrientationType>()
{
    return OrientationType::NOT_SET;
}

template<> PixelType Init::undefined<PixelType>()
{
    return PixelType::NOT_SET;
}

template<> PolarizationSequenceType Init::undefined<PolarizationSequenceType>()
{
    return PolarizationSequenceType::NOT_SET;
}

template<> PolarizationType Init::undefined<PolarizationType>()
{
    return PolarizationType::NOT_SET;
}

template<> ProjectionType Init::undefined<ProjectionType>()
{
    return ProjectionType::NOT_SET;
}

template<> RMAlgoType Init::undefined<RMAlgoType>()
{
    return RMAlgoType::NOT_SET;
}

template<> RadarModeType Init::undefined<RadarModeType>()
{
    return RadarModeType::NOT_SET;
}

template<> RegionType Init::undefined<RegionType>()
{
    return RegionType::NOT_SET;
}

template<> RowColEnum Init::undefined<RowColEnum>()
{
    return RowColEnum::NOT_SET;
}

template<> SCPType Init::undefined<SCPType>()
{
    return SCPType::NOT_SET;
}

template<> SideOfTrackType Init::undefined<SideOfTrackType>()
{
    return SideOfTrackType::NOT_SET;
}

template<> SlowTimeBeamCompensationType Init::undefined<SlowTimeBeamCompensationType>()
{
    return SlowTimeBeamCompensationType::NOT_SET;
}

template<> XYZEnum Init::undefined<XYZEnum>()
{
    return XYZEnum::NOT_SET;
}

template<typename T>
static bool isUndefined_(T value)
{
    return (value == Init::undefined<T>()) || std::isnan(value);
}
template<>
bool Init::isUndefined(float value)
{
    return isUndefined_(value);
}
template<>
bool Init::isUndefined(double value)
{
    return isUndefined_(value);
}

}
