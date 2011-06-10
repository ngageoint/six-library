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
#ifndef __SIX_NITF_READ_CONTROL_H__
#define __SIX_NITF_READ_CONTROL_H__

#include "six/NITFImageInfo.h"
#include "six/ReadControl.h"
#include "six/ReadControlFactory.h"
#include "six/Adapters.h"
#include <import/nitf.hpp>

namespace six
{

/*!
 *  \class NITFReadControl
 *  \brief Uses NITRO to read in a Sensor-Independent NITF file
 *
 *  This class encapsulates the details of SICD/SIDD segmentation
 *  presenting the caller with a contiguous view of an Image.  It
 *  uses the XML readers to get DES data into the Data model.
 *
 *  This class takes advantage of optimizations in NITRO specific to
 *  pixel-interleaved, or single band data.
 *
 */
class NITFReadControl : public ReadControl
{
public:

    //!  Constructor
    NITFReadControl()
    {
    }

    //!  Destructor
    virtual ~NITFReadControl()
    {
        reset();
    }

    virtual DataType getDataType(const std::string& fromFile) const;

    /*!
     *  Performs (Basic) validation when a segment is being
     *  read.  This function is able to test that the image
     *  segment in question at least follows some of the rules
     *  that its supposed to.
     */
    void validateSegment(nitf::ImageSubheader subheader, NITFImageInfo* info);

    /*!
     *  Fulfills our obligationss to the parent class, using the IOInterface
     *  method provided with the same name
     */
    virtual void load(const std::string& fromFile);

    virtual UByte* interleaved(Region& region, int imageNumber);

    virtual std::string getFileType() const
    {
        return "NITF";
    }

    // Just in case you need it and are willing to cast
    nitf::Record getRecord() const
    {
        return mRecord;
    }

    // Just in case you need it and are willing to cast
    nitf::Reader getReader() const
    {
        return mReader;
    }

protected:
    //! We keep a ref to the reader
    mutable nitf::Reader mReader;

    //! We keep a ref to the record
    nitf::Record mRecord;

    std::vector<NITFImageInfo*> mInfos;

    /*!
     *  This function grabs the IID out of the NITF file.
     *  If the data is Complex, it follows the following convention.
     *
     *  - Single-segment NITF: IID is 000
     *  - Multi-segment NITF: IID 001-999
     *
     *  If the data is Derived, the conventions are as follows:
     *
     *  - First 3 characters are the image #, second three characters
     *    are the segment #
     *
     *  NOTE: We are using the image # to populate the Data ID.  Therefore
     *  it is only unique when its Complex data.
     *
     */
    std::pair<int, int> getIndices(nitf::ImageSubheader& subheader);

    void addImageClassOptions(nitf::ImageSubheader& s,
            six::Classification& c) const;

    void addDEClassOptions(nitf::DESubheader& s, six::Classification& c) const;

    void addSecurityOptions(nitf::FileSecurity security,
            const std::string prefix, six::Options& options) const;

    //! Resets the object internals
    void reset();

};


struct NITFReadControlCreator : public ReadControlCreator
{
    six::ReadControl* newReadControl() const;

    bool supports(const std::string& filename) const;

};


}

#endif
