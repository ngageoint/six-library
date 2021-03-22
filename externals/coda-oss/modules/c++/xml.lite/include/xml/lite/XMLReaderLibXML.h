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

#ifndef __XML_LITE_LIB_XML_READER_H__
#define __XML_LITE_LIB_XML_READER_H__

#include "xml/lite/xml_lite_config.h"

#if defined(USE_LIBXML)

#include <libxml/parser.h>
#include <io/OutputStream.h>
#include <io/InputStream.h>
#include "xml/lite/XMLException.h"
#include "xml/lite/ContentHandler.h"
#include "xml/lite/Attributes.h"
#include "xml/lite/NamespaceStack.h"
#include "xml/lite/XMLReaderInterface.h"

namespace xml
{
namespace lite
{

class XMLReaderLibXML : public XMLReaderInterface
{
    
    xmlSAXHandler  mSAXLibXML;
    xmlParserCtxt* mContextLibXML;
    
public:

    //! Constructor.  Creates a new XML parser
    XMLReaderLibXML()
    {
        create();
    }

    //! Destructor.
    ~XMLReaderLibXML()
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
        parse(nullptr, 0, true);
        mContentHandler->endDocument();
    }

    void parse(io::InputStream & is,
               int size = io::InputStream::IS_END);

    //! Method to create an xml reader
    void create();

    //! Method to destroy an xml reader
    void destroy();

    int getCurrentLineNumber()
    {

        return xmlSAX2GetLineNumber(mContextLibXML);
    }

    int getCurrentColumnNumber()
    {
        return xmlSAX2GetColumnNumber(mContextLibXML);
    }

    std::string getDriverName() const { return "libxml"; }

private:
    //! This is how we maintain our content handler
    xml::lite::ContentHandler* mContentHandler;

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
    virtual void write(const void* data, size_t size)
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
    void pushNamespaceContext(const xmlChar **);

    //! Remove the context
    void popNamespaceContext();


    static void warningCallback(void *,
                                const char*, ...);
        
    static void errorCallback(void *,
                              const char*, ...);

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void commentCallback(void *,
                                const xmlChar *);

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void startElementCallback(void *,
                                     const xmlChar *,
                                     const xmlChar **);

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void endElementCallback(void *,
                                   const xmlChar *);

    /*!
     *  Take the raw input, and resolve it to something our
     *  SAX 2.0 content handler knows about, and fire it
     */
    static void charactersCallback(void *,
                                   const xmlChar *,
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
                 std::string & uri,
                 std::string & localName,
                 std::string & qname);

    

};
}
}

#endif

#endif

