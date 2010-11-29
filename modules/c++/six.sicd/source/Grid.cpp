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
#include "six/sicd/Grid.h"

using namespace six;
using namespace six::sicd;

DirectionParameters::DirectionParameters()
{
    unitVector = Init::undefined<Vector3>();
    sampleSpacing = Init::undefined<double>();
    impulseResponseWidth = Init::undefined<double>();
    sign = Init::undefined<FFTSign>();
    impulseResponseBandwidth = Init::undefined<double>();
    kCenter = Init::undefined<double>();
    deltaK1 = Init::undefined<double>();
    deltaK2 = Init::undefined<double>();
    weightType = Init::undefined<std::string>();
    deltaKCOAPoly = Init::undefined<Poly2D>();   
}

DirectionParameters* DirectionParameters::clone() const 
{
    DirectionParameters* p = new DirectionParameters(*this);
    return p;
}

Grid::Grid() : row(NULL), col(NULL)
{
    // This is a good assumption, I think
    imagePlane = ComplexImagePlaneType::SLANT;
    // Not so sure about this one
    type = ComplexImageGridType::RGAZIM;
    row = new DirectionParameters();
    col = new DirectionParameters();
}

Grid::~Grid()
{
    if (row)
        delete row;
    if (col)
        delete col;
}

Grid* Grid::clone() const 
{
    Grid* g = new Grid;
    g->imagePlane = this->imagePlane;
    g->type = this->type;
    g->timeCOAPoly = this->timeCOAPoly;
    if (this->row)
        g->row = this->row->clone();
    if (this->col)
        g->col = this->col->clone();
    return g;
}
