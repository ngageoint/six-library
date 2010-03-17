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
#ifndef __SIX_DATA_H__
#define __SIX_DATA_H__

#include "six/Types.h"
#include "six/Classification.h"
#include <vector>

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
    //!  Constructor
    Data() {}

    //!  Destructor
    virtual ~Data() {}

    /*!
     *  Contract that subclasses must provide a deep copy of
     *  their inner data-structures
     */
    virtual Data* clone() const = 0;

    /*!
     *  Data class is DATA_DERIVED for DerivedData and 
     *  DATA_COMPLEX for ComplexData
     */
    virtual DataClass getDataClass() const = 0;

    /*!
     *  Utility function for getting the pixel type.  
     *  This is stored in the SICD along with the width  
     *
     *  Maps to: /SICD/ImageData/PixelType,
     *  /SIDD/Display/PixelType
     */
    virtual PixelType getPixelType() const = 0;
    virtual void setPixelType(PixelType pixelType) = 0;

    /*!
     *  Maps to: /SICD/ImageData/NumRows,/SICD/ImageData/FullImage/Row
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual unsigned long getNumRows() const = 0;

    /*!
     *  Maps to: /SICD/ImageData/NumRows,/SICD/ImageData/FullImage/Row
     *  /SIDD/Measurement/PixelFootprint/Row
     */
    virtual void setNumRows(unsigned long numRows) = 0;

    /*!
     *  Maps to: /SICD/ImageData/NumCols,/SICD/ImageData/FullImage/Col
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual unsigned long getNumCols() const = 0;


    /*!
     *  Maps to: /SICD/ImageData/NumCols,/SICD/ImageData/FullImage/Col
     *  /SIDD/Measurement/PixelFootprint/Col
     */
    virtual void setNumCols(unsigned long numCols) = 0;

    /*!
     *  Maps to: /SICD/GeoData/ImageCorners,
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint
     */
    virtual std::vector<LatLon> getImageCorners() const = 0;

    /*!
     *  Maps to: /SICD/GeoData/ImageCorners,
     *  /SIDD/GeographicAndTarget/GeographicCoverage/Footprint
     */
    virtual void setImageCorners(const std::vector<LatLon>& corners) = 0;

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
     *  Maps to: /SICD/ImageCreation/DateTime,  
     *  /SIDD/ProductCreation/ProcessorInformation/ProcessingDateTime  
     */
    virtual DateTime getCreationTime() const = 0;

    /*!
     *  Maps to: /SICD/ImageCreation/DateTime,  
     *  /SIDD/ProductCreation/ProcessorInformation/ProcessingDateTime  
     */
    virtual void setCreationTime(DateTime creationTime) = 0;

    //!  Get back the number of bytes per pixel for the product
    virtual unsigned long getNumBytesPerPixel() const;

    /*!  
     *  This reader is only used for informational purposes, but it
     *  contains the number of channels of pixel data.  For SIDD truecolor,
     *  this would be 3, for SIDD pseudo, it would be 1.  For SICD complex
     *  there would be 2, one for real and one for imaginary.
     */  
    virtual unsigned long getNumChannels() const;

    /*!
     *  Classification is defined differently for SICD and SIDD, but this
     *  treats them the same, normalizing their content from different parts
     *  of the XML/NITF, etc.
     */
    virtual Classification getClassification() const = 0;
    
    /*!
     *  Non-const, by reference alterative function.
     *
     */
    virtual Classification& getClassification() = 0;

    /*!
     *  Set the classification block (without traversing the data model
     *  specific to SICD or SIDD).
     */
    virtual void setClassification(Classification classification) = 0;

    virtual LUT* getDisplayLUT() = 0;

};

}

#endif
