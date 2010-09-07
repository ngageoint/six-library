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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_LIST_H__
#define __NITF_LIST_H__
/*!
 *  \file
 *  \brief Contains OO-like linked-list data structure for pairs
 *
 *  This file contains an OO-like API for a linked list API of
 *  nitf_Pair objects (the first item is a string, the second is a
 *  generic pointer.
 *
 *  The structures are modeled on the C++ Standard Template Library.
 */
#include "nitf/System.h"
#include "nrt/List.h"

NITF_CXX_GUARD

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

NITF_CXX_ENDGUARD

#endif
