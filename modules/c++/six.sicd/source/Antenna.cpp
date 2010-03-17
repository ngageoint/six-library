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
#include "six/sicd/Antenna.h"

using namespace six;
using namespace six::sicd;

ElectricalBoresight* ElectricalBoresight::clone() const 
{
    return new ElectricalBoresight(*this); 
}
HalfPowerBeamwidths::HalfPowerBeamwidths()
{
    // TODO: make zero?
    dcx = Init::undefined<double>();
    dcy = Init::undefined<double>();
}
HalfPowerBeamwidths* HalfPowerBeamwidths::clone() const
{
    return new HalfPowerBeamwidths(*this);
}

GainAndPhasePolys* GainAndPhasePolys::clone() const 
{
    return new GainAndPhasePolys(*this); 
}

AntennaParameters::~AntennaParameters()
{
    if (electricalBoresight)
        delete electricalBoresight;
    if (halfPowerBeamwidths)
        delete halfPowerBeamwidths;
    if (array)
        delete array;
    if (element)
        delete element;
}
AntennaParameters* AntennaParameters::clone() const
{
    AntennaParameters* ap = new AntennaParameters(*this);
    if (electricalBoresight)
        ap->electricalBoresight = electricalBoresight->clone();
    
    if (halfPowerBeamwidths)
        ap->halfPowerBeamwidths = halfPowerBeamwidths->clone();
    
    if (array)
        ap->array = array->clone();
    
    if (element)
        ap->element = element->clone();
    return ap;
}

Antenna::~Antenna() 
{
    if (tx)
        delete tx;
    if (rcv)
        delete rcv;
    if (twoWay)
        delete twoWay;
}  

Antenna* Antenna::clone() const 
{
    Antenna* a = new Antenna();
    if (tx)
        a->tx = tx->clone();
    if (rcv)
        a->rcv = rcv->clone();
    if (twoWay)
        a->twoWay = twoWay->clone();
    return a;
    
}
