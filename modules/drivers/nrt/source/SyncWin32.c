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

#include "nrt/Sync.h"

NRT_CXX_GUARD
#if defined(WIN32)
NRTPROT(void) nrt_Mutex_lock(nrt_Mutex * m)
{
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION) (*m);
    /* OpenMutex(MUTEX_ALL_ACCESS, FALSE, "PluginRegistry::getMutex()"); */
    EnterCriticalSection(lpCriticalSection);
}

NRTPROT(void) nrt_Mutex_unlock(nrt_Mutex * m)
{
    /* ReleaseMutex((HANDLE)m); */
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION) (*m);
    LeaveCriticalSection(lpCriticalSection);
}

NRTPROT(void) nrt_Mutex_init(nrt_Mutex * m)
{

    LPCRITICAL_SECTION lpCriticalSection =
        (LPCRITICAL_SECTION) NRT_MALLOC(sizeof(CRITICAL_SECTION));
    /* nrt_Debug_flogf("***Initializing Mutex***\n"); */
    InitializeCriticalSection(lpCriticalSection);
    /**m = CreateMutex(NULL, TRUE, "PluginRegistry::getMutex()");*/
    /* if (*m && GetLastError() == ERROR_ALREADY_EXISTS)
     * OpenMutex(MUTEX_ALL_ACCESS, FALSE, "PluginRegistry::getMutex()"); */
    *m = (nrt_Mutex) lpCriticalSection;
}

NRTPROT(void) nrt_Mutex_delete(nrt_Mutex * m)
{
    LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION) (*m);
    /* nrt_Debug_flogf("***Trying to Destroy Mutex***\n"); */
    if (lpCriticalSection)
    {
        DeleteCriticalSection(lpCriticalSection);
        /* nrt_Debug_flogf("***Destroyed Mutex***\n"); */
        NRT_FREE(lpCriticalSection);
    }
}
#endif

NRT_CXX_ENDGUARD
