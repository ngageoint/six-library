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
#include <iostream>
#include <fstream>
#include <memory>
#include <logging/NullLogger.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
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

    std::vector<std::string> schemaPaths;
    if (schema.length())
    {
        schemaPaths.push_back(schema);
    }

    cphd::CPHDXMLControl xmlControl(new logging::NullLogger(), true, schemaPaths);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument());

    const std::string xmlMetadata(xmlControl.toXMLString(*metadata));
    io::FileOutputStream ofs(FILE_NAME);

    ofs.write(xmlMetadata.c_str(), xmlMetadata.size());

    xml::lite::MinidomParser xmlParser2;
    parseXMLFile(xmlParser2, FILE_NAME);

    cphd::CPHDXMLControl xmlControl2(new logging::NullLogger(), true, schemaPaths);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata2 =
            xmlControl2.fromXML(xmlParser2.getDocument());

    TEST_ASSERT_EQ(*metadata, *metadata2);
}

TEST_CASE(testWriteNotEqual)
{
    // Get pathname from cmd line
    std::string pathname = "/data1/u/vyadav/six-library/six/modules/c++/cphd/unittests/sample_xml/cphd02.xml";
    std::string schema = "/data1/u/vyadav/six-library/six/modules/c++/cphd/conf/schema/CPHD_schema_V1.0.0_2017_10_20.xsd";
    // std::string schema = "";

    xml::lite::MinidomParser xmlParser;
    parseXMLFile(xmlParser, pathname);

    std::vector<std::string> schemaPaths;
    if (schema.length())
    {
        schemaPaths.push_back(schema);
    }

    cphd::CPHDXMLControl xmlControl(new logging::NullLogger(), true, schemaPaths);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument());

    const std::string xmlMetadata(xmlControl.toXMLString(*metadata));
    io::FileOutputStream ofs(FILE_NAME);
    ofs.write(xmlMetadata.c_str(), xmlMetadata.size());

    xml::lite::MinidomParser xmlParser2;
    parseXMLFile(xmlParser2, FILE_NAME);

    cphd::CPHDXMLControl xmlControl2(new logging::NullLogger(), true, schemaPaths);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata2 =
            xmlControl2.fromXML(xmlParser2.getDocument());
    metadata2->collectionID.collectorName = "Collector2";

    TEST_ASSERT_NOT_EQ(*metadata, *metadata2);
}


int main()
{
    try
    {
        TEST_CHECK(testWrite);
        sys::OS().remove(FILE_NAME);
        TEST_CHECK(testWriteNotEqual);
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
