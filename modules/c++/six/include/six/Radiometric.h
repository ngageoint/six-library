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
#ifndef __SIX_RADIOMETRIC_H__
#define __SIX_RADIOMETRIC_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{

/*!
 *  \struct Radiometric
 *  \brief Radiometric calibration parameters
 *
 *  The radiometric parameters are all optional.  They start as
 *  undefined, in which case they will not be written out.
 *
 */
struct Radiometric
{
    //!  Constructor
    Radiometric();
    
    //!  Destructor
    ~Radiometric() {}

    //!  Deep copy
    Radiometric* clone() const;

    /*!
     *  Polynomial coefs that yield thermal noise power in a
     *  in a pixel as a function of image row/col coords
     */
    Poly2D noisePoly;

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
     */
    AppliedType gammaZeroSFIncidenceMap;

};

}

#endif
