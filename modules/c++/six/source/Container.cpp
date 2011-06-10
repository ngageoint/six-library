/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "six/Container.h"

using namespace six;

Container::Container(const Container& c)
{
    cleanup();
    mDataType = c.mDataType;
    for (ConstDataIterator p = c.mData.begin(); p != c.mData.end(); ++p)
    {
        addData((*p)->clone());
    }
}

Container& Container::operator=(const Container& c)
{
    if (&c != this)
    {
        cleanup();
        mDataType = c.mDataType;
        for (ConstDataIterator p = c.mData.begin(); p != c.mData.end(); ++p)
        {
            addData((*p)->clone());
        }
    }
    return *this;
}

void Container::addData(Data* data)
{
    addData(std::auto_ptr<Data>(data));
}

void Container::addData(std::auto_ptr<Data> data)
{
    mData.push_back(data.get());
}

void Container::setData(size_t i, Data* data)
{
    if (mData.size() <= i)
    {
        throw except::Exception(Ctxt("Cannot set a non-existent segment!"));
    }
    if (mData[i] != NULL)
        delete mData[i];

    mData[i] = data;
}

/*!
 *  We need to go through the list and eliminate any
 *  data references as well.
 *
 */
void Container::removeData(Data* data)
{
    //mData.remove(data);
    for (DataIterator p = mData.begin(); p != mData.end(); ++p)
    {
        if (*p == data)
        {
            mData.erase(p);
            break;
        }
    }
}

Container::~Container()
{
    cleanup();
}

void Container::cleanup()
{
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        delete mData[ii];
    }
    mData.clear();
}
