/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_DERIVED_DATA_H__
#define __SIX_DERIVED_DATA_H__

#include <mem/ScopedCopyablePtr.h>
#include <six/Data.h>
#include <six/ErrorStatistics.h>
#include <six/MatchInformation.h>
#include <six/GeoDataBase.h>
#include <six/sidd/ProductCreation.h>
#include <six/sidd/Display.h>
#include <six/sidd/GeographicAndTarget.h>
#include <six/sidd/Measurement.h>
#include <six/sidd/ExploitationFeatures.h>
#include <six/sidd/ProductProcessing.h>
#include <six/sidd/DownstreamReprocessing.h>
#include <six/Radiometric.h>
#include <six/sidd/Compression.h>
#include <six/sidd/DigitalElevationData.h>
#include <six/sidd/Annotations.h>

namespace six
{
namespace sidd
{
/*!
 *  \class DerivedData
 *  \brief The implementation of Data for derived products
 *
 *  Sub-class of Data for handling Derived Products (SIDD)
 *  Contains the structs that are the model for SIDD products
 *
 */
struct DerivedData: public Data
{
    /*!
     *  Information related to processor, classification,
     *  and product type
     */
    mem::ScopedCopyablePtr<ProductCreation> productCreation;

    /*!
     *  Contains information on the parameters needed to display
     *  the product in an exploitation tool
     */
    mem::ScopedCopyablePtr<Display> display;

    /*!
     *  Contains generic and extensible targeting and geographic
     *  region information.
     */
    mem::ScopedCopyablePtr<GeographicAndTarget> geographicAndTarget;

    /*!
     *  Contains generic and extensible targeting and geographic
     *  region information.
     Required SIDD 2.0
     */
    mem::ScopedCopyablePtr<GeoDataBase> geoData;


    /*!
     *  Contains the meta-data necessary for performing
     *  measurements
     */
    mem::ScopedCopyablePtr<Measurement> measurement;

    /*!
     *  Computed metadata for collections
     */
    mem::ScopedCopyablePtr<ExploitationFeatures> exploitationFeatures;

    /*!
     *  (Optional) Contains meta-data related to algorithms used
     *  during product generation
     */
    mem::ScopedCopyablePtr<ProductProcessing> productProcessing;

    /*!
     *  (Optional) Contains meta-data related to downstream
     *  processing of the product
     */
    mem::ScopedCopyablePtr<DownstreamReprocessing> downstreamReprocessing;

    /*!
     *  (Optional) Contains error statistics structures
     */
    mem::ScopedCopyablePtr<ErrorStatistics> errorStatistics;

    /*!
     *  (Optional) Contains radiometric calibration params
     */
    mem::ScopedCopyablePtr<Radiometric> radiometric;

    //!  (Optional) Params describing other related imaging collections
    mem::ScopedCopyablePtr<MatchInformation> matchInformation;

    //!  (Optional) Params describing compression
    mem::ScopedCopyablePtr<Compression> compression;

    //!  (Optional) Params describing digital elevation data
    mem::ScopedCopyablePtr<DigitalElevationData> digitalElevationData;

    /*!
     * (Optional) Contains SFA annotations
     */
    Annotations annotations;

    /*
     * (Optional) In SIDD 2.0, the LUT stored in the NITF
     * In 1.0, each Display element has a single LUT which corresponds
     * to the LUT in the NITF, making this redundant.
     */
    mem::ScopedCopyablePtr<LUT> nitfLUT;

    /*!
     *  Constructor.  Creates only the product creation.  All other
     *  elements are set to nullptr.  A builder should be used where possible
     *  rather than invoking this object directly.
     */
    DerivedData();

    /*!
     *  We are dealing with derived data
     */
    DataType getDataType() const
    {
        return DataType::DERIVED;
    }

    /*!
     *  Make a deep copy of all of the objects here.
     */
    virtual Data* clone() const;

