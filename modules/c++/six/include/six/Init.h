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
#ifndef __SIX_SET_H__
#define __SIX_SET_H__

#include "six/Types.h"
#include "six/Parameter.h"
#include <import/except.h>

namespace six
{

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

    template<typename T> static T undefined()
    {
        throw except::Exception(Ctxt("No defined init value for data type"));
    }

};

template<> std::string Init::undefined<std::string>();
template<> int Init::undefined<int>();
template<> unsigned int Init::undefined<unsigned int>();
template<> long Init::undefined<long>();
template<> unsigned long Init::undefined<unsigned long>();
template<> float Init::undefined<float>();
template<> double Init::undefined<double>();
template<> RangeAzimuth<double> Init::undefined<RangeAzimuth<double> >();
template<> RowColInt Init::undefined<RowColInt>();
template<> RowColDouble Init::undefined<RowColDouble>();
template<> DecorrType Init::undefined<DecorrType>();
template<> AngleMagnitude Init::undefined<AngleMagnitude>();
template<> DateTime Init::undefined<DateTime>();
template<> Vector3 Init::undefined<Vector3>();
template<> LatLon Init::undefined<LatLon>();
template<> LatLonAlt Init::undefined<LatLonAlt>();
//template<> Corners Init::undefined<Corners>();
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
template<> CornerIndex Init::undefined<CornerIndex>();
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
