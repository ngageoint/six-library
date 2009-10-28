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
#ifndef __SIX_GRID_H__
#define __SIX_GRID_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{

/*!
 *  \struct DirectionParameters
 *  \brief Struct for SICD Row/Col Parameters
 *
 *  Parameters describing increasing row or column
 *  direction image coords
 */
struct DirectionParameters
{
    DirectionParameters();
    ~DirectionParameters() {}
    DirectionParameters* clone() const;

    //! Unit vector in increasing row or col direction
    Vector3 unitVector;

    //!  Sample spacing in row or col direction
    double sampleSpacing;

    //!  Half-power impulse response width in increasing row/col dir
    //!  Measured at scene center point
    double impulseResponseWidth;

    //! FFT sign
    FFTSign sign;

    //! Spatial bandwidth in Krow/Kcol used to form the impulse response
    //! in row/col direction, measured at scene center
    double impulseResponseBandwidth;

    //! Center spatial frequency in the Krow/Kcol
    double kCenter;

    //!  Minimum r/c offset from kCenter of spatial freq support for image
    double deltaK1;

    //!  Maximum r/c offset from kCenter of spatial freq support for image
    double deltaK2;

    /*!
     *  Offset from kCenter of the center of support in the r/c
     *  spatial frequency.  The polynomial is a function of the image
     *  r/c
     */
    Poly2D deltaKCOAPoly;

    /*!
     *  Optional type of aperture weighting applied in the spatial
     *  frequency domain to yield the impulse response in the r/c
     *  direction.  Examples incclude UNIFORM, TAYLOR, UNKNOWN
     */
    std::string weightType;

    /*!
     *  Sampled aperture amplitude weighting function applied
     *  in Krow/col to form the SCP impulse response in the row
     *  direction
     *  \note You cannot have less than two weights if you have any
     *  2 <= NW <= 512 according to spec
     *
     *  \todo could make this an object (WeightFunction)
     *
     */
    std::vector<double> weights;

};

/*!
 *  \struct Grid
 *  \brief SICD Grid parameters
 *
 *  The block of parameters that describes the image sample grid
 *
 */
struct Grid
{

    //! TODO what to do with plane
    Grid();
    ~Grid();

    Grid* clone() const;
    ComplexImagePlaneType imagePlane;
    ComplexImageGridType type;
    Poly2D timeCOAPoly;
    DirectionParameters* row;
    DirectionParameters* col;
};

}

#endif
