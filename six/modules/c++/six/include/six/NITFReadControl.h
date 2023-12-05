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
#pragma once
#ifndef __SIX_NITF_READ_CONTROL_H__
#define __SIX_NITF_READ_CONTROL_H__

#include <map>
#include <memory>
#include <std/filesystem>

#include "six/NITFImageInfo.h"
#include "six/ReadControl.h"
#include "six/ReadControlFactory.h"
#include "six/Adapters.h"
#include "six/Exports.h"
#include <io/SeekableStreams.h>
#include <import/nitf.hpp>
#include <nitf/IOStreamReader.hpp>

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
 *  This class is not copyable.
 *
 */
struct SIX_SIX_API NITFReadControl : public ReadControl
{
    //!  Constructor
    NITFReadControl(FILE* log);
    NITFReadControl();

    //!  Destructor
    virtual ~NITFReadControl() noexcept(true)
    {
        reset();
    }

    // NITFReadControl is not copyable
    NITFReadControl(const NITFReadControl& other) = delete;
    NITFReadControl& operator=(const NITFReadControl& other) = delete;

    /*!
     *  Read whether a file has COMPLEX or DERIVED data
     *  \param fromFile path to file
     *  \return datatype of file contents
     */
    DataType getDataType(const std::string& fromFile) const override;

    /*!
    *  Read whether a Record has COMPLEX or DERIVED data
    *  \param record the Record in question
    *  \return datatype of Record contents
    */
    static
    DataType getDataType(const nitf::Record& record);

    /*!
    *  Read whether a DESegment has COMPLEX or DERIVED data
    *  \param segment the DESegment in question
    *  \return datatype of DESegment contents
    */
    static
    DataType getDataType(const nitf::DESegment& segment);

    /*!
    *  Determine whether specific attribute outline COMPLEX or DERIVED contents
    *  Interface to allow communication with programs that use different ways
    *  of storing NITF data
    *  \param desid SICD_XML, SIDD_XML, XML_DATA_CONTENT, etc.
    *  \param subheaderLength length of subheader
    *  \param desshsiField Specification identifier
    *  \param treTag tag of TRE (e.g. XML_DATA_CONTENT)
    *  \return datatype
    */
    static
    DataType getDataType(const std::string& desid,
            uint64_t subheaderLength,
            const std::string& desshsiField,
            const std::string& treTag="");

    /*!
     *  Performs (Basic) validation when a segment is being
     *  read.  This function is able to test that the image
     *  segment in question at least follows some of the rules
     *  that its supposed to.
     */
    void validateSegment(const nitf::ImageSubheader&, const NITFImageInfo*) const;
    void validateSegment(const nitf::ImageSubheader&, const NITFImageInfo&) const;

    using ReadControl::load;

    /*!
     *  Fulfills our obligations to the parent class, using the IOInterface
     *  method provided with the same name
     *  \param fromFile    Input filepath
     *  \param schemaPaths Directories or files of schema locations
     */
    void load(const std::string& fromFile, const std::vector<std::string>* pSchemaPaths);
    void load(const std::string& fromFile, const std::vector<std::string>& schemaPaths) override
    {
        load(fromFile, &schemaPaths);
    }
    void load(const std::filesystem::path& fromFile, const std::vector<std::filesystem::path>* pSchemaPaths) override;
    void load(const std::filesystem::path& fromFile, const std::vector<std::filesystem::path>& schemaPaths)
    {
        load(fromFile, &schemaPaths);
    }

    /*
     *  \func load
     *  \brief Loads a SICD from an io::SeekableInputStream.
     *
     *  \param ioStream The stream to read from.
     *  \param schemaPaths Directories or files of schema locations.
     */
    template<typename TSchemaPath>
    void load(io::SeekableInputStream& stream, const std::vector<TSchemaPath>* pSchemaPaths)
    {
        std::shared_ptr<nitf::IOInterface> handle(std::make_shared<nitf::IOStreamReader>(stream));
        load(handle, pSchemaPaths);
    }
    template<typename TSchemaPath>
    void load(io::SeekableInputStream& ioStream, const std::vector<TSchemaPath>& schemaPaths)
    {
        load(ioStream, &schemaPaths);
    }

