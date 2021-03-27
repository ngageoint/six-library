/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <cstdio>

#include <std/filesystem>

#include <six/NITFWriteControl.h>
#include <six/Types.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <io/TempFile.h>

namespace fs = std::filesystem;

namespace
{

void validateArguments(int argc, char** argv)
{
    if (argc != 2)
    {
        std::string message = "Usage: " + fs::path(argv[0]).filename().string()
                + " <XML pathname>";
        throw except::Exception(Ctxt(message));
    }
    if (!str::endsWith(argv[1], ".xml"))
    {
        std::string message = "Extension suggests input may not be an XML file. "
            "Expecting '*.xml'.";
        throw except::Exception(Ctxt(message));
    }
}

std::vector<std::byte> generateBandData(const six::sicd::ComplexData& data)
{
    std::vector<std::byte> bandData(data.getNumRows() * data.getNumCols()
            * data.getNumBytesPerPixel());

    for (size_t ii = 0; ii < bandData.size(); ++ii)
    {
        bandData[ii] = static_cast<std::byte>(ii);
    }

    return bandData;
}

bool addingNullSegmentWriterShouldThrow(const std::string& xmlPathname)
{
    std::cout << "Running addingNullSegmentWriterShouldThrow\n";
    logging::Logger log;
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::parseDataFromFile(xmlPathname,
            std::vector<std::string>(),
            log);

    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::COMPLEX));
    container->addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(container);

    mem::SharedPtr<nitf::SegmentWriter> segmentWriter;
    try
    {
        writer.addAdditionalDES(segmentWriter);
        // Above line should throw
        std::cerr << "Test failed\n";
        return false;
    }
    catch (const except::Exception&)
    {
        std::cout << "Test passed\n";
        return true;
    }
}


bool addingUnloadedSegmentWriterShouldThrow(const std::string& xmlPathname)
{
    std::cout << "Running addingUnLoadedSegmentWriterShouldThrow\n";
    logging::Logger log;
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::parseDataFromFile(xmlPathname,
            std::vector<std::string>(),
            log);
    std::vector<std::byte> bandData(
        generateBandData(*data));

    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::COMPLEX));
    container->addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(container);

    nitf::Record record = writer.getRecord();
    nitf::DESegment des = record.newDataExtensionSegment();
    des.getSubheader().getFilePartType().set("DE");
    des.getSubheader().getTypeID().set("XML_DATA_CONTENT_005");
    des.getSubheader().getVersion().set("01");
    des.getSubheader().getSecurityClass().set("U");

    writer.addAdditionalDES(mem::SharedPtr<nitf::SegmentWriter>(new nitf::SegmentWriter));

    io::TempFile temp;
    try
    {
        writer.save(bandData.data(), temp.pathname());
        std::cerr << "Test failed" << std::endl;
        return false;
    }
    catch (const except::Exception&)
    {
        std::cout << "Test passed" << std::endl;
        return true;
    }
}

bool canAddProperlyLoadedSegmentWriter(const std::string& xmlPathname)
{
    std::cout << "Running canAddProperlyLoadedSegmentWriter\n";
    logging::Logger log;
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::parseDataFromFile(xmlPathname,
            std::vector<std::string>(),
            log);

    std::vector<std::byte> bandData(
        generateBandData(*data));

    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::COMPLEX));
    container->addData(dynamic_cast<six::Data*>(data.release()));

    six::NITFWriteControl writer;
    writer.initialize(container);

    nitf::Record record = writer.getRecord();
    nitf::DESegment des = record.newDataExtensionSegment();
    des.getSubheader().getFilePartType().set("DE");
    des.getSubheader().getTypeID().set("XML_DATA_CONTENT_005");
    des.getSubheader().getVersion().set("01");
    des.getSubheader().getSecurityClass().set("U");

    static const char segmentData[] = "123456789ABCDEF0";
    nitf::SegmentMemorySource sSource(segmentData, strlen(segmentData),
        0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> segmentWriter(new nitf::SegmentWriter);
    segmentWriter->attachSource(sSource);
    writer.addAdditionalDES(segmentWriter);

    io::TempFile temp;
    try
    {
        writer.save(bandData.data(), temp.pathname());
        std::cout << "Test passed" << std::endl;
        return true;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        std::cerr << "Test failed" << std::endl;
        return false;
    }
}

bool canAddTwoSegmentWriters(const std::string& xmlPathname)
{
    std::cout << "Running canAddTwoSegmentWriters\n";
    logging::Logger log;
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::parseDataFromFile(xmlPathname,
            std::vector<std::string>(),
            log);
    std::vector<std::byte> bandData(
        generateBandData(*data));

    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::COMPLEX));
    container->addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(container);

    nitf::Record record = writer.getRecord();
    nitf::DESegment desOne = record.newDataExtensionSegment();
    desOne.getSubheader().getFilePartType().set("DE");
    desOne.getSubheader().getTypeID().set("XML_DATA_CONTENT_005");
    desOne.getSubheader().getVersion().set("01");
    desOne.getSubheader().getSecurityClass().set("U");

    static const char segmentOneData[] = "123456789ABCDEF0";
    nitf::SegmentMemorySource sOneSource(segmentOneData, strlen(segmentOneData),
        0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> segmentOneWriter(new nitf::SegmentWriter);
    segmentOneWriter->attachSource(sOneSource);
    writer.addAdditionalDES(segmentOneWriter);

    nitf::DESegment desTwo = record.newDataExtensionSegment();
    desTwo.getSubheader().getFilePartType().set("DE");
    desTwo.getSubheader().getTypeID().set("XML_DATA_CONTENT_283");
    desTwo.getSubheader().getVersion().set("01");
    desTwo.getSubheader().getSecurityClass().set("U");

    static const char segmentTwoData[] = "123456789ABCDEF0";
    nitf::SegmentMemorySource sTwoSource(segmentTwoData, strlen(segmentTwoData),
        0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> segmentTwoWriter(new nitf::SegmentWriter);
    segmentTwoWriter->attachSource(sTwoSource);
    writer.addAdditionalDES(segmentTwoWriter);

    io::TempFile temp;
    try
    {
        writer.save(bandData.data(), temp.pathname());
        std::cout << "Test passed" << std::endl;
        return true;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        std::cerr << "Test failed" << std::endl;
        return false;
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        validateArguments(argc, argv);
        const std::string xmlPathname(argv[1]);

        six::XMLControlFactory::getInstance().addCreator(
            six::DataType::COMPLEX,
            new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        bool allPassed = true;
        allPassed = addingNullSegmentWriterShouldThrow(xmlPathname) && allPassed;
        allPassed = canAddProperlyLoadedSegmentWriter(xmlPathname) && allPassed;
        allPassed = addingUnloadedSegmentWriterShouldThrow(xmlPathname) && allPassed;
        allPassed = canAddTwoSegmentWriters(xmlPathname) && allPassed;

        return allPassed ? 0 : 1;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
