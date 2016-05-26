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


#include <six/NITFWriteControl.h>
#include <six/Types.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/Utilities.h>

namespace
{
void validateArguments(int argc, char** argv)
{
    if (argc != 2)
    {
        std::string message = "Usage: " + sys::Path::basename(argv[0])
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

sys::Int16_T* generateBandData(const six::sicd::ComplexData& data)
{
    std::cerr << "1" << std::endl;
    size_t length = data.getNumRows() * data.getNumCols();
    std::cerr << "2" << std::endl;
    six::UByte* bandData =
        new six::UByte[length * data.getNumBytesPerPixel()];
    std::cerr << 2 << std::endl;
    for (size_t ii = 0; ii < length; ++ii)
    {
        bandData[ii] = static_cast<six::UByte>(ii);
    }

    return reinterpret_cast<sys::Int16_T*>(bandData);
}
/*
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
    catch (const except::Exception&)
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
    des.getSubheader().getTypeID().set("XML_DATA_CONTENT_005");
    des.getSubheader().getVersion().set("01");
    des.getSubheader().getSecurityClass().set("U");

    mem::SharedPtr<nitf::SegmentWriter> segmentWriter(new nitf::SegmentWriter);
    writer.addAdditionalDES(segmentWriter);

    std::string name(std::tmpnam(NULL));
    try
    {
        writer.save(reinterpret_cast<const six::UByte*>(bandData.get()), name);
        std::cerr << "Test failed" << std::endl;
        std::remove(name.c_str());
        return false;
    }
    catch (const except::Exception&)
    {
        std::cout << "Test passed" << std::endl;
        return true;
    }
}
*/
bool canAddProperlyLoadedSegmentWriter(const std::string& xmlName)
{
    std::cout << "Running canAddProperlyLoadedSegmentWriter\n";
    std::auto_ptr<six::sicd::ComplexData> data = six::sicd::Utilities::readXML(xmlName);
    std::cerr << data->imageData->numRows << std::endl;
    std::cerr << data->getNumRows() << std::endl;
    mem::ScopedArray<sys::Int16_T> bandData(
        generateBandData(*data));

    std::cerr << "Creating container" << std::endl;
    six::Container container(six::DataType::COMPLEX);
    container.addData(dynamic_cast<six::Data*>(data.get()));
    //data.reset(); //Container takes ownership
    std::cerr << "Data added" << std::endl;

    six::NITFWriteControl writer;
    writer.initialize(&container);

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

    std::string name(std::tmpnam(NULL));
    try
    {
        writer.save(reinterpret_cast<const six::UByte*>(bandData.get()), name);
        std::cout << "Test passed" << std::endl;
        std::remove(name.c_str());
        return true;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        std::cerr << "Test failed" << std::endl;
        return false;
    }
}
/*
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

    std::string name(std::tmpnam(NULL));
    try
    {
        writer.save(reinterpret_cast<const six::UByte*>(bandData.get()), name);
        std::cout << "Test passed" << std::endl;
        std::remove(name.c_str());
        return true;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        std::cerr << "Test failed" << std::endl;
        return false;
    }
}
*/
}

int main(int argc, char** argv)
{
    try
    {
        validateArguments(argc, argv);
        const std::string xmlName(argv[1]);

        bool allPassed = true;
        //allPassed = addingNullSegmentWriterShouldThrow(xmlName) && allPassed;
        allPassed = canAddProperlyLoadedSegmentWriter(xmlName) && allPassed;
        //allPassed = addingUnloadedSegmentWriterShouldThrow(xmlName) && allPassed;
        //allPassed = canAddTwoSegmentWriters(xmlName) && allPassed;

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