    void load(std::shared_ptr<nitf::IOInterface>);
    void load(std::shared_ptr<nitf::IOInterface>, const std::vector<std::string>* pSchemaPaths);
    void load(std::shared_ptr<nitf::IOInterface>, const std::vector<std::filesystem::path>* pSchemaPaths);
    template<typename TSchemaPath>
    void load(std::shared_ptr<nitf::IOInterface> ioInterface, const std::vector<TSchemaPath>& schemaPaths)
    {
        load(ioInterface, &schemaPaths);
    }

    using ReadControl::interleaved;
    /*!
     * Read section of image data specified by region
     *
     * \param region Rows and columns of the image to read.  If the number
     * of rows and/or number of columns is set to -1, this indicates to read
     * the entirety of the image in that dimension.  In this case, this
     * parameter will be updated with the actual number of rows and/or
     * columns that were read.
     * \param imageNumber Index of the image to read
     *
     * \return Buffer of image data.  This is simply a pointer to the buffer
     * that is held by 'region'.  If it is nullptr in the incoming region, the
     * memory is allocated and the region's buffer is updated.  In this case
     * it is up to the caller to delete the memory.
     */
    virtual UByte* interleaved(Region& region, size_t imageNumber) override;

    std::string getFileType() const override
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

    std::map<std::string, void*> mCompressionOptions;

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
     *  it is only unique when it's Complex data.
     *
     */
    std::pair<size_t, size_t>
    getIndices(const nitf::ImageSubheader& subheader) const;
    struct ImageAndSegment final
    {
        size_t image = 0;
        size_t segment = 0;
    };
    void getIndices(const nitf::ImageSubheader& subheader, ImageAndSegment&) const;

    void addImageClassOptions(const nitf::ImageSubheader& s,
            six::Classification& c) const;

    void addDEClassOptions(const nitf::DESubheader& s,
                           six::Classification& c) const;

    void addSecurityOptions(nitf::FileSecurity security,
            const std::string& prefix, six::Options& options) const;

    //! Resets the object internals
    void reset();

    //! All pointers populated within the options need
    //  to be cleaned up elsewhere. There is no access
    //  to deallocation in NITFReadControl directly
    virtual void createCompressionOptions(
            std::map<std::string, void*>& )
    {
    }

private:
    template<typename TSchemaPath>
    void load_(std::shared_ptr<nitf::IOInterface> ioInterface, const std::vector<TSchemaPath>* pSchemaPaths);

    std::unique_ptr<Legend> findLegend(size_t productNum);

    void readLegendPixelData(const nitf::ImageSubheader& subheader,
                             size_t imageSeg,
                             Legend& legend);

    static
    bool isLegend(const nitf::ImageSubheader& subheader)
    {
        const auto iCat = subheader.imageCategory();
        return (iCat == "LEG");
    }

    static
    bool isDed(const nitf::ImageSubheader& subheader)
    {
        const auto iCat = subheader.imageCategory();
        return (iCat == "DED");
    }

    void setDisplayLUT(six::NITFImageInfo&, const nitf::ImageSubheader&); // LUT processing for SIDDs

    // We need this for one of the load overloadings
    // to prevent data from being deleted prematurely
    // The issue occurs from the explicit destructor of
    // IOControl
    std::shared_ptr<nitf::IOInterface> mInterface;
};


struct NITFReadControlCreator final : public ReadControlCreator
{
    six::ReadControl* newReadControl() const override
    {
        std::unique_ptr<six::ReadControl> retval;
        newReadControl(retval);
        return retval.release();
    }
    void newReadControl(std::unique_ptr<six::ReadControl>& result) const override;

    bool supports(const std::string& filename) const override;

};


}

#endif

