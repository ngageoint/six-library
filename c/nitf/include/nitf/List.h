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
#include "nitf/Error.h"

NITF_CXX_GUARD
/*!
 *  \struct nitf_ListNode
 *  \brief Node in a nitf_List
 *
 *  This object provides the base for a chain in a hash.
 *  It is a doubly-linked list with pair data (the first of which
 *  is copied, the second of which is not).
 */
typedef struct _nitf_ListNode
{
    /*!  Pointer to the next structure  */
    struct _nitf_ListNode *next;
    /*!  Pointer to the previous structure  */
    struct _nitf_ListNode *prev;
    /*!  The data  */
    NITF_DATA *data;

}
nitf_ListNode;

typedef NITF_DATA *(*NITF_DATA_ITEM_CLONE) (NITF_DATA *, nitf_Error *);

/*!
 *  \struct nitf_ListIterator
 *  \brief This is a basic iterator object for a chain
 *
 *  This object is a basic iterator for a nitf_List object.
 *  It is modeled on the C++ Standard Template Library
 */
typedef struct _nitf_ListIterator
{
    /*!  Pointer to the current node  */
    nitf_ListNode *current;

}
nitf_ListIterator;

/*!
 *  \struct nitf_List
 *  \brief A linked list of nitf_Pair items
 *
 *  This object is the controller for the nitf_ListNode nodes.
 *  It contains a pointer to the first and last items in its set.
 */
typedef struct _nitf_List
{
    /*!  A pointer to the beginning node  */
    nitf_ListNode *first;
    /*!  A pointer to the final node  */
    nitf_ListNode *last;

}
nitf_List;

/*!
 *  Construct a new node.  This node will be attached to its
 *  previous and next nodes, if any.  The data in the nitf_Pair
 *  will be the data that is assigned.
 *
 *  \param prev  The previous node (the one in front of this one)
 *  \param next  The next node (the one after this one)
 *  \param data  The data to insert into the list
 *  \param error An error to populate on fatal issue
 *  \return The list node, or NULL if a problem occurred
 */
NITFAPI(nitf_ListNode *) nitf_ListNode_construct(nitf_ListNode * prev,
        nitf_ListNode * next,
        NITF_DATA * data,
        nitf_Error * error);

/*!
 *  Destroy the current chain link, and NULL set it
 *  We are not deleting the data, so I hope you did.
 *  \param this_node Node to destroy
 */
NITFAPI(void) nitf_ListNode_destruct(nitf_ListNode ** this_node);

/*!
 *  Is our chain empty?  Since our list ends with a NULL
 *  link, we simply check to see if the first link is NULL.
 *
 *  \param this_chain  The chain to check
 *  \return 1 if empty, 0 if contains items
 */
NITFAPI(NITF_BOOL) nitf_List_isEmpty(nitf_List * this_chain);

/*!
 *  Push something onto the front of our chain.  Note, as usual
 *  we REFUSE to allocate your data for you.  If you need a copy, make one
 *  up front.
 *
 *  \param this_chain  The chain to push information onto
 *  \param data The data to push to the front
 *  \param error An error if one occurred
 */
NITFAPI(NITF_BOOL) nitf_List_pushFront(nitf_List * this_chain,
                                       NITF_DATA * data,
                                       nitf_Error * error);

/*!
 *  Push something onto the back of our chain
 *  \param this_chain The chain to push information onto
 *  \param data The data to push onto the back
 *  \param error The error if one occurred
 */
NITFAPI(NITF_BOOL) nitf_List_pushBack(nitf_List * this_chain,
                                      NITF_DATA * data,
                                      nitf_Error * error);

/*!
 *  Pop the node off the front and return it.
 *  We check the first item to see if it exists.  If it does,
 *  Then we fill in, and return the value.
 *
 *  \param this_chain  The chain to pop from
 *  \return The link we popped off
 */
NITFAPI(NITF_DATA *) nitf_List_popFront(nitf_List * this_chain);

/*!
 *  Pop the node off the back and return it.
 *  We check the first item to see if it exists.  If it does,
 *  Then we fill in, and return the value
 *
 *  \param this_chain  The chain to pop from
 *  \return The link we popped off
 */
