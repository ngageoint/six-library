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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_LIST_HPP__
#define __NITF_LIST_HPP__
#pragma once

#include <string>

#include "nitf/coda-oss.hpp"
#include "nitf/System.hpp"
#include "nitf/Object.hpp"

/*!
 *  \file List.hpp
 *  \brief  Contains wrapper implementation for List
 */

namespace nitf
{

/*!
 *  \class ListNode
 *  \brief  The C++ wrapper for the nitf_ListNode
 *
 *  This object provides the base for a chain in a hash.
 *  It is a doubly-linked list with pair data (the first of
 *  which is copied, the second of which is not).
 */
DECLARE_CLASS(ListNode)
{
public:
    //! Constructor
    ListNode() = default;

    //! Copy constructor
    ListNode(const ListNode& x) { *this = x; }

    //! Assignment Operator
    ListNode & operator=(const ListNode & x);

    //! Set native object
    ListNode(nitf_ListNode * x);

    /*!
     *  Constructor
     *  \param prev  The previous node
     *  \param next  The next node
     *  \param data  The data to insert into the list
     */
    ListNode(nitf::ListNode & prev, nitf::ListNode & next, NITF_DATA* data);

    //! Destructor
    ~ListNode() {}

    //! Get the data
    NITF_DATA * getData() const;

    //! Set the data
    void setData(NITF_DATA * value);

private:
    friend class List;
    friend struct ListIterator;

    nitf_Error error{};
};

/*!
 *  \class ListIterator
 *  \brief  The C++ wrapper for the nitf_ListIterator
 *
 *  Iterates a linked list.
 */
struct ListIterator /*final*/   // no "final", SWIG doesn't like it
{
    ListIterator() = default;
    ~ListIterator() = default;

    //! Copy constructor
    ListIterator(const ListIterator & x) noexcept { handle = x.handle; }

    //! Assignment Operator
    ListIterator & operator=(const ListIterator & x);

    //! Set native object
    ListIterator(nitf_ListIterator x);

    //! Get native object
    nitf_ListIterator & getHandle() noexcept;
    nitf_ListIterator& getHandle() const noexcept;

    //! Checks to see if two iterators are equal
    bool equals(const nitf::ListIterator& it2) const noexcept;

    //! Checks to see if two iterators are not equal
    bool notEqualTo(const nitf::ListIterator& it2) const noexcept;

    bool operator==(const nitf::ListIterator& it2) const noexcept;

    bool operator!=(const nitf::ListIterator& it2) const noexcept;

    //! Increment the iterator
    void increment();

    //! Increment the iterator (postfix);
    void operator++(int x);

    //! Increment the iterator by a specified amount
    ListIterator & operator+=(int x);

    ListIterator operator+(int x);

    //! Increment the iterator (prefix);
    void operator++() { increment(); }

    //! Get the data
    NITF_DATA* operator*() noexcept { return get(); }

    //! Get the data
    NITF_DATA* get() noexcept { return nitf_ListIterator_get(&handle); }

    //! Get the current
    nitf::ListNode & getCurrent() noexcept { return mCurrent; }

private:
    mutable nitf_ListIterator handle{ };
    nitf::ListNode mCurrent;

    //! Set native object
    void setHandle(nitf_ListIterator x)
    {
        handle = x;
        setMembers();
    }

    //! Reset member objects
    void setMembers() { mCurrent.setNative(handle.current); }

    //! Set the current
    void setCurrent(const nitf::ListNode & value) { mCurrent = value; }
};


/*!
 *  \class List
 *  \brief  The C++ wrapper for the nitf_List
 *
 *  This object is the controller for the ListNode nodes.
 *  It contains a pointer to the first and last items in its set.
 */
DECLARE_CLASS(List)
{
public:

    //! Copy constructor
    List(const List & x);

    //! Assignment Operator
    List & operator=(const List & x);

    //! Set native object
    List(nitf_List * x);

    /*!
     *  Is our chain empty?
     *  \return True if so, False otherwise
     */
    bool isEmpty() const;

    /*!
     *  Push something onto the front of our chain.  Note, as
     *  usual we REFUSE to allocate your data for you.  If you
     *  need a copy, make one up front.
     *  \param data  The data to push to the front
     */
    void pushFront(NITF_DATA* data);

    template <typename  T>
    void pushFront(nitf::Object<T> object)
    {
        pushFront((NITF_DATA*)object.getNativeOrThrow());
    }

    /*!
     *  Push something onto the back of our chain
     *  \param data  The data to push onto the back
     */
    void pushBack(NITF_DATA* data);

    template <typename  T>
    void pushBack(T object)
    {
        pushBack((NITF_DATA*)object.getNativeOrThrow());
    }

    /*!
     *  Pop the node off the front and return it.
     *  We check the first item to see if it exists.  If it does,
     *  we then fill in, and return the value.
     *  \return  The link we popped off
     */
    NITF_DATA* popFront();

    /*!
     *  Pop the node off the back and return it.
     *  We check the first item to see if it exists.  If it does,
     *  we then fill in, and return the value.
     *  \return  The link we popped off
     */
    NITF_DATA* popBack();

    //! Constructor
    List();

    //! Clone
    nitf::List clone(NITF_DATA_ITEM_CLONE cloner) const;

    ~List() = default;

    /*!
     *  Get the begin iterator
     *  \return  The iterator pointing to the first item in the list
     */
    nitf::ListIterator begin() const;

    /*!
     *  Get the end iterator
     *  \return  The iterator pointing to PAST the last item in the list (null);
     */
    nitf::ListIterator end() const;

    /*!
     *  Insert data into the chain BEFORE the iterator, and make
     *  the iterator point at the new node.
     *
     *  If the iterator is pointing to NULL, we will insert into the
     *  back of this list.  If it is pointing to the beginning, we
     *  will insert into the front.
     *
     *  If the iterator is pointing to something else, we will insert
     *  after the iterator (same case, in practice as pointing to the
     *  beginning).
     *  \param iter  The iterator to insert before
     *  \param data  This is a pointer assignment, not a data copy
     */
    void insert(nitf::ListIterator & iter, NITF_DATA* data);

    template <typename T>
    void insert(nitf::ListIterator & iter, nitf::Object<T> object)
    {
        insert(iter, (NITF_DATA*)object.getNativeOrThrow());
    }

    /*!
     *  Remove the data from an arbitrary point in the list.
     *  This WILL NOT delete the data, but it will make sure
     *  that we have successfully updated the chain.  The
     *  iterator is moved to the value that the current node
     *  pointed to (the next value).
     *  \param where  Where to remove, this will be updated to the NEXT position
     *  \return  We don't know how to delete YOUR object, so return it.
     */
    NITF_DATA* remove(nitf::ListIterator & where);

    //! Get the first
    nitf::ListNode getFirst() const;

    //! Get the last
    nitf::ListNode getLast() const;

    //! Returns the size of the list
    size_t getSize() const;

    //! Returns the data at the given index
    NITF_DATA* operator[] (size_t index);

private:
    mutable nitf_Error error{};
};

}
#endif
