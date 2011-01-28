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

#ifndef __NRT_SYNC_H__
#define __NRT_SYNC_H__

#include "nrt/Defines.h"
#include "nrt/Types.h"
#include "nrt/Memory.h"

NRT_CXX_GUARD
#if defined(WIN32)
typedef LPCRITICAL_SECTION nrt_Mutex;
#elif defined(__sgi)
#   include <sys/atomic_ops.h>
#   define NRT_MUTEX_INIT 0
typedef int nrt_Mutex;
#else
#   include <pthread.h>
#   define NRT_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
typedef pthread_mutex_t nrt_Mutex;
#endif

NRTPROT(void) nrt_Mutex_lock(nrt_Mutex * m);
NRTPROT(void) nrt_Mutex_unlock(nrt_Mutex * m);
NRTPROT(void) nrt_Mutex_init(nrt_Mutex * m);
NRTPROT(void) nrt_Mutex_delete(nrt_Mutex * m);

NRT_CXX_ENDGUARD
#endif
