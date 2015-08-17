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
#ifndef __SIX_CONTAINER_H__
#define __SIX_CONTAINER_H__

#include <memory>
#include <vector>
#include <utility>

#include <mem/ScopedCloneablePtr.h>
#include <six/Legend.h>
#include <six/Data.h>

namespace six
{

/*!
 *  \class Container
 *  \brief Object abstraction for the container file format
 *
 *  A container is storage for the Data objects, and determines how these
 *  components are laid out in whatever file is being written.  The Container
 *  takes ownership of its Data objects.
 */
class Container
{
public:

    /*! 
     *  The data class is either COMPLEX or DERIVED
     *  This just tells us what kind of container.  Note that, in 
     *  the case of DERIVED, one or more Data*'s DataType will
     *  be DERIVED.
     *
     */
    DataType getDataType() const
    {
        return mDataType;
    }

    /*!
     *  Construct a container of type COMPLEX (for SICD products)
     *  or DERIVED (for SIDD products).
     *
     */
    Container(DataType dataType);

    //! Destructor
    virtual ~Container();

    /*!
     *  Add a new Data object to the back of this container.
     *
     *  \param data Add the data
     *
     */
    void addData(Data* data);

    /*!
     *  Add a new Data object to the back of this container.
     *
     *  \param data Add the data
     *
     */
    void addData(std::auto_ptr<Data> data);

    /*!
     * Same as above but also supports passing in a legend.  Only valid for
     * derived data.
     */
    void addData(std::auto_ptr<Data> data, std::auto_ptr<Legend> legend);

    /*!
     *  Set the data item at location i.  If there is an item in the
     *  slot already, we delete it.
     *
     *  \param i The slot number
     *  \param data A pointer to the data object
     */
    void setData(size_t i, Data* data);

    /*!
     *  Get the item leaving in the ith slot.
     *  \return The data
     */
    Data* getData(size_t i)
    {
        return mData[i].first.get();
    }

    /*!
     *  Get the item leaving in the ith slot.
     *  \return The data
     */
    const Data* getData(size_t i) const
    {
        return mData[i].first.get();
    }

    /*!
     *  Get the legend, if present, living in the ith slot.
     *  \return The legend
     */
    const Legend* getLegend(size_t i) const
    {
    	return mData[i].second.get();
    }

    /*!
     *  Get the item from the ImageSubheader field IID1.
     *  \return The data
     */
    Data* getData(const std::string& iid, size_t numImages);

    size_t getNumData() const
    {
        return mData.size();
    }

    void removeData(const Data* data);

protected:
    typedef std::pair<mem::ScopedCloneablePtr<Data>,
    		          mem::ScopedCopyablePtr<Legend> > DataPair;

    typedef std::vector<DataPair> DataVec;

    DataType mDataType;
    DataVec mData;

private:
    static
    mem::ScopedCopyablePtr<Legend> nullLegend()
    {
    	return mem::ScopedCopyablePtr<Legend>(NULL);
    }

    void addData(std::auto_ptr<Data> data,
    	         mem::ScopedCopyablePtr<Legend> legend);
};
}

#endif

