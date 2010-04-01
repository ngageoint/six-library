/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include <import/six.h>
#include <import/six/sidd.h>
#include <import/six/sicd.h>
#include <import/xml/lite.h>
#include <import/io.h>

/*
 *  Open the round trip product in whatever you use to view XML files
 *  (e.g., Altova XMLSpy).  This only works currently on windows
 *
 */
#if defined(WIN32) && defined(PREVIEW)
#   include <shellapi.h>
    void preview(std::string outputFile)
    {        
        // ShellExecute might get assigned to ShellExecuteW if we arent careful
        ShellExecuteA(NULL, "open", outputFile.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    }
#   else
    // TODO Could open this using EDITOR or html view
    void preview(std::string outputFile)
    {
        std::cerr << "Preview unavailable for: " << outputFile << std::endl;
    }
#endif

/*
 *  Register the XMLControl handler objects for
 *  SICD and SIDD
 */
void registerHandlers()
{
    
    six::XMLControlFactory::getInstance().
        addCreator(
                six::DATA_COMPLEX, 
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>()
    );

    six::XMLControlFactory::getInstance().
        addCreator(
                six::DATA_DERIVED, 
                new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>()
    );

}
/*
 * Dump all files out to the local directory
 *
 */
std::vector<std::string> extractXML(std::string inputFile)
{
    std::vector<std::string> allFiles;
    std::string prefix = sys::Path::basename(inputFile, true);
    nitf::Reader reader;
    nitf::IOHandle io(inputFile);
    io.setAutoClose(true);
    nitf::Record record = reader.read(io);

    nitf::Uint32 numDES =  record.getNumDataExtensions();
    for (nitf::Uint32 i = 0; i < numDES; ++i)
    {
        nitf::DESegment segment = record.getDataExtensions()[i];
        nitf::DESubheader subheader = segment.getSubheader();

        nitf::SegmentReader deReader = reader.newDEReader(i);
        nitf::Off size = deReader.getSize();

        std::string typeID = subheader.getTypeID();
        str::trim(typeID);
        std::string fileName = FmtX("%s-%s%d.xml", prefix.c_str(),
                                            typeID.c_str(), i);

        char* xml = new char[size];
        deReader.read(xml, size);

        io::ByteStream bs;
        bs.write(xml, size);

        xml::lite::MinidomParser parser;
        parser.parse(bs);

        xml::lite::Document* doc = parser.getDocument();


        io::FileOutputStream fos(fileName);
        doc->getRootElement()->prettyPrint(fos);
        fos.close();
        delete [] xml;
        allFiles.push_back(fileName);
    }
    return allFiles;
}


int main(int argc, char** argv)
{

    if (argc < 3)
    {
        die_printf("Usage: %s <nitf/xml-file> <sidd|sicd> [output-xml-file]\n", argv[0]);
    }

    // The input file (an XML or a NITF file)
    std::string inputFile = argv[1];

    // Is the data type SICD or SIDD
    std::string dataType = argv[2];

    // Ignore case to be safe
    str::lower(dataType);

    // And check that everything is ok
    if (dataType != "sicd" && dataType != "sidd")
        die_printf("Error - data type should be sicd or sidd");

    // Do this prior to reading any XML
    registerHandlers();


    try
    {
        std::string xmlFile = inputFile;
        if (nitf::Reader::getNITFVersion(inputFile) != NITF_VER_UNKNOWN)
        {
            std::vector<std::string> allFiles = extractXML(inputFile);
            if (!allFiles.size())
                throw except::Exception(Ctxt(std::string("Invalid input NITF: ") + 
                                             inputFile));
            xmlFile = allFiles[0];
        }
        preview(xmlFile);
        io::FileInputStream xmlFileStream(xmlFile);
        xml::lite::MinidomParser treeBuilder;
        treeBuilder.parse(xmlFileStream);
        xmlFileStream.close();

        six::DataClass dataClass = (dataType == "sicd") ? six::DATA_COMPLEX : six::DATA_DERIVED;

        six::XMLControl *control =
            six::XMLControlFactory::getInstance().newXMLControl(dataClass);
                
        six::Data *data = control->fromXML(treeBuilder.getDocument());

        std::cout << "Data Class: " << str::toString(data->getDataClass())
                << std::endl;
        std::cout << "Pixel Type: " << str::toString(data->getPixelType())
                << std::endl;
        std::cout << "Num Rows  : " << str::toString(data->getNumRows())
                << std::endl;
        std::cout << "Num Cols  : " << str::toString(data->getNumCols())
                << std::endl;

        if (argc > 3)
        {
            std::string outputFile = argv[3];
            //round-trip it
            xml::lite::Document* outDom = control->toXML(data);
            io::FileOutputStream outXML(outputFile);
            outDom->getRootElement()->prettyPrint(outXML);
            outXML.close();
            delete outDom;
            preview(outputFile);
        }

        delete control;
        delete data;
    }
    catch (except::Exception& ex)
    {
        std::cout << "ERROR!: " << ex.getMessage() << std::endl;
    }
    return 0;
}
