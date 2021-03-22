/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NRT_LIST_H__
#define __NRT_LIST_H__
/*!
 *  \file
 *  \brief Contains OO-like linked-list data structure for pairs
 *
 *  This file contains an OO-like API for a linked list API of
 *  nrt_Pair objects (the first item is a string, the second is a
 *  generic pointer.
 *
 *  The structures are modeled on the C++ Standard Template Library.
 */
#include "nrt/System.h"

NRT_CXX_GUARD
/*!
 *  \struct nrt_ListNode
 *  \brief Node in a nrt_List
 *
 *  This object provides the base for a chain in a hash.
 *  It is a doubly-linked list with pair data (the first of which
 *  is copied, the second of which is not).
 */
typedef struct _NRT_ListNode
{
    /* ! Pointer to the next structure */
    struct _NRT_ListNode *next;
    /* ! Pointer to the previous structure */
    struct _NRT_ListNode *prev;
    /* ! The data */
    NRT_DATA *data;
} nrt_ListNode;

typedef NRT_DATA *(*NRT_DATA_ITEM_CLONE) (NRT_DATA *, nrt_Error *);

/*!
 *  \struct nrt_ListIterator
 *  \brief This is a basic iterator object for a chain
 *
 *  This object is a basic iterator for a nrt_List object.
 *  It is modeled on the C++ Standard Template Library
 */
typedef struct _NRT_ListIterator
{
    /* ! Pointer to the current node */
    nrt_ListNode *current;

} nrt_ListIterator;

/*!
 *  \struct nrt_List
 *  \brief A linked list of nrt_Pair items
 *
 *  This object is the controller for the nrt_ListNode nodes.
 *  It contains a pointer to the first and last items in its set.
 */
typedef struct _NRT_List
{
    /* ! A pointer to the beginning node */
    nrt_ListNode *first;
    /* ! A pointer to the final node */
    nrt_ListNode *last;

} nrt_List;

/*!
 *  Construct a new node.  This node will be attached to its
 *  previous and next nodes, if any.  The data in the nrt_Pair
 *  will be the data that is assigned.
 *
 *  \param prev  The previous node (the one in front of this one)
 *  \param next  The next node (the one after this one)
 *  \param data  The data to insert into the list
 *  \param error An error to populate on fatal issue
 *  \return The list node, or NULL if a problem occurred
 */
NRTAPI(nrt_ListNode *) nrt_ListNode_construct(nrt_ListNode * prev,
                                              nrt_ListNode * next,
                                              NRT_DATA * data,
                                              nrt_Error * error);

/*!
 *  Destroy the current chain link, and NULL set it
 *  We are not deleting the data, so I hope you did.
 *  \param this_node Node to destroy
 */
NRTAPI(void) nrt_ListNode_destruct(nrt_ListNode ** this_node);

/*!
 *  Is our chain empty?  Since our list ends with a NULL
 *  link, we simply check to see if the first link is NULL.
 *
 *  \param this_chain  The chain to check
 *  \return 1 if empty, 0 if contains items
 */
NRTAPI(NRT_BOOL) nrt_List_isEmpty(nrt_List * this_chain);

/*!
 *  Push something onto the front of our chain.  Note, as usual
 *  we REFUSE to allocate your data for you.  If you need a copy, make one
 *  up front.
 *
 *  \param this_chain  The chain to push information onto
 *  \param data The data to push to the front
 *  \param error An error if one occurred
 */
NRTAPI(NRT_BOOL) nrt_List_pushFront(nrt_List * this_chain, NRT_DATA * data,
                                    nrt_Error * error);

/*!
 *  Push something onto the back of our chain
 *  \param this_chain The chain to push information onto
 *  \param data The data to push onto the back
 *  \param error The error if one occurred
 */
NRTAPI(NRT_BOOL) nrt_List_pushBack(nrt_List * this_chain, NRT_DATA * data,
                                   nrt_Error * error);

/*!
 *  Pop the node off the front and return it.
 *  We check the first item to see if it exists.  If it does,
 *  Then we fill in, and return the value.
 *
 *  \param this_chain  The chain to pop from
 *  \return The link we popped off
 */
NRTAPI(NRT_DATA *) nrt_List_popFront(nrt_List * this_chain);

/*!
 *  Pop the node off the back and return it.
 *  We check the first item to see if it exists.  If it does,
 *  Then we fill in, and return the value
 *
 *  \param this_chain  The chain to pop from
 *  \return The link we popped off
 */
NRTAPI(NRT_DATA *) nrt_List_popBack(nrt_List * this_chain);

/*!
 *  Construct our chain and null-initialize the first and last pointers
 *  \param  error An error to populate on failure
 *  \return The chain, or NULL upon failure
 */
NRTAPI(nrt_List *) nrt_List_construct(nrt_Error * error);

