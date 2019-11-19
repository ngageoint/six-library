/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#ifndef __SIX_SIDD_COMPRESSED_SIDD_BYTE_PROVIDER_H__
#define __SIX_SIDD_COMPRESSED_SIDD_BYTE_PROVIDER_H__

#include <six/CompressedByteProvider.h>
#include <six/sidd/DerivedData.h>

namespace six
{
namespace sidd
{
/*!
 * \class CompressedSIDDByteProvider
 * \brief Used to provide corresponding compressed NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the SIDD pixel data and are all trying to write out a single file; in that
 * scenario, this class provides all the raw bytes corresponding to the caller's
 * AOI, including NITF headers if necessary.  The caller does not need to
 * understand anything about the NITF file layout in order to write out the
 * file.  The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to not perform any
 * copies.  A single logical SIDD image which spans multiple NITF image segments
 * is supported; unrelated SIDD images in one NITF are not yet supported.
 */
class CompressedSIDDByteProvider : public six::CompressedByteProvider
{
public:
    /*!
     * Constructor
     *
     * \param data Representation of the derived data
     * \param schemPaths Directories or files of schema locations
     * \param bytesPerBlock A vector for each image segment. Each inner vector
     *        contains the compressed size for each block in the segment,
     *        in bytes.
     * \param isNumericallyLossless Flag whether compression was lossless
     * \param numRowsPerBlock The number of rows per block.  Defaults to no
     * blocking.
     * \param numColsPerBlock The number of columns per block.  Defaults to no
     * blocking.
     * \param maxProductSize The max number of bytes in an image segment.
     * By default this is set automatically for you based on NITF file rules.
     */
    CompressedSIDDByteProvider(const DerivedData& data,
                               const std::vector<std::string>& schemaPaths,
                               const std::vector<std::vector<size_t> >& bytesPerBlock,
                               bool isNumericallyLossless,
                               size_t numRowsPerBlock = 0,
                               size_t numColsPerBlock = 0,
                               size_t maxProductSize = 0);

    /*!
     * Constructor
     * This option allows you to pass in an initialized writer,
     * in case you need something specific in the header
     *
     * \param writer Initialized NITFWriteControl.  Must have all desired
     * product size and blocking values set.
     * \param schemaPaths Directories or files of schema locations
     * \param bytesPerBlock A vector for each image segment. Each inner vector
     *        contains the compressed size for each block in the segment,
     *        in bytes.
     */
    CompressedSIDDByteProvider(const NITFWriteControl& writer,
                               const std::vector<std::string>& schemaPaths,
                               const std::vector<std::vector<size_t> >& bytesPerBlock);
};
}
}

#endif
