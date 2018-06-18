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

#ifndef __SIX_COMPRESSED_BYTE_PROVIDER_H__
#define __SIX_COMPRESSED_BYTE_PROVIDER_H__

#include <nitf/CompressedByteProvider.hpp>
#include <mem/SharedPtr.h>
#include <six/Container.h>
#include <six/NITFWriteControl.h>
#include <six/NITFSegmentInfo.h>
#include <six/XMLControlFactory.h>

namespace six
{
/*!
 * \class CompressedByteProvider
 * \brief Used to provide corresponding compressed NITF bytes
 * (and uncompressed NITF headers) when provided with some AOI of the pixel data.
 * The idea is that if getBytes() is called multiple times, eventually for the
 * entire image, the raw bytes provided back will be the entire NITF file.
 * This abstraction is useful if separate threads, processes, or even machines
 * have only portions of the SICD/SIDD pixel data and are all trying to write
 * out a single file; in that scenario, this class provides all the raw bytes
 * corresponding to the caller's AOI, including NITF headers if necessary.
 * The caller does not need to understand anything about the NITF file layout
 * in order to write out the file.
 * The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to minimize the number of
 * copies.
 */
class CompressedByteProvider : public nitf::CompressedByteProvider
{
protected:
    /*!
     * Initialize the byte provider.  Must be called in the constructor of
     * inheriting classes.
     *
     * \param container Container initialized with all associated data
     * \param xmlRegistry XML registry
     * \param schemaPaths Directories or files of schema locations
     * \param bytesPerBlock A vector for each image segment. Each inner vector
     *        contains the compressed size for each block in the segment,
     *        in bytes.
     * \param isNumericallyLossless Flag whether compression was lossless
     * \param maxProductSize The max number of bytes in an image segment.
     * \param numRowsPerBlock The number of rows per block.  Only applies for
     * SIDD.  Defaults to no blocking.
     * \param numColsPerBlock The number of columns per block.  Only applies
     * for SIDD.  Defaults to no blocking.
     */
    void initialize(mem::SharedPtr<Container> container,
                    const XMLControlRegistry& xmlRegistry,
                    const std::vector<std::string>& schemaPaths,
                    const std::vector<std::vector<size_t> >& bytesPerBlock,
                    bool isNumericallyLossless,
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
     * \param bytesPerBlock A vector for each image segment. Each inner vector
     *        contains the compressed size for each block in the segment,
     *        in bytes.
     */
    void initialize(const NITFWriteControl& writer,
                    const std::vector<std::string>& schemaPaths,
                    const std::vector<std::vector<size_t> >& bytesPerBlock);
};
}

#endif
