/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <cphd/CPHDReader.h>

#include <std/memory>
#include <algorithm>

#include <except/Exception.h>
#include <io/StringStream.h>
#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <mem/ScopedArray.h>
#include <xml/lite/MinidomParser.h>
#include <gsl/gsl.h>
#include <sys/Path.h>

#include <six/XmlLite.h>
#include <cphd/CPHDXMLControl.h>

cphd::CPHDReader::CPHDReader(const std::string& fromFile,
                       size_t numThreads,
                       const std::vector<std::string>& schemaPaths,
                       std::shared_ptr<logging::Logger> logger)
    : CPHDReader(std::make_shared<io::FileInputStream>(fromFile), numThreads, schemaPaths, logger)
{
}

static cphd::Metadata fromXML(io::SeekableInputStream& inStream,
    const std::vector<std::string>& schemaPaths_,
    std::shared_ptr<logging::Logger> logger,
    const cphd::FileHeader& mFileHeader)
{
    // Read in the XML string
    inStream.seek(mFileHeader.getXMLBlockByteOffset(), io::Seekable::START);

    six::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(inStream, gsl::narrow<int>(mFileHeader.getXMLBlockSize()));

    if (logger.get() == nullptr)
    {
        logger = std::make_shared<logging::NullLogger>();
    }

    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    return cphd::CPHDXMLControl(logger.get()).fromXML(xmlParser.getDocument(), schemaPaths);
}
cphd::CPHDReader::CPHDReader(std::shared_ptr<io::SeekableInputStream> inStream,
                       size_t numThreads,
                       const std::vector<std::string>& schemaPaths_,
                       std::shared_ptr<logging::Logger> logger)
    : mFileHeader(cphd::FileHeader::read(*inStream)),
    mMetadata(fromXML(*inStream, schemaPaths_, logger, mFileHeader))
{
    mSupportBlock = std::make_unique<SupportBlock>(inStream, mMetadata.data, mFileHeader);

    // Load the PVPBlock into memory
    mPVPBlock = PVPBlock(mMetadata);
    mPVPBlock.load(*inStream, mFileHeader, numThreads);

    // Setup for wideband reading
    mWideband = std::make_unique<Wideband>(inStream, mMetadata,
        mFileHeader.getSignalBlockByteOffset(), mFileHeader.getSignalBlockSize());
}
