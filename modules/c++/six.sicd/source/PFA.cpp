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
#include "six/sicd/PFA.h"

using namespace six;
using namespace six::sicd;


PFA::~PFA()
{
    if (slowTimeDeskew)
        delete slowTimeDeskew;
    
    for (unsigned int i = 0; i < compensations.size(); ++i)
    {
        Compensation* c = compensations[i];
        delete c;
    }
}

PFA* PFA::clone() const
{
    PFA* pfa = new PFA();
    pfa->focusPlaneNormal = focusPlaneNormal;
    pfa->imagePlaneNormal = imagePlaneNormal;
    pfa->polarAngleRefTime = polarAngleRefTime;
    pfa->polarAnglePoly = polarAnglePoly;
    pfa->spatialFrequencyScaleFactorPoly = 
        spatialFrequencyScaleFactorPoly;
    pfa->krg1 = krg1;
    pfa->krg2 = krg2;
    pfa->kaz1 = kaz1;
    pfa->kaz2 = kaz2;
    
    if (slowTimeDeskew)
        pfa->slowTimeDeskew = slowTimeDeskew->clone();
    for (unsigned int i = 0; i < compensations.size(); ++i)
    {
        Compensation* c = compensations[i];
        pfa->compensations.push_back(c);
    }
    
    return pfa;
}
