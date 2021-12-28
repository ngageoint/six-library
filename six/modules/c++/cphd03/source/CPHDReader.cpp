/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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
#include <cphd03/CPHDReader.h>

#include <scene/sys_Conf.h>
#include <except/Exception.h>
#include <io/StringStream.h>
#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <mem/ScopedArray.h>
#include <xml/lite/MinidomParser.h>
#include <cphd03/CPHDXMLControl.h>
#include <six/XmlLite.h>

namespace cphd03
{
CPHDReader::CPHDReader(std::shared_ptr<io::SeekableInputStream> inStream,
                       size_t numThreads,
                       std::shared_ptr<logging::Logger> logger)
{
    initialize(inStream, numThreads, logger);
}

CPHDReader::CPHDReader(const std::string& fromFile,
                       size_t numThreads,
                       std::shared_ptr<logging::Logger> logger)
{
    initialize(std::make_shared<io::FileInputStream>(fromFile),
        numThreads, logger);
}

void CPHDReader::initialize(std::shared_ptr<io::SeekableInputStream> inStream,
                            size_t numThreads,
                            std::shared_ptr<logging::Logger> logger)
{
    mFileHeader.read(*inStream);

    // Read in the XML string
    const auto xmlSize = static_cast<int>(mFileHeader.getXMLsize());
    inStream->seek(mFileHeader.getXMLoffset(), io::Seekable::START);

    six::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(*inStream, xmlSize);

    if (logger.get() == nullptr)
    {
        logger = std::make_shared<logging::NullLogger>();
    }

    mMetadata = CPHDXMLControl(logger.get()).fromXML(xmlParser.getDocument());

    // Load the VBP into memory
    mVBM = VBM(mMetadata);
    mVBM.load(*inStream, mFileHeader, numThreads);

    // Setup for wideband reading
    mWideband = std::make_unique<cphd::Wideband>(inStream, mMetadata,
                                 mFileHeader.getCPHDoffset(), mFileHeader.getCPHDsize());
}
}
