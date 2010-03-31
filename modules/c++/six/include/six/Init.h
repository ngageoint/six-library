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
#include <import/except.h>

namespace six
{

/*!
 *  \struct Init
 *  \brief Helper functions to tell if objects are undefined
 *
 *  To minimize the model, we check values against known 'undefined'
 *  values.  This allows us to specialize those calls for each type.
 *  Some times, the init value is the same as the default constructor
 *  (e.g., std::string), in which case this doesnt need to be used
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

template<> DataClass Init::undefined<DataClass>();
template<> std::string Init::undefined<std::string>();
template<> int Init::undefined<int>();
template<> unsigned int Init::undefined<unsigned int>();
template<> long Init::undefined<long>();
template<> unsigned long Init::undefined<unsigned long>();
template<> float Init::undefined<float>();
template<> double Init::undefined<double>();
template<> PixelType Init::undefined<PixelType>();
template<> MagnificationMethod Init::undefined<MagnificationMethod>();
template<> DecimationMethod Init::undefined<DecimationMethod>();
template<> RadarModeType Init::undefined<RadarModeType>();
template<> RangeAzimuth<double> Init::undefined<RangeAzimuth<double> >();
template<> RowColInt Init::undefined<RowColInt>();
template<> RowColDouble Init::undefined<RowColDouble>();
template<> AngleMagnitude Init::undefined<AngleMagnitude>();
template<> DecorrType Init::undefined<DecorrType>();
template<> DateTime Init::undefined<DateTime>();
template<> CollectType Init::undefined<CollectType>();
template<> Vector3 Init::undefined<Vector3>();
template<> LatLon Init::undefined<LatLon>();
//template<> Corners Init::undefined<Corners>();
template<> BooleanType Init::undefined<BooleanType>();
template<> SideOfTrackType Init::undefined<SideOfTrackType>();
template<> ReferencePoint Init::undefined<ReferencePoint>();
template<> FFTSign Init::undefined<FFTSign>();
template<> AppliedType Init::undefined<AppliedType>();
template<> Poly2D Init::undefined<Poly2D>();
template<> Poly1D Init::undefined<Poly1D>();
template<> PolyXYZ Init::undefined<PolyXYZ>();

}

#endif
