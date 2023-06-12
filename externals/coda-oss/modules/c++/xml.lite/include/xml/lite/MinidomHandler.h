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

#ifndef CODA_OSS_xml_lite_MinidomHandler_h_INCLUDED_
#define CODA_OSS_xml_lite_MinidomHandler_h_INCLUDED_
#pragma once

/*!
 *  \file MinidomHandler.h
 *  \brief A fast, small, alternative to DOM.
 *
 *  This handler inherits a ContentHandler, and loads all components
 *  into a more simple and logical tree structure.  Whereas, in DOM,
 *  EVERYTHING is a node, here, the hierarchical approach is used.
 *  An element is a tagset (i.e., &lt;blah att1="hello"&gt;hi I'm dan&lt;/blah&gt;,
 *  and embedded tags are children.  To walk the tree, get the top-
 *  level element and walk it.
 *
 *  There is memory allocation in this file, however, the paradigm is
 *  that since it occurs internally, it should be deleted internally
 *  as well.  Thus, when the Document goes out of scope (i.e., the destructor
 *  is called), it will delete its element nodes.  This makes sense if you
 *  consider that, without the parser, there is no tree.  So, let the
 *  parser handle its own business.
 *
 *  \todo Add iterators
 */

#include <stack>
#include <memory>
#include "coda_oss/string.h"

#include "str/EncodedString.h"
#include "str/EncodedStringView.h"
#include "XMLReader.h"
#include "io/StandardStreams.h"
#include "Document.h"

namespace xml
{
namespace lite
{
/*!
 * \class MinidomHandler
 * \brief This class is an XML reader that fills a tree.
 *
 * The class has a tree that it populates with XML data as
 * it parses,  The Document* is owned by this class, no matter,
 * whether it is allocated externally or not.  DONT delete it 
 * explicitly unless you are looking for disaster.
 */
struct MinidomHandler final : public ContentHandler
{
    //! Constructor.  Uses default document
    MinidomHandler() 
    {
        setDocument(std::make_unique<Document>());
    }

    //! Destructor
    ~ MinidomHandler()
    {
        setDocument(nullptr, true);
    }
    MinidomHandler(const MinidomHandler&) = delete;
    MinidomHandler& operator=(const MinidomHandler&) = delete;
    MinidomHandler(MinidomHandler&&) = default;
    MinidomHandler& operator=(MinidomHandler&&) = default;

    void setDocument(Document *newDocument, bool own = true);
    void setDocument(std::unique_ptr<Document>&&);  // own = true

    /**
     * Retrieves the Document.
     * @param steal     if specified, ownership will be given up (if owned)
     */
    Document *getDocument(bool steal = false)
    {
        if (steal)
            mOwnDocument = false;
        return mDocument;
    }
    std::unique_ptr<Document>& getDocument(std::unique_ptr<Document>&);  // steal = true

    Document *getDocument() const
    {
        return mDocument;
    }

    /*!
     * This is the function overload for handling char data.
     * We append the chunk to the current data, and add the
     * length to the top of the bytes stack.
     * \param value The value of the char data
     * \param length The length of the char data
     */
    void characters(const char* value, int length) override;
    bool vcharacters(const void /*XMLCh*/*, size_t length) override;  

    /*!
     * This method is fired when a new tag is entered.
     * We use our newElement() method to spawn a new
     * element and we set its tag to our tag param and its attributes
     * to our atts param.  We then push it on the node stack,
     * and we reset the char data byte count stack for this new object
     * \param uri  The uri
     * \param localName  The local name
     * \param qname  The qname
     * \param atts  The attributes
     */
    void startElement(const std::string & uri,
                              const std::string & localName,
                              const std::string & qname,
                              const Attributes & atts) override;

    /*!
     * Handles the actual popping of the node off the node
     * stack, and the adjustment of that node's character data.
     * Here we add the node to the tree
     * \param uri The uri
     * \param localName The local name
     * \param qname  The qname
     */
    void endElement(const std::string & uri,
                            const std::string & localName,
                            const std::string & qname) override;

    void clear();

    /*!
     * If set to true, whitespaces will be preserved in the parsed
     * character data. Otherwise, it will be trimmed.
     */
    void preserveCharacterData(bool preserve);
    
private:
    /*!
     * We want to push only the proper amount of bytes
     * to the node when we start writing.  Here we chew
     * up the pieces we take as we are taking them.
     * \return The chracter data for the node
     */
    coda_oss::u8string adjustCharacterData();

    /*!
     *  Trim the white space off the back and front of a string
     *  \param  s  String to trim
     */
    static void trim(coda_oss::u8string& s);

    coda_oss::u8string currentCharacterData;
    std::stack<int> bytesForElement;
    std::stack<Element *> nodeStack;
    Document* mDocument = nullptr;
    bool mOwnDocument = true;
    bool mPreserveCharData = false;
    void characters(coda_oss::u8string&&);
};
}
}

#endif  // CODA_OSS_xml_lite_MinidomHandler_h_INCLUDED_
