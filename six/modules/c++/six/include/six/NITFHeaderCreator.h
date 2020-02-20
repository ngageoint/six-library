/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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
#ifndef __SIX_NITF_HEADER_CREATOR_H__
#define __SIX_NITF_HEADER_CREATOR_H__

#include <map>

#include <mem/SharedPtr.h>
#include "six/Types.h"
#include "six/Container.h"
#include "six/NITFImageInfo.h"
#include "six/Adapters.h"
#include <import/logging.h>

namespace six
{

/*!
 *  \class NITFHeaderCreator
 *  \brief NITF writer utilities.
 *
 *  This class helps in populating NITF header fields
 *  prior to writing.
 */
class NITFHeaderCreator
{
public:

    //! Constructor. Must call initialize to use.
    NITFHeaderCreator();

    /*!
     * Constructor. Calls initialize.
     * \param container The data container
     */
    NITFHeaderCreator(mem::SharedPtr<Container> container);

    /*!
     * Constructor. Calls initialize.
     * \param options Initialization options
     * \param container The data container
     */
    NITFHeaderCreator(const six::Options& options,
                      mem::SharedPtr<Container> container);

    //!  Destructor.
    virtual ~NITFHeaderCreator()
    {
        if (mLog && mOwnLog)
        {
            delete mLog;
        }
    }

    //!  Keys that allow us to override the ILOC rules for tests
    static const char OPT_MAX_PRODUCT_SIZE[];
    static const char OPT_MAX_ILOC_ROWS[];

    // The following two options control how the `comrat` field is set

    //! Bytes/pixel/band for j2k compression
    static const char OPT_J2K_COMPRESSION_BYTERATE[];

    //! True if numerically lossless, false for visually lossless
    //! We assume visually lossy compression will not be used
    static const char OPT_J2K_COMPRESSION_LOSSLESS[];

    //! These determine the NITF blocking
    //  They only pertain to SIDD
    //  SICDs are never blocked, so setting this for a SICD will
    //  result in an error
    static const char OPT_NUM_ROWS_PER_BLOCK[];
    static const char OPT_NUM_COLS_PER_BLOCK[];

    //!  Buffered IO
    static const size_t DEFAULT_BUFFER_SIZE;

    /*!
     *  This function sets the organization ID (the 40 character DESSHRP field
     *  in the DES's user-defined subheader).
     * \param organizationId The organization identifier.
     */
    void setOrganizationId(const std::string& organizationId);

    /*!
     *  This function sets the location ID and associated namespace (the
     *  25 character DESSHLI and 120 character DESSHLIN fields in the
     *  DES's user-defined subheader).
     * \param locationId The location identifier.
     * \param locationIdNamespace The location identifier namespace.
     */
    void setLocationIdentifier(const std::string& locationId,
                               const std::string& locationIdNamespace);

    /*!
     *  This function sets the abstract (the 200 character DESSHABS field
     *  in the DES's user-defined subheader).
     * \param abstract The abstract.
     */
    void setAbstract(const std::string& abstract);

    /*!
     * Add segment data to the record.
     * \static
     * \param data The data buffer.
     * \param size The number of bytes in the data buffer.
     * \param start The starting offset into the input bufffer.
     * \param byteSkip The starting offset into the record.
     * \param copyData TBD
     * \param[out] record The record to modify.
     */
    static void addSegmentData(const char* data, size_t size, nitf::Off start,
            int byteSkip, bool copyData,
            nitf::Record& record);

    /* !
     *  Add a SegmentWriter for writing an extra DES. Writer must
     *  already have an attached Segment source. For proper usage,
     *  see test_adding_additional_des.cpp in six/sicd/tests.
     *  Note that this function has deferred execution. Errors
     *  will manifest after NITFHeaderCreator.write() is called.
     *
     * \param writer A SegmentWriter with loaded, attached SegmentSource
     */
    void addAdditionalDES(mem::SharedPtr<nitf::SegmentWriter> writer);

    /*!
     * Convert classification level to NITF classification
     * \static
     * \param level Classification level string
     * \return Classification level expected by the NITF
     */
    static std::string getNITFClassification(const std::string& level);

    //! \return A copy of the NITF record by value.
    nitf::Record getRecord() const
    {
        return mRecord;
    }

    //! \return A reference to the NITF record.
    nitf::Record& getRecord()
    {
        return mRecord;
    }

    //! Get the infos.
    std::vector<mem::SharedPtr<NITFImageInfo> > getInfos() const
    {
        return mInfos;
    }

    //! Get the container.
    mem::SharedPtr<six::Container> getContainer()
    {
        return mContainer;
    }

    //! Get the container.
    mem::SharedPtr<const six::Container> getContainer() const
    {
        return mContainer;
    }

    //! Get the options.
    six::Options& getOptions()
    {
        return mOptions;
    }

    //! Get the options.
    const six::Options& getOptions() const
    {
        return mOptions;
    }

    // Get the record that was generated during initialization
    std::vector<mem::SharedPtr<nitf::SegmentWriter> > getSegmentWriters() const
    {
        return mSegmentWriters;
    }

    /*!
     * Set the internal XML registry
     * \param xmlRegistry XML registry used to format and write the NITF
     */
    void setXMLControlRegistry(const XMLControlRegistry* xmlRegistry)
    {
        mXMLRegistry = xmlRegistry;
        if (!mXMLRegistry)
        {
            mXMLRegistry = &XMLControlFactory::getInstance();
        }
    }

