/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_xml_lite_XMLReaderInterface_h_INCLUDED_
#define CODA_OSS_xml_lite_XMLReaderInterface_h_INCLUDED_

#include <typeinfo>

#include <config/Exports.h>

#include "XMLException.h"

namespace xml
{
namespace lite
{
class CODA_OSS_API XMLReaderInterface : public io::OutputStream
{
public:

    //!  Constructor
    XMLReaderInterface()
    {
    }

    XMLReaderInterface(const XMLReaderInterface&) = delete;
    XMLReaderInterface& operator=(const XMLReaderInterface&) = delete;

    //!  Destructor
    virtual ~XMLReaderInterface()
    {
    }

    /*!
     *  Returns the xml::lite::ContentHandler
     *  \return The ContentHandler
     */
    virtual xml::lite::ContentHandler* getContentHandler() = 0;

    /*!
     *  Set the internal xml::lite::ContentHandler
     *  \param newHandler The ContentHandler to set
     */
    virtual void setContentHandler(xml::lite::ContentHandler* newHandler) = 0;

    /*!
     *  Parse the input stream
     *  \param is The input stream to read from
     *  \param size The number of bytes to read
     */
    void parse(io::InputStream & is, int size = io::InputStream::IS_END);

    //! Method to create an xml reader
    virtual void create() = 0;

    //! Method to destroy an xml reader
    virtual void destroy() = 0;

    virtual std::string getDriverName() const = 0;
};
}
}

#endif  // CODA_OSS_xml_lite_XMLReaderInterface_h_INCLUDED_
