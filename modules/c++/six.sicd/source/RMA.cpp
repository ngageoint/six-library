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
#include "six/sicd/RMA.h"

using namespace six;
using namespace six::sicd;

RMAT::RMAT()
{
    refTime = Init::undefined<double>();
    refPos = Init::undefined<Vector3>();
    refVel = Init::undefined<Vector3>();
    kx1 = Init::undefined<double>();
    kx2 = Init::undefined<double>();
    ky1 = Init::undefined<double>();
    ky2 = Init::undefined<double>();
}

INCA::INCA()
{
    rangeCA = Init::undefined<double>();
    freqZero = Init::undefined<double>();
    dopplerCentroidCOA = Init::undefined<BooleanType>();
}

RMA::RMA()
    : algoType(RMA_NOT_SET), rmat(NULL), inca(NULL)
{
}

RMA::~RMA()
{
    if (rmat)
        delete rmat;
    if (inca)
        delete inca;
}

RMA* RMA::clone() const
{
    RMA* rma = new RMA();

    rma->algoType = algoType;

    if (rmat)
        rma->rmat = new RMAT(*rmat);
    if (inca)
        rma->inca = new INCA(*inca);

    return rma;
}
