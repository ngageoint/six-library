/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#ifndef __XML_LITE_ABSTRACT_XML_READER_H__
#define __XML_LITE_ABSTRACT_XML_READER_H__

#include <typeinfo>
#include "XMLException.h"

namespace xml
{
namespace lite
{
template<typename Native_T>
class XMLReaderInterface : public io::OutputStream
{
public:

    //!  Constructor
    XMLReaderInterface()
    {
    }

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

    /*!
     *  Get the native item back out
     *  \return The native parser
     */
    Native_T& getNative()
    {
        return mNative;
    }

    /*!
     *  Get the native item back out as content
     *  \return The native parser
     */
    const Native_T& getNative() const
    {
        return mNative;
    }

    /*!
     *  Get the native type back out as a string
     *  \return The native type
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }

    virtual void setValidation(bool validate) = 0;
    virtual bool getValidation() = 0;

protected:
    Native_T mNative;

private:
    //! Private copy constructor
    XMLReaderInterface(const XMLReaderInterface &);

    //! Private overloaded assignment operator
    XMLReaderInterface & operator=(const XMLReaderInterface &);

};
}
}

#endif
