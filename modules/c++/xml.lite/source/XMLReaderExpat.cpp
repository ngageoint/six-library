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

#if defined (USE_EXPAT)


//  This parse routine knows the start of a document by definition
//  It does not know the end
void xml::lite::XMLReaderExpat::parse(io::InputStream & is, int size)
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
        __xml_parse_ex(FmtX(getErrorString((XML_Error) getLastError())));
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
                          __startElement,
                          (XML_EndElementHandler) xml::lite::XMLReaderExpat::
                          __endElement);
    XML_SetCharacterDataHandler(mNative,
                                (XML_CharacterDataHandler) xml::lite::
                                XMLReaderExpat::__characters);
    XML_SetCommentHandler(mNative,
                          (XML_CommentHandler) xml::lite::XMLReaderExpat::
                          __comment);

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
void xml::lite::XMLReaderExpat::__pushNamespaceContext(const char **atts)
{
    mNamespaceStack.push();

    // Iterate through and find the mappings
    for (int i = 0; atts[i] != NULL; i += 2)
    {
        std::string __attr(atts[i]);
        std::string::size_type x = __attr.find_first_of(':');

        // Either we have a namespace prefix
        if (x != std::string::npos)
        {
            // Either our namespace prefix is xmlns
            if (__attr.substr(0, x) == "xmlns")
            {
                // Get the value
                std::string __uri = atts[i + 1];
                mNamespaceStack.newMapping(__attr.substr(x + 1), __uri);

            }
            // Or its not
        }
        else if (__attr == "xmlns")
        {
            std::string __u = atts[i + 1];
            mNamespaceStack.newMapping(__attr, __u);
        }
    }
}

// This function removes the context from the stack
void xml::lite::XMLReaderExpat::__popNamespaceContext()
{
    // All we have to do is pop the namespace stack context and we're done
    mNamespaceStack.pop();
}


// This method resolves a name to all of its SAX 2.0 parts
void xml::lite::XMLReaderExpat::__resolve(const char *name,
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
void xml::lite::XMLReaderExpat::__startElement(void *p,
        const char *tag,
        const char **atts)
{
    // Pull out our parser
    xml::lite::XMLReaderExpat * xmlReader = (xml::lite::XMLReaderExpat *) p;

    // Push the namespace context on for this element's attributes
    xmlReader->__pushNamespaceContext(atts);

    std::string __uri;
    std::string __lname;
    std::string __qname;

    // Resolve the tag into what we really want
    xmlReader->__resolve(tag, __uri, __lname, __qname);

    xml::lite::Attributes __attrs;
    // Resolve the attributes to what we really want
    for (int i = 0; atts[i] != NULL; i += 2)
    {
        xml::lite::AttributeNode __attr;
        std::string __attr_qname;
        std::string __attr_lname;
        std::string __attr_uri;

        xmlReader->__resolve(atts[i],
                             __attr_uri,
                             __attr_lname,
                             __attr_qname);
        __attr.setValue(atts[i + 1]);

        __attr.setQName(__attr_qname);
        __attr.setUri(__attr_uri);
        assert(__attr_lname == __attr.getLocalName());
        __attrs.add(__attr);
    }

    // Fire an event
    xmlReader->getContentHandler()->startElement(__uri,
            __lname,
            __qname, __attrs);
}

// This function fires off the content handler's endElement() function
void xml::lite::XMLReaderExpat::__endElement(void *p, const char *tag)
{
    // Pull out our parser
    xml::lite::XMLReaderExpat * xmlReader = (xml::lite::XMLReaderExpat *) p;

    std::string __uri;
    std::string __localName;
    std::string __qname;

    // Resolve the tag into what we really want
    xmlReader->__resolve(tag, __uri, __localName, __qname);

    // Fire the event
    xmlReader->getContentHandler()->endElement(__uri, __localName, __qname);

    xmlReader->__popNamespaceContext();
}


// This function fires off the content handler's characters() function
void xml::lite::XMLReaderExpat::__characters(void *p,
        const char *data,
        int size)
{

    // Pull out our parser
    xml::lite::XMLReaderExpat * xmlReader = (xml::lite::XMLReaderExpat *) p;
    xmlReader->getContentHandler()->characters(data, size);
}

void xml::lite::XMLReaderExpat::__comment(void *p, const char *c)
{
    // Pull out our parser
    xml::lite::XMLReaderExpat * xmlReader = (xml::lite::XMLReaderExpat *) p;
    std::string __c(c);
    xmlReader->getContentHandler()->comment(__c);
}

#endif
