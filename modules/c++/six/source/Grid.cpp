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
#include "six/Grid.h"

using namespace six;

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
    
}

DirectionParameters* DirectionParameters::clone() const 
{
    DirectionParameters* p = new DirectionParameters(*this);
    return p;
}

Grid::Grid() : row(NULL), col(NULL)
{
    // This is a good assumption, I think
    imagePlane = PLANE_SLANT;
    // Not so sure about this one
    type = GRID_RGAZIM;
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
    Grid* g = new Grid(*this);

    if (g->row)
        delete g->row;
    if (g->col)
        delete g->col;

    if (row)
        g->row = row->clone();
    if (col)
        g->col = col->clone();
    return g;
}
