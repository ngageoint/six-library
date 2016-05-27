/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sicd/Grid.h"

using namespace six;
using namespace six::sicd;

WeightType::WeightType() :
    windowName(Init::undefined<std::string>())
{
}

DirectionParameters::DirectionParameters() :
    unitVector(Init::undefined<Vector3>()),
    sampleSpacing(Init::undefined<double>()),
    impulseResponseWidth(Init::undefined<double>()),
    sign(Init::undefined<FFTSign>()),
    impulseResponseBandwidth(Init::undefined<double>()),
    kCenter(Init::undefined<double>()),
    deltaK1(Init::undefined<double>()),
    deltaK2(Init::undefined<double>()),
    deltaKCOAPoly(Init::undefined<Poly2D>())
{
}

DirectionParameters* DirectionParameters::clone() const 
{
    return new DirectionParameters(*this);
}

bool DirectionParameters::operator==(const DirectionParameters& rhs) const
{
    return (unitVector == rhs.unitVector &&
        sampleSpacing == rhs.sampleSpacing &&
        impulseResponseWidth == rhs.impulseResponseWidth &&
        sign == rhs.sign &&
        impulseResponseBandwidth == rhs.impulseResponseBandwidth &&
        kCenter == rhs.kCenter &&
        deltaK1 == rhs.deltaK1 &&
        deltaK2 == rhs.deltaK2 &&
        deltaKCOAPoly == rhs.deltaKCOAPoly &&
        weightType == rhs.weightType &&
        weights == rhs.weights);
}

Grid::Grid() :
    // This is a good assumption, I think
    imagePlane(ComplexImagePlaneType::SLANT),
    // Not so sure about this one
    type(ComplexImageGridType::RGAZIM),
    row(new DirectionParameters()),
    col(new DirectionParameters())
{
}

Grid* Grid::clone() const 
{
    return new Grid(*this);
}

bool Grid::operator==(const Grid& rhs) const
{
    return (imagePlane == rhs.imagePlane &&
        type == rhs.type &&
        timeCOAPoly == rhs.timeCOAPoly &&
        row == rhs.row && col == rhs.col);
}

