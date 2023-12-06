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
    std::vector<std::byte> bandData(getExtent(data).area() * data.getNumBytesPerPixel());

    for (size_t ii = 0; ii < bandData.size(); ++ii)
    {
        bandData[ii] = static_cast<std::byte>(ii);
    }

    return bandData;
}

std::unique_ptr<io::TempFile> createNITFFromXML(const std::string& xmlPathname)
{
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
    des.getSubheader().getTypeID().set("XML_DATA_CONTENT");
    des.getSubheader().getVersion().set("01");
    des.getSubheader().getSecurityClass().set("U");

    static const char segmentData[] = "123456789ABCDEF0";
    nitf::SegmentMemorySource sSource(segmentData, strlen(segmentData),
            0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> segmentWriter(new nitf::SegmentWriter);
    segmentWriter->attachSource(sSource);
    writer.addAdditionalDES(segmentWriter);

    //Wrong tag
    nitf::TRE usrHdr("PIAIMB", "PIAIMB");
    record.getHeader().getUserDefinedSection().appendTRE(usrHdr);

    //Good tag, wrong DESSHSI
    nitf::TRE secondHdr(six::Constants::DES_USER_DEFINED_SUBHEADER_TAG,
            six::Constants::DES_USER_DEFINED_SUBHEADER_ID);
    secondHdr.setField("DESCRC", "99999");
    secondHdr.setField("DESSHFT", "XML00000");
    secondHdr.setField("DESSHDT", "2016-06-01T00:00:00Z");
    secondHdr.setField("DESSHRP", "1234567890123456789012345678901234567890");
    secondHdr.setField("DESSHSI", std::string(" ", 60));
    secondHdr.setField("DESSHSV", "Version 10");
    secondHdr.setField("DESSHSD", "2016-06-01T00:00:00Z");
    secondHdr.setField("DESSHTN", std::string(" ", 120));
    secondHdr.setField("DESSHLPG", std::string("1", 125));
    secondHdr.setField("DESSHLPT", std::string(" ", 20));
    secondHdr.setField("DESSHLI", std::string(" ", 20));
    secondHdr.setField("DESSHLIN", std::string(" ", 120));
    secondHdr.setField("DESSHABS", std::string(" ", 200));
    des.getSubheader().setSubheaderFields(secondHdr);

    //Wrong length
    nitf::TRE middleTRE(six::Constants::DES_USER_DEFINED_SUBHEADER_TAG,
            "XML_DATA_CONTENT_283");
    middleTRE.setField("DESCRC", "99999");
    middleTRE.setField("DESSHFT", "XML00000");
    middleTRE.setField("DESSHDT", "2016-06-01T00:00:00Z");
    middleTRE.setField("DESSHRP", "1234567890123456789012345678901234567890");
    middleTRE.setField("DESSHSI", std::string(" ", 60));
    middleTRE.setField("DESSHSV", "Version 10");
    middleTRE.setField("DESSHSD", "2016-06-01T00:00:00Z");
    middleTRE.setField("DESSHTN", std::string(" ", 120));

    nitf::DESegment middleDES = record.newDataExtensionSegment();
    middleDES.getSubheader().getFilePartType().set("DE");
    middleDES.getSubheader().getTypeID().set("XML_DATA_CONTENT");
    middleDES.getSubheader().getVersion().set("01");
    middleDES.getSubheader().getSecurityClass().set("U");

    nitf::SegmentMemorySource middleSource(segmentData, strlen(segmentData),
            0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> middleSegmentWriter(new nitf::SegmentWriter);
    middleSegmentWriter->attachSource(middleSource);
    writer.addAdditionalDES(middleSegmentWriter);


    //Wrong length again
    nitf::TRE shortTRE(six::Constants::DES_USER_DEFINED_SUBHEADER_TAG,
            "XML_DATA_CONTENT_005");
    shortTRE.setField("DESCRC", "99999");

    nitf::DESegment shortDES = record.newDataExtensionSegment();
    shortDES.getSubheader().getFilePartType().set("DE");
    shortDES.getSubheader().getTypeID().set("XML_DATA_CONTENT");
    shortDES.getSubheader().getVersion().set("01");
    shortDES.getSubheader().getSecurityClass().set("U");

    nitf::SegmentMemorySource shortSource(segmentData, strlen(segmentData),
            0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> shortSegmentWriter(new nitf::SegmentWriter);
    shortSegmentWriter->attachSource(shortSource);
    writer.addAdditionalDES(shortSegmentWriter);

    std::unique_ptr<io::TempFile> temp(new io::TempFile());
    writer.save(bandData.data(), temp->pathname());
    return temp;
}
}


int main(int argc, char** argv)
{
    try
    {
        validateArguments(argc, argv);
        const std::string xmlPathname(argv[1]);

        six::getXMLControlFactory().addCreator<six::sicd::ComplexXMLControl>();

        std::unique_ptr<io::TempFile> nitf = createNITFFromXML(xmlPathname);
        six::NITFReadControl reader;
        reader.load(nitf->pathname());

        // Make sure ComplexData got read in
        logging::Logger log;
        std::unique_ptr<six::sicd::ComplexData> originalData =
                six::sicd::Utilities::parseDataFromFile(xmlPathname,
                std::vector<std::string>(),
                log);

        auto container = reader.getContainer();
        // container retains ownership of this pointer
        const six::Data* readData = container->getData(0);

        if (*readData == *originalData)
        {
            std::cout << "Test passed\n";
            return 0;
        }
        else
        {
            std::cerr << "Round-tripped data not the same as original."
                    << "Test failed.\n";
            return 1;
        }
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
