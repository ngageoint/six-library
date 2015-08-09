/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <algorithm>

#include <six/Container.h>

namespace six
{
Container::Container(DataType dataType) :
    mDataType(dataType)
{
}

Container::~Container()
{
}

void Container::addData(Data* data)
{
    addData(std::auto_ptr<Data>(data));
}

void Container::addData(std::auto_ptr<Data> data)
{
	mem::ScopedCloneablePtr<Data> cloneableData(data.release());
    mData.push_back(cloneableData);
}

void Container::setData(size_t i, Data* data)
{
    if (mData.size() <= i)
    {
        throw except::Exception(Ctxt("Cannot set a non-existent segment!"));
    }

    mData[i].reset(data);
}

void Container::removeData(const Data* data)
{
	for (DataVec::iterator iter = mData.begin(); iter != mData.end(); ++iter)
	{
		if (iter->get() == data)
		{
			mData.erase(iter);
			break;
		}
	}
}

Data* Container::getData(const std::string& iid, size_t numImages)
{
    if (!str::startsWith(iid, "SICD") && !str::startsWith(iid, "SIDD") &&
        iid.length() >= 7)
    {
        throw except::Exception(Ctxt(
                "This is not a properly formed IID1 field"));
    }

    //! Index is 1-based except for SICDs with one image segment --
    //  In this case it's 0-based
    size_t dataID = str::toType<size_t>(iid.substr(4, 6));
    if (str::startsWith(iid, "SICD") && numImages > 1)
    {
        --dataID;
    }

    return getData(dataID);
}
}
