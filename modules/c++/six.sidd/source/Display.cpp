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
#include "six/sidd/Display.h"

using namespace six;
using namespace six::sidd;

Display::Display() :
    remapInformation(NULL), histogramOverrides(NULL),
            monitorCompensationApplied(NULL)
{
    pixelType = PixelType::NOT_SET;
    magnificationMethod = MagnificationMethod::NOT_SET;
    decimationMethod = DecimationMethod::NOT_SET;
}

Display* Display::clone() const
{
    Display* d = new Display;
    d->pixelType = pixelType;
    if (remapInformation)
        d->remapInformation = remapInformation->clone();
    d->magnificationMethod = magnificationMethod;
    d->decimationMethod = decimationMethod;
    if (histogramOverrides)
        d->histogramOverrides = histogramOverrides->clone();
    if (monitorCompensationApplied)
        d->monitorCompensationApplied = monitorCompensationApplied->clone();
    d->displayExtensions = displayExtensions;
    return d;
}

Display::~Display()
{
    if (remapInformation)
        delete remapInformation;
    if (histogramOverrides)
        delete histogramOverrides;
    if (monitorCompensationApplied)
        delete monitorCompensationApplied;
}
