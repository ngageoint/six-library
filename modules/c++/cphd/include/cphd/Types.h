/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#ifndef __CPHD_TYPES_H__
#define __CPHD_TYPES_H__

#include <six/Types.h>
#include <six/Enums.h>
#include <six/sicd/CollectionInformation.h>

namespace cphd
{
// Use the same types that SIX uses

typedef six::Vector3 Vector3;

typedef six::DateTime DateTime;

typedef six::UByte UByte;

typedef six::Poly1D Poly1D;

typedef six::Poly2D Poly2D;

typedef six::PolyXYZ PolyXYZ;

typedef six::LatLon LatLon;

typedef six::LatLonAlt LatLonAlt;

typedef six::LatLonCorners LatLonCorners;

typedef six::LatLonAltCorners LatLonAltCorners;

typedef six::FFTSign FFTSign;
 
typedef six::RadarModeType RadarModeType;

typedef six::CollectType CollectType;

typedef six::DataType DataType;

typedef six::BooleanType BooleanType;

typedef six::sicd::CollectionInformation CollectionInformation;
}

#endif
