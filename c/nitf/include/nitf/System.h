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

#ifndef __NITF_SYSTEM_H__
#define __NITF_SYSTEM_H__

#ifdef NITF_DEBUG
    #define NRT_DEBUG 1
    #include "nrt/Debug.h"

    #define NITF_MEM_LOG        NRT_MEM_LOG
    #define nitf_Debug_malloc   nrt_Debug_malloc
    #define nitf_Debug_realloc  nrt_Debug_realloc
    #define nitf_Debug_free     nrt_Debug_free
#endif
#define nitf_Debug_flogf    nrt_Debug_flogf

#include "nrt/Memory.h"
#define NITF_MALLOC NRT_MALLOC
#define NITF_REALLOC NRT_REALLOC
#define NITF_FREE NRT_FREE

#include "nitf/Defines.h"
#include "nitf/Types.h"

#include "nrt/Error.h"
#define  NITF_MAX_EMESSAGE                  NRT_MAX_EMESSAGE
#define  NITF_CTXT                          NRT_CTXT

#define NITF_ERRNO                          NRT_ERRNO
#define NITF_STRERROR                       NRT_STRERROR

#define NITF_NO_ERR                         NRT_NO_ERR
#define NITF_ERR_MEMORY                     NRT_ERR_MEMORY
#define NITF_ERR_OPENING_FILE               NRT_ERR_OPENING_FILE
#define NITF_ERR_READING_FROM_FILE          NRT_ERR_READING_FROM_FILE
#define NITF_ERR_SEEKING_IN_FILE            NRT_ERR_SEEKING_IN_FILE
#define NITF_ERR_WRITING_TO_FILE            NRT_ERR_WRITING_TO_FILE
#define NITF_ERR_STAT_FILE                  NRT_ERR_STAT_FILE
#define NITF_ERR_LOADING_DLL                NRT_ERR_LOADING_DLL
#define NITF_ERR_UNLOADING_DLL              NRT_ERR_UNLOADING_DLL
#define NITF_ERR_RETRIEVING_DLL_HOOK        NRT_ERR_RETRIEVING_DLL_HOOK
#define NITF_ERR_UNINITIALIZED_DLL_READ     NRT_ERR_UNINITIALIZED_DLL_READ
#define NITF_ERR_INVALID_PARAMETER          NRT_ERR_INVALID_PARAMETER
#define NITF_ERR_INVALID_OBJECT             NRT_ERR_INVALID_OBJECT
#define NITF_ERR_INVALID_FILE               NRT_ERR_INVALID_FILE
#define NITF_ERR_COMPRESSION                NRT_ERR_COMPRESSION
#define NITF_ERR_DECOMPRESSION              NRT_ERR_DECOMPRESSION
#define NITF_ERR_PARSING_FILE               NRT_ERR_PARSING_FILE
#define NITF_ERR_INT_STACK_OVERFLOW         NRT_ERR_INT_STACK_OVERFLOW
#define NITF_ERR_UNK                        NRT_ERR_UNK

typedef nrt_Error                           nitf_Error;

#define nitf_Error_init                     nrt_Error_init
#define nitf_Error_flogf                    nrt_Error_flogf
#define nitf_Error_fprintf                  nrt_Error_fprintf
#define nitf_Error_initf                    nrt_Error_initf
#define nitf_Error_print                    nrt_Error_print


#include "nrt/DLL.h"
#define NITF_DLL_EXTENSION  NRT_DLL_EXTENSION
typedef nrt_DLL             nitf_DLL;
#define nitf_DLL_construct  nrt_DLL_construct
#define nitf_DLL_destruct   nrt_DLL_destruct
#define nitf_DLL_load       nrt_DLL_load
#define nitf_DLL_unload     nrt_DLL_unload
#define nitf_DLL_retrieve   nrt_DLL_retrieve
#define nitf_DLL_isValid    nrt_DLL_isValid


#include "nrt/Sync.h"
#define nitf_Mutex          nrt_Mutex
#define NITF_MUTEX_INIT     NRT_MUTEX_INIT
#define nitf_Mutex_lock     nrt_Mutex_lock
#define nitf_Mutex_unlock   nrt_Mutex_unlock
#define nitf_Mutex_init     nrt_Mutex_init
#define nitf_Mutex_delete   nrt_Mutex_delete

#include "nitf/Directory.h"
#include "nitf/IOHandle.h"
/*  Now we are cross-platform (except for IO -- see IOHandle.h)  */
#endif
