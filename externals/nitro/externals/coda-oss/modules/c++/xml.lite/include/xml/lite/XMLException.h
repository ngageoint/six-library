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

#ifndef CODA_OSS_xml_lite_XMLException_h_INCLUDED_
#define CODA_OSS_xml_lite_XMLException_h_INCLUDED_

#include "except/Exception.h"

/*!
 *  \file XMLException.h
 *  \brief Contains the exceptions specific to XML
 *
 *  This file contains all of the specialized XML exceptions used by
 *  the xml::lite package
 */
namespace xml
{
namespace lite
{

/*!
 *  \class XMLException
 *  \brief The base XML exception class
 *
 *  This is the default XML exception, for when
 *  other, more specialized exception make no sense
 */
DECLARE_EXCEPTION(XML);

/*!
 *  \class XMLNotRecognizedException
 *  \brief Specialized for badly formatted/incorrectly processed data
 *
 *  Provides the derived implementation for bad formatting or
 *  for incorrect processing
 */
DECLARE_EXTENDED_EXCEPTION(XMLNotRecognized, xml::lite::XMLException);

/*!
 *  \class XMLNotSupportedException
 *  \brief You might get this if we dont support some XML feature
 *
 *  This is specifically for problems that occur with incompleteness
 *  of implementation, or with custom implementations on other
 *  systems that are not supported by the SAX/DOM standard
 *
 */
DECLARE_EXTENDED_EXCEPTION(XMLNotSupported, xml::lite::XMLException);

/*!
 *  \class XMLParseException
 *  \brief The interface for parsing exception
 *
 *  This class provides the exception interface for handling
 *  XML exception while processing documents
 *
 */
struct XMLParseException final : public XMLException
{
    /*!
     *  Construct a parse exception
     *  \param message A message as presented by the parser
     *  \param row As reported by the parser
     *  \param column As reported by the parser
     */
    explicit XMLParseException(const char *message, int row = 0, int column = 0) :
        XMLException(message)
    {
        form(row, column);
    }

    /*!
     *  Construct a parse exception
     *  \param message A message as presented by the parser
     *  \param row As reported by the parser
     *  \param column As reported by the parser
     *  \param errNum An error number given by the parser
     */
    explicit XMLParseException(const std::string & message, int row = 0, int column = 0,
            int errNum = 0) :
        XMLException(message)
    {
        form(row, column, errNum);
    }

    /*!
     *  Construct a parse exception
     *  \param c A context with a message as presented by the parser
     *  \param row As reported by the parser
     *  \param column As reported by the parser
     *  \param errNum An error number given by the parser
     */
    XMLParseException(const except::Context& c, int row = 0, int column = 0,
            int errNum = 0) :
        XMLException(c)
    {
        form(row, column, errNum);
    }

    /*!
     *  Constructor taking a Throwable and a Context
     *  \param t The Throwable
     *  \param c The exception Context
     *  \param row As reported by the parser
     *  \param column As reported by the parser
     *  \param errNum An error number given by the parser
     */
    XMLParseException(const except::Throwable& t, const except::Context& c,
            int row = 0, int column = 0, int errNum = 0) :
        XMLException(t, c)
    {
        form(row, column, errNum);
    }

    //! Destructor
    virtual ~XMLParseException() = default;

private:

    /*!
     *  Creates the actual message
     *  \param row As reported by the constructor
     *  \param col As reported by the constructor
     *
     */
    void form(int row, int column, int errNum = 0)
    {
        std::ostringstream oss;
	    
	if (errNum > 0)
	{
	    oss << "Error #" << errNum << ":";
	}
	
        oss << " (" << row << ',' << column << "): " << mMessage;
        mMessage = oss.str();
    }
};
}
}
#endif  // CODA_OSS_xml_lite_XMLException_h_INCLUDED_
