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
#ifndef __SIX_XML_CONTROL_FACTORY_H__
#define __SIX_XML_CONTROL_FACTORY_H__

#include "six/XMLControl.h"
#include "six/ComplexXMLControl.h"
#include "six/DerivedXMLControl.h"

namespace six
{

/*!
 *  \class XMLControlFactory
 *  \brief Factory design pattern for generating a new XML reader/writer
 *
 *  This class generates the proper reader/writer for the Data content.
 *  There are two methods currently, one that uses the DataClass to
 *  identify which reader/writer to create, and one that uses a
 *  string (the same one that identifies the type in the container)
 */
class XMLControlFactory
{
public:
    //!  Constructor
    XMLControlFactory()
    {
    }
    //!  Destructor
    virtual ~XMLControlFactory()
    {
    }

    static XMLControl* newXMLControl(DataClass dataClass);

    /*!
     *  Static method to create a new XMLControl from a string.  
     *  The only currently valid strings are "SICD_XML" and "SIDD_XML"
     *  The created control must be deleted by the caller.
     *
     *  \param identifier what type of XML control to create.
     *  \return XMLControl the produced XML bridge
     *
     */
    static XMLControl* newXMLControl(std::string identifier);

};

/*!
 *  Method to convert from a ComplexData or DerivedData into a C-style
 *  array containing XML.  It is up to the caller to delete this array.
 * 
 *  \param a ComplexData or DerivedData object
 *  \return A new allocated string containing the XML representation of the
 *  data
 */
char* toXMLCharArray(Data* data);

/*!
 *  Convenience method to convert from a ComplexData or DerivedData
 *  into a C++ style string containing XML.
 *
 *  \param a ComplexData or DerivedData object
 *  \return A C++ string object containing the XML
 *
 */
std::string toXMLString(Data* data);


}

#endif
