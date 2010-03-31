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

template<> DataClass Init::undefined<DataClass>()
{
    return DATA_UNKNOWN;
}

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

template<> PixelType Init::undefined<PixelType>()
{
    return PIXEL_TYPE_NOT_SET;
}
template<> MagnificationMethod Init::undefined<MagnificationMethod>()
{
    return MAG_NOT_SET;
}

template<> DecimationMethod Init::undefined<DecimationMethod>()
{
    return DEC_NOT_SET;
}

template<> RadarModeType Init::undefined<RadarModeType>()
{
    return MODE_NOT_SET;
}

template<> RangeAzimuth<double> Init::undefined<RangeAzimuth<double> >()
{
    return RangeAzimuth<double>(Init::undefined<double>(),
                                Init::undefined<double>());
}

template<> RowColInt Init::undefined<RowColInt>()
{
    return RowColInt(Init::undefined<unsigned long>(),
                     Init::undefined<unsigned long>());
}

template<> RowColDouble Init::undefined<RowColDouble>()
{
    return RowColDouble(Init::undefined<double>(),
                        Init::undefined<double>());
}

template<> AngleMagnitude Init::undefined<AngleMagnitude>()
{
    return AngleMagnitude(Init::undefined<double>(),
                          Init::undefined<double>());
}

template<> DecorrType Init::undefined<DecorrType>()
{
    return DecorrType(Init::undefined<double>(),
                      Init::undefined<double>());
}



template<> DateTime Init::undefined<DateTime>()
{
    return DateTime(0.0);
}

template<> CollectType Init::undefined<CollectType>()
{
    return COLLECT_NOT_SET;
}

template<> Vector3 Init::undefined<Vector3>()
{
    return Vector3(Init::undefined<double>());
}

template<> LatLon Init::undefined<LatLon>()
{
    return LatLon(Init::undefined<double>(),
                  Init::undefined<double>());
}

// template<> Corners Init::undefined<Corners>()
// {
//     return Corners(Init::undefined<LatLon>(),
//                         Init::undefined<LatLon>(),
//                         Init::undefined<LatLon>(),
//                         Init::undefined<LatLon>());
// }

template<> BooleanType Init::undefined<BooleanType>()
{
    return BOOL_NOT_SET;
}

template<> SideOfTrackType Init::undefined<SideOfTrackType>()
{
    return SIDE_NOT_SET;
}

template<> ReferencePoint Init::undefined<ReferencePoint>()
{
    double ud = Init::undefined<double>();
    return ReferencePoint(ud, ud, ud, ud, ud);
}

template<> FFTSign Init::undefined<FFTSign>()
{
    return FFT_SIGN_NOT_SET;
}

template<> AppliedType Init::undefined<AppliedType>()
{
    return APPLIED_NOT_SET;
}

template<> Poly2D Init::undefined<Poly2D>()
{
    Poly2D poly(0, 0);
    poly[0][0] = Init::undefined<double>();
    return poly;
}

template<> Poly1D Init::undefined<Poly1D>()
{
    Poly1D poly(0);
    poly[0] = Init::undefined<double>();
    return poly;
}

template<> PolyXYZ Init::undefined<PolyXYZ>()
{
    PolyXYZ poly(0);
    poly[0][0] = poly[0][1] = poly[0][2] = Init::undefined<double>();
    return poly;
}
