/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "nrt/Debug.h"
#include "nrt/Sync.h"

NRT_CXX_GUARD
#if defined(__sgi)
NRTPROT(void) nrt_Mutex_lock(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Locking Mutex*** [sgi]\n");
    while (!__compare_and_swap(m, 0, 1));
}

NRTPROT(void) nrt_Mutex_unlock(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Unlocking Mutex***\n");
    *m = 0;
}

NRTPROT(void) nrt_Mutex_init(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Initializing Mutex*** [sgi]\n");
    *m = 0;
}

NRTPROT(void) nrt_Mutex_delete(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Destroy Mutex*** [sgi] (empty)\n");
}
#endif

NRT_CXX_ENDGUARD
