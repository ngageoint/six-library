/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_SYNC_H__
#define __NITF_SYNC_H__

#include "nitf/Defines.h"
#include "nitf/Types.h"
#include "nitf/Memory.h"

NITF_CXX_GUARD

#if defined(WIN32)
    typedef LPCRITICAL_SECTION nitf_Mutex;
#elif defined(__sgi)
#   include <sys/atomic_ops.h>
#   define NITF_MUTEX_INIT 0
    typedef int nitf_Mutex;
#else
#   include <pthread.h>
#   define NITF_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
    typedef pthread_mutex_t nitf_Mutex;
#endif

NITFPROT(void) nitf_Mutex_lock(nitf_Mutex* m);
NITFPROT(void) nitf_Mutex_unlock(nitf_Mutex* m);
NITFPROT(void) nitf_Mutex_init(nitf_Mutex* m);
NITFPROT(void) nitf_Mutex_delete(nitf_Mutex* m);

NITF_CXX_ENDGUARD

#endif

