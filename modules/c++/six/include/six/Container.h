/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include "six/Data.h"
#include "six/Utilities.h"
#include <import/xml/lite.h>
#include <import/io.h>
#include <list>
#include <memory>

namespace six
{

/*!
 *  \class Container
 *  \brief Object abstraction for the container file format
 *
 *  A container is storage for the Data objects, and determines how these
 *  components are laid out in whatever file is being written.  The Container
 *  takes ownership of its Data objects
 */
class Container
{

public:

    //! Container type is going to contain either 'NITF' or 'GeoTIFF'
    //std::string getContainerType() const { return mContainerType; }

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
    Container(DataType dataType) :
        mDataType(dataType)
    {
    }

    //!  Copy constructor
    Container(const Container& c);

    //!  Assignment operator
    Container& operator=(const Container& c);

    //! Destructor
    virtual ~Container();

    /*!
     *  Add a new Data object to the back of this container.
     *  If you leave the data in the container and do not remove
     *  it we delete it for you (TODO: should we?).
     *
     *  \param data Add the data
     *
     */
    void addData(Data* data);

    /*!
     *  Add a new Data object to the back of this container.
     *  If you leave the data in the container and do not remove
     *  it we delete it for you (TODO: should we?).
     *
     *  \param data Add the data
     *
     */
    void addData(std::auto_ptr<Data> data);

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
        return this->mData[i];
    }

    /*!
     *  Get the item leaving in the ith slot.
     *  \return The data
     */
    const Data* getData(size_t i) const
    {
        return this->mData[i];
    }

    /*!
     *  Get the item from the ImageSubheader field IID1.
     *  \return The data
     */
    Data* getData(const std::string& iid, size_t numImages)
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

    size_t getNumData() const
    {
        return this->mData.size();
    }

    /*!
     *  We need to go through the list and eliminate any
     *  data references as well.
     *
     */
    void removeData(const Data* data);

protected:
    std::vector<Data*> mData;
    DataType mDataType;
    typedef std::vector<Data*>::iterator DataIterator;
    typedef std::vector<Data*>::const_iterator ConstDataIterator;

    void cleanup();

};

}

#endif

