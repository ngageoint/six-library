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

#  define TEST_CASE_ARGUMENTS(X) void X(std::string testName, int argc, char** argv)
#  define TEST_CHECK_ARGUMENTS(X) try{ X(std::string(#X),argc,argv); std::cerr << #X << ": PASSED" << std::endl; } catch(except::Throwable& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.toString().c_str()); }

std::string getPathname(int argc, char** argv)
{
    // Parse the command line
    if (argc != 2)
    {
        throw except::Exception(Ctxt(
                    "Invalid number of cmd line args"));
    }
    else
    {
        return argv[1];
    }
}

void parseXMLFile(xml::lite::MinidomParser& xmlParser, std::string pathname)
{
    // Parse XML file
    io::FileInputStream ifs(pathname);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(ifs, ifs.available());
}

TEST_CASE_ARGUMENTS(testRoundTrip)
{
    // Get pathname from cmd line
    std::string pathname = "/data1/u/vyadav/six-library/six/modules/c++/cphd/unittests/sample_xml/cphd02.xml";
    std::string pathnameOut = "/data1/u/vyadav/six-library/six/modules/c++/cphd/unittests/sample_xml/cphdRes.xml";

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

    // Select specific nodes to be populated
    std::vector<std::string> nodeNames;
    nodeNames.push_back("CollectionID");
    nodeNames.push_back("Global");
    nodeNames.push_back("SceneCoordinates");
    nodeNames.push_back("Data");
    nodeNames.push_back("Channel");
    nodeNames.push_back("PVP");
    nodeNames.push_back("SupportArray");
    nodeNames.push_back("Dwell");
    nodeNames.push_back("ReferenceGeometry");
    nodeNames.push_back("Antenna");
    nodeNames.push_back("TxRcv");
    nodeNames.push_back("ErrorParameters");
    nodeNames.push_back("ProductInfo");
    nodeNames.push_back("GeoInfo");
    nodeNames.push_back("MatchInfo");

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument(), nodeNames);


    const std::string xmlMetadata(xmlControl.toXMLString(*metadata));
    std::cout << xmlMetadata << std::endl;
    io::FileOutputStream ofs(pathnameOut);
    // FileHeader header;
    // // set header size, final step before write
    // header.set(xmlMetadata.size(), vbmSize, cphd03Size);
    // mFile.write(header.toString().c_str(), header.size());
    ofs.write(xmlMetadata.c_str(), xmlMetadata.size());

}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK_ARGUMENTS(testRoundTrip);
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
    return 1;

}
