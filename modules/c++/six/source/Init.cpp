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
#include "six/Init.h"

using namespace six;

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/*     template<> bool Init::isUndefined<std::string>(std::string s) */
/*     { */
/*         return s.length(); */
/*     } */

template<> std::string Init::undefined<std::string>()
{
    return std::string("");
}

/*     template<> bool Init::isUndefined<int>(int i) */
/*     { */
/*         return i != std::numeric_limits<int>::max(); */
/*     } */

template<> int Init::undefined<int>()
{
    return std::numeric_limits<int>::max();
}

/*     template<> bool Init::isUndefined<int>(unsigned int i) */
/*     { */
/*         return i != std::numeric_limits<unsigned int>::max(); */
/*     } */

template<> unsigned int Init::undefined<unsigned int>()
{
    return std::numeric_limits<unsigned int>::max();
}

/*     template<> bool Init::isUndefined<long>(long l) */
/*     { */
/*         return l != std::numeric_limits<long>::max(); */
/*     } */

template<> long Init::undefined<long>()
{
    return std::numeric_limits<long>::max();
}

template<> unsigned long Init::undefined<unsigned long>()
{
    return std::numeric_limits<unsigned long>::max();
}

/*     template<> bool Init::isUndefined<long>(unsigned long l) */
/*     { */
/*         return l != std::numeric_limits<unsigned long>::max(); */
/*     } */

template<> float Init::undefined<float>()
{
    return std::numeric_limits<float>::infinity();
}

/*     template<> bool Init::isUndefined<float>(float f) */
/*     { */
/*         return f != std::numeric_limits<float>::infinity(); */
/*     } */

template<> double Init::undefined<double>()
{
    return std::numeric_limits<double>::infinity();
}

/*     template<> bool Init::isUndefined<float>(double d) */
/*     { */
/*         return d != std::numeric_limits<double>::infinity(); */
/*     } */


template<> RangeAzimuth<double>Init::undefined<RangeAzimuth<double> >()
{
    return RangeAzimuth<double>(Init::undefined<double>(), Init::undefined<
            double>());
}

template<> RowColInt Init::undefined<RowColInt>()
{
    return RowColInt(Init::undefined<unsigned long>(), Init::undefined<
            unsigned long>());
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

// template<> Corners Init::undefined<Corners>()
// {
//     return Corners(Init::undefined<LatLon>(),
//                         Init::undefined<LatLon>(),
//                         Init::undefined<LatLon>(),
//                         Init::undefined<LatLon>());
// }


template<> ReferencePoint Init::undefined<ReferencePoint>()
{
    double ud = Init::undefined<double>();
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

template<> CornerIndex Init::undefined<CornerIndex>()
{
    return CornerIndex::NOT_SET;
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
