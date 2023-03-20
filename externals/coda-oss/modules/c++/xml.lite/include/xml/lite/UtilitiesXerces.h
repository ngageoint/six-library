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

#ifndef CODA_OSS_xml_lite_UtilitiesXerces_h_INCLUDED_
#define CODA_OSS_xml_lite_UtilitiesXerces_h_INCLUDED_

#include <stdint.h>

#include <string>
#include <mutex>
#include <type_traits>

#include "config/compiler_extensions.h"
#include "xml/lite/xml_lite_config.h"

#if defined(USE_XERCES)

CODA_OSS_disable_warning_system_header_push
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

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>

CODA_OSS_disable_warning_pop

#include <sys/Mutex.h>
#include <mt/CriticalSection.h>
#include <except/Error.h>
#include <io/StringStream.h>
#include <io/OutputStream.h>
#include <io/InputStream.h>

#include "xml/lite/XMLException.h"
#include "xml/lite/ContentHandler.h"
#include "xml/lite/Attributes.h"
#include "xml/lite/NamespaceStack.h"
#include "xml/lite/XMLReaderInterface.h"

#if defined(XERCES_VERSION_MAJOR)
#   if XERCES_VERSION_MAJOR == 2
        typedef unsigned int XercesSize_T;
#   else
        typedef XMLSize_t XercesSize_T;
#   endif
#else
    typedef XMLSize_t XercesSize_T;
#endif


//XERCES_CPP_NAMESPACE_END
#if defined(XERCES_CPP_NAMESPACE_USE)
XERCES_CPP_NAMESPACE_USE
#endif
typedef ContentHandler XercesContentHandlerInterface_T;
typedef Attributes     XercesAttributesInterface_T;
typedef ErrorHandler   XercesErrorHandlerInterface_T;


namespace xml
{
namespace lite
{


typedef xml::lite::ContentHandler LiteContentHandler_T;
typedef xml::lite::Attributes     LiteAttributes_T;
typedef xml::lite::AttributeNode  LiteAttributesNode_T;

/*!
 *  Interface to Xerces-C XMLCh*.  This is an RAII converter
 *  between xerces-c 16bit chars to 8bit chars.
 *
 *  This class works off of the assumption that Xerces-c passes
 *  around pointers as const or non-const depending on the 
 *  ownership limitations of the memory and reacts accordingly.
 *  This object always accepts the memory internally, but for
 *  non-const it takes ownership, and for const memory (assumed
 *  to be owned elsewhere) it makes a deep copy for its own use.
 */

class XercesLocalString
{
public:

    /*!
     *  Constructor from XMLCh*
     *  Takes ownership of the memory and will clean it up
     *  \param xmlStr   An XMLCh*
     */
    XercesLocalString(XMLCh* xmlStr);

    /*!
     *  Constructor from const XMLCh*
     *  Performs a deep copy of the memory for internal use and cleanup
     *  \param xmlStr   An XMLCh*
     */
    XercesLocalString(const XMLCh* xmlStr);


    /*!
     *  Constructor from const char*
     *  \param str   A const char*
     */
    XercesLocalString(const char* str);

    /*!
     *  Constructor from std::string
     *  \param str   A std::string*
     */
    XercesLocalString(const std::string& str);


    /*!
     *  Copy Constructor
     *  Performs a deep copy of the objects internal memory
     *  \param rhs the right-hand side operand of the operation
     */
    XercesLocalString(const XercesLocalString& rhs);

    //! Destructor
    ~XercesLocalString()
    {
        try
        {
            destroyXMLCh(&mLocal);
        }
        catch(...)
        {
        }
    }

    /*!
     *  Gives back a const XMLCh*
     *    
     *  returning it as const is important so other classes
     *  know its owned by this object
     *
     *  \return          a const XMLCh*
     */
    const XMLCh* toXMLCh() const
    {
        return mLocal;
    }
    
    /*!
     *  Assign from an XMLCh*
     *  Takes ownership of the memory and will clean it up
     *  \param xmlStr    An XMLCh*
     *  \return          A reference to *this
     */
    XercesLocalString& operator=(XMLCh* xmlStr);

    /*!
     *  Assign from a const XMLCh*
     *  Performs a deep copy of the memory for internal use and cleanup
     *  \param xmlStr    An XMLCh*
     *  \return          A reference to *this
     */
    XercesLocalString& operator=(const XMLCh* xmlStr);

    /*!
     *  Assign from a XercesLocalString
     *  Performs a deep copy of the objects internal memory
     *  \param rhs    A XercesLocalString*
     *  \return       A reference to *this
     */
    XercesLocalString& operator=(const XercesLocalString& rhs);

    /*! Return a native std::string */
    std::string str() const
    {
        return toStr(mLocal);
    }

    /*!
     *  Convert from const XMLCh* to std::string
     *  \param  xmlStr    an XMLCh* string
     *  \return           a local std::string
     */
    static std::string toStr(const XMLCh* xmlStr)
    {
        char* transcodedStr = XMLString::transcode(xmlStr);
        std::string ret = transcodedStr;
        destroyChArray(&transcodedStr);
        return ret;
    }

