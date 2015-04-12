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

#include "xml/lite/UtilitiesXerces.h"

#if defined(USE_XERCES)

sys::Mutex xml::lite::XercesContext::mMutex;

xml::lite::XercesLocalString::XercesLocalString(XMLCh* xmlStr) :
    mLocal(xmlStr)
{
}

xml::lite::XercesLocalString::XercesLocalString(const XMLCh* xmlStr) :
    mLocal(XMLString::replicate(xmlStr))
{
}

xml::lite::XercesLocalString::XercesLocalString(const char* str) :
    mLocal(XMLString::transcode(str))
{
}

xml::lite::XercesLocalString::XercesLocalString(const std::string& str) :
    mLocal(XMLString::transcode(str.c_str()))
{
}

xml::lite::XercesLocalString::
XercesLocalString(const XercesLocalString& rhs)
{
    mLocal = XMLString::replicate(rhs.toXMLCh());
}

xml::lite::XercesLocalString& xml::lite::XercesLocalString::
operator=(XMLCh* xmlStr)
{
    // clean up old memory first
    if (xmlStr != mLocal)
    {
        destroyXMLCh(&mLocal);
        mLocal = xmlStr;
    }
    return *this;
}

xml::lite::XercesLocalString& xml::lite::XercesLocalString::
operator=(const XMLCh* xmlStr)
{
    // clean up old memory first
    if (xmlStr != mLocal)
    {
        destroyXMLCh(&mLocal);
        mLocal = XMLString::replicate(xmlStr);
    }
    return *this;
}

xml::lite::XercesLocalString& xml::lite::XercesLocalString::
operator=(const xml::lite::XercesLocalString& rhs)
{
    if (this != &rhs)
    {
        destroyXMLCh(&mLocal);
        mLocal = XMLString::replicate(rhs.toXMLCh());
    }
    return *this;
}

void xml::lite::XercesContentHandler::characters(const XMLCh* const chars,
                                                 const XercesSize_T length)
{
    xml::lite::XercesLocalString xstr(chars);
    mLiteHandler->characters(xstr.str().c_str(), (int)length);
}

void xml::lite::XercesContentHandler::startDocument()
{
    mLiteHandler->startDocument();
}

void xml::lite::XercesContentHandler::endDocument()
{
    mLiteHandler->endDocument();
}

void xml::lite::XercesContentHandler::endElement(const XMLCh* const uri,
                                                 const XMLCh* const localName,
                                                 const XMLCh* const qname)
{
    xml::lite::XercesLocalString xuri(uri);
    xml::lite::XercesLocalString xlocalName(localName);
    xml::lite::XercesLocalString xqname(qname);

    mLiteHandler->endElement(xuri.str(),
                             xlocalName.str(),
                             xqname.str());
}

void xml::lite::XercesContentHandler::startElement(
        const XMLCh* const uri,
        const XMLCh* const localName,
        const XMLCh* const qname,
        const XercesAttributesInterface_T &attrs)
{
    // We have to copy the whole array
    LiteAttributes_T attributes;
    for (unsigned int i = 0; i < attrs.getLength(); i++)
    {
        LiteAttributesNode_T attributeNode;
        attributeNode.setQName(
            XercesLocalString(attrs.getQName(i)).str()
        );

        assert(attributeNode.getLocalName() == 
               XercesLocalString(attrs.getLocalName(i)).str());

        attributeNode.setUri(XercesLocalString(attrs.getURI(i)).str());

        attributeNode.setValue(XercesLocalString(attrs.getValue(i)).str());

        //don't add duplicate attributes
        if (attributes.getIndex(attributeNode.getUri(),
                                attributeNode.getLocalName()) == -1)
            attributes.add(attributeNode);
    }

    XercesLocalString xuri(uri);
    XercesLocalString xlocalName(localName);
    XercesLocalString xqname(qname);
    mLiteHandler->startElement(xuri.str(),
                               xlocalName.str(),
                               xqname.str(),
                               attributes);
}

void xml::lite::XercesErrorHandler::
warning(const SAXParseException &exception)
{
}

void xml::lite::XercesErrorHandler::
error(const SAXParseException &exception)
{
    XercesLocalString m(exception.getMessage());
    throw(xml::lite::XMLParseException(m.str(),
                                       exception.getLineNumber(),
                                       exception.getColumnNumber()));
}

void xml::lite::XercesErrorHandler::
fatalError(const SAXParseException &exception)
{
    XercesLocalString m(exception.getMessage());
    xml::lite::XMLParseException xex(m.str(),
                                     exception.getLineNumber(),
                                     exception.getColumnNumber());

    throw except::Error(Ctxt(xex.getMessage()));
}

xml::lite::XercesContext::XercesContext() :
    mIsDestroyed(false)
{
    //! XMLPlatformUtils::Initialize is not thread safe!
    try
    {
        mt::CriticalSection<sys::Mutex> cs(&mMutex);
        XMLPlatformUtils::Initialize();
    }
    catch (const ::XMLException& toCatch)
    {
        xml::lite::XercesLocalString local(toCatch.getMessage());
        except::Error e(Ctxt(local.str() + " (Initialization error)"));
        throw (e);
    }
}

xml::lite::XercesContext::~XercesContext()
{
    try
    {
        destroy();
    }
    catch (...)
    {
    }
}

void xml::lite::XercesContext::destroy()
{
    // wrapping it here saves the mutex lock
    if (!mIsDestroyed)
    {
        //! XMLPlatformUtils::Terminate is not thread safe!
        try
        {
            mt::CriticalSection<sys::Mutex> cs(&mMutex);
            XMLPlatformUtils::Terminate();
            mIsDestroyed = true;
        }
        catch (const ::XMLException& toCatch)
        {
            mIsDestroyed = false;
            xml::lite::XercesLocalString local(toCatch.getMessage());
            except::Error e(Ctxt(local.str() + " (Termination error)"));
            throw (e);
        }
    }
}

#endif
