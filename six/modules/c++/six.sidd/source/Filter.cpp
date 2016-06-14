/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include <six/Init.h>
#include <six/sidd/Filter.h>

namespace six
{
namespace sidd
{
Filter::Predefined::Predefined() :
    filterFamily(six::Init::undefined<size_t>()),
    filterMember(six::Init::undefined<size_t>())
{
}

bool Filter::Predefined::operator==(const Filter::Predefined& rhs) const
{
    return (databaseName == rhs.databaseName &&
        filterFamily == rhs.filterFamily &&
        filterMember == rhs.filterMember);
}

Filter::Kernel::Custom::Custom() :
    size(six::Init::undefined<RowColInt>())
{
}

bool Filter::Kernel::Custom::operator==(
        const Filter::Kernel::Custom& rhs) const
{
    return (size == rhs.size && filterCoef == rhs.filterCoef);
}

bool Filter::Kernel::operator==(const Filter::Kernel& rhs) const
{
    return (predefined == rhs.predefined &&
        custom == rhs.custom);
}

Filter::Bank::Custom::Custom() :
    numPhasings(six::Init::undefined<size_t>()),
    numPoints(six::Init::undefined<size_t>())
{
}

bool Filter::Bank::Custom::operator==(const Filter::Bank::Custom& rhs) const
{
    return (numPhasings == rhs.numPhasings &&
        numPoints == rhs.numPoints &&
        filterCoef == rhs.filterCoef);
}
bool Filter::Bank::operator==(const Filter::Bank& rhs) const
{
    return (predefined == rhs.predefined &&
        custom == rhs.custom);
}

bool Filter::operator==(const Filter& rhs) const
{
    return (filterName == rhs.filterName &&
            filterKernel == rhs.filterKernel &&
            filterBank == rhs.filterBank);
}
}
}
