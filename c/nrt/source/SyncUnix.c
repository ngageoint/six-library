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
#if !defined(WIN32) && !defined(__sgi)
NRTPROT(void) nrt_Mutex_lock(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Locking Mutex***\n");
    pthread_mutex_lock((pthread_mutex_t *) m);
}

NRTPROT(void) nrt_Mutex_unlock(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Unlocking Mutex***\n");
    pthread_mutex_unlock((pthread_mutex_t *) m);
}

NRTPROT(void) nrt_Mutex_init(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Initializing Mutex***\n");
    pthread_mutex_init(m, NULL);
}

NRTPROT(void) nrt_Mutex_delete(nrt_Mutex * m)
{
    nrt_Debug_flogf(stdout, "***Trying to Destroy Mutex***\n");
    if (m)
    {
        pthread_mutex_destroy((pthread_mutex_t *) m);
        nrt_Debug_flogf(stdout, "***Destroyed Mutex***\n");
    }
}
#endif

NRT_CXX_ENDGUARD
