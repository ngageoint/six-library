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

#include <xml/lite/UtilitiesXerces.h>

#if defined(USE_XERCES)

namespace xml
{
namespace lite
{
std::mutex XercesContext::mMutex;

XercesLocalString::XercesLocalString(XMLCh* xmlStr) :
    mLocal(xmlStr)
{
}

XercesLocalString::XercesLocalString(const XMLCh* xmlStr) :
    mLocal(XMLString::replicate(xmlStr))
{
}

XercesLocalString::XercesLocalString(const char* str) :
    mLocal(XMLString::transcode(str))
{
}

XercesLocalString::XercesLocalString(const std::string& str) :
    mLocal(XMLString::transcode(str.c_str()))
{
}

XercesLocalString::XercesLocalString(const XercesLocalString& rhs)
{
    mLocal = XMLString::replicate(rhs.toXMLCh());
}

XercesLocalString& XercesLocalString::operator=(XMLCh* xmlStr)
{
    // clean up old memory first
    if (xmlStr != mLocal)
    {
        destroyXMLCh(&mLocal);
        mLocal = xmlStr;
    }
    return *this;
}

XercesLocalString& XercesLocalString::operator=(const XMLCh* xmlStr)
{
    // clean up old memory first
    if (xmlStr != mLocal)
    {
        destroyXMLCh(&mLocal);
        mLocal = XMLString::replicate(xmlStr);
    }
    return *this;
}

XercesLocalString& XercesLocalString::operator=(const XercesLocalString& rhs)
{
    if (this != &rhs)
    {
        destroyXMLCh(&mLocal);
        mLocal = XMLString::replicate(rhs.toXMLCh());
    }
    return *this;
}

void XercesContentHandler::characters(const XMLCh* const chars,
                                      const XercesSize_T length)
{
    if (mLiteHandler->vcharacters(chars, length))
    {
        return;  // processed as void*
    }

    // Either use_wchar_t() is false (default, legacy behavior) or
    // we couldn't process the wide-character (Windows).
    XercesLocalString xstr(chars);
    mLiteHandler->characters(xstr.str().c_str(), (int)length);
}

void XercesContentHandler::startDocument()
{
    mLiteHandler->startDocument();
}

void XercesContentHandler::endDocument()
{
    mLiteHandler->endDocument();
}

void XercesContentHandler::endElement(const XMLCh* const uri,
                                      const XMLCh* const localName,
                                      const XMLCh* const qname)
{
    XercesLocalString xuri(uri);
    XercesLocalString xlocalName(localName);
    XercesLocalString xqname(qname);

    mLiteHandler->endElement(xuri.str(),
                             xlocalName.str(),
                             xqname.str());
}

void XercesContentHandler::startElement(
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

void XercesErrorHandler::
warning(const SAXParseException& /*exception*/)
{
}

void XercesErrorHandler::
error(const SAXParseException &exception)
{
    XercesLocalString m(exception.getMessage());
    throw XMLParseException(m.str(),
                                       static_cast<int>(exception.getLineNumber()),
                                       static_cast<int>(exception.getColumnNumber()));
}

void XercesErrorHandler::
fatalError(const SAXParseException &exception)
{
    XercesLocalString m(exception.getMessage());
    XMLParseException xex(m.str(),
                                     static_cast<int>(exception.getLineNumber()),
                                     static_cast<int>(exception.getColumnNumber()));

    throw except::Error(Ctxt(xex.getMessage()));
}

XercesContext::XercesContext() :
    mIsDestroyed(false)
{
    //! XMLPlatformUtils::Initialize is not thread safe!
    try
    {
        std::lock_guard<std::mutex> cs(mMutex);
        XMLPlatformUtils::Initialize();
    }
    catch (const ::XMLException& toCatch)
    {
        XercesLocalString local(toCatch.getMessage());
        except::Error e(Ctxt(local.str() + " (Initialization error)"));
        throw e;
    }
}

XercesContext::~XercesContext()
{
    try
    {
        destroy();
    }
    catch (...)
    {
    }
}

void XercesContext::destroy()
{
    // wrapping it here saves the mutex lock
    if (!mIsDestroyed)
    {
        //! XMLPlatformUtils::Terminate is not thread safe!
        try
        {
            std::lock_guard<std::mutex> cs(mMutex);
            XMLPlatformUtils::Terminate();
            mIsDestroyed = true;
        }
        catch (const ::XMLException& toCatch)
        {
            mIsDestroyed = false;
            XercesLocalString local(toCatch.getMessage());
            except::Error e(Ctxt(local.str() + " (Termination error)"));
            throw e;
        }
    }
}
}
}

#endif
