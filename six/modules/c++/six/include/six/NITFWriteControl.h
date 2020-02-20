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
#ifndef __SIX_NITF_WRITE_CONTROL_H__
#define __SIX_NITF_WRITE_CONTROL_H__

#include <map>

#include <mem/SharedPtr.h>
#include "six/Types.h"
#include "six/Container.h"
#include "six/WriteControl.h"
#include "six/NITFImageInfo.h"
#include "six/Adapters.h"
#include <six/NITFHeaderCreator.h>

namespace six
{

/*!
 *  \class NITFWriteControl
 *  \brief Use NITRO to write out a properly segmented NITF
 *
 *  This class uses NITRO to write out a NITF that is properly
 *  segmented using the algorithm in the SICD/SIDD spec.  In order
 *  to do this efficiently for the pixel-interleaved unblocked case
 *  we take advantage of the WriteHandler API and bypass the normal
 *  blocker.
 */
class NITFWriteControl : public WriteControl
{
public:

    //! Constructor. Must call initialize to use.
    NITFWriteControl();

    /*!
     * Constructor. Calls initialize.
     * \param container The data container
     */
    NITFWriteControl(mem::SharedPtr<Container> container);

    /*!
     * Constructor. Calls initialize.
     * \param options Initialization options
     * \param container The data container
     * \param xmlRegistry Optional XMLControlRegistry
     */
    NITFWriteControl(const six::Options& options,
                     mem::SharedPtr<Container> container,
                     const XMLControlRegistry* xmlRegistry = NULL);

    //!  We are a 'NITF'
    std::string getFileType() const
    {
        return "NITF";
    }

    // Get the record that was generated during initialization
    nitf::Record getRecord() const
    {
        return mNITFHeaderCreator->getRecord();
    }

    /*!
     * Sets XMLControlRegistry. Overriding so we can pass to NITFHeaderCreator
     * as well.
     * \param xmlRegistry XMLControlRegistry to set
     */
    virtual void setXMLControlRegistry(const XMLControlRegistry* xmlRegistry)
    {
        setXMLControlRegistryImpl(xmlRegistry);
    }

    // Get the record that was generated during initialization
    nitf::Record& getRecord()
    {
        return mNITFHeaderCreator->getRecord();
    }

    //! \return Collection of NITF image info pointers
    std::vector<mem::SharedPtr<NITFImageInfo> > getInfos()
    {
        return mNITFHeaderCreator->getInfos();
    }

    //! \return Mutable data container
    mem::SharedPtr<Container> getContainer()
    {
        return mNITFHeaderCreator->getContainer();
    }

    //! \return Const data container
    mem::SharedPtr<const Container> getContainer() const
    {
        return mNITFHeaderCreator->getContainer();
    }

    //! \return Const NITF options
    const six::Options& getOptions() const
    {
        return mNITFHeaderCreator->getOptions();
    }

    //! \return Collection of NITF segment writers
    std::vector<mem::SharedPtr<nitf::SegmentWriter> > getSegmentWriters()
    {
        return mNITFHeaderCreator->getSegmentWriters();
    }

    //! \return The underlying NITF header creator object
    const six::NITFHeaderCreator* getNITFHeaderCreator() const
    {
        return mNITFHeaderCreator.get();
    }

    /*!
     * Set the internal NITF header creator
     * \param headerCreator Populated NITF header creator
     */
    void setNITFHeaderCreator(std::auto_ptr<six::NITFHeaderCreator> headerCreator);

    virtual void initialize(const six::Options& options,
                            mem::SharedPtr<Container> container);

    virtual void initialize(mem::SharedPtr<Container> container);

    using WriteControl::save;

    /*!
     * Set the logger.
     * \param logger The logger.
     */
    void setLogger(logging::Logger* logger, bool ownLog = false)
    {
        mNITFHeaderCreator->setLogger(logger, ownLog);
    }

    /*
     *  \func  save
     *  \brief writes the product to disk
     *  \param imageData   List of image segments
     *  \param outputFile  Output path to write
     *  \param schemaPaths Directories or files of schema locations
     */
    virtual void save(const SourceList& imageData,
                      const std::string& outputFile,
                      const std::vector<std::string>& schemaPaths);

    /*!
     *  Bind an interleaved (IQIQIQIQ) memory buffer
     *  to this record and write out a SICD, or interleaved
     *  or single band equivalents for SIDD
     *
     *  This function creates a nitf::BufferedWriter with
     *  the buffer size set to the third argument.  We do
     *  automatic byte swapping unless you tell us otherwise.
     *
     *  This means that if the system is little endian, we
     *  swap the source.
     *
     *  If you are using a big endian file as the supply stream,
     *  you should set SWAP_OFF, and if you are using a little
     *  endian file as the supply stream, you should set BYTE_SWAP to
     *  on.
     *
     *  \func  save
     *  \brief writes the product to disk
     *  \param imageData   List of image segments
     *  \param outputFile  Output path to write
     *  \param schemaPaths Directories or files of schema locations
     */
    virtual void save(const BufferList& imageData,
                      const std::string& outputFile,
                      const std::vector<std::string>& schemaPaths);

