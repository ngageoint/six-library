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

#ifndef __NITF_SYSTEM_H__
#define __NITF_SYSTEM_H__

#ifdef WIN32
#      if defined(NITF_MODULE_EXPORTS)
#          ifndef NRT_MODULE_EXPORTS
#              define NRT_MODULE_EXPORTS
#          endif
#      elif defined(NITF_MODULE_IMPORTS)
#          ifndef NRT_MODULE_IMPORTS
#              define NRT_MODULE_IMPORTS
#          endif
#      endif
#endif

/******************************************************************************/
/* DEFINES                                                                    */
/******************************************************************************/
#include "nrt/Defines.h"
#define NITF_C              NRT_C
#define NITF_GUARD          NRT_GUARD
#define NITF_ENDGUARD       NRT_ENDGUARD
#define NITF_BOOL           NRT_BOOL
#define NITFAPI(RT)         NRTAPI(RT)
#define NITFPROT(RT)        NRTPROT(RT)
#define NITF_ATO32          NRT_ATO32
#define NITF_ATOU32         NRT_ATOU32
#define NITF_ATOU32_BASE    NRT_ATOU32_BASE
#define NITF_ATO64          NRT_ATO64
#define NITF_SNPRINTF       NRT_SNPRINTF
#define NITF_VSNPRINTF      NRT_VSNPRINTF
#define NITF_CXX_GUARD      NRT_CXX_GUARD
#define NITF_CXX_ENDGUARD   NRT_CXX_ENDGUARD
#define NITFPRIV            NRTPRIV
#define NITF_FILE           NRT_FILE
#define NITF_LINE           NRT_LINE
#define NITF_FUNC           NRT_FUNC


/******************************************************************************/
/* DEBUG                                                                      */
/******************************************************************************/
#ifdef NITF_DEBUG
    #define NRT_DEBUG 1
    #include "nrt/Debug.h"

    #define NITF_MEM_LOG        NRT_MEM_LOG
    #define nitf_Debug_malloc   nrt_Debug_malloc
    #define nitf_Debug_realloc  nrt_Debug_realloc
    #define nitf_Debug_free     nrt_Debug_free
#endif
#define nitf_Debug_flogf    nrt_Debug_flogf

/******************************************************************************/
/* MEMORY                                                                     */
/******************************************************************************/
#include "nrt/Memory.h"
#define NITF_MALLOC NRT_MALLOC
#define NITF_REALLOC NRT_REALLOC
#define NITF_FREE NRT_FREE


/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/
#include "nrt/Types.h"
typedef nrt_Uint8                   nitf_Uint8;
typedef nrt_Uint16                  nitf_Uint16;
typedef nrt_Uint32                  nitf_Uint32;
typedef nrt_Uint64                  nitf_Uint64;
typedef nrt_Int8                    nitf_Int8;
typedef nrt_Int16                   nitf_Int16;
typedef nrt_Int32                   nitf_Int32;
typedef nrt_Int64                   nitf_Int64;
typedef nrt_IOHandle                nitf_IOHandle;
typedef NRT_NATIVE_DLL              NITF_NATIVE_DLL;
typedef NRT_DLL_FUNCTION_PTR        NITF_DLL_FUNCTION_PTR;
typedef nrt_AccessFlags             nitf_AccessFlags;
typedef nrt_CreationFlags           nitf_CreationFlags;
typedef nrt_Off                     nitf_Off;

