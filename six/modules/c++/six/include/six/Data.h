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
#ifndef __SIX_DATA_H__
#define __SIX_DATA_H__

#include <assert.h>

#include <vector>
#include <memory>
#include <std/span>
#include <std/cstddef>

#include "six/Types.h"
#include "six/Classification.h"
#include "six/Utilities.h"

namespace six
{



/*!
 *  \class Data
 *  \brief Abstract base class for the SICD and SIDD Data objects.
 *
 *  The Data object is derived for ComplexData (SICD) and
 *  DerivedData (SIDD).  This class provides the interface needed
 *  to interact with the file format ReadControl and WriteControl,
 *  currently NITF, GeoTIFF (SIDD only).
 *
 */
struct Data
{
    Data() = default;

    //!  Destructor
    virtual ~Data() {}

    /*!
     *  Contract that subclasses must provide a deep copy of
     *  their inner data-structures
     */
    virtual Data* clone() const = 0;
    virtual std::unique_ptr<Data> unique_clone() const
    {
        return std::unique_ptr<Data>(clone());
    }

    bool equals_(const Data& rhs) const
    {
        return this->equalTo(rhs) && rhs.equalTo(*this);
    }

    /*!
     *  Data type/class is DERIVED for DerivedData and
     *  COMPLEX for ComplexData
     */
    virtual DataType getDataType() const = 0;

    /*!
     *  Utility function for getting the pixel type.
     *  This is stored in the SICD along with the width
     *
     *  Maps to: /SICD/ImageData/PixelType,
     *  /SIDD/Display/PixelTypeking with. In other words, a
     */
    virtual PixelType getPixelType() const = 0;
    virtual void setPixelType(PixelType pixelType) = 0;
    virtual bool convertPixels_(std::span<const std::byte>, std::span<std::byte>) const { return false; }
    template<typename T, typename U>
    bool convertPixels(std::span<const T> from, std::span<U> to) const
    {
        return convertPixels_(as_bytes(from), as_bytes(to));
    }

    /*!
     *  Maps to: /SICD/ImageData/NumRows,/SICD/ImageData/FullImage/Row
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual size_t getNumRows() const = 0;

    /*!
     *  Maps to: /SICD/ImageData/NumRows,/SICD/ImageData/FullImage/Row
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual void setNumRows(size_t numRows) = 0;

    /*!
     *  Maps to: /SICD/ImageData/NumCols,/SICD/ImageData/FullImage/Col
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual size_t getNumCols() const = 0;


    /*!
     *  Maps to: /SICD/ImageData/NumCols,/SICD/ImageData/FullImage/Col
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual void setNumCols(size_t numCols) = 0;

    /*!
     *  Maps to: /SICD/GeoData/ImageCorners,
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint
     */
    virtual LatLonCorners getImageCorners() const = 0;

    /*!
     *  Maps to: /SICD/GeoData/ImageCorners,
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint
     */
    virtual void setImageCorners(const LatLonCorners& corners) = 0;

    /*!
     *  Maps to: /SICD/CollectionInfo/CoreName,
     *  /SIDD/ProductCreation/ProductName
     */
    virtual std::string getName() const = 0;

    /*!
     *  Maps to: /SICD/CollectionInfo/CoreName,
     *  /SIDD/ProductCreation/ProductName
     */
    virtual void setName(std::string name) = 0;

    /*!
     *  Maps to: /SICD/CollectionInfo/CollectorName,
     *  /SIDD/AdvancedExploitation/Collection/Information/SensorName
     */
    virtual std::string getSource() const = 0;

    /*!
     *  Maps to: /SICD/CollectionInfo/CollectorName,
     *  /SIDD/AdvancedExploitation/Collection/Information/SensorName
     */
    virtual void setSource(std::string name) = 0;

    /*!
     *  Maps to: /SICD/ImageCreation/DateTime,
     *  /SIDD/ProductCreation/ProcessorInformation/ProcessingDateTime
     */
    virtual DateTime getCreationTime() const = 0;

    /*!
     *  Maps to: /SICD/ImageCreation/DateTime,
     *  /SIDD/ProductCreation/ProcessorInformation/ProcessingDateTime
     */
    virtual void setCreationTime(DateTime creationTime) = 0;

    /*!
     *  Maps to: /SICD/Timeline/CollectStart,
     *  /SIDD/ExploitationFeatures/Collection[0]/Information/CollectionDateTime
     */
    virtual DateTime getCollectionStartDateTime() const = 0;

    //!  Get back the number of bytes per pixel for the product
    virtual size_t getNumBytesPerPixel() const;

    /*!
     *  This reader is only used for informational purposes, but it
     *  contains the number of channels of pixel data.  For SIDD truecolor,
     *  this would be 3, for SIDD pseudo, it would be 1.  For SICD complex
     *  there would be 2, one for real and one for imaginary.
     */
    virtual size_t getNumChannels() const;

    /*!
     *  Classification is defined differently for SICD and SIDD, but this
     *  treats them the same, normalizing their content from different parts
     *  of the XML/NITF, etc.
     */
    virtual const Classification& getClassification() const = 0;

    virtual Classification& getClassification() = 0;

    virtual const mem::ScopedCopyablePtr<LUT>& getDisplayLUT() const = 0;
    virtual void setDisplayLUT(std::unique_ptr<AmplitudeTable>&&) = 0;
    virtual AmplitudeTable* getAmplitudeTable() const { return nullptr; }

    /*!
     * Returns an identifier of the Vendor supplying the implementation code.
     */
    virtual std::string getVendorID() const = 0;

    /*!
     * Returns an identifier of the Version of the model contained within
     */
    virtual std::string getVersion() const = 0;

    //!  Set the version of the model contained within
    virtual void setVersion(const std::string& version) = 0;

private:
    virtual bool equalTo(const Data& rhs) const = 0;
};
template<typename T>
inline bool operator==(const Data& lhs, const T& rhs)
{
    return lhs.equals_(rhs);
}
template<typename T>
inline bool operator!=(const Data& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

inline types::RowCol<size_t> getExtent(const Data& data)
{
    return types::RowCol<size_t>(data.getNumRows(), data.getNumCols());
}
inline types::RowCol<size_t> getExtent(const Data* pData)
{
    assert(pData != nullptr);
    return getExtent(*pData);
}
inline void setExtent(Data& data, const types::RowCol<size_t>& extent)
{
    data.setNumRows(extent.row);
    data.setNumCols(extent.col);
}

}

#endif