    void save(const NonConstBufferList& imageData,
              const std::string& outputFile,
              const std::vector<std::string>& schemaPaths)
    {
        save(convertBufferList(imageData), outputFile, schemaPaths);
    }

    /*!
     *  Bind an interleaved (IQIQIQIQ) input stream
     *  to this record and write out a SICD/SIDD.  We do
     *  automatic byte swapping unless you tell us otherwise.
     *
     *  This means that if the system is little endian, we
     *  swap the source.
     *
     *  If you are using a big endian file as the supply stream,
     *  you should set SWAP_OFF, and if you are using a little
     *  endian file as the supply stream, you should set BYTE_SWAP to
     *  on.
     */
    virtual void save(const SourceList& list,
                      nitf::IOInterface& outputFile,
                      const std::vector<std::string>& schemaPaths);

    /*!
     *  Bind an interleaved (IQIQIQIQ) input stream
     *  to this record and write out a SICD/SIDD.  We do
     *  automatic byte swapping unless you tell us otherwise.
     *
     *  This means that if the system is little endian, we
     *  swap the source.
     *
     *  If you are using a big endian file as the supply stream,
     *  you should set SWAP_OFF, and if you are using a little
     *  endian file as the supply stream, you should set BYTE_SWAP to
     *  on.
     */
    virtual void save(const BufferList& list,
                      nitf::IOInterface& outputFile,
                      const std::vector<std::string>& schemaPaths);

    void save(const NonConstBufferList& list,
              nitf::IOInterface& outputFile,
              const std::vector<std::string>& schemaPaths)
    {
        save(convertBufferList(list), outputFile, schemaPaths);
    }

    /*!
     *  This function sets the organization ID (the 40 character DESSHRP field
     *  in the DES's user-defined subheader).
     */
    void setOrganizationId(const std::string& organizationId);

    /*!
     *  This function sets the location ID and associated namespace (the
     *  25 character DESSHLI and 120 character DESSHLIN fields in the
     *  DES's user-defined subheader).
     */
    void setLocationIdentifier(const std::string& locationId,
                               const std::string& locationIdNamespace);

    /*!
     *  This function sets the abstract (the 200 character DESSHABS field
     *  in the DES's user-defined subheader).
     */
    void setAbstract(const std::string& abstract);

    void addSegmentData(const char* data, size_t size, nitf::Off start,
            int byteSkip, bool copyData);

    /* !
     *  Add a SegmentWriter for writing an extra DES. Writer must
     *  already have an attached Segment source. For proper usage,
     *  see test_adding_additional_des.cpp in six/sicd/tests.
     *  Note that this function has deferred execution. Errors
     *  will manifest after NITFWriteControl.write() is called.
     *
     * \param writer A SegmentWriter with loaded, attached SegmentSource
     */
    void addAdditionalDES(mem::SharedPtr<nitf::SegmentWriter> writer);

    /*!
     *  Takes in a string representing the classification level
     *  and returns the value expected by the NITF
     */
    static std::string getNITFClassification(const std::string& level);

protected:
    nitf::Writer mWriter;
    std::map<std::string, void*> mCompressionOptions;
    std::auto_ptr<six::NITFHeaderCreator> mNITFHeaderCreator;

    void writeNITF(nitf::IOInterface& os);

