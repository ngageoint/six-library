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
#include "six/XMLControlFactory.h"

using namespace six;

six::XMLControl* six::XMLControlFactory::newXMLControl(DataClass dataClass)
 {
     switch (dataClass)
     {
     case six::DATA_COMPLEX:
         return new six::ComplexXMLControl();
     case six::DATA_DERIVED:
         return new six::DerivedXMLControl();
     default:
         throw except::Exception("Invalid data class");
     }
 }

six::XMLControl* six::XMLControlFactory::newXMLControl(std::string identifier)
{
    if (identifier == "SICD_XML")
    {
        return new six::ComplexXMLControl();
    }
    else
    {
        return new six::DerivedXMLControl();
    }
}

char* six::toXMLCharArray(Data* data)
{
    std::string xml = six::toXMLString(data);
    char* raw = new char[xml.length() + 1];
    strcpy(raw, xml.c_str());
    return raw;

}
std::string six::toXMLString(Data* data)
{
    six::XMLControl* xmlControl = 
        six::XMLControlFactory::newXMLControl(data->getDataClass());
    xml::lite::Document *doc = xmlControl->toXML(data);
    
    io::ByteStream bs;
    doc->getRootElement()->print(bs);
    delete xmlControl;
    delete doc;

    return bs.stream().str();
}
