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
#ifdef WIN32
#   include <shellapi.h>
    void preview(std::string outputFile)
    {        
        ShellExecuteA(NULL, "open", outputFile.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    }
#   else
    void preview(std::string outputFile)
    {
        std::cerr << outputFile << " was created, but no preview is available" << std::endl;
    }
#endif

int main(int argc, char** argv)
{

    if (argc < 3)
    {
        die_printf("Usage: %s <xml-file> <sidd|sicd> [output-xml-file]\n", argv[0]);
    }

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


    std::string inputFile = argv[1];
    std::string dataType = argv[2];
    
    str::lower(dataType);
    if (dataType != "sicd" && dataType != "sidd")
        die_printf("Error - data type should be sicd or sidd");

    try
    {
        io::FileInputStream xmlFile(inputFile);
        xml::lite::MinidomParser treeBuilder;
        treeBuilder.parse(xmlFile);

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
