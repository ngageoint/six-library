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

#ifndef __XML_LITE_EXPAT_XML_READER_H__
#define __XML_LITE_EXPAT_XML_READER_H__

#include "xml/lite/xml_lite_config.h"

#if defined(USE_EXPAT)
#include <expat.h>
#include <io/OutputStream.h>
#include <io/InputStream.h>
#include "xml/lite/XMLException.h"
#include "xml/lite/ContentHandler.h"
#include "xml/lite/Attributes.h"
#include "xml/lite/NamespaceStack.h"
#include "xml/lite/XMLReaderInterface.h"


/*!
 *  \file
 *  \brief This a barebones XML implementation, based on the SAX 2.0
 *  Specification, though it differs in many ways, as well.
 *  Where the specification deals with a design that is implemented via class
 *  upon interface upon interface, we are concerned more with the performance/
 *  simplicity aspect.  Thus, underlying interfaces
 *  which could be overlooked are, and API classes that could be finalized
 *  are.  The classes are also simplified under the following assumptions.
 *
 *  1) The charset will be english only, and we are not doing utf16
 *
 *  2) The error handler has been provided so that we can use the raise
 *  macro instead
 *
 *  3) We are not supporting DTD handling right now, and right now, we
 *  are non-validating.  It is our opinion that DTDs are obsolete anyway,
 *  and should be replaced by XML schema where validation is needed.
 *
 *  4) Empty default implementations replace pure virtual here.  If you
 *  bind a standard content handler, you will get an empty implementation
 *  automatically.
 *
 *  The assertion here is that the user is a programmer interested more
 *  in speed and simplicity than a meaty implementation.  Look at Apache
 *  Xerces, MSXML or even Arabica if this is not the case.
 */

namespace xml
{
namespace lite
{
/*!
 *  \class XMLReader
 *  \brief SAX 2.0 XML Parsing class, replaces 1.0 Parser class 
 *  Provide the API for SAX-driven parsing.  We are using
 *  the Expat C Parser underneath, and wiring it to
 *  generic event calls, via the content handler.
 */
class XMLReaderExpat : public XMLReaderInterface
{

    XML_Parser mNative;
public:

    //! Constructor.  Creates a new XML parser
    XMLReaderExpat()
    {
        mNative = NULL;
        create();
    }

    //! Destructor.
    ~XMLReaderExpat()
    {
        destroy();
    }

    /*!
     *  This method returns a reference to the content handler.
     *  \return content handler
     */
    xml::lite::ContentHandler *getContentHandler()
    {
        return mContentHandler;
    }

    /*!
     *  This method should be handed a new-allocated ContentHandler.
     *  It will set this internally.
     *  \param handler  The content handler to pass
     */
    void setContentHandler(xml::lite::ContentHandler * handler)
    {
        mContentHandler = handler;
    }

    //! Finish parsing
    void finish()
    {
        parse(NULL, 0, true);
        mContentHandler->endDocument();
    }

    void parse(io::InputStream & is,
               int size = io::InputStream::IS_END);

    //! Method to create an xml reader
    void create();

    //! Method to destroy an xml reader
    void destroy();

    const char *getErrorString(enum XML_Error errorCode)
    {
#if defined (XML_UNICODE_WCHAR_T)
#  error "For some reason, we are using wide characters"
#else
        return (char *) XML_ErrorString(errorCode);
#endif
    }

    int getCurrentLineNumber()
    {
        return XML_GetCurrentLineNumber(mNative);
    }

    int getLastError()
    {
        return XML_GetErrorCode(mNative);
    }

    int getCurrentColumnNumber()
    {
        return XML_GetCurrentColumnNumber(mNative);
    }

    int getCurrentByteOffset()
    {
        return XML_GetCurrentByteIndex(mNative);
    }

    std::string getDriverName() const { return "expat"; }

private:
    //! This is how we maintain our content handler
    xml::lite::ContentHandler * mContentHandler;

    /*!
     *  This is the driver parser method
     */
    void parse(const sys::byte *data,
               int size,
               bool done);

    /*!
     *  This is the mandatory io::OutputStream.
     *  \param data The buffer
     *  \param size The buffer size
     */
    void write(const sys::byte * data,
               sys::Size_T size)
    {
        parse(data, (int)size, false);
    }

    //! Namespace mapping
    xml::lite::NamespaceStack mNamespaceStack;


    /*!
     *  Iterate through the raw attribute list and look for
     *  xmlns:prefix="uri" type mappings, and insert them
     *  on the namespace stack
     */
    void pushNamespaceContext(const char **);

    //! Remove the context
    void popNamespaceContext();

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void commentCallback(void *,
                                const char *);

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void startElementCallback(void *,
                                     const char *,
                                     const char **);

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void endElementCallback(void *,
                                   const char *);
    
    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void charactersCallback(void *,
                                   const char *,
                                   int);

    /*!
     *  Resolve the name to all of the things the content handler
     *  wants
     *  \param name      The input
     *  \param uri       The uri
     *  \param localName The local name
     *  \param qname The QName
           */
    void resolve(const char *name,
                 std::string& uri,
                 std::string& localName,
                 std::string& qname);

    
};
}
}

#endif

#endif