#define NITF_MAX_PATH               NRT_MAX_PATH
#define NITF_DEFAULT_PERM           NRT_DEFAULT_PERM
#define NITF_INVALID_HANDLE_VALUE   NRT_INVALID_HANDLE_VALUE
#define NITF_INVALID_HANDLE         NRT_INVALID_HANDLE
#define NITF_SEEK_CUR               NRT_SEEK_CUR
#define NITF_SEEK_SET               NRT_SEEK_SET
#define NITF_SEEK_END               NRT_SEEK_END
#define NITF_CREATE                 NRT_CREATE
#define NITF_OPEN_EXISTING          NRT_OPEN_EXISTING
#define NITF_TRUNCATE               NRT_TRUNCATE
#define NITF_ACCESS_READONLY        NRT_ACCESS_READONLY
#define NITF_ACCESS_WRITEONLY       NRT_ACCESS_WRITEONLY
#define NITF_ACCESS_READWRITE       NRT_ACCESS_READWRITE
#define NITF_INT64                  NRT_INT64
#define NITF_SUCCESS                NRT_SUCCESS
#define NITF_FAILURE                NRT_FAILURE

typedef nrt_CornersType             nitf_CornersType;
#define NITF_CORNERS_UNKNOWN        NRT_CORNERS_UNKNOWN
#define NITF_CORNERS_UTM            NRT_CORNERS_UTM
#define NITF_CORNERS_UTM_UPS_S      NRT_CORNERS_UTM_UPS_S
#define NITF_CORNERS_UTM_UPS_N      NRT_CORNERS_UTM_UPS_N
#define NITF_CORNERS_GEO            NRT_CORNERS_GEO
#define NITF_CORNERS_DECIMAL        NRT_CORNERS_DECIMAL

#define nitf_System_swap16          nrt_System_swap16
#define nitf_System_swap32          nrt_System_swap32
#define nitf_System_swap64          nrt_System_swap64
#define nitf_System_swap64c         nrt_System_swap64c

#define NITF_NTOHS                  NRT_NTOHS
#define NITF_HTONS                  NRT_HTONS
#define NITF_NTOHL                  NRT_NTOHL
#define NITF_HTONL                  NRT_HTONL
#define NITF_HTONLL                 NRT_HTONLL
#define NITF_NTOHLL                 NRT_NTOHLL
#define NITF_HTONLC                 NRT_HTONLC
#define NITF_NTOHLC                 NRT_NTOHLC


/******************************************************************************/
/* ERROR                                                                      */
/******************************************************************************/
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


/******************************************************************************/
/* DLL                                                                        */
/******************************************************************************/
#include "nrt/DLL.h"
#define NITF_DLL_EXTENSION  NRT_DLL_EXTENSION
typedef nrt_DLL             nitf_DLL;
#define nitf_DLL_construct  nrt_DLL_construct
#define nitf_DLL_destruct   nrt_DLL_destruct
#define nitf_DLL_load       nrt_DLL_load
#define nitf_DLL_unload     nrt_DLL_unload
#define nitf_DLL_retrieve   nrt_DLL_retrieve
#define nitf_DLL_isValid    nrt_DLL_isValid


/******************************************************************************/
/* SYNC                                                                       */
/******************************************************************************/
#include "nrt/Sync.h"
#define nitf_Mutex          nrt_Mutex
#define NITF_MUTEX_INIT     NRT_MUTEX_INIT
#define nitf_Mutex_lock     nrt_Mutex_lock
#define nitf_Mutex_unlock   nrt_Mutex_unlock
#define nitf_Mutex_init     nrt_Mutex_init
#define nitf_Mutex_delete   nrt_Mutex_delete


/******************************************************************************/
/* DIRECTORY                                                                  */
/******************************************************************************/
#include "nrt/Directory.h"
typedef nrt_Directory                   nitf_Directory;
#define nitf_Directory_construct        nrt_Directory_construct
#define nitf_Directory_findFirstFile    nrt_Directory_findFirstFile
#define nitf_Directory_findNextFile     nrt_Directory_findNextFile
#define nitf_Directory_destruct         nrt_Directory_destruct
#define nitf_Directory_exists           nrt_Directory_exists


/******************************************************************************/
/* IOHANDLE                                                                   */
/******************************************************************************/
#include "nrt/IOHandle.h"
#define NITF_IO_SUCCESS         NRT_IO_SUCCESS
#define NITF_MAX_READ_ATTEMPTS  NRT_MAX_READ_ATTEMPTS

