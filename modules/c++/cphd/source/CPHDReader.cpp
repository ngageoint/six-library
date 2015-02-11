/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <sys/Conf.h>
#include <except/Exception.h>
#include <io/StringStream.h>
#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <mem/ScopedArray.h>
#include <mem/SharedPtr.h>
#include <xml/lite/MinidomParser.h>
#include <cphd/CPHDReader.h>
#include <cphd/CPHDXMLControl.h>

namespace cphd
{
CPHDReader::CPHDReader(mem::SharedPtr<io::SeekableInputStream> inStream,
                       size_t numThreads,
                       mem::SharedPtr<logging::Logger> logger)
{
    initialize(inStream, numThreads, logger);
}

CPHDReader::CPHDReader(const std::string& fromFile,
                       size_t numThreads,
                       mem::SharedPtr<logging::Logger> logger)
{
    initialize(mem::SharedPtr<io::SeekableInputStream>(
        new io::FileInputStream(fromFile)), numThreads, logger);
}

void CPHDReader::initialize(mem::SharedPtr<io::SeekableInputStream> inStream,
                            size_t numThreads,
                            mem::SharedPtr<logging::Logger> logger)
{
    mFileHeader.read(*inStream);

    // Read in the XML string
    const size_t xmlSize = mFileHeader.getXMLsize();
    inStream->seek(mFileHeader.getXMLoffset(), io::Seekable::START);

    xml::lite::MinidomParser xmlParser;
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(*inStream, xmlSize);

    if (logger.get() == NULL)
    {
        logger.reset(new logging::NullLogger());
    }

    mMetadata = CPHDXMLControl(logger.get()).fromXML(xmlParser.getDocument());

    // Load the VBP into memory
    mVBM.reset(new VBM(mMetadata->data, mMetadata->vectorParameters));
    mVBM->load(*inStream,
               mFileHeader.getVBMoffset(),
               mFileHeader.getVBMsize(),
               numThreads);

    // Setup for wideband reading
    mWideband.reset(new Wideband(inStream, mMetadata->data,
                                 mFileHeader.getCPHDoffset(),
                                 mFileHeader.getCPHDsize()));
}
}
