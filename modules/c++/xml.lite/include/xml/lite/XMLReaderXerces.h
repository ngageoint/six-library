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

#ifndef __XML_LITE_XERCES_XML_READER_H__
#define __XML_LITE_XERCES_XML_READER_H__

#if defined(USE_XERCES)

#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>

#include <xercesc/util/XMLUni.hpp>

#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <string>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>

#include "io/ByteStream.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "xml/lite/XMLException.h"
#include "xml/lite/ContentHandler.h"
#include "xml/lite/Attributes.h"
#include "xml/lite/NamespaceStack.h"
#include "xml/lite/XMLReaderInterface.h"
#include "except/Error.h"

//XERCES_CPP_NAMESPACE_BEGIN
// Make sure its obvious which parser we are using

#if defined(XERCES_VERSION_MAJOR)
#   if XERCES_VERSION_MAJOR == 2
        typedef unsigned int __xmlSize_t;
#   else
        typedef XMLSize_t __xmlSize_t;
#   endif
#else
    typedef XMLSize_t __xmlSize_t;
#endif


//XERCES_CPP_NAMESPACE_END
#if defined(XERCES_CPP_NAMESPACE_USE)
XERCES_CPP_NAMESPACE_USE
#endif
typedef ContentHandler __XercesContentHandlerInterface_T;
typedef Attributes     __XercesAttributesInterface_T;
typedef ErrorHandler   __XercesErrorHandlerInterface_T;


namespace xml
{
namespace lite
{


typedef xml::lite::ContentHandler __LiteContentHandler_T;
typedef xml::lite::Attributes     __LiteAttributes_T;
typedef xml::lite::AttributeNode  __LiteAttributesNode_T;

/*!
 *  Interface to Xerces-C XMLCh*.  This is a string conversion
 *  class allowing unicode/native transparency
 */
class __XercesLocalString
{
public:

    /*!
     *  Constructor from XMLCh*
     *  \param xmlStr   An XMLCh*
     */
    __XercesLocalString(const XMLCh *xmlStr);

    /*!
     *  Constructor from c_str*
     *  \param c_str   A c_str*
     */
    __XercesLocalString(const char *c_str);

    /*!
     *  Copy Constructor
     *  \param rhs the right-hand side operand of the operation
     */
    __XercesLocalString(const __XercesLocalString& rhs);

    //! Destructor
    ~__XercesLocalString()
    { }

    /*!
     *  Convert a __XercesLocalString to an XMLCh*
     *  \return          an XMLCh*
     */
    XMLCh *toXMLCh() const;

    /*!
     *  Assign from an XMLCh*
     *  \param xmlStr    An XMLCh*
     *  \return          A reference to *this
     */
    __XercesLocalString& operator=(const XMLCh *xmlStr);

    /*!
     *  Assign from a __XercesLocalString
     *  \param rhs    A __XercesLocalString*
     *  \return       A reference to *this
     */
    __XercesLocalString& operator=(const __XercesLocalString& rhs);

    /*! Return a native c string */
    const char *c_str() const
    {
        return mLocal.c_str();
    }

    /*! Return a native std::string */
    std::string str()
    {
        return mLocal;
    }

protected:

    /*!
     *  Convert a string from XMLCh* to std::string
     *  \param  xmlStr    an XMLCh* string
     *  \return           a local std::string
     */
    std::string toLocal(const XMLCh *xmlStr);

