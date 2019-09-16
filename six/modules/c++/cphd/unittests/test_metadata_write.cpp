#include <iostream>
#include <fstream>
#include <memory>

#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>
#include <io/MockSeekableInputStream.h>
#include <xml/lite/MinidomParser.h>

#include "TestCase.h"

static const std::string FILE_NAME("temp.cphd");

void parseXMLFile(xml::lite::MinidomParser& xmlParser, std::string pathname)
{
    // Parse XML file
    io::FileInputStream ifs(pathname);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(ifs, ifs.available());
}

TEST_CASE(testWrite)
{
    // Get pathname from cmd line
    std::string pathname = "/data1/u/vyadav/six-library/six/modules/c++/cphd/unittests/sample_xml/cphd02.xml";
    std::string schema = "/data1/u/vyadav/six-library/six/modules/c++/cphd/conf/schema/CPHD_schema_V1.0.0_2017_10_20.xsd";
    // std::string schema = "";

    xml::lite::MinidomParser xmlParser;
    parseXMLFile(xmlParser, pathname);

    cphd::CPHDXMLControl xmlControl;
    std::vector<std::string> schemaPaths;
    if (schema.length())
    {
        schemaPaths.push_back(schema);
        xmlControl.setSchemaPaths(schemaPaths);
    }

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument());

    const std::string xmlMetadata(xmlControl.toXMLString(*metadata));
    io::FileOutputStream ofs(FILE_NAME);
    // FileHeader header;
    // // set header size, final step before write
    // header.set(xmlMetadata.size(), vbmSize, cphd03Size);
    // mFile.write(header.toString().c_str(), header.size());
    ofs.write(xmlMetadata.c_str(), xmlMetadata.size());

    xml::lite::MinidomParser xmlParser2;
    parseXMLFile(xmlParser2, FILE_NAME);

    cphd::CPHDXMLControl xmlControl2;
    xmlControl2.setSchemaPaths(schemaPaths);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata2 =
            xmlControl2.fromXML(xmlParser2.getDocument());

    TEST_ASSERT_EQ(*metadata, *metadata2);
}

int main()
{
    try
    {
        TEST_CHECK(testWrite);
        sys::OS().remove(FILE_NAME);
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    sys::OS().remove(FILE_NAME);
    return 1;

}
