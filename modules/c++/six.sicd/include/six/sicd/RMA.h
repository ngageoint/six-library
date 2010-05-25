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
#ifndef __SIX_RMA_H__
#define __SIX_RMA_H__

#include "six/Types.h"
#include "six/Init.h"

namespace six
{
namespace sicd
{

/*!
 *  \struct  RMAT
 *  \brief   Parameters for Range Migration with Along Track phase
 *           stabilization.
 */
struct RMAT
{
    //!  Constructor
    RMAT();

    //!  Destructor
    ~RMAT()
    {
    }

    double refTime;

    Vector3 refPos;

    Vector3 refVel;

    Poly2D cosDCACOAPoly;

    double kx1;

    double kx2;

    double ky1;

    double ky2;
};

struct INCA
{
    //!  Constructor
    INCA();

    //!  Destructor
    ~INCA()
    {
    }

    Poly1D timeCAPoly;

    double rangeCA;

    double freqZero;

    Poly2D dopplerRateScaleFactorPoly;

    //! (Optional) Polynomial that yields Doppler Centroid value as a function
    //  of image location.
    Poly2D dopplerCentroidPoly;

    //! (Optional) Flag indicating that the COA at peak signal.
    BooleanType dopplerCentroidCOA;
};

struct RMA
{
    //!  Constructor
    RMA();

    //!  Destructor
    ~RMA();

    //!  Copy this object
    RMA* clone() const;

    //!  Type of migration algorithm used.
    RMAlgoType algoType;

    //!  (Choice) Parameters for range migration with along track motion
    //   compensation -- if this is present, inca should be NULL.
    RMAT* rmat;

    //!  (Choice) Parameter for imaging near closest approach image
    //   description -- if this is present, rmat should be NULL.
    INCA* inca;
};

}
}
#endif
