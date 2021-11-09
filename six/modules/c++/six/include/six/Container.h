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

#include <scene/sys_Conf.h>
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
 *  takes ownership of its Data objects.  Callers should make all
 *  desired modifications to their Data objects prior to adding them to the
 *  Container.
 *
 *  Compiler-generated copy constructor and assignment operator are
 *  sufficient.
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
    DataType getDataType() const noexcept
    {
        return mDataType;
    }

    /*!
     *  Construct a container of type COMPLEX (for SICD products)
     *  or DERIVED (for SIDD products).
     *
     */
    Container(DataType dataType);    
    Container(Data*); // Note that the container takes ownership of the data, so the caller should not delete it.
    Container(std::unique_ptr<Data>&& data);
    Container(std::unique_ptr<Data>&& data, std::unique_ptr<Legend>&& legend) noexcept(false); // Only valid for derived data.

    //! Destructor
    virtual ~Container() = default;

    /*!
     *  Add a new Data object to the back of this container.
     *
     *  \param data Pointer to the data to add.  Note that the container
     *  takes ownership of the data, so the caller should not delete it.
     *
     */
    void addData(Data* data);

    /*!
     *  Add a new Data object to the back of this container.
     *
     *  \param data Pointer to the data to add
     *
     */
    void addData(std::unique_ptr<Data>&& data);
#if !CODA_OSS_cpp17    
    void addData(mem::auto_ptr<Data> data);
#endif


    /*!
     * Same as above but also supports passing in a legend.  Only valid for
     * derived data.
     */
    void addData(std::unique_ptr<Data>&& data, std::unique_ptr<Legend>&& legend);
#if !CODA_OSS_cpp17
    void addData(mem::auto_ptr<Data> data, mem::auto_ptr<Legend> legend);
#endif

    /*!
     *  Set the data item at location i.  If there is an item in the
     *  slot already, we delete it.
     *
     *  \param i The slot number
     *  \param data Pointer to the data to set.  Note that the container takes
     *  ownership of the data, so the caller should not delete it.
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

    size_t size() const { return mData.size(); }
    size_t getNumData() const
    {
        return size();
    }
    bool empty() const { return mData.empty(); }

    /*!
     * Removes data from the container that matches this pointer
     */
    void removeData(const Data* data);

    /*!
     * Add segment source for a DES other than the
     * SICD/SIDD DES.
     * \param source SegmentSource to add
     */
    void addDESSource(const nitf::SegmentSource& source);

    /*!
     * Get all added DES sources for DES other than the
     * SICD/SIDD DES.
     * \return The DES sources
     */
    const std::vector<nitf::SegmentSource>& getDESSources() const;

protected:
    typedef std::pair<mem::ScopedCloneablePtr<Data>,
                      mem::ScopedCopyablePtr<Legend> > DataPair;

    typedef std::vector<DataPair> DataVec;

    DataType mDataType;
    DataVec mData;
    std::vector<nitf::SegmentSource> mDESSources;

private:
    static
    mem::ScopedCopyablePtr<Legend> nullLegend()
    {
        return mem::ScopedCopyablePtr<Legend>(nullptr);
    }

    void addData(std::unique_ptr<Data>&& data,
                 mem::ScopedCopyablePtr<Legend> legend);
#if !CODA_OSS_cpp17
    void addData(mem::auto_ptr<Data> data,
                 mem::ScopedCopyablePtr<Legend> legend);
#endif
};
}

#endif

