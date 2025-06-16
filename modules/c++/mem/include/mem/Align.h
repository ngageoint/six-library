/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, Radiant Geospatial Solutions
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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
#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__

#include <sys/Conf.h>

namespace mem
{
/*!
 * Align the input pointer to lay at the next multiple of
 * of the provided alignment.
 *
 * \param[in, out] data Pointer to the address to align to a multiple
 *                 alignment
 * \param alignment The alignment to use
 */
void align(sys::ubyte* __restrict* data,
           size_t alignment = sys::SSE_INSTRUCTION_ALIGNMENT);
}

#endif