    /*!
     *  Initialize a writer.  Using the SICD spec., this auto-initializes
     *  the following fields (from this library or automatically
     *  from NITRO where indicated).
     *
     *  FHDR:     NITF     (NITRO)
     *  FVER:     02.10    (NITRO)
     *  CLEVEL:            (NITRO)
     *  STYPE:    BF01     (NITRO)
     *  FDT:               (NITRO - future)
     *  FTITLE:   SICD: container.getCoreName()
     *
     *  FSCOP:    00000    (NITRO)
     *  FSCPYS:   00000    (NITRO)
     *  ENCRYP:   0        (NITRO)
     *  FBKGC:             (NITRO)
     *
     *  FL:                (NITRO - on write)
     *  HL:                (NITRO - on write)
     *  NUMI:     mInfos[0]->imageSegments.size()
     *
     *  LISH:              (NITRO)
     *  LIn:               (NITRO)
     *
     *  NUMS:     0        (NITRO)
     *  NUMX:     0        (NITRO)
     *  NUMT:     0        (NITRO)
     *  LTSH:     0        (NITRO)
     *  LTn:      0        (NITRO)
     *  NUMDES:   1
     *  LDSH:              (NITRO)
     *  LDn                (NITRO)
     *
     *  NUMRES:   0        (NITRO)
     *  UDHDL:    0        (NITRO)
     *  XHDL:     0        (NITRO)
     *
     *  *SCLAS    U        (NITRO)
     *
     *  IM*       IM       (NITRO)
     *  IID1      SICDnnn  (000,001-999)
     *
     *  TGTID     \s+      (NITRO)
     *  IID2      SICD: SICDHelper::getCoreName()
     *
     *  ENCRYP:   0        (NITRO)
     *  ISORCE:   SICDHelper::getCollectorName()
     *
     *  NROWS:    mInfos[0]->imageSegments[j].numRows
     *  NCOLS:    mInfos[0]->imageSegments[j].numCols
     *  PVTYPE:   R (I for MP?)
     *  IREP:     NODISPLY
     *  ICAT:     SAR
     *  ABPP:     mInfos[0]->data.getNumBytesPerPixel() * 8
     *  PJUST:    R
     *  ICORDS:   G
     *  IGEOLO:   SICDHelper::getGeographicCorners()
     *  NICOM:    0        (NITRO)
     *  IC:       NC       (NITRO)
     *  NBANDS:   2
     *  IREPBANDn: \s
     *  ISUBCATn: I,Q
     *  IFCm:     N        (NITRO? Should this do it automatically?)
     *  IMFLTn:   \s\s\s   (NITRO)
     *  NLUTS:    0        (NITRO)
     *  ISYNC:    0        (NITRO)
     *  IMODE:    P
     *  NBPR:     1        (NITRO)
     *  NBPC:     1        (NITRO)
     *  NPPBV:    0000/i<8192 (NITRO)
     *  NPPBV     0000/i<8192 (NITRO)
     *  NBPP:     mInfos[0]->data.getNumBytesPerPixel() * 8
     *  IDLVL:    001-999  (NITRO)
     *  IALVL:    000-998
     *  ILOC:     mInfos[0]->imageSegment[j].rowOffset + 00000
     *  IMAG:     1.0\s    (NITRO)
     *
     *  DE:       DE       (NITRO)
     *  DESID:    XML_DATA_CONTENT
     *  DESVER    01
     *  DESSHL    0000     (NITRO)
     *  DESDAT    <SICD XML Data> / <SIDD XML Data>
     *
     */

    /*!
     *  This function sets up the container data, attaching
     *  it to the DES segments.  Then it completes the write.
     */
    void addDataAndWrite(const std::vector<std::string>& schemaPaths);

    /*!
     * This function sets the NITF blocking.  By default, the product
     * will be unblocked, but for SIDDs the user can override this via
     * the options.  To be pedantic, the SIDD spec defines the NITF
     * header such that blocking is not allowed, but this is a typo and
     * will cause problems with some ELTs for images > 1 GB.
     */
    void setBlocking(const std::string& imode,
                     const types::RowCol<size_t>& segmentDims,
                     nitf::ImageSubheader& subheader);

    /*!
     *  This function sets the image security fields in the
     *  given image subheader using the parameters in the
     *  classification object.  This allows a manual override of
     *  these fields in the NITF product.
     */
    void setImageSecurity(const six::Classification& c,
                          nitf::ImageSubheader& subheader);

    /*!
     *  This function sets the image security fields in the
     *  given DE subheader using the parameters in the
     *  classification object.  This allows a manual override of
     *  these fields in the NITF product.
     */
    void setDESecurity(const six::Classification& c,
                       nitf::DESubheader& subheader);

    void setSecurity(const six::Classification& c,
                     nitf::FileSecurity security,
                     const std::string& prefix);

    /*!
     *  This function scans through the security fields for each
     *  image segment and sets the security information in the file
     *  subheader so that it matches the highest level of the
     *  image segments.
     */
    void updateFileHeaderSecurity();

    //! All pointers populated within the options need
    //  to be cleaned up elsewhere. There is no access
    //  to deallocation in NITFWriteControl directly
    virtual void createCompressionOptions(
            std::map<std::string, void*>& /*options*/)
    {
    }

    bool shouldByteSwap() const;

    void setXMLControlRegistryImpl(const XMLControlRegistry* xmlRegistry);

private:
    /*!
     * Get the DES type identifier.
     * \param data The data object.
     * \result The DES type identifier.
     */
    static
    std::string getDesTypeID(const six::Data& data);

    /*!
     * Determine if a user-defined sub-header if required.
     * \param data The data object.
     * \result Boolean true if a user-defined sub-header is required.
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
     * Construct complex image sub-header identifier.
     * \param segmentNum The segment number.
     * \param numImageSegments The number of image segments.
     * \result Sub-header identifier.
     */
    static
    std::string getComplexIID(size_t segmentNum, size_t numImageSegments);

    /*!
     * Construct detected image sub-header identifier.
     * \param segmentNum The segment number.
     * \param productNum The product number.
     * \result Sub-header identifier.
     */
    static
    std::string getDerivedIID(size_t segmentNum, size_t productNum);

    /*!
     * Construct image sub-header identifier.
     * \param segmentNum The segment number.
     * \param numImageSegments The number of image segments.
     * \param productNum The product number.
     * \result Sub-header identifier.
     */
    static
    std::string getIID(DataType dataType,
                       size_t segmentNum,
                       size_t numImageSegments,
                       size_t productNum);

private:
    //! Noncopyable
    NITFWriteControl(const NITFWriteControl& );
    const NITFWriteControl& operator=(const NITFWriteControl& );
};
}
#endif

