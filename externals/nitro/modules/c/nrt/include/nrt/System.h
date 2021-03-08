/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NRT_SYSTEM_H__
#define __NRT_SYSTEM_H__

#include "nrt/Defines.h"
#include "nrt/Types.h"
#include "nrt/Error.h"
#include "nrt/Memory.h"
#include "nrt/DLL.h"
#include "nrt/Sync.h"
#include "nrt/Directory.h"
#include "nrt/IOHandle.h"

NRTPROT(uint16_t) nrt_System_swap16(uint16_t ins);
NRTPROT(uint32_t) nrt_System_swap32(uint32_t inl);
NRTPROT(uint64_t) nrt_System_swap64c(uint64_t inl);
NRTPROT(uint64_t) nrt_System_swap64(uint64_t inl);

#endif
