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

#if defined(USE_LIBXML)


//  This parse routine knows the start of a document by definition
//  It does not know the end
void xml::lite::XMLReaderLibXML::parse(io::InputStream& is, int size)
{
    mContentHandler->startDocument();
    is.streamTo(*this, size);
    finish();

}

// This parse routine knows the end of a document based on the boolean done
// Thus, if done's value is incorrect, endDocument() will be inaccurate
void xml::lite::XMLReaderLibXML::parse(const sys::byte *data,
                                       int size,
                                       bool done)
{
    // Gotta love this... piece of cake :)
    if (xmlParseChunk(mContextLibXML, data, size, done) != 0)
        throw xml::lite::XMLException(Ctxt("LibXML xmlParseChunk error"));
}


// This function creates the parser
void xml::lite::XMLReaderLibXML::create()
{
    //xmlSAX2InitDefaultSAXHandler(&mSAXLibXML, 0);

    xmlSAXVersion(&mSAXLibXML, 1);
/*
    mSAXLibXML.internalSubset = NULL;
    mSAXLibXML.externalSubset = NULL;
    mSAXLibXML.isStandalone = NULL;
    mSAXLibXML.hasInternalSubset = NULL;
    mSAXLibXML.hasExternalSubset = NULL;
    mSAXLibXML.resolveEntity = NULL;
    mSAXLibXML.getEntity = NULL;
    mSAXLibXML.getParameterEntity = NULL;
    mSAXLibXML.entityDecl = NULL;
    mSAXLibXML.attributeDecl = NULL;
    mSAXLibXML.elementDecl = NULL;
    mSAXLibXML.notationDecl = NULL;
    mSAXLibXML.unparsedEntityDecl = NULL;
    mSAXLibXML.setDocumentLocator = NULL;
    mSAXLibXML.startDocument = NULL;
    mSAXLibXML.endDocument = NULL;
    mSAXLibXML.reference = NULL;
    mSAXLibXML.cdataBlock = NULL;
    mSAXLibXML.processingInstruction = NULL;
*/

    mSAXLibXML.startDocument = NULL;
    mSAXLibXML.endDocument = NULL;
    mSAXLibXML.startElement = startElementCallback;
    mSAXLibXML.endElement = endElementCallback;
    mSAXLibXML.characters = charactersCallback;
    mSAXLibXML.ignorableWhitespace = charactersCallback;
    mSAXLibXML.comment = commentCallback;
    mSAXLibXML.warning = warningCallback;
    mSAXLibXML.error = errorCallback;
    mSAXLibXML.fatalError = errorCallback;

    // Arguments are, in order:
    // 1) SAX parser
    // 2) User data
    // 3) Chunk of data (presumably to allow it to test decoding)
    // 4) Size (of chunk)
    // 5) Filename (probably for parse error info)
    // Returns the new parser context or NULL
    mContextLibXML =
        xmlCreatePushParserCtxt(&mSAXLibXML,
                                this,
                                NULL,
                                0,
                                NULL);
    
    if (mContextLibXML == NULL)
        throw except::Exception(Ctxt("Failed to create parser context"));

}
// This function destroys the parser
void xml::lite::XMLReaderLibXML::destroy()
{
    if (mContextLibXML)
        xmlFreeParserCtxt(mContextLibXML);
    
}


// This function puts the context on the stack
void xml::lite::XMLReaderLibXML::pushNamespaceContext(const xmlChar **atts)
{
    mNamespaceStack.push();

    // Iterate through and find the mappings
    for (int i = 0; atts && atts[i] != NULL; i += 2)
    {
        std::string attr((const char*)atts[i]);
        std::string::size_type x = attr.find_first_of(':');

        // Either we have a namespace prefix
        if (x != std::string::npos)
        {
            // Either our namespace prefix is xmlns
            if (attr.substr(0, x) == "xmlns")
            {
                // Get the value
                std::string uri( (const char*)atts[i + 1] );
                mNamespaceStack.newMapping(attr.substr(x + 1), uri);

            }
            // Or its not
        }
        else if (attr == "xmlns")
        {
            std::string u( (const char*)atts[i + 1] );
            mNamespaceStack.newMapping(attr, u);
        }
    }
}

