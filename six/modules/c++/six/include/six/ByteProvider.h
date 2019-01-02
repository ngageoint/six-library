/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#ifndef __SIX_BYTE_PROVIDER_H__
#define __SIX_BYTE_PROVIDER_H__

#include <nitf/ByteProvider.hpp>
#include <mem/SharedPtr.h>
#include <six/Container.h>
#include <six/NITFWriteControl.h>
#include <six/NITFHeaderCreator.h>
#include <six/NITFSegmentInfo.h>
#include <six/XMLControlFactory.h>

namespace six
{
/*!
 * \class ByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the SICD/SIDD pixel data and are all trying to write out a single file; in
 * that scenario, this class provides all the raw bytes corresponding to the
 * caller's AOI, including NITF headers if necessary.  The caller does not need
 * to understand anything about the NITF file layout in order to write out the
 * file.  The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to minimize the number of
 * copies.
 */
class ByteProvider : public nitf::ByteProvider
{
public:
    /*!
     * Constructor. Calls initialize() internally to populate class
     * \param headerCreator Class for populating and managing NITF
     *  header information
     * \param schemaPaths Paths to XML schemas
     * \param desBuffers Collection of pointers to serialized Data
     *  Extension Segment (DES) buffers and their lengths. This data
     *  will be stored in the DES portion of the NITF
     */
    ByteProvider(std::auto_ptr<six::NITFHeaderCreator> headerCreator,
                 const std::vector<std::string>& schemaPaths,
                 const std::vector<PtrAndLength>& desBuffers);

    /*!
     * Populates the writer Options from given parameters
     * \param container Container holding Data object
     * \param maxProductSize Maximum size of image segment in bytes
     * \param numRowsPerBlock Rows per block. Will be truncated if greater than
     *        num rows in image
     * \param numColsPerBlock Cols per block. Will be truncated if greater than
     *        num cols in image
     * \param[out] options Options to populate
     */
    static void populateOptions(
            mem::SharedPtr<Container> container,
            size_t maxProductSize,
            size_t numRowsPerBlock,
            size_t numColsPerBlock,
            Options& options);

    /*!
     * Compute the XML metadata, data extension segment (DES) buffers,
     * and blocking information from a populated NITF writer
     * \static
     * \param writer Populated NITF writer
     * \param schemaPaths Paths to XML schemas
     * \param[out] xmlStrings Collection of XML metadata strings
     *  stored in the populated writer. There is one string per Data
     *  entry in the underlying Container object.
     * \param[out] desData Collection of DES buffers. There will be
     *  one entry per XML string.
     * \param[out] numRowsPerBlock Number of image rows per NITF block
     * \param[out] numColsPerBlock Number of image columns per NITF
     *  block
     */
    static void populateInitArgs(
            const NITFWriteControl& writer,
            const std::vector<std::string>& schemaPaths,
            std::vector<std::string>& xmlStrings,
            std::vector<PtrAndLength>& desData,
            size_t& numRowsPerBlock,
            size_t& numColsPerBlock);

    /*!
     * Compute the XML metadata, data extension segment (DES) buffers,
     * and blocking information from a populated NITF header creator
     * \static
     * \param writer Populated NITF header creator object
     * \param schemaPaths Paths to XML schemas
     * \param[out] xmlStrings Collection of XML metadata strings
     *  stored in the populated writer. There is one string per Data
     *  entry in the underlying Container object.
     * \param[out] desData Collection of DES buffers. There will be
     *  one entry per XML string.
     * \param[out] numRowsPerBlock Number of image rows per NITF block
     * \param[out] numColsPerBlock Number of image columns per NITF
     *  block
     */
    static void populateInitArgs(
            const NITFHeaderCreator& headerCreator,
            const std::vector<std::string>& schemaPaths,
            std::vector<std::string>& xmlStrings,
            std::vector<PtrAndLength>& desData,
            size_t& numRowsPerBlock,
            size_t& numColsPerBlock);

    /*!
     * Initialize the ByteProvider
     * \param headerCreator Class for populating and managing NITF
     *  header information
     * \param schemaPaths Paths to XML schemas
     * \param desBuffers Collection of pointers to serialized Data
     *  Extension Segment (DES) buffers and their lengths. This data
     *  will be stored in the DES portion of the NITF
     */
    void initialize(std::auto_ptr<six::NITFHeaderCreator> headerCreator,
                    const std::vector<std::string>& schemaPaths,
                    const std::vector<PtrAndLength>& desBuffers);
protected:
    /*!
     * Default constructor. Client code must call initialize() to
     * use. Only accessible through classes inheriting from ByteProvider
     */
    ByteProvider();

    /*!
     * Initialize the byte provider.  Must be called in the constructor of
     * inheriting classes.
     *
     * \param container Container initialized with all associated data
     * \param xmlRegistry XML registry
     * \param schemaPaths Directories or files of schema locations
     * \param maxProductSize The max number of bytes in an image segment.
     * \param numRowsPerBlock The number of rows per block.  Only applies for
     * SIDD.  Defaults to no blocking.
     * \param numColsPerBlock The number of columns per block.  Only applies
     * for SIDD.  Defaults to no blocking.
     */
    void initialize(mem::SharedPtr<Container> container,
                    const XMLControlRegistry& xmlRegistry,
                    const std::vector<std::string>& schemaPaths,
                    size_t maxProductSize,
                    size_t numRowsPerBlock = 0,
                    size_t numColsPerBlock = 0);

    /*!
     * Initialize the byte provider.  Must be called in the constructor of
     * inheriting classes.
     *
     * \param writer Writer.  Must be initialized via
     * NITFWriteControl::initialize() and have all desired product size and
     * blocking values set.
     * \param schemaPaths Directories or files of schema locations
     */
    void initialize(const NITFWriteControl& writer,
                    const std::vector<std::string>& schemaPaths);

    void initialize(const NITFWriteControl& writer,
                    const std::vector<std::string>& schemaPaths,
                    const std::vector<PtrAndLength>& desBuffers);
};
}

#endif
