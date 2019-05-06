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

#ifndef __SYS_RESOURCE_CHECK_H__
#define __SYS_RESOURCE_CHECK_H__

#include <stddef.h>

namespace sys 
{

/*!
 * Determines if a job requiring an approximate amount of memory can fit
 * on a machine with the given amount of memory, with amounts reserved for
 * other applications or a relative margin of error, whichever reserves more.
 *
 * If the reserved bytes is greater than the systemMemBytes we treat this as 
 * though the job cannot fit, regardless of the jobMemBytes or margin.
 *
 * \param jobMemBytes the approximate amount of memory the job will consume
 * \param systemMemBytes the approximate amount of physical RAM on
 *      the system
 * \param reservedBytes the number of bytes of systemMemBytes to reserve for
 *      the operating system or other applications, defaults to 4GB
 * \param margin the fraction of system RAM to assume is available for use,
 *      defaults to 95% (0.95)
 * \return true if the job can fit, false otherwise
 */
bool canProcessFit(size_t jobMemBytes,
                   size_t systemMemBytes,
                   size_t reservedBytes = static_cast<size_t>(1024)*1024*1024*4,
                   double margin = 0.95);

/*!
 * Calculates the amount of available system memory given the total, an absolute
 * reservation, or a relative margin of reservation, whichever reserves more.
 *
 * \param systemMemBytes the amount of physical RAM on the system
 * \param reservedBytes the absolute amount of bytes to reserve for system use,
 *      defaults to 4GB
 * \param margin the fraction of system RAM that is available for use, defaults
 *      to 95% (0.95)
 * \return the number of bytes available 
 */
size_t getAvailableMem(size_t systemMemBytes, 
                       size_t reservedBytes = static_cast<size_t>(1024)*1024*1024*4,
                       double margin = 0.95);

}

#endif