    //! \return XML registry being used by the writer
    const XMLControlRegistry* getXMLControlRegistry() const
    {
        return mXMLRegistry;
    }

    /*!
     * Set the logger.
     * \param logger The logger.
     * \param ownLog Flag for whether the class takes ownership of the
     *  logger. Default is false.
     */
    void setLogger(logging::Logger* logger, bool ownLog = false)
    {
        // Delete the current logger if it exists and is owned
        if (mLog && mOwnLog && logger != mLog)
        {
            delete mLog;
        }

        if (logger)
        {
            // Logger is passed in: set it and determine ownership
            mLog = logger;
            mOwnLog = ownLog;
        }
        else
        {
            // No logger passed in: create a null logger
            mLog = new logging::NullLogger;
            mOwnLog = true;
        }
    }

    /*!
     * Load a mesh segment's information.
     * \param meshName The mesh name.
     * \param meshBuffer The vector of byte-serialized mesh buffers.
     * \param classification The classification of the information.
     */
    void loadMeshSegment(const std::string& meshName,
                         const std::vector<sys::byte>& meshBuffer,
                         const six::Classification& classification);

    /*!
     * Construct complex image sub-header identifier.
     * \static
     * \param segmentNum The segment number.
     * \param numImageSegments The number of image segments.
     * \return Sub-header identifier.
     */
    static
    std::string getComplexIID(size_t segmentNum, size_t numImageSegments);

    /*!
     * Construct detected image sub-header identifier.
     * \static
     * \param segmentNum The segment number.
     * \param productNum The product number.
     * \return Sub-header identifier.
     */
    static
    std::string getDerivedIID(size_t segmentNum, size_t productNum);

    /*!
     * Construct image sub-header identifier.
     * \static
     * \param segmentNum The segment number.
     * \param numImageSegments The number of image segments.
     * \param productNum The product number.
     * \return Sub-header identifier.
     */
    static std::string getIID(DataType dataType,
                              size_t segmentNum,
                              size_t numImageSegments,
                              size_t productNum);

    /*!
     * This function sets the NITF blocking.  By default, the product
     * will be unblocked, but for SIDDs the user can override this via
     * the options.  To be pedantic, the SIDD spec defines the NITF
     * header such that blocking is not allowed, but this is a typo and
     * will cause problems with some ELTs for images > 1 GB.
     * \param imode Image mode, specifying if multiple bands exist and
     *  how they are interleved.
     * \param segmentDims The segment dimensions.
     * \param[output] subheader The sub-header to modify.
     */
    void setBlocking(const std::string& imode,
                     const types::RowCol<size_t>& segmentDims,
                     nitf::ImageSubheader& subheader);

    /*!
     *  This function sets the image security fields in the
     *  given image subheader using the parameters in the
     *  classification object.  This allows a manual override of
     *  these fields in the NITF product.
     * \param classification Classification of image.
     * \param subheader The sub-header to modify.
     */
    void setImageSecurity(const six::Classification& classification,
                          nitf::ImageSubheader& subheader);
 
    /*!
     *  This function sets the image security fields in the
     *  given DE subheader using the parameters in the
     *  classification object.  This allows a manual override of
     *  these fields in the NITF product.
     * \param classification Classification of DES.
     * \param subheader The sub-header to modify.
     */
    void setDESecurity(const six::Classification& classification,
                       nitf::DESubheader& subheader);

    /*!
     * This function constructs security field content.
     * \param classification Classification of the object.
     * \param security The security structure.
     * \param prefix The prefix.
     */
    void setSecurity(const six::Classification& classification,
                     nitf::FileSecurity security,
                     const std::string& prefix);
    /*!
     * This function updates the file subheader security information
     * to match the highest level security of the image segments.
     */
    void updateFileHeaderSecurity();

    /*!
     * Get the DES type identifier.
     * \static
     * \param data The data object.
     * \return The DES type identifier.
     */
    static
    std::string getDesTypeID(const six::Data& data);

    /*!
     * Determine if a user-defined sub-header if required.
     * \static
     * \param data The data object.
     * \return Boolean true if a user-defined sub-header is required.
     */
    static
    bool needUserDefinedSubheader(const six::Data& data);

    /*!
     * Add a user-defined sub-header.
     * \param data The data object.
     * \param[out] subheader The sub-header to modify.
     */
    void addUserDefinedSubheader(const six::Data& data,
                                 nitf::DESubheader& subheader) const;

    /*!
     * Initialize the NITFHeaderCreator object.
     * \param The container.
     */
    void initialize(mem::SharedPtr<Container> container);

    /*!
     * Initialize the NITFHeaderCreator object.
     * \param options The options.
     * \param The container.
     */
    void initialize(const six::Options& options,
                    mem::SharedPtr<Container> container);

protected:
    nitf::Record mRecord;
    std::vector<mem::SharedPtr<NITFImageInfo> > mInfos;
    std::vector<mem::SharedPtr<nitf::SegmentWriter> > mSegmentWriters;
    mem::SharedPtr<six::Container> mContainer;
    six::Options mOptions;
    const XMLControlRegistry* mXMLRegistry;

    //! All pointers populated within the options need
    //  to be cleaned up elsewhere. There is no access
    //  to deallocation in NITFHeaderCreator directly
    virtual void createCompressionOptions(
        std::map<std::string, void*>& /*options*/)
    {
    }

private:
    std::string mOrganizationId;
    std::string mLocationId;
    std::string mLocationIdNamespace;
    std::string mAbstract;
    logging::Logger* mLog;
    bool mOwnLog;
};
}
#endif
