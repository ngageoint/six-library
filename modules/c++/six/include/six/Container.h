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
#ifndef __SIX_CONTAINER_H__
#define __SIX_CONTAINER_H__

#include "six/Data.h"
#include "six/Utilities.h"
#include <import/xml/lite.h>
#include <import/io.h>
#include <list>

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
     *  the case of DERIVED, one or more Data*'s DataClass will
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
     *  Set the data item at location i.  If there is an item in the
     *  slot already, we delete it.
     *
     *  \param i The slot number
     *  \param data A pointer to the data object
     */
    void setData(unsigned int i, Data* data);

    /*!
     *  Get the item leaving in the ith slot.
     *  \return The data
     */
    Data* getData(unsigned int i)
    {
        return this->mData[i];
    }

    unsigned int getNumData() const
    {
        return this->mData.size();
    }

    /*!
     *  We need to go through the list and eliminate any
     *  data references as well.
     *
     */
    void removeData(Data* data);

protected:
    std::vector<Data*> mData;
    //std::string mContainerType;
    DataType mDataType;
    typedef std::vector<Data*>::iterator DataIterator;

};

}

#endif
