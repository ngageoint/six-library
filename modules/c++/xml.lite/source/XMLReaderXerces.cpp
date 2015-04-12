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

#include "xml/lite/XMLReader.h"

#if defined(USE_XERCES)

xml::lite::__XercesLocalString::__XercesLocalString(const XMLCh *xmlStr)
{
    mLocal = toLocal(xmlStr);
}

xml::lite::__XercesLocalString::__XercesLocalString(const char *c_str)
{
    mLocal = c_str;
}

xml::lite::__XercesLocalString::
__XercesLocalString(const __XercesLocalString& rhs)
{
    mLocal = rhs.mLocal;
}

XMLCh *xml::lite::__XercesLocalString::toXMLCh() const
{
    return XMLString::transcode(mLocal.c_str());
}

xml::lite::__XercesLocalString& xml::lite::__XercesLocalString::
operator=(const XMLCh *xmlStr)
{
    mLocal = toLocal(xmlStr);
    return *this;
}

xml::lite::__XercesLocalString& xml::lite::__XercesLocalString::
operator=(const xml::lite::__XercesLocalString& rhs)
{
    if (this != &rhs)
    {
        mLocal = rhs.mLocal;
    }
    return *this;
}

std::string xml::lite::__XercesLocalString::toLocal(const XMLCh *xmlStr)
{
    char *localCStr = XMLString::transcode(xmlStr);
    std::string local = localCStr;
    XMLString::release(&localCStr);
    return local;
}

void xml::lite::__XercesContentHandler::characters(const XMLCh* const chars,
        const __xmlSize_t length)
{
    xml::lite::__XercesLocalString __xstr(chars);
    mLiteHandler->characters(__xstr.c_str(), (int)length);
}

void xml::lite::__XercesContentHandler::startDocument()
{
    mLiteHandler->startDocument();
}

void xml::lite::__XercesContentHandler::endDocument()
{
    mLiteHandler->endDocument();
}

void xml::lite::__XercesContentHandler::endElement(const XMLCh *const uri,
        const XMLCh *const localName,
        const XMLCh *const qname)
{
    xml::lite::__XercesLocalString __uri(uri);
    xml::lite::__XercesLocalString __localName(localName);
    xml::lite::__XercesLocalString __qname(qname);

    mLiteHandler->endElement(__uri.str(),
                             __localName.str(),
                             __qname.str());
}

void xml::lite::__XercesContentHandler::
startElement(const XMLCh *const uri,
             const XMLCh *const localName,
             const XMLCh *const qname,
             const __XercesAttributesInterface_T &attrs)

{
    // We have to copy the whole array
    __LiteAttributes_T __attributes;
    for (unsigned int i = 0; i < attrs.getLength(); i++)
    {
        __LiteAttributesNode_T __attributeNode;
        __attributeNode.setQName(
            __XercesLocalString(attrs.getQName(i)).str()
        );

        assert(__attributeNode.getLocalName() ==
               __XercesLocalString(attrs.getLocalName(i)).str()
              );

        __attributeNode.setUri(
            __XercesLocalString(attrs.getURI(i)).str()
        );

        __attributeNode.setValue(
            __XercesLocalString(attrs.getValue(i)).str()
        );

        //don't add duplicate attributes
        if (__attributes.getIndex(__attributeNode.getUri(),
                                  __attributeNode.getLocalName()) == -1)
            __attributes.add(__attributeNode);
    }

    __XercesLocalString __uri(uri);
    __XercesLocalString __localName(localName);
    __XercesLocalString __qname(qname);
    //std::cout << "__qname=" << __qname.str() << std::endl;
    mLiteHandler->startElement(__uri.str(),
                               __localName.str(),
                               __qname.str(),
                               __attributes);
}

void xml::lite::__XercesErrorHandler::
warning(const SAXParseException &exception)
{
    //__warning__(__XercesLocalString(exception.getMessage()).c_str());
}

void xml::lite::__XercesErrorHandler::
error(const SAXParseException &exception)
{
    __XercesLocalString m(exception.getMessage());
    throw(xml::lite::XMLParseException(m.str(),
                                       exception.getLineNumber(),
                                       exception.getColumnNumber()));
}

void xml::lite::__XercesErrorHandler::
fatalError(const SAXParseException &exception)
{
    __XercesLocalString m(exception.getMessage());
    xml::lite::XMLParseException xex(m.str(),
                                     exception.getLineNumber(),
                                     exception.getColumnNumber());

    throw except::Error(Ctxt(xex.getMessage()));
}

xml::lite::XMLReaderXerces::XMLReaderXerces()
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const ::XMLException& toCatch)
    {
        xml::lite::__XercesLocalString local(toCatch.getMessage());
        except::Error e(Ctxt(local.str() + " (Initialization error)"));
        throw(e);
    }
    create();
}

void xml::lite::XMLReaderXerces::parse(io::InputStream & is, int size)
{
    io::ByteStream byteStream;
    is.streamTo(byteStream, size);

    off_t available = byteStream.available();
    if ( available <= 0 )
    {
        throw xml::lite::XMLParseException(Ctxt("No stream available"));
    }
    sys::byte* buffer = new sys::byte[available];
    byteStream.read(buffer, available);

    // Adopt my buffer, and delete it for me
    MemBufInputSource memBuffer((const unsigned char *)buffer,
                                available,
                                XMLReaderXerces::MEM_BUFFER_ID(),
                                false);

    mNative->parse(memBuffer);

    delete [] buffer;
}

void xml::lite::XMLReaderXerces::setValidation(bool validate)
{
    /*
    mNative->setFeature(__XercesLocalString("http://xml.org/sax/features/validation").toXMLCh(),
    validate);
    mNative->setFeature(__XercesLocalString("http://xml.org/sax/features/validation/schema").toXMLCh(),
    validate);
    */
    mNative->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
    mNative->setFeature(XMLUni::fgXercesSchema, validate);
    mNative->setFeature(XMLUni::fgSAX2CoreValidation, validate);   // optional
    mNative->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
}

bool xml::lite::XMLReaderXerces::getValidation()
{
    return (
               /* mNative->
                   getFeature(__XercesLocalString("http://xml.org/sax/features/validation").toXMLCh()) &&
                mNative->
                   getFeature(__XercesLocalString("http://xml.org/sax/features/validation/schema").toXMLCh() )
                   */
               mNative->getFeature(XMLUni::fgSAX2CoreValidation)
           );
}


// This function creates the parser
void xml::lite::XMLReaderXerces::create()
{
    mNative = XMLReaderFactory::createXMLReader();

    // By default we auto validate
    /*
        mNative->setFeature(__XercesLocalString("http://xml.org/sax/features/namespaces").toXMLCh(), true);
        mNative->setFeature(__XercesLocalString("http://xml.org/sax/features/namespaces-prefixes").toXMLCh(), true);

        mNative->setFeature(__XercesLocalString("http://xml.org/sax/features/validation").toXMLCh(), false);
        mNative->setFeature(__XercesLocalString("http://xml.org/sax/features/validation/schema").toXMLCh(),
       false);
    */

    mNative->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
    mNative->setFeature(XMLUni::fgSAX2CoreValidation, false);   // optional
    mNative->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
    mNative->setFeature(XMLUni::fgXercesSchema, false);
    mNative->setContentHandler(&mDriverContentHandler);
    mNative->setErrorHandler(&mErrorHandler);
}
// This function destroys the parser
void xml::lite::XMLReaderXerces::destroy()
{
    if (mNative != NULL)
    {
        delete mNative;
        mNative = NULL;
    }
}

#endif
