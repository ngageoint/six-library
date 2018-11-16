/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_SICD_BYTE_PROVIDER_H__
#define __SIX_SICD_BYTE_PROVIDER_H__

#include <six/ByteProvider.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
/*!
 * \class SICDByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the SICD pixel data and are all trying to write out a single file; in that
 * scenario, this class provides all the raw bytes corresponding to the caller's
 * AOI, including NITF headers if necessary.  The caller does not need to
 * understand anything about the NITF file layout in order to write out the
 * file.  The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to not perform any
 * copies.
 */
class SICDByteProvider : public six::ByteProvider
{
public:
    /*!
     * Constructor
     *
     * \param data Representation of the complex data
     * \param schemPaths Directories or files of schema locations
     * \param maxProductSize The max number of bytes in an image segment.
     * By default this is set automatically for you based on NITF file rules.
     */
    SICDByteProvider(const ComplexData& data,
                     const std::vector<std::string>& schemaPaths,
                     size_t maxProductSize = 0);

    /*!
     * Constructor
     * This option allows you to pass in an initialized writer,
     * in case you need something specific in the header
     *
     * \param writer Initialized NITFWriteControl
     * \param schemaPaths Directories or files of schema locations
     */
    SICDByteProvider(const NITFWriteControl& writer,
                     const std::vector<std::string>& schemaPaths);

    /*!
     * Constructor
     * This option allows you to pass in an initialized writer and
     * additional data extension segment (DES) buffers for the NITF
     *
     * \param writer Initialized NITFWriteControl
     * \param schemaPaths Directories or files of schema locations
     * \param desBuffers Buffers of NITF DES information
     */
    SICDByteProvider(const NITFWriteControl& writer,
                     const std::vector<std::string>& schemaPaths,
                     const std::vector<PtrAndLength>& desBuffers);
};
}
}

#endif