/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param cloner A NRT_DATA_ITEM_CLONE function that gets called foreach
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NRTAPI(nrt_List *) nrt_List_clone(nrt_List * source, NRT_DATA_ITEM_CLONE cloner,
                                  nrt_Error * error);

/*!
 *  Delete the chain (from the back).  This means we destruct each node.
 *  This also means that you need to free all of the data
 *  in your list, prior to us
 *
 *  \param this_chain  The chain to delete
 */
NRTAPI(void) nrt_List_destruct(nrt_List ** this_chain);

/*!
 *  Return an iterator to the head of the chain
 *
 *  \param chain The chain to search
 *  \return An iterator to the head of the chain
 */
NRTAPI(nrt_ListIterator) nrt_List_begin(const nrt_List * chain);

/*!
 *  Return an iterator to the position in the chain
 *  specified by the index i.
 *
 *  \param chain The chain to search
 *  \param i the index
 *  \return An iterator to the index of the chain, as specified by i
 */
NRTAPI(nrt_ListIterator) nrt_List_at(const nrt_List * chain, int i);

/*!
 *  Check to see if two iterators point at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are equal, 0 if not
 */
NRTAPI(NRT_BOOL) nrt_ListIterator_equals(const nrt_ListIterator * it1,
                                         const nrt_ListIterator * it2);

/*!
 *  Check to see if two iterators are not pointing at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are not equal, 0 if so
 */
NRTAPI(NRT_BOOL) nrt_ListIterator_notEqualTo(const nrt_ListIterator * it1,
                                             const nrt_ListIterator * it2);

/*!
 *  Get an iterator to the tail of the chain
 *
 *  \param this_chain
 *  \return Iterator to chain tail
 */
NRTAPI(nrt_ListIterator) nrt_List_end(const nrt_List * this_chain);

/*!
 *  Insert data into the chain BEFORE the iterator, and make the iterator
 *  point at the new node.
 *
 *  <br>
 *  <ol>
 *  <li>If the iterator is pointing to NULL, we will insert into the back of
 *  this list</li>
 *  <li>If the iterator is pointing to the beginning, we will insert into the
 *  front</li>
 *
 *  <li>If the iterator is pointing at something else, we will insert after
 *  the iterator  (same case, in practice as pointing to the beginning)</li>
 *  </ol>
 *
 *  \param chain The chain
 *  \param iter The iterator to insert before
 *  \param data This is a pointer assignment, not a data copy
 *  \param error An error to populate on failure
 *
 *  \return 1 on success, 0 on failure
 */
NRTAPI(NRT_BOOL) nrt_List_insert(nrt_List * chain, nrt_ListIterator iter,
                                 NRT_DATA * data, nrt_Error * error);

/*!
 *  Remove the data from an arbitrary point in the list.
 *  This WILL NOT delete the data, but it will make sure
 *  that we have successfully updated the chain.  The
 *  iterator is moved to the value that the current node pointed to
 *  (the next value).
 *
 *  \param chain The list to remove from
 *  \param where Where to remove, this will be updated to the NEXT position
 *  \return We dont know how to delete YOUR object, so return it
 */
NRTAPI(NRT_DATA *) nrt_List_remove(nrt_List * chain, nrt_ListIterator * where);

/*!
 *  Moves the data located at oldIndex to newIndex.
 *
 *  \param chain    The source list
 *  \param oldIndex the index of the data item to move
 *  \param newIndex the index where the data item will be moved to
 *  \return NRT_SUCCESS on success, or NRT_FAILURE
 */
NRTAPI(NRT_BOOL) nrt_List_move(nrt_List * chain, uint32_t oldIndex,
                               uint32_t newIndex, nrt_Error * error);

/*!
 *  Return the size of the list
 *
 *  \param list The list to check
 *  \return size of the list
 */
NRTAPI(uint32_t) nrt_List_size(nrt_List * list);
NRTAPI(uint16_t) nrt_List_size16(nrt_List* list);

/*!
 *  Return the element at the specified position in the list
 *
 *  \param list The list
 *  \param index The index
 *  \param index The index
 *  \param error An error to populate on failure, if the index is out of bounds
 *  \return the data at the specified position
 */
NRTAPI(NRT_DATA *) nrt_List_get(nrt_List * list, int index, nrt_Error * error);

/*!
 *  Increment the iterator.  Eventually, this will point at NULL.
 *
 *  \param this_iter Iterator to increment
 *
 */
NRTAPI(void) nrt_ListIterator_increment(nrt_ListIterator * this_iter);

/*!
 *  Get the data at this iterators pointer
 *
 *  \return The data
 *
 */
NRTAPI(NRT_DATA *) nrt_ListIterator_get(nrt_ListIterator * this_iter);

NRT_CXX_ENDGUARD
#endif
