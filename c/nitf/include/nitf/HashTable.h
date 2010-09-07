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

#ifndef __NITF_HASH_TABLE_H__
#define __NITF_HASH_TABLE_H__

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

#endif
