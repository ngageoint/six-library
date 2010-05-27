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
#include "six/sidd/SFA.h"

const char six::sidd::sfa::Point::TYPE_NAME[] = "Point";
const char six::sidd::sfa::LineString::TYPE_NAME[] = "LineString";
const char six::sidd::sfa::Polygon::TYPE_NAME[] = "Polygon";
const char six::sidd::sfa::Triangle::TYPE_NAME[] = "Triangle";
const char six::sidd::sfa::PolyhedralSurface::TYPE_NAME[] = "PolyhedralSurface";
const char six::sidd::sfa::TriangulatedIrregularNetwork::TYPE_NAME[] =
        "TriangulatedIrregularNetwork";
const char six::sidd::sfa::MultiPoint::TYPE_NAME[] = "MultiPoint";
const char six::sidd::sfa::MultiLineString::TYPE_NAME[] = "MultiLineString";
const char six::sidd::sfa::MultiPolygon::TYPE_NAME[] = "MultiPolygon";

const char six::sidd::sfa::GeocentricCoordinateSystem::TYPE_NAME[] =
        "GeocentricCoordinateSystem";
const char six::sidd::sfa::GeographicCoordinateSystem::TYPE_NAME[] =
        "GeographicCoordinateSystem";
const char six::sidd::sfa::ProjectedCoordinateSystem::TYPE_NAME[] =
        "ProjectedCoordinateSystem";
