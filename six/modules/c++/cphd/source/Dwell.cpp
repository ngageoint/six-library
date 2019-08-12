/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/Dwell.h>
#include <six/Init.h>

namespace cphd
{

DwellTime::DwellTime() :
    identifier(six::Init::undefined<std::string>()),
    DwellTimePoly(six::Init::undefined<Poly2D>())
{}

COD::COD() :
    identifier(six::Init::undefined<std::string>()),
    CODTimePoly(six::Init::undefined<Poly2D>())
{}

Dwell::Dwell() :
    numCODTimes(six::Init::undefined<size_t>()),
    numDwellTimes(six::Init::undefined<size_t>())
{}
}
