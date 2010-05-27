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
#include "six/Profile.h"


using namespace six;
const char StubProfile::OPT_CONTAINER_FILE_LIST[] = "FileList";
const char StubProfile::OPT_XML_FILE[] = "XMLFile";
const char StubProfile::OPT_CONTAINER_TYPE[] = "ContainerType";

Container* StubProfile::newContainer(const Options& options) 
{
    const char* inFileList =
        options.getParameter(OPT_CONTAINER_FILE_LIST);
    
    DataType containerType = (DataType)
        (int)options.getParameter(OPT_CONTAINER_TYPE);
    std::vector<std::string> files = str::Tokenizer(std::string(inFileList), 
                                                    ",");
    Container* container = new Container(containerType);
    
    Options copy = options;
    
    for (unsigned int i = 0; i < files.size(); ++i)
    {
        copy.setParameter(OPT_XML_FILE, Parameter(files[i]));
        container->addData( newData(copy) );
    }
    return container;
}

Data* StubProfile::newData(const Options& options)
{
    
    std::string inFileName =
        (std::string)options.getParameter(OPT_XML_FILE);
        
    io::FileInputStream fis(inFileName);
    
    // Create an XML parser
    xml::lite::MinidomParser xmlParser;
    
    // Parse the XML input file
    xmlParser.parse(fis);
    
    // Get the SICD DOM
    xml::lite::Document *doc = xmlParser.getDocument();
    
    DataClass dataClass = DataClass::UNKNOWN;
    xml::lite::Element* element = doc->getRootElement();
    
    if (element->getLocalName() == "SICD")
        dataClass = DataClass::COMPLEX;
    else if (element->getLocalName() == "SIDD")
        dataClass = DataClass::DERIVED;
    
    if (dataClass == DataClass::UNKNOWN)
        throw except::Exception(
            Ctxt(FmtX("Unknown file with root: <%s>", 
                      element->getLocalName().c_str()
                     )
                )
            );
    
    XMLControl* control = XMLControlFactory::getInstance().
        newXMLControl(dataClass);
    Data* data = control->fromXML(doc);
    delete control;
    return data;
}