// This function removes the context from the stack
void xml::lite::XMLReaderLibXML::popNamespaceContext()
{
    // All we have to do is pop the namespace stack context and we're done
    mNamespaceStack.pop();
}


// This method resolves a name to all of its SAX 2.0 parts
void xml::lite::XMLReaderLibXML::resolve(const char *name,
                                           std::string& uri,
                                           std::string& lname,
                                           std::string& qname)
{
    // The QName is the input
    qname = name;

    // Find the delimiter if any
    std::string::size_type x = qname.find_first_of(':');

    // If it exists, set the associated uri and localName
    if (x != std::string::npos)
    {
        uri = mNamespaceStack.getMapping(qname.substr(0, x));
        lname = qname.substr(x + 1);
    }
    // Check for xmlns then set the uri and local name to nothing
    else
    {
        uri = mNamespaceStack.getMapping("xmlns");
        lname = qname;
    }
}

// This function fires off the content handler's startElement() function
void xml::lite::XMLReaderLibXML::startElementCallback(void *ctx,
                                                      const xmlChar *tag,
                                                      const xmlChar **atts)
{
    // Pull out our parser
    xml::lite::XMLReaderLibXML * xmlReader = (xml::lite::XMLReaderLibXML *) ctx;

    // Push the namespace context on for this element's attributes
    xmlReader->pushNamespaceContext(atts);

    std::string uri;
    std::string lname;
    std::string qname;

    // Resolve the tag into what we really want
    xmlReader->resolve((const char*)tag, uri, lname, qname);

    xml::lite::Attributes attrs;
    // Resolve the attributes to what we really want
    for (int i = 0; atts && atts[i] != NULL; i += 2)
    {
        xml::lite::AttributeNode attr;
        std::string attrQName;
        std::string attrLName;
        std::string attrUri;

        xmlReader->resolve((const char*)atts[i],
                           attrUri,
                           attrLName,
                           attrQName);
        attr.setValue((const char*)atts[i + 1]);

        attr.setQName(attrQName);
        attr.setUri(attrUri);
        assert(attrLName == attr.getLocalName());
        attrs.add(attr);
    }

    // Fire an event
    xmlReader->getContentHandler()->startElement(uri,
                                                 lname,
                                                 qname, attrs);
}

// This function fires off the content handler's endElement() function
void xml::lite::XMLReaderLibXML::endElementCallback(void *ctx, const xmlChar *tag)
{
    // Pull out our parser
    xml::lite::XMLReaderLibXML * xmlReader = (xml::lite::XMLReaderLibXML *) ctx;

    std::string uri;
    std::string localName;
    std::string qname;

    // Resolve the tag into what we really want
    xmlReader->resolve((const char*)tag, uri, localName, qname);

    // Fire the event
    xmlReader->getContentHandler()->endElement(uri, localName, qname);

    xmlReader->popNamespaceContext();
}


// This function fires off the content handler's characters() function
void xml::lite::XMLReaderLibXML::charactersCallback(void *ctx,
                                                    const xmlChar *data,
                                                    int size)
{

    // Pull out our parser
    xml::lite::XMLReaderLibXML * xmlReader = (xml::lite::XMLReaderLibXML *) ctx;
    xmlReader->getContentHandler()->characters((const char*)data, size);
}

void xml::lite::XMLReaderLibXML::commentCallback(void *ctx, const xmlChar *c)
{
    // Pull out our parser
    xml::lite::XMLReaderLibXML * xmlReader = (xml::lite::XMLReaderLibXML *) ctx;
    std::string cmt((const char*)c);
    xmlReader->getContentHandler()->comment(cmt);
}

void xml::lite::XMLReaderLibXML::warningCallback(void *ctx,
                                                 const char* msg,
                                                 ...)
{
    // Do nothing for now, these should become wired up to something later
}

void xml::lite::XMLReaderLibXML::errorCallback(void *ctx,
                                               const char* msg,
                                               ...)
{
    throw xml::lite::XMLException(Ctxt(FmtX("LibXML parse exception: %s", msg)));
}





#endif