#define nitf_IOHandle_create    nrt_IOHandle_create
#define nitf_IOHandle_read      nrt_IOHandle_read
#define nitf_IOHandle_write     nrt_IOHandle_write
#define nitf_IOHandle_seek      nrt_IOHandle_seek
#define nitf_IOHandle_tell      nrt_IOHandle_tell
#define nitf_IOHandle_getSize   nrt_IOHandle_getSize
#define nitf_IOHandle_close     nrt_IOHandle_close


/******************************************************************************/
/* IOINTERFACE                                                                */
/******************************************************************************/
#include "nrt/IOInterface.h"

typedef NRT_IO_INTERFACE_READ           NITF_IO_INTERFACE_READ;
typedef NRT_IO_INTERFACE_WRITE          NITF_IO_INTERFACE_WRITE;
typedef NRT_IO_INTERFACE_CAN_SEEK       NITF_IO_INTERFACE_CAN_SEEK;
typedef NRT_IO_INTERFACE_SEEK           NITF_IO_INTERFACE_SEEK;
typedef NRT_IO_INTERFACE_TELL           NITF_IO_INTERFACE_TELL;
typedef NRT_IO_INTERFACE_GET_SIZE       NITF_IO_INTERFACE_GET_SIZE;
typedef NRT_IO_INTERFACE_GET_MODE       NITF_IO_INTERFACE_GET_MODE;
typedef NRT_IO_INTERFACE_CLOSE          NITF_IO_INTERFACE_CLOSE;
typedef NRT_IO_INTERFACE_DESTRUCT       NITF_IO_INTERFACE_DESTRUCT;

typedef nrt_IIOInterface                nitf_IIOInterface;
typedef nrt_IOInterface                 nitf_IOInterface;

#define nitf_IOInterface_read           nrt_IOInterface_read
#define nitf_IOInterface_write          nrt_IOInterface_write
#define nitf_IOInterface_canSeek        nrt_IOInterface_canSeek
#define nitf_IOInterface_seek           nrt_IOInterface_seek
#define nitf_IOInterface_tell           nrt_IOInterface_tell
#define nitf_IOInterface_getSize        nrt_IOInterface_getSize
#define nitf_IOInterface_getMode        nrt_IOInterface_getMode
#define nitf_IOInterface_close          nrt_IOInterface_close
#define nitf_IOInterface_destruct       nrt_IOInterface_destruct
#define nitf_IOHandleAdapter_construct  nrt_IOHandleAdapter_construct
#define nitf_IOHandleAdapter_open       nrt_IOHandleAdapter_open
#define nitf_BufferAdapter_construct    nrt_BufferAdapter_construct


/******************************************************************************/
/* DATETIME                                                                   */
/******************************************************************************/
#include "nrt/DateTime.h"
#define NITF_DATE_FORMAT_20  "%d%H%M%SZ%b%y"
#define NITF_DATE_FORMAT_21  "%Y%m%d%H%M%S"

typedef nrt_DateTime                    nitf_DateTime;
#define nitf_DateTime_now               nrt_DateTime_now
#define nitf_DateTime_fromMillis        nrt_DateTime_fromMillis
#define nitf_DateTime_setYear           nrt_DateTime_setYear
#define nitf_DateTime_setMonth          nrt_DateTime_setMonth
#define nitf_DateTime_setDayOfMonth     nrt_DateTime_setDayOfMonth
#define nitf_DateTime_setDayOfWeek      nrt_DateTime_setDayOfWeek
#define nitf_DateTime_setDayOfYear      nrt_DateTime_setDayOfYear
#define nitf_DateTime_setHour           nrt_DateTime_setHour
#define nitf_DateTime_setMinute         nrt_DateTime_setMinute
#define nitf_DateTime_setSecond         nrt_DateTime_setSecond
#define nitf_DateTime_setTimeInMillis   nrt_DateTime_setTimeInMillis
#define nitf_DateTime_fromString        nrt_DateTime_fromString
#define nitf_DateTime_destruct          nrt_DateTime_destruct
#define nitf_DateTime_format            nrt_DateTime_format
#define nitf_DateTime_formatMillis      nrt_DateTime_formatMillis