    /*!
     *  Maps to:
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual size_t getNumRows() const
    {
        return static_cast<size_t>(measurement->pixelFootprint.row);
    }

    /*!
     *  Maps to:
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual void setNumRows(size_t numRows)
    {
        measurement->pixelFootprint.row = static_cast<ptrdiff_t>(numRows);
    }

    /*!
     *  Maps to:
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual size_t getNumCols() const
    {
        return static_cast<size_t>(measurement->pixelFootprint.col);
    }

    /*!
     *  Maps to:
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual void setNumCols(size_t numCols)
    {
        measurement->pixelFootprint.col = static_cast<ptrdiff_t>(numCols);
    }

    /*!
     *  Maps to:
     *  /SIDD/Display/PixelType
     */
    virtual PixelType getPixelType() const
    {
        return display->pixelType;
    }

    /*!
     *  Maps to:
     *  /SIDD/Display/PixelType
     */
    virtual void setPixelType(PixelType pixelType)
    {
        display->pixelType = pixelType;
    }

    /*!
     *  Maps to:
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint in 1.0 and
     *  /SIDD/GeographicAndTarget/ImageCorners in 2.0
     */
    virtual LatLonCorners getImageCorners() const;

    /*!
     *  Maps to:
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint in 1.0 and
     *  /SIDD/GeographicAndTarget/ImageCorners in 2.0
     */
    virtual void setImageCorners(const LatLonCorners& imageCorners);

    /*!
     *  Maps to:
     *  /SIDD/ProductCreation/ProductName
     */
    virtual std::string getName() const
    {
        return productCreation->productName;
    }

    /*!
     *  Maps to:
     *  /SIDD/ProductCreation/ProductName
     */
    virtual void setName(std::string name)
    {
        productCreation->productName = name;
    }

    /*!
     *  Maps to:
     *  /SIDD/AdvancedExploitation/Collection/Information/SensorName
     */
    virtual std::string getSource() const
    {
        // TODO throw exception instead of returning empty string?
        return (exploitationFeatures.get()
                && !exploitationFeatures->collections.empty() ?
                    exploitationFeatures->collections[0]->information.sensorName :
                    std::string(""));
    }

    /*!
     *  Maps to:
     *  /SIDD/AdvancedExploitation/Collection/Information/SensorName
     */
    virtual void setSource(std::string name)
    {
        // TODO throw exception if cannot set?
        if (exploitationFeatures.get() &&
            !exploitationFeatures->collections.empty())
        {
            exploitationFeatures->collections[0]->information.sensorName =
                name;
        }
    }

    /*!
     *  Maps to:
     *  /SIDD/ProductCreation/ProcessorInformation/ProcessingDateTime
     */
    virtual DateTime getCreationTime() const
    {
        return productCreation->processorInformation.processingDateTime;
    }

    /*!
     *  Maps to:
     *  /SIDD/ProductCreation/ProcessorInformation/ProcessingDateTime
     */
    virtual void setCreationTime(DateTime creationTime)
    {
        productCreation->processorInformation.processingDateTime
                = creationTime;
    }

    /*!
     *  Maps to: /SIDD/ExploitationFeatures/Collection[0]/Information/CollectionDateTime
     */
    virtual DateTime getCollectionStartDateTime() const;

    /*!
     *  Maps to:
     *  /SIDD/ProductCreation/Classification
     */
    virtual const Classification& getClassification() const
    {
        return productCreation->classification;
    }

    virtual Classification& getClassification()
    {
        return productCreation->classification;
    }

    virtual mem::ScopedCopyablePtr<LUT>& getDisplayLUT();

    virtual std::string getVendorID() const
    {
        return std::string(VENDOR_ID);
    }

    virtual std::string getVersion() const
    {
        return mVersion;
    }

    virtual void setVersion(const std::string& strVersion)
    {
        mVersion = strVersion;
    }

    /*
     * Convert the output plane pixel location into meters from the reference
     * point
     */
    types::RowCol<double>
    pixelToImagePoint(const types::RowCol<double>& pixelLoc) const;

    bool operator==(const DerivedData& rhs) const;

private:
    static const char VENDOR_ID[];
    virtual bool equalTo(const Data& rhs) const;
    std::string mVersion;
};
}
}
#endif
