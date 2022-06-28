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

#include <assert.h>

#include <vector>

#include "xml/lite/XMLReader.h"
#include <mem/ScopedArray.h>

#if defined(USE_XERCES)

xml::lite::XMLReaderXerces::XMLReaderXerces()
{
    create();
}

void xml::lite::XMLReaderXerces::parse(bool storeEncoding,
    io::InputStream& is, const StringEncoding* pEncoding, int size)
{
    io::StringStream oss;
    is.streamTo(oss, size);

    const auto available = oss.available();
    if ( available <= 0 )
    {
        throw xml::lite::XMLParseException(Ctxt("No stream available"));
    }
    std::vector<sys::byte> buffer(available);
    oss.read(buffer.data(), buffer.size());
    parse(storeEncoding, buffer, pEncoding);
}
void xml::lite::XMLReaderXerces::parse(bool storeEncoding,
    const std::vector<sys::byte>& buffer, const StringEncoding* pEncoding)
{
    // Does not take ownership
    MemBufInputSource memBuffer((const unsigned char *)buffer.data(),
                                buffer.size(),
                                XMLReaderXerces::MEM_BUFFER_ID(),
                                false);

    if ((pEncoding != nullptr) && (*pEncoding == StringEncoding::Windows1252))
    {
        // The only other value is StringEncoding::Utf8 which is the default
        memBuffer.setEncoding(XMLUni::fgWin1252EncodingString);
    }

    try
    {
        mNative->parse(memBuffer);
        return; // successful parse
    }
    catch (const except::Error& e)
    {
        const auto msg = e.getMessage();
        if (msg != " (1,1): invalid byte 'X' at position 2 of a 2-byte sequence")
        {
            throw;
        }
        // Caller specified an encoding; don't try calling parse() again
        if (pEncoding != nullptr)
        {
            throw;
        }
        // legacy code, didn't pass storeEncoding=true to MinidomParser
        if (!storeEncoding)
        {
            throw;
        }
    }

    // If we're here, the initial parse failed and the caller did NOT specify an encoding
    // (pEncoding == NULL).  Since the default is UTF-8 and that failed, try again
    // with Windows-1252.
    assert(pEncoding == nullptr);
    assert(storeEncoding);
    const auto windows1252 = StringEncoding::Windows1252;
    parse(true /*storeEncoding*/, buffer, &windows1252);
}
void xml::lite::XMLReaderXerces::parse(io::InputStream& is, int size)
{
    parse(false /*storeEncoding*/, is, size);
}
void xml::lite::XMLReaderXerces::parse(bool storeEncoding, io::InputStream& is, int size)
{
    parse(storeEncoding, is, nullptr /*pEncoding*/, size);
}
void xml::lite::XMLReaderXerces::parse(bool storeEncoding, 
    io::InputStream& is, StringEncoding encoding, int size)
{
    parse(storeEncoding, is, &encoding, size);
}


// This function creates the parser
void xml::lite::XMLReaderXerces::create()
{
    mDriverContentHandler.reset(new XercesContentHandler());
    mErrorHandler.reset(new XercesErrorHandler());

    mNative.reset(XMLReaderFactory::createXMLReader());
    mNative->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
    mNative->setFeature(XMLUni::fgSAX2CoreValidation, false);   // optional
    mNative->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);    // optional
    mNative->setFeature(XMLUni::fgXercesSchema, false);
    // We do schema validation as an option not DTDs
    mNative->setFeature(XMLUni::fgXercesSkipDTDValidation, true);
    // Trying to load external DTDs can cause the parser to hang
    mNative->setFeature(XMLUni::fgXercesLoadExternalDTD, false);
    mNative->setContentHandler(mDriverContentHandler.get());
    mNative->setErrorHandler(mErrorHandler.get());
}
// This function destroys the parser
void xml::lite::XMLReaderXerces::destroy()
{
    mNative.reset();
    mDriverContentHandler.reset();
    mErrorHandler.reset();

    mCtxt.destroy();
}

#endif