/******************************************************************************/
/* PAIR                                                                       */
/******************************************************************************/
#include "nrt/Pair.h"
typedef nrt_Pair        nitf_Pair;
#define nitf_Pair_init  nrt_Pair_init
#define nitf_Pair_copy  nrt_Pair_copy


/******************************************************************************/
/* LIST                                                                       */
/******************************************************************************/
#include "nrt/List.h"
typedef nrt_ListNode                    nitf_ListNode;
typedef NRT_DATA_ITEM_CLONE             NITF_DATA_ITEM_CLONE;
typedef nrt_ListIterator                nitf_ListIterator;
typedef nrt_List                        nitf_List;

#define nitf_ListNode_construct         nrt_ListNode_construct
#define nitf_ListNode_destruct          nrt_ListNode_destruct
#define nitf_List_isEmpty               nrt_List_isEmpty
#define nitf_List_pushFront             nrt_List_pushFront
#define nitf_List_pushBack              nrt_List_pushBack
#define nitf_List_popFront              nrt_List_popFront
#define nitf_List_popBack               nrt_List_popBack
#define nitf_List_construct             nrt_List_construct
#define nitf_List_clone                 nrt_List_clone
#define nitf_List_destruct              nrt_List_destruct
#define nitf_List_begin                 nrt_List_begin
#define nitf_List_at                    nrt_List_at
#define nitf_ListIterator_equals        nrt_ListIterator_equals
#define nitf_ListIterator_notEqualTo    nrt_ListIterator_notEqualTo
#define nitf_List_end                   nrt_List_end
#define nitf_List_insert                nrt_List_insert
#define nitf_List_remove                nrt_List_remove
#define nitf_List_move                  nrt_List_move
#define nitf_List_size                  nrt_List_size
#define nitf_List_get                   nrt_List_get
#define nitf_ListIterator_increment     nrt_ListIterator_increment
#define nitf_ListIterator_get           nrt_ListIterator_get


/******************************************************************************/
/* HASHTABLE                                                                  */
/******************************************************************************/
#include "nrt/HashTable.h"
#define NITF_TRE_HASH_SIZE 8

#define NITF_DATA_RETAIN_OWNER              NRT_DATA_RETAIN_OWNER
#define NITF_DATA_ADOPT                     NRT_DATA_ADOPT

typedef nrt_HashTable                       nitf_HashTable;
typedef NRT_HASH_FUNCTION                   NITF_HASH_FUNCTION;
typedef NRT_HASH_FUNCTOR                    NITF_HASH_FUNCTOR;
typedef nrt_HashTableIterator               nitf_HashTableIterator;

#define nitf_HashTable_construct            nrt_HashTable_construct
#define nitf_HashTable_clone                nrt_HashTable_clone
#define nitf_HashTable_setPolicy            nrt_HashTable_setPolicy
#define nitf_HashTable_remove               nrt_HashTable_remove
#define nitf_HashTable_initDefaults         nrt_HashTable_initDefaults
#define nitf_HashTable_destruct             nrt_HashTable_destruct
#define nitf_HashTable_exists               nrt_HashTable_exists
#define nitf_HashTable_print                nrt_HashTable_print
#define nitf_HashTable_foreach              nrt_HashTable_foreach
#define nitf_HashTable_insert               nrt_HashTable_insert
#define nitf_HashTable_find                 nrt_HashTable_find
#define nitf_HashTableIterator_equals       nrt_HashTableIterator_equals
#define nitf_HashTableIterator_notEqualTo   nrt_HashTableIterator_notEqualTo
#define nitf_HashTable_begin                nrt_HashTable_begin
#define nitf_HashTable_end                  nrt_HashTable_end
#define nitf_HashTableIterator_increment    nrt_HashTableIterator_increment
#define nitf_HashTableIterator_get          nrt_HashTableIterator_get


