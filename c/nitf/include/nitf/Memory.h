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

#ifndef __NITF_MEMORY_H__
#define __NITF_MEMORY_H__

/*
 *  \file
 *  Memory is a very simple allocation tracker.  When NITF_DEBUG
 *  is on, NITF_MALLOC and NITF_FREE to book-keeping.  When it is not,
 *  they turn into malloc() and free()
 */

#ifdef NITF_DEBUG
#   include "nitf/Debug.h"
#   define NITF_MALLOC(P)  nitf_Debug_malloc(__FILE__, __LINE__, P)
#   define NITF_REALLOC(P, S) nitf_Debug_realloc(__FILE__, __LINE__, P, S)
#   define NITF_FREE(P)    nitf_Debug_free(__FILE__, __LINE__, P)
#else
#   define NITF_MALLOC  malloc
#   define NITF_REALLOC realloc
#   define NITF_FREE    free
#endif

#endif
