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

#include "nitf/List.hpp"

#include "gsl/gsl.h"

///////////////////////////////////////////////////////////////////////////////
// ListNode

nitf::ListNode & nitf::ListNode::operator=(const nitf::ListNode & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

nitf::ListNode::ListNode(nitf_ListNode * x)
{
    setNative(x);
    getNativeOrThrow();
}

nitf::ListNode::ListNode(nitf::ListNode & prev, nitf::ListNode & next, NITF_DATA* data)
    : ListNode(nitf_ListNode_construct(prev.getNative(), next.getNative(), data, &error))
{
    setManaged(false);
}

NITF_DATA * nitf::ListNode::getData() const
{
    return getNativeOrThrow()->data;
}

void nitf::ListNode::setData(NITF_DATA * value)
{
    getNativeOrThrow()->data = value;
}

///////////////////////////////////////////////////////////////////////////////
// ListIterator

nitf::ListIterator & nitf::ListIterator::operator=(const nitf::ListIterator & x)
{
    if (&x != this)
    {
        handle = x.handle;
        setMembers();
    }
    return *this;
}

nitf::ListIterator::ListIterator(nitf_ListIterator x) { setHandle(x); }

nitf_ListIterator & nitf::ListIterator::getHandle() noexcept { return handle; }
nitf_ListIterator& nitf::ListIterator::getHandle() const noexcept { return handle; }

bool nitf::ListIterator::equals(const nitf::ListIterator& it2) const noexcept
{
    const NITF_BOOL x = nitf_ListIterator_equals(&handle, &it2.getHandle());
    if (!x) return false;
    return true;
}

bool nitf::ListIterator::operator==(const nitf::ListIterator& it2) const noexcept
{
    return this->equals(it2);
}

bool nitf::ListIterator::notEqualTo(const nitf::ListIterator& it2) const noexcept
{
    const NITF_BOOL x = nitf_ListIterator_notEqualTo(&handle, &it2.getHandle());
    if (!x) return false;
    return true;
}

bool nitf::ListIterator::operator!=(const nitf::ListIterator& it2) const noexcept
{
    return this->notEqualTo(it2);
}

void nitf::ListIterator::increment()
{
    nitf_ListIterator_increment(&handle);
    setMembers();
}

void nitf::ListIterator::operator++(int) { increment(); }

nitf::ListIterator & nitf::ListIterator::operator+=(int x)
{
    for (int i = 0; i < x; ++i)
        increment();
    return *this;
}

nitf::ListIterator nitf::ListIterator::operator+(int x)
{
    nitf::ListIterator it = nitf::ListIterator(*this);
    it += x;
    return it;
}

NITF_DATA* nitf::ListIterator::get() noexcept
{
    return nitf_ListIterator_get(&handle);
}


///////////////////////////////////////////////////////////////////////////////
// List

nitf::List::List(const nitf::List & x)
{
    *this = x;
}

nitf::List & nitf::List::operator=(const nitf::List & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

nitf::List::List(nitf_List * x)
{
    setNative(x);
    getNativeOrThrow();
}

bool nitf::List::isEmpty() const
{
    const NITF_BOOL x = nitf_List_isEmpty(getNativeOrThrow());
    return x ? true : false;
}

void nitf::List::pushFront(NITF_DATA* data)
{
    const NITF_BOOL x = nitf_List_pushFront(getNativeOrThrow(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void nitf::List::pushBack(NITF_DATA* data)
{
    const NITF_BOOL x = nitf_List_pushBack(getNativeOrThrow(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

NITF_DATA* nitf::List::popFront()
{
    NITF_DATA * data = nitf_List_popFront(getNativeOrThrow());
    return data;
}

NITF_DATA* nitf::List::popBack()
{
    NITF_DATA * data = nitf_List_popBack(getNativeOrThrow());
    return data;
}

nitf::List::List() : List(nitf_List_construct(&error))
{
    setManaged(false);
}

nitf::List nitf::List::clone(NITF_DATA_ITEM_CLONE cloner) const
{
    nitf::List dolly(nitf_List_clone(getNativeOrThrow(), cloner, &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::ListIterator nitf::List::begin() const
{
    const nitf_ListIterator x = nitf_List_begin(getNativeOrThrow());
    return nitf::ListIterator(x);
}

nitf::ListIterator nitf::List::end() const
{
    const nitf_ListIterator x = nitf_List_end(getNativeOrThrow());
    return nitf::ListIterator(x);
}

void nitf::List::insert(nitf::ListIterator & iter, NITF_DATA* data)
{
    const NITF_BOOL x = nitf_List_insert(getNativeOrThrow(), iter.getHandle(), data, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

NITF_DATA* nitf::List::remove(nitf::ListIterator & where)
{
    NITF_DATA * data = nitf_List_remove(getNativeOrThrow(), &where.getHandle());
    return data;
}

nitf::ListNode nitf::List::getFirst() const
{
    return nitf::ListNode(getNativeOrThrow()->first);
}

nitf::ListNode nitf::List::getLast() const
{
    return nitf::ListNode(getNativeOrThrow()->last);
}

size_t nitf::List::getSize() const
{
    return static_cast<size_t>(nitf_List_size(getNativeOrThrow()));
}

NITF_DATA* nitf::List::operator[] (size_t index)
{
    NITF_DATA* x = nitf_List_get(getNativeOrThrow(), gsl::narrow<int>(index), &error);
    if (!x)
        throw nitf::NITFException(&error);
    return x;
}

const NITF_DATA* nitf::List::operator[] (size_t index) const
{
    NITF_DATA* x = nitf_List_get(getNativeOrThrow(), gsl::narrow<int>(index), &error);
    if (!x)
        throw nitf::NITFException(&error);
    return x;
}
