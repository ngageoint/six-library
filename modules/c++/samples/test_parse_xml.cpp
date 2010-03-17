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
#include <import/xml/lite.h>
#include <import/io.h>

int main(int argc, char** argv)
{

    if (argc < 3)
    {
        die_printf("Usage: %s <xml-file> <sidd|sicd> [output-xml-file]\n", argv[0]);
    }

    std::string dataType = argv[2];
    str::lower(dataType);
    if (dataType != "sicd" && dataType != "sidd")
        die_printf("Error - data type should be sicd or sidd");

    try
    {
        io::FileInputStream xmlFile(argv[1]);
        xml::lite::MinidomParser treeBuilder;
        treeBuilder.parse(xmlFile);

        six::XMLControl *control = six::XMLControlFactory::newXMLControl(
                dataType == "sicd" ? six::DATA_COMPLEX : six::DATA_DERIVED);
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
            //round-trip it
            xml::lite::Document* outDom = control->toXML(data);
            io::FileOutputStream outXML(argv[3]);
            outDom->getRootElement()->prettyPrint(outXML);
            outXML.close();
            delete outDom;
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
