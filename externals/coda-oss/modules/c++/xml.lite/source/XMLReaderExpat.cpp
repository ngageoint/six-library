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

#if defined (USE_EXPAT)


//  This parse routine knows the start of a document by definition
//  It does not know the end
void xml::lite::XMLReaderExpat::parse(io::InputStream& is, int size)
{
    mContentHandler->startDocument();
    is.streamTo(*this, size);
    finish();

}

// This parse routine knows the end of a document based on the boolean done
// Thus, if done's value is incorrect, endDocument() will be inaccurate
void xml::lite::XMLReaderExpat::parse(const sys::byte *data,
                                      int size,
                                      bool done)
{
    if (!XML_Parse(mNative, (const char *)data, size, (int) done))
        throw
            xml::lite::XMLParseException(
                getErrorString((XML_Error) getLastError()),
                getCurrentLineNumber(),
                getCurrentColumnNumber()
                );
}


// This function creates the parser
void xml::lite::XMLReaderExpat::create()
{
    // 1. Take the default character mapping
    // 2. Note we are not using expat's NS expanding
    if (mNative == NULL)
        mNative = XML_ParserCreate(NULL);
    else
        dbg_printf("Tried to re-create an existing parser!\n");

    XML_SetUserData(mNative, this);
    XML_SetElementHandler(mNative,
                          (XML_StartElementHandler) xml::lite::XMLReaderExpat::
                          startElementCallback,
                          (XML_EndElementHandler) xml::lite::XMLReaderExpat::
                          endElementCallback);
    XML_SetCharacterDataHandler(mNative,
                                (XML_CharacterDataHandler) xml::lite::
                                XMLReaderExpat::charactersCallback);
    XML_SetCommentHandler(mNative,
                          (XML_CommentHandler) xml::lite::XMLReaderExpat::
                          commentCallback);

}

// This function destroys the parser
void xml::lite::XMLReaderExpat::destroy()
{
    // 1. Delete the parser
    // 2. Free the parser
    XML_ParserFree(mNative);
    mNative = NULL;
}


// This function puts the context on the stack
void xml::lite::XMLReaderExpat::pushNamespaceContext(const char **atts)
{
    mNamespaceStack.push();

    // Iterate through and find the mappings
    for (int i = 0; atts[i] != NULL; i += 2)
    {
        std::string attr(atts[i]);
        std::string::size_type x = attr.find_first_of(':');

        // Either we have a namespace prefix
        if (x != std::string::npos)
        {
            // Either our namespace prefix is xmlns
            if (attr.substr(0, x) == "xmlns")
            {
                // Get the value
                std::string uri = atts[i + 1];
                mNamespaceStack.newMapping(attr.substr(x + 1), uri);

            }
            // Or its not
        }
        else if (attr == "xmlns")
        {
            std::string u = atts[i + 1];
            mNamespaceStack.newMapping(attr, u);
        }
    }
}

// This function removes the context from the stack
void xml::lite::XMLReaderExpat::popNamespaceContext()
{
    // All we have to do is pop the namespace stack context and we're done
    mNamespaceStack.pop();
}


// This method resolves a name to all of its SAX 2.0 parts
void xml::lite::XMLReaderExpat::resolve(const char *name,
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
void xml::lite::XMLReaderExpat::startElementCallback(void *p,
                                                     const char *tag,
                                                     const char **atts)
{
    // Pull out our parser
    xml::lite::XMLReaderExpat * xmlReader = (xml::lite::XMLReaderExpat *) p;

    // Push the namespace context on for this element's attributes
    xmlReader->pushNamespaceContext(atts);

    std::string uri;
    std::string lname;
    std::string qname;

    // Resolve the tag into what we really want
    xmlReader->resolve(tag, uri, lname, qname);

    xml::lite::Attributes attrs;
    // Resolve the attributes to what we really want
    for (int i = 0; atts[i] != NULL; i += 2)
    {
        xml::lite::AttributeNode attr;
        std::string attrQName;
        std::string attrLName;
        std::string attrUri;

        xmlReader->resolve(atts[i],
                           attrUri,
                           attrLName,
                           attrQName);
        attr.setValue(atts[i + 1]);
        
        attr.setQName(attrQName);
        attr.setUri(attrUri);
        assert(attrLName == attr.getLocalName());
        attrs.add(attr);
    }

    // Fire an event
    xmlReader->getContentHandler()->startElement(uri,
                                                 lname,
                                                 qname,
                                                 attrs);
}

// This function fires off the content handler's endElement() function
void xml::lite::XMLReaderExpat::endElementCallback(void *p, const char *tag)
{
    // Pull out our parser
    xml::lite::XMLReaderExpat * xmlReader = (xml::lite::XMLReaderExpat *) p;

    std::string uri;
    std::string localName;
    std::string qname;

    // Resolve the tag into what we really want
    xmlReader->resolve(tag, uri, localName, qname);

    // Fire the event
    xmlReader->getContentHandler()->endElement(uri, localName, qname);

    xmlReader->popNamespaceContext();
}


// This function fires off the content handler's characters() function
void xml::lite::XMLReaderExpat::charactersCallback(void *p,
                                                   const char *data,
                                                   int size)
{

    // Pull out our parser
    xml::lite::XMLReaderExpat* xmlReader = (xml::lite::XMLReaderExpat *) p;
    xmlReader->getContentHandler()->characters(data, size);
}

void xml::lite::XMLReaderExpat::commentCallback(void *p, const char *c)
{
    // Pull out our parser
    xml::lite::XMLReaderExpat* xmlReader = (xml::lite::XMLReaderExpat *) p;
    std::string cmt(c);
    xmlReader->getContentHandler()->comment(cmt);
}

#endif