    static void destroyXMLCh(XMLCh** a)
    {
        if (a != nullptr && *a != nullptr)
        {
            try 
            {
                XMLString::release(a);
                *a = nullptr;
            }
            catch (...)
            {
                throw except::Exception(Ctxt(
                    "XercesLocalString unsuccessful in destroying memory"));
            }
        }
    }

    static void destroyChArray(char** a)
    {
        if (a != nullptr && *a != nullptr)
        {
            try 
            {
                XMLString::release(a);
                *a = nullptr;
            }
            catch (...)
            {
                throw except::Exception(Ctxt(
                    "XercesLocalString unsuccessful in destroying memory"));
            }
        }
    }

private:

    //! The local string
    XMLCh* mLocal;
};


/*!
 *  \class XercesContentHandler
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
struct XercesContentHandler : public XercesContentHandlerInterface_T
{
    /*!
     *  Our constructor will use an underlying LiteContentHandler
     *  We will only bind to this, not free it.
     *  \param ch  The handler to bind
     */
    XercesContentHandler(xml::lite::ContentHandler* ch = nullptr)
    {
        mLiteHandler = ch;
    }

    ~XercesContentHandler()
    {}

    XercesContentHandler(const XercesContentHandler&) = delete;
    XercesContentHandler& operator=(const XercesContentHandler&) = delete;

    virtual void ignorableWhitespace(const XMLCh* const /*chars*/,
                                     const XercesSize_T /*length*/) override
    {}
    virtual void  processingInstruction(const XMLCh* const /*target*/,
                                        const XMLCh* const /*data*/) override
    {}
    virtual void  setDocumentLocator(const Locator* const /*locator*/) override
    {}

    /*!
     *  The great thing about standards compliance is that it
     *  makes our job easier.  Here we just map the inputs for
     *  Xerces to xml.lite, which is usually just a string conversion
     *  \param chars The character data
     *  \param length   The length
     */
    virtual void characters(const XMLCh* const chars,
                            const XercesSize_T length) override;

    /*!
     *  Fire off the end document notification
     */
    virtual void endDocument() override;

    /*!
     *  Map input string types to output string types
     *  and pass to the xml::lite::ContentHandler
     *  \param uri The uri for the tag
     *  \param localName The local (unprefixed name)
     *  \param qname The fully qualified name
     */
    virtual void endElement(const XMLCh* const uri,
                            const XMLCh* const localName,
                            const XMLCh* const qname) override;

    virtual void skippedEntity (const XMLCh* const /*name*/) override
    {}

    //! Fire off the start document notification
    virtual void startDocument() override;

    /*!
     *  Map input string types to output string types
     *  and pass to the xml::lite::ContentHandler.  Here
     *  we transfer the attributes to xml::lite::Attributes.
     *  \param uri The uri for the tag
     *  \param localName The local (unprefixed name)
     *  \param qname The fully qualified name
     *  \param attrs The attributes in Xerces form
     */
    virtual void startElement(const XMLCh* const uri,
                              const XMLCh* const localName,
                              const XMLCh* const qname,
                              const XercesAttributesInterface_T &attrs) override;

    /*!
     *  Begin prefix mapping.  Transfer string types
     *  \param prefix The prefix to start mapping
     *  \param uri The corresponding uri
     */
    virtual void startPrefixMapping (const XMLCh* const /*prefix*/,
                                     const XMLCh* const /*uri*/) override
    {
    }

    /*!
     *  End prefix mapping.  Transfer string types
     *  \param prefix The prefix to stop mapping
     */
    virtual void endPrefixMapping (const XMLCh* const /*prefix*/) override
    {
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
;


/*!
*  /class XercesErrorHandler
*
*  The error handler in xml::lite does is essentially non-existant
*  (and unneccessary, due to the existance of the notification single).
*  Our error handler implementation, then, simply calls the raise,
*  and warning macros in the factory.
*/
struct XercesErrorHandler final : public XercesErrorHandlerInterface_T
{
    XercesErrorHandler() = default;
    XercesErrorHandler(const XercesErrorHandler&) = delete;
    XercesErrorHandler& operator=(const XercesErrorHandler&) = delete;
    XercesErrorHandler(XercesErrorHandler&&) = delete;
    XercesErrorHandler& operator=(XercesErrorHandler&&) = delete;

    /*!
     *  Receive notification of a warning. We want to call
     *  __warning__(message);
     *  \param exception  The exception
     */
    void warning(const SAXParseException &exception) override;

    void error(const SAXParseException& exception) override;

    void fatalError(const SAXParseException& exception) override;

    // Useless??
    void resetErrors() override {}
};

/*!
 *  \class XercesContext
 *  \brief This class safely creates and destroys Xerces
 */
struct XercesContext final
{
    //! Constructor
    XercesContext();
    
    //! Destructor
    ~XercesContext();

    void destroy();
    
private:
    static std::mutex mMutex;
    bool mIsDestroyed;
};
}
}

#endif

#endif  // CODA_OSS_xml_lite_UtilitiesXerces_h_INCLUDED_