NITFAPI(NITF_DATA *) nitf_List_popBack(nitf_List * this_chain);


/*!
 *  Construct our chain and null-initialize the first and last pointers
 *  \param  error An error to populate on failure
 *  \return The chain, or NULL upon failure
 */
NITFAPI(nitf_List *) nitf_List_construct(nitf_Error * error);

/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param cloner A NITF_DATA_ITEM_CLONE function that gets called foreach
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_List *) nitf_List_clone(nitf_List * source,
                                     NITF_DATA_ITEM_CLONE cloner,
                                     nitf_Error * error);

/*!
 *  Delete the chain (from the back).  This means we destruct each node.
 *  This also means that you need to free all of the data
 *  in your list, prior to us
 *
 *  \param this_chain  The chain to delete
 */
NITFAPI(void) nitf_List_destruct(nitf_List ** this_chain);

/*!
 *  Return an iterator to the head of the chain
 *
 *  \param chain The chain to search
 *  \return An iterator to the head of the chain
 */
NITFAPI(nitf_ListIterator) nitf_List_begin(nitf_List * chain);

/*!
 *  Return an iterator to the position in the chain
 *  specified by the index i.
 *
 *  \param chain The chain to search
 *  \param i the index
 *  \return An iterator to the index of the chain, as specified by i
 */
NITFAPI(nitf_ListIterator) nitf_List_at(nitf_List * chain, int i);

/*!
 *  Check to see if two iterators point at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are equal, 0 if not
 */
NITFAPI(NITF_BOOL) nitf_ListIterator_equals(nitf_ListIterator * it1,
        nitf_ListIterator * it2);

/*!
 *  Check to see if two iterators are not pointing at the same thing
 *
 *  \param it1  Iterator 1
 *  \param it2  Iterator 2
 *  \return 1 if they are not equal, 0 if so
 */
NITFAPI(NITF_BOOL) nitf_ListIterator_notEqualTo(nitf_ListIterator * it1,
        nitf_ListIterator * it2);

/*!
 *  Get an iterator to the tail of the chain
 *
 *  \param this_chain
 *  \return Iterator to chain tail
 */
NITFAPI(nitf_ListIterator) nitf_List_end(nitf_List * this_chain);

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
NITFAPI(NITF_BOOL) nitf_List_insert(nitf_List * chain,
                                    nitf_ListIterator iter,
                                    NITF_DATA * data, nitf_Error * error);

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
NITFAPI(NITF_DATA *) nitf_List_remove(nitf_List * chain,
                                      nitf_ListIterator * where);


/*!
 *  Moves the data located at oldIndex to newIndex.
 *
 *  \param chain    The source list
 *  \param oldIndex the index of the data item to move
 *  \param newIndex the index where the data item will be moved to
 *  \return NITF_SUCCESS on success, or NITF_FAILURE
 */
NITFAPI(NITF_BOOL) nitf_List_move(nitf_List * chain,
                                  nitf_Uint32 oldIndex,
                                  nitf_Uint32 newIndex,
                                  nitf_Error * error);

/*!
 *  Return the size of the list
 *
 *  \param list The list to check
 *  \return size of the list
 */
NITFAPI(nitf_Uint32) nitf_List_size(nitf_List * list);


/*!
 *  Return the element at the specified position in the list
 *
 *  \param list The list
 *  \param index The index
 *  \param index The index
 *  \param error An error to populate on failure, if the index is out of bounds
 *  \return the data at the specified position
 */
NITFAPI(NITF_DATA*) nitf_List_get(nitf_List * list,
                                  int index,
                                  nitf_Error* error);


/*!
 *  Increment the iterator.  Eventually, this will point at NULL.
 *
 *  \param this_iter Iterator to increment
 *
 */
NITFAPI(void) nitf_ListIterator_increment(nitf_ListIterator * this_iter);

/*!
 *  Get the data at this iterators pointer
 *
 *  \return The data
 *
 */
NITFAPI(NITF_DATA *) nitf_ListIterator_get(nitf_ListIterator * this_iter);

NITF_CXX_ENDGUARD

#endif