    //! The local string
    std::string mLocal;
};


/*!
 *  \class __XercesContentHandler
 *  \brief This class is a mapping between Xerces SAX2.0 and xml.lite
 *
 *  With Expat, a C parser, we had to bind function pointers to
 *  C subroutines, and use those to drive our xml.lite ContentHandler.
 *  Here, we are starting with an already SAX2.0 compliant parser,
 *  so our work is very simple.  We want to map the fully compliant
 *  parser to our subset, in a fashion that allows the xml.lite
 *  interface to remain constant.
 *
 *  Recall that xml.lite is based on a subset of SAX2.0 and DOM calls.
 *  Under Expat, we get significant performance and simplicity gains,
 *  by making the parser start with a defined base handler for
 *  everything.  But having Xerces is beneficial as well, particularly
 *  if you need more conformance.  While Expat is the world's fastest
 *  XML parser, and should be used wherever possible, Apache has
 *  possibly the most standards conformant implementation.  We want
 *  to allow (in a performance tradeoff) the possibility for
 *  Xerces use as well as Expat (and ultimately MSXML as well)
 *
 */
class __XercesContentHandler :
            public __XercesContentHandlerInterface_T
{
public:
    /*!
     *  Our constructor will use an underlying __LiteContentHandler
     *  We will only bind to this, not free it.
     *  \param ch  The handler to bind
     */
    __XercesContentHandler(xml::lite::ContentHandler* ch = NULL)
    {
        mLiteHandler = ch;
    }

    ~__XercesContentHandler()
    {}

    virtual void ignorableWhitespace(const XMLCh *const chars,
                                     const __xmlSize_t length)
    {}
    virtual void  processingInstruction(const XMLCh *const target,
                                        const XMLCh *const data)
    {}
    virtual void  setDocumentLocator(const Locator *const locator)
    {}

    /*!
     *  The great thing about standards compliance is that it
     *  makes our job easier.  Here we just map the inputs for
     *  Xerces to xml.lite, which is usually just a string conversion
     *  \param chars The character data
     *  \param length   The length
     */
    virtual void characters(const XMLCh* const chars,
                            const __xmlSize_t length);

    /*!
     *  Fire off the end document notification
     */
    virtual void endDocument();

    /*!
     *  Map input string types to output string types
     *  and pass to the xml::lite::ContentHandler
     *  \param uri The uri for the tag
     *  \param localName The local (unprefixed name)
     *  \param qname The fully qualified name
     */
    virtual void endElement(const XMLCh *const uri,
                            const XMLCh *const localName,
                            const XMLCh *const qname);

    virtual void skippedEntity (const XMLCh *const name)
    {}

    //! Fire off the start document notification
    virtual void startDocument();

    /*!
     *  Map input string types to output string types
     *  and pass to the xml::lite::ContentHandler.  Here
     *  we transfer the attributes to xml::lite::Attributes.
     *  \param uri The uri for the tag
     *  \param localName The local (unprefixed name)
     *  \param qname The fully qualified name
     *  \param attrs The attributes in Xerces form
     */
    virtual void startElement(const XMLCh *const uri,
                              const XMLCh *const localName,
                              const XMLCh *const qname,
                              const
                              __XercesAttributesInterface_T &attrs);

    /*!
     *  Begin prefix mapping.  Transfer string types
     *  \param prefix The prefix to start mapping
     *  \param uri The corresponding uri
     */
    virtual void startPrefixMapping (const XMLCh *const prefix,
                                     const XMLCh *const uri)
    {
        /*
        __XercesLocalString __prefix(prefix);
        __XercesLocalString __uri(uri);
        mLiteHandler->startPrefixMapping(__prefix.str(),
        __uri.str()
        );
        */
    }

    /*!
     *  End prefix mapping.  Transfer string types
     *  \param prefix The prefix to stop mapping
     */
    virtual void endPrefixMapping (const XMLCh *const prefix)
    {
        /*
        __XercesLocalString __prefix(prefix);
        mLiteHandler->endPrefixMapping(__prefix.str());
        */
    }

    virtual void
    setXMLLiteContentHandler(xml::lite::ContentHandler* handler)
    {
        mLiteHandler = handler;
    }

    virtual xml::lite::ContentHandler*
    retrieveXMLLiteContentHandler()
    {
        return mLiteHandler;
    }

protected:
    xml::lite::ContentHandler* mLiteHandler;

}
; // That's all for the ContentHandler, folks!


/*!
*  /class __XercesErrorHandler
*
*  The error handler in xml::lite does is essentially non-existant
*  (and unneccessary, due to the existance of the notification single).
*  Our error handler implementation, then, simply calls the raise,
*  and warning macros in the factory.
*/
class __XercesErrorHandler :
            public __XercesErrorHandlerInterface_T
{
public:
    /*!
     *  Receive notification of a warning. We want to call
     *  __warning__(message);
     *  \param exception  The exception
     */
    virtual void warning(const SAXParseException &exception);

    virtual void error (const SAXParseException &exception);

    virtual void fatalError (const SAXParseException &exception);

    // Useless??
    virtual void resetErrors ()
    {}
};

/*!
 *  \class XMLReaderXerces
 *  \brief SAX 2.0 XML Parsing class, replaces 1.0 Parser class
 *
 *  Provide the API for SAX-driven parsing.  We are using
 *  the Expat C Parser underneath, and wiring it to
 *  generic event calls, via the content handler.
 */
class XMLReaderXerces : public XMLReaderInterface<SAX2XMLReader*>
{
public:

    //! Constructor.  Creates a new XML parser
    XMLReaderXerces();

    //! Destructor.
    ~XMLReaderXerces()
    {
        destroy();
    }

    static const char* MEM_BUFFER_ID()
    {
        return "XMLReaderXerces";
    }

    /*!
     *  This method returns a reference to the content handler.
     *  \return content handler
     */
    xml::lite::ContentHandler *getContentHandler()
    {
        return mDriverContentHandler.retrieveXMLLiteContentHandler();
    }

    virtual void setValidation(bool validate);
    virtual bool getValidation();

    /*!
     *  This method should be handed a new-allocated ContentHandler.
     *  It will set this internally.
     *  \param handler  The content handler to pass
     */
    void setContentHandler(xml::lite::ContentHandler * handler)
    {
        mDriverContentHandler.setXMLLiteContentHandler(handler);
    }

    void parse(io::InputStream& is, int size = io::InputStream::IS_END);
    //! Method to create an xml reader
    void create();

    //! Method to destroy an xml reader
    void destroy();

private:

    void write(const sys::byte *b, sys::Size_T len)
    {
        throw xml::lite::XMLException(Ctxt("Im not sure how you got here!"));
    }

    //! Underlying parser implementation (Currently expat);
    __XercesContentHandler mDriverContentHandler;
    __XercesErrorHandler   mErrorHandler;
};

}
}

//! Overloaded output operator for __XercesLocalString
std::ostream& operator<<(std::ostream& os,
                         const xml::lite::__XercesLocalString& ls);


#endif

#endif
