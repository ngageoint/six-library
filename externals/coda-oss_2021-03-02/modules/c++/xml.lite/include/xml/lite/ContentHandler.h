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

#ifndef __XML_LITE_CONTENT_HANDLER_H__
#define __XML_LITE_CONTENT_HANDLER_H__
#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <typeinfo>

#include "sys/CPlusPlus.h"

#include "xml/lite/Attributes.h"

/*!
 *  \file  ContentHandler.h
 *  \brief Implementation of a SAX 2.0 content handler
 *
 *  Contains the implementation of the SAX 2.0 content handler.  A "content
 *  handler"  is the processing class for an XML reader.  To do any XML
 *  processing, you simply redefine (parts of) the ContentHandler to
 *  suit your needs.
 */

// If `wchar_t` is built-in (as it should be), then `f(wchar_t)` can be overloaded
// with `f(uint16_t)` and/or `f(uint32_t)`.  If it is **not** (old "C++11" compilers)
// then one of those overload attempts will fail.
#ifndef CODA_OSS_wchar_t_is_type_
    #if defined(_MSC_VER)
        // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-160
        // "Defined as 1 when the /Zc:wchar_t compiler option is set. Otherwise, undefined."
        #define CODA_OSS_wchar_t_is_type_  (_NATIVE_WCHAR_T_DEFINED == 1)
    #else
        #if CODA_OSS_cpp14
            #define CODA_OSS_wchar_t_is_type_ 1
        #else
            // If your "wchar_t" isn't a distinct type, set this to 0.
            // On old systems, it might be a typedef for uint16_t or uint32_t
            //#define CODA_OSS_wchar_t_is_type_ 0
            #define CODA_OSS_wchar_t_is_type_ 1
        #endif // CODA_OSS_cpp14
    #endif
#endif
// https://docs.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type?view=msvc-160
// "... the C++ standard requires that wchar_t be a built-in type. "
static_assert(CODA_OSS_wchar_t_is_type_, "wchar_t should be a built-in type.");

namespace xml
{
namespace lite
{
/*!
 *  \class ContentHandler
 *  \brief The handler for SAX 2.0
 *  \todo  Implement start/endPrefixMapping
 * 
 *  A SAX reader requires a processing unit known as the 
 *  "content handler."  Contained herein is the implementation for
 *  a simple one.  Unlike the SAX specification, we dont separate the
 *  ContentHandler (which is an interface in SAX) from the
 *  DefaultContentHandler.  This lowers package complexity, 
 *  and leaves us with a practical base from which to inherit.  
 *  Any functionality that we dont add is not pure virtual, but is 
 *  an empty function call.
 *
 *  This saves the trouble of having to redefine every interface 
 *  method, allowing the developer to focus only on the routines
 *  which he or she is interested in defining.
 *  It also leaves us with less virtual classes hanging around, 
 *  which should increase performance and efficiency.
 */

class ContentHandler
{
public:
    //! Constructor
    ContentHandler()
    {
    }

    //! Destructor
    virtual ~ContentHandler()
    {
    }

    //! Receive notification of the beginning of a document.
    virtual void startDocument()
    {
    }
    //! Receive notification of the end of a document.
    virtual void endDocument()
    {
    }

    /*!
     *  Receive notification of character data.
     *  Fired when we get character data
     *  \param data  The character data
     *  \param length The length of the new data
     */
    virtual void characters(const char *data, int length) = 0;
    #if CODA_OSS_wchar_t_is_type_
    virtual bool characters(const wchar_t* /*data*/, size_t /*length*/)
    { return false; /* continue on to existing characters()*/ } /* =0 would break existing code */
    #endif
    virtual bool characters(const uint16_t* /*data*/, size_t /*length*/)
    { return false; /* continue on to existing characters()*/ } /* =0 would break existing code */
    virtual bool characters(const uint32_t* /*data*/, size_t /*length*/)
    { return false; /* continue on to existing characters()*/ } /* =0 would break existing code */

    virtual bool use_wchar_t() const // =0 would break existing code
    {
        return false; // call characters(const char*)
    }

    /*!
     *  Receive notification of the beginning of an element.
     *  \param uri  The associated uri
     *  \param localName The local name of the element
     *  \param qname the qualified name
     *  \param attributes  The attributes for this element
     */
    virtual void startElement(const std::string & uri,
                              const std::string & localName,
                              const std::string & qname,
                              const xml::lite::Attributes & attributes) = 0;
    /*!
     *  Receive notification of the end of an element.
     *  \param uri  The associated uri
     *  \param localName The local name of the element
     *  \param qname the qualified name
     */
    virtual void endElement(const std::string & uri,
                            const std::string & localName,
                            const std::string & qname) = 0;

    /*!
     *  This isnt in SAX 2.0, but I think it might be useful
     *  \param c The comment
     */
    virtual void comment(const std::string &)
    {
    }
};
}
}

#endif
