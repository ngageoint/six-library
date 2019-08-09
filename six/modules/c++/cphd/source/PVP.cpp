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

#include <cphd/PVP.h>
#include <six/Init.h>

namespace cphd
{

PVPType::PVPType() :
    size(0),
    offset(six::Init::undefined<size_t>()),
    format(six::Init::undefined<std::string>())
{}

APVPType::APVPType() :
    size(0),
    offset(six::Init::undefined<size_t>()),
    name(six::Init::undefined<std::string>()),
    format(six::Init::undefined<std::string>())
{}

}