/******************************************************************************/
/* INTSTACK                                                                   */
/******************************************************************************/
#include "nrt/IntStack.h"
typedef nrt_IntStack                nitf_IntStack;

#define nitf_IntStack_construct     nrt_IntStack_construct
#define nitf_IntStack_clone         nrt_IntStack_clone
#define nitf_IntStack_destruct      nrt_IntStack_destruct
#define nitf_IntStack_top           nrt_IntStack_top
#define nitf_IntStack_push          nrt_IntStack_push
#define nitf_IntStack_pop           nrt_IntStack_pop
#define nitf_IntStack_depth         nrt_IntStack_depth

#define NITF_INT_STACK_DEPTH        NRT_INT_STACK_DEPTH


/******************************************************************************/
/* TREE                                                                       */
/******************************************************************************/
#include "nrt/Tree.h"
typedef nrt_TreeNode                nitf_TreeNode;
typedef nrt_Tree                    nitf_Tree;

#define nitf_TreeNode_construct     nrt_TreeNode_construct
#define nitf_TreeNode_destruct      nrt_TreeNode_destruct
#define nitf_TreeNode_addChild      nrt_TreeNode_addChild
#define nitf_TreeNode_hasChildren   nrt_TreeNode_hasChildren
#define nitf_TreeNode_removeChild   nrt_TreeNode_removeChild
#define nitf_TreeNode_clone         nrt_TreeNode_clone
#define nitf_Tree_construct         nrt_Tree_construct
#define nitf_Tree_destruct          nrt_Tree_destruct

typedef nrt_Traversal               nitf_Traversal;
#define NITF_PRE_ORDER              NRT_PRE_ORDER
#define NITF_POST_ORDER             NRT_POST_ORDER
typedef NRT_TREE_TRAVERSER          NITF_TREE_TRAVERSER;
#define nitf_Tree_walk              nrt_Tree_walk
#define nitf_Tree_clone             nrt_Tree_clone


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/
#include "nrt/Utils.h"
#define nitf_Utils_splitString                  nrt_Utils_splitString
#define nitf_Utils_isNumeric                    nrt_Utils_isNumeric
#define nitf_Utils_isAlpha                      nrt_Utils_isAlpha
#define nitf_Utils_isBlank                      nrt_Utils_isBlank
#define nitf_Utils_trimString                   nrt_Utils_trimString
#define nitf_Utils_replace                      nrt_Utils_replace
#define nitf_Utils_baseName                     nrt_Utils_baseName
#define nitf_Utils_parseDecimalString           nrt_Utils_parseDecimalString
#define nitf_Utils_getCurrentTimeMillis         nrt_Utils_getCurrentTimeMillis
#define nitf_Utils_strncasecmp                  nrt_Utils_strncasecmp
#define nitf_Utils_decimalToGeographic          nrt_Utils_decimalToGeographic
#define nitf_Utils_geographicToDecimal          nrt_Utils_geographicToDecimal
#define nitf_Utils_parseGeographicString        nrt_Utils_parseGeographicString
#define nitf_Utils_geographicLatToCharArray     nrt_Utils_geographicLatToCharArray
#define nitf_Utils_geographicLonToCharArray     nrt_Utils_geographicLonToCharArray
#define nitf_Utils_decimalLatToCharArray        nrt_Utils_decimalLatToCharArray
#define nitf_Utils_decimalLonToCharArray        nrt_Utils_decimalLonToCharArray
#define nitf_Utils_decimalLatToGeoCharArray     nrt_Utils_decimalLatToGeoCharArray
#define nitf_Utils_decimalLonToGeoCharArray     nrt_Utils_decimalLonToGeoCharArray
#define nitf_Utils_cornersTypeAsCoordRep        nrt_Utils_cornersTypeAsCoordRep

/******************************************************************************/
/* NITRO-SPECIFIC DEFINES/TYPES                                               */
/******************************************************************************/
#include "nitf/Defines.h"
#include "nitf/Types.h"

#endif
