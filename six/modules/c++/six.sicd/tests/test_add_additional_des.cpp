/* =========================================================================
* This file is part of six.sidd-c++
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

#include <six/NITFWriteControl.h>
#include <six/Types.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/Utilities.h>


void validateArguments(int argc, char** argv)
{
    if (argc != 2)
    {
        std::string message = "Usage: " + sys::Path::basename(argv[0]) +
            " <XML pathname>";
        throw except::Exception(Ctxt(message));
    }
    if (!str::endsWith(argv[1], ".xml"))
    {
        std::string message = "Extension suggests input may not be an XML file. "
            "Expecting '*.xml'.";
        throw except::Exception(Ctxt(message));
    }
}

sys::Int16_T* generateBandData(const six::Data& data)
{
    size_t length = data.getNumRows() * data.getNumCols();
    six::UByte* bandData =
        new six::UByte[length * data.getNumBytesPerPixel()];

    for (size_t ii = 0; ii < length; ++ii)
    {
        bandData[ii] = static_cast<six::UByte>(ii);
    }

    return reinterpret_cast<sys::Int16_T*>(bandData);
}

bool addingNullSegmentWriterShouldThrow(const std::string& xmlName)
{
    std::cout << "Running addingNullSegmentWriterShouldThrow\n";
    std::auto_ptr<six::Data> data = six::sicd::Utilities::readXML(xmlName);
    six::Container container(six::DataType::COMPLEX);
    container.addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(&container);

    mem::SharedPtr<nitf::SegmentWriter> segmentWriter;
    try
    {
        writer.addAdditionalDES(segmentWriter);
        // Above line should throw
        std::cerr << "Test failed\n";
        return false;
    }
    catch (except::Exception&)
    {
        std::cout << "Test passed\n";
        return true;
    }
}

bool addingUnloadedSegmentWriterShouldThrow(const std::string& xmlName)
{
    std::cout << "Running addingUnLoadedSegmentWriterShouldThrow\n";
    std::auto_ptr<six::Data> data = six::sicd::Utilities::readXML(xmlName);
    mem::ScopedArray<sys::Int16_T> bandData(
        generateBandData(*data));

    six::Container container(six::DataType::COMPLEX);
    container.addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(&container);

    nitf::Record record = writer.getRecord();
    nitf::DESegment des = record.newDataExtensionSegment();
    des.getSubheader().getFilePartType().set("DE");
    des.getSubheader().getTypeID().set("XML_DATA_CONTENT");
    des.getSubheader().getVersion().set("01");
    des.getSubheader().getSecurityClass().set("U");

    mem::SharedPtr<nitf::SegmentWriter> segmentWriter(new nitf::SegmentWriter);
    writer.addAdditionalDES(segmentWriter);

    std::string name(std::tmpnam(NULL));
    try
    {
        writer.save(reinterpret_cast<const six::UByte*>(bandData.get()), name);
        // Actually, the "default" behavior is to segfault...
        std::cerr << "Test failed" << std::endl;
        std::remove(name.c_str());
        return false;
    }
    catch (except::Exception&)
    {
        std::cout << "Test passed" << std::endl;
        return true;
    }
}

bool canAddProperlyLoadedSegmentWriter(const std::string& xmlName)
{
    std::cout << "Running canAddProperlyLoadedSegmentWriter\n";
    std::auto_ptr<six::Data> data = six::sicd::Utilities::readXML(xmlName);
    mem::ScopedArray<sys::Int16_T> bandData(
        generateBandData(*data));

    six::Container container(six::DataType::COMPLEX);
    container.addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(&container);

    nitf::Record record = writer.getRecord();
    nitf::DESegment des = record.newDataExtensionSegment();
    des.getSubheader().getFilePartType().set("DE");
    des.getSubheader().getTypeID().set("XML_DATA_CONTENT");
    des.getSubheader().getVersion().set("01");
    des.getSubheader().getSecurityClass().set("U");

    static const char segmentData[] = "123456789ABCDEF0";
    nitf::SegmentMemorySource sSource(segmentData, strlen(segmentData),
            0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> segmentWriter(new nitf::SegmentWriter);
    segmentWriter->attachSource(sSource);
    writer.addAdditionalDES(segmentWriter);

    std::string name(std::tmpnam(NULL));
    try
    {
        writer.save(reinterpret_cast<const six::UByte*>(bandData.get()), name);
        std::cout << "Test passed" << std::endl;
        std::remove(name.c_str());
        return true;
    }
    catch (except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        std::cerr << "Test failed" << std::endl;
        return false;
    }
}

bool canAddTwoSegmentWriters(const std::string& xmlName)
{
    std::cout << "Running canAddTwoSegmentWriters\n";
    std::auto_ptr<six::Data> data = six::sicd::Utilities::readXML(xmlName);
    mem::ScopedArray<sys::Int16_T> bandData(
        generateBandData(*data));

    six::Container container(six::DataType::COMPLEX);
    container.addData(data.release());

    six::NITFWriteControl writer;
    writer.initialize(&container);

    nitf::Record record = writer.getRecord();
    nitf::DESegment desOne = record.newDataExtensionSegment();
    desOne.getSubheader().getFilePartType().set("DE");
    desOne.getSubheader().getTypeID().set("XML_DATA_CONTENT");
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
    desTwo.getSubheader().getTypeID().set("XML_DATA_CONTENT");
    desTwo.getSubheader().getVersion().set("01");
    desTwo.getSubheader().getSecurityClass().set("U");

    static const char segmentTwoData[] = "123456789ABCDEF0";
    nitf::SegmentMemorySource sTwoSource(segmentTwoData, strlen(segmentTwoData),
        0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> segmentTwoWriter(new nitf::SegmentWriter);
    segmentTwoWriter->attachSource(sTwoSource);
    writer.addAdditionalDES(segmentTwoWriter);

    std::string name(std::tmpnam(NULL));
    try
    {
        writer.save(reinterpret_cast<const six::UByte*>(bandData.get()), name);
        std::cout << "Test passed" << std::endl;
        std::remove(name.c_str());
        return true;
    }
    catch (except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        std::cerr << "Test failed" << std::endl;
        return false;
    }
}

int main(int argc, char** argv)
{
    try
    {
        validateArguments(argc, argv);
        std::string xmlName(argv[1]);

        bool allPassed = true;
        allPassed = addingNullSegmentWriterShouldThrow(xmlName) && allPassed;
        allPassed = canAddProperlyLoadedSegmentWriter(xmlName) && allPassed;
        allPassed = addingUnloadedSegmentWriterShouldThrow(xmlName) && allPassed;
        allPassed = canAddTwoSegmentWriters(xmlName) && allPassed;

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
