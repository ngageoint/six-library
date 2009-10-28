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
#ifndef __SIX_COMPLEX_DATA_H__
#define __SIX_COMPLEX_DATA_H__

#include "six/Data.h"
#include "six/CollectionInformation.h"
#include "six/ImageCreation.h"
#include "six/ImageData.h"
#include "six/GeoData.h"
#include "six/Grid.h"
#include "six/Timeline.h"
#include "six/Position.h"
#include "six/RadarCollection.h"
#include "six/ImageFormation.h"
#include "six/SCPCOA.h"
#include "six/Radiometric.h"
#include "six/Antenna.h"
#include "six/ErrorStatistics.h"
#include "six/PFA.h"
#include "six/MatchInformation.h"

namespace six
{

/*!
 *  \class ComplexData
 *  \brief SICD data component
 *
 *  This class extends the Data object providing the SICD
 *  layer and utilities for access.  In order to write a SICD,
 *  you must have a ComplexData object that is populated with
 *  the mandatory parameters, and you must add it to the Container
 *  object first.
 *
 *  This object contains pointers to all of the sub-blocks for SICD.
 *  SIDD profiles may require that SICD collections are propagated through
 *  so for SIDD products, source SICDs should be added to the appropriate
 *  places in the SIDD Container object.
 *
 *  The best way to initialize a ComplexData object is using the
 *  ComplexDataBuilder, although it can be initialized directly.
 *
 *
 */
struct ComplexData: public Data
{

    //!  CollectionInfo block.  Contains the general collection information
    CollectionInformation* collectionInformation;

    //!  (Optional) Block contains general information about the image creation
    ImageCreation* imageCreation;

    //!  Block describes the image pixel data
    ImageData* imageData;

    //!  Describes the geographic coords of the region covered by the image
    GeoData* geoData;

    //!  Block of parameters describes the image sample grid
    Grid* grid;

    //!  This block describes the imaging collection timeline
    Timeline* timeline;

    //!  Describes the platform and the ground ref positions vs. time
    Position* position;

    //!  This block describes the radar collection info
    RadarCollection* radarCollection;

    //!  This block describes the image formation process
    ImageFormation* imageFormation;

    //!  Describes Center of Aperture (COA) params for Scene Center Point (SCP)
    SCPCOA* scpcoa;

    //!  (Optional) Radiometric calibration params
    Radiometric* radiometric;

    //!  (Optional) Params describe the antenna during collection.
    Antenna* antenna;

    //!  (Optional) Params needed for computing error statistics
    ErrorStatistics* errorStatistics;

    //!  (Optioanl) Params describing other related imaging collections
    MatchInformation* matchInformation;

    /*!
     *  Polar Format Algorithm params
     *
     *  This is technically conditional on imageFormationAlgorithm => PFA, but
     *  currently there is no other supported algorithm
     *
     */
    PFA* pfa;

    ComplexData();

    ~ComplexData();

    /*!
     *  Returns DATA_COMPLEX.  This is used by Read/Write to determine
     *  whether we have a ComplexData (SICD) object or a DerivedData (SIDD)
     *  product in the Container.
     */
    DataClass getDataClass() const
    {
        return DATA_COMPLEX;
    }

    /*!
     *  Deep copy of this, including all initialized sub-params
     *
     */
    Data* clone() const;

    /*!
     *  Utility function for getting the pixel type.  
     *  This is stored in the SICD along with the width  
     *
     *  Maps to: /SICD/ImageData/PixelType,
     *  /SIDD/Display/PixelType
     */
    virtual PixelType getPixelType() const
    {
        return imageData->pixelType;
    }
    virtual void setPixelType(PixelType pixelType)
    {
        imageData->pixelType = pixelType;
    }

    /*!
     *  Maps to: /SICD/ImageData/NumRows,
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual unsigned long getNumRows() const
    {
        return imageData->numRows;
    }
    /*!
     *  This accessor not only sets rows, it also sets
     *  the full image rows, and the scene center rows
     *
     */
    virtual void setNumRows(unsigned long numRows)
    {
        imageData->numRows = numRows;
        imageData->fullImage.row = numRows;
        imageData->scpPixel.row = numRows / 2;
    }

    /*!
     *  Maps to: /SICD/ImageData/NumCols,
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual unsigned long getNumCols() const
    {
        return imageData->numCols;
    }
    /*!
     *  This accessor not only sets columns, it also sets
     *  the full image columns, and the scene center column
     *
     */
    virtual void setNumCols(unsigned long numCols)
    {
        imageData->numCols = numCols;
        imageData->fullImage.col = numCols;
        imageData->scpPixel.col = numCols / 2;
    }

    /*!
     *  Maps to: /SICD/GeoData/ImageCorners,
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint
     */
    virtual Corners getImageCorners() const
    {
        return geoData->imageCorners;
    }
    virtual void setImageCorners(Corners imageCorners)
    {
        geoData->imageCorners = imageCorners;
    }

    /*!
     *  Maps to: /SICD/CollectionInfo/CoreName,
     *  /SIDD/ProductCreation/ProductName
     */
    virtual std::string getName() const
    {
        return collectionInformation->coreName;
    }
    virtual void setName(std::string name)
    {
        collectionInformation->coreName = name;
    }

    /*!
     *  Maps to: /SICD/Timeline/CollectStart,  
     */
    virtual DateTime getCreationTime() const
    {
        return timeline->collectStart;
    }

    /*!
     *  Maps to: /SICD/Timeline/CollectStart,  
     */
    virtual void setCreationTime(DateTime creationTime)
    {
        timeline->collectStart = creationTime;
    }

    /*!
     *  Classification info is needed by the NITFReadControl/NITFWriteControl
     *  to populate the nitf::FileSecurity.  This method is defined
     *  differently for SICD than SIDD.  Any profile parameters for 
     *  FileSecurity that do not conform to Classification transfer must
     *  be initialized directly.
     *  Maps to SICD/CollectionInfo/Classification
     */
    virtual Classification getClassification() const
    {
        return collectionInformation->classification;
    }

    /*!
     *  Maps to SICD/CollectionInfo/Classification
     */
    virtual Classification& getClassification()
    {
        return collectionInformation->classification;
    }

    /*!
     *  Classification info is needed by the NITFReadControl/NITFWriteControl
     *  to populate the nitf::FileSecurity.  This method is defined
     *  differently for SICD than SIDD.  Any profile parameters for 
     *  FileSecurity that do not conform to Classification transfer must
     *  be initialized directly.
     */
    virtual void setClassification(Classification classification)
    {
        collectionInformation->classification = classification;
    }

};

}

#endif
