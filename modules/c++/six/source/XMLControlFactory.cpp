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

XMLControl* XMLControlRegistry::newXMLControl(DataClass dataClass)
{
    XMLControlCreator* creator = mRegistry[ dataClass ];
    if (creator == NULL)
        throw except::NoSuchKeyException(Ctxt(FmtX("No data class creator %d",
                                                   (int)dataClass )));
    return creator->newXMLControl();
}

//!  Destructor
XMLControlRegistry::~XMLControlRegistry()
{
    for (std::map<DataClass, XMLControlCreator*>::iterator p = 
             mRegistry.begin(); p != mRegistry.end(); ++p)
    {
        if (p->second)
            delete p->second;
    }
    mRegistry.clear();
    
}

XMLControl* XMLControlRegistry::newXMLControl(std::string identifier)
{
    DataClass dataClass;

    if (identifier == "SICD_XML")
    {
        dataClass = DataClass::DATA_COMPLEX;
    }
    else if (identifier == "SIDD_XML")
    {
        dataClass = DataClass::DATA_DERIVED;
    }

    return newXMLControl(dataClass);

}

char* six::toXMLCharArray(Data* data)
{
    std::string xml = toXMLString(data);
    char* raw = new char[xml.length() + 1];
    strcpy(raw, xml.c_str());
    return raw;

}
std::string six::toXMLString(Data* data)
{
    XMLControl* xmlControl = 
        XMLControlFactory::getInstance().newXMLControl(data->getDataClass());
    xml::lite::Document *doc = xmlControl->toXML(data);
    
    io::ByteStream bs;
    doc->getRootElement()->print(bs);
    delete xmlControl;
    delete doc;

    return bs.stream().str();
}
