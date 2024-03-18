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
#ifndef __SIX_RADIOMETRIC_H__
#define __SIX_RADIOMETRIC_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/Exports.h"

namespace six
{

/*!
 *  \struct NoiseLevel
 *  \brief Noise Level Structure
 *
 *  Field and subfields added in 1.0.0
 */
struct SIX_SIX_API NoiseLevel
{
    NoiseLevel();

    /*!
     *  Parameter to indicate that the noise
     *  power polynomial yields either absolute
     *  power level or power level relative to
     *  the SCP pixel location.
     *  Added in 1.0.0
     */
     std::string noiseType;

    /*!
     *  Polynomial coefficients that yield
     *  thermal noise power (in dB) in a pixel
     *  as a function of image row coordinate
     *  (variable 1) and column coordinate
     *
     *  Moved in 1.0.0
     */
    Poly2D noisePoly;

    //! Equality operators
    bool operator==(const NoiseLevel& rhs) const
    {
        return noiseType == rhs.noiseType && noisePoly == rhs.noisePoly;
    }

    bool operator!=(const NoiseLevel& rhs) const
    {
        return !(*this == rhs);
    }

};

/*!
 *  \struct Radiometric
 *  \brief Radiometric calibration parameters
 *
 *  The radiometric parameters are all optional.  They start as
 *  undefined, in which case they will not be written out.
 *
 */
struct SIX_SIX_API Radiometric
{
public:

    static const char NL_ABSOLUTE[];
    static const char NL_RELATIVE[];

    //!  Constructor
    Radiometric();

    //! Added in 1.0.0
    NoiseLevel noiseLevel;

    /*!
     *  Polynomial coefficients that yield a scale factor to
     *  convert pixel power into RCS as a function of image row/col
     *  coords
     */
    Poly2D rcsSFPoly;

    /*!
     *  Poly coefs that yield a scale factor to convert pixel power into
     *  radar brightness as a function of image row/col coords from SCP
     */
    Poly2D betaZeroSFPoly;

    /*!
     *  Poly coefs that yield a scale factor to convert pixel power into
     *  clutter parameter sigma zero as a function of image row/col
     */
    Poly2D sigmaZeroSFPoly;

    /*!
     *  Parameter indicating if local terrain elevation/incidence map was
     *  used to compute sigma zero coefs.
     *  Removed in 1.0.0
     */
    AppliedType sigmaZeroSFIncidenceMap;

    /*!
     *  Poly coefs that yield a scale factor to convert pixel power into
     *  clutter parameter gamma zero as a function of image row
     *  coord and column coord from SCP
     */
    Poly2D gammaZeroSFPoly;

    /*!
     *  Parameter indicating if a local terrain elev/incid. map was
     *  used to compute gamma zero coefs
     *  Removed in 1.0.0
     */
    AppliedType gammaZeroSFIncidenceMap;

    //! Equality operators
    bool operator==(const Radiometric& rhs) const;

    bool operator!=(const Radiometric& rhs) const
    {
        return !(*this == rhs);
    }

};

}
#endif

