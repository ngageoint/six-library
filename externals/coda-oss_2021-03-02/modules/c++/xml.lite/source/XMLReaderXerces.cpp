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

#include "xml/lite/XMLReader.h"
#include <mem/ScopedArray.h>

#if defined(USE_XERCES)

xml::lite::XMLReaderXerces::XMLReaderXerces()
{
    create();
}

void xml::lite::XMLReaderXerces::parse(io::InputStream & is, int size)
{
    io::StringStream oss;
    is.streamTo(oss, size);

    const auto available = oss.available();
    if ( available <= 0 )
    {
        throw xml::lite::XMLParseException(Ctxt("No stream available"));
    }
    mem::ScopedArray<sys::byte> buffer(new sys::byte[available]);
    oss.read(buffer.get(), available);

    // Does not take ownership
    MemBufInputSource memBuffer((const unsigned char *)buffer.get(),
                                available,
                                XMLReaderXerces::MEM_BUFFER_ID(),
                                false);

    mNative->parse(memBuffer);
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
