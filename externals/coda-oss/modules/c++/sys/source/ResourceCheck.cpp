/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <algorithm>
#include <sys/ResourceCheck.h>
#include <gsl/gsl.h>

namespace sys
{

bool canProcessFit(size_t jobMemBytes, 
                   size_t systemMemBytes,
                   size_t reservedBytes,
                   double margin)
{
    return jobMemBytes < getAvailableMem(systemMemBytes,
                                         reservedBytes,
                                         margin);
}

size_t getAvailableMem(size_t systemMemBytes,
                       size_t reservedBytes,
                       double margin)
{
    const auto relativeAvailable = gsl::narrow_cast<size_t>(gsl::narrow_cast<double>(systemMemBytes) * margin);
    if (reservedBytes > systemMemBytes)
    {
        return 0;
    }

    const auto absAvailable = systemMemBytes - reservedBytes;
    return std::min(relativeAvailable, absAvailable);
}

}
