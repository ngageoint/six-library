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

#ifndef __XML_LITE_MINIDOM_HANDLER_H__
#define __XML_LITE_MINIDOM_HANDLER_H__
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

#include "XMLReader.h"
#include "io/StandardStreams.h"
#include "io/DbgStream.h"
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
class MinidomHandler : public ContentHandler
{
public:
    //! Constructor.  Uses default document
    MinidomHandler() :
        mDocument(nullptr), mOwnDocument(true), mPreserveCharData(false)
    {
        setDocument(new Document());
    }

    //! Destructor
    virtual ~ MinidomHandler()
    {
        setDocument(nullptr, true);
    }

    virtual void setDocument(Document *newDocument, bool own = true);

    /**
     * Retrieves the Document.
     * @param steal     if specified, ownership will be given up (if owned)
     */
    virtual Document *getDocument(bool steal = false)
    {
        if (steal)
            mOwnDocument = false;
        return mDocument;
    }

    virtual Document *getDocument() const
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
    virtual void characters(const char* value, int length) override;

    bool wcharacters_(const uint16_t* /*data*/, size_t /*length*/) override;
    bool wcharacters_(const uint32_t* /*data*/, size_t /*length*/) override;

    // Which characters() routine should be called?
    bool use_wchar_t() const override;

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
    virtual void startElement(const std::string & uri,
                              const std::string & localName,
                              const std::string & qname,
                              const Attributes & atts);

    /*!
     * We want to push only the proper amount of bytes
     * to the node when we start writing.  Here we chew
     * up the pieces we take as we are taking them.
     * \return The chracter data for the node
     */
    virtual std::string adjustCharacterData();

    /*!
     * Handles the actual popping of the node off the node
     * stack, and the adjustment of that node's character data.
     * Here we add the node to the tree
     * \param uri The uri
     * \param localName The local name
     * \param qname  The qname
     */
    virtual void endElement(const std::string & uri,
                            const std::string & localName,
                            const std::string & qname);

    virtual void clear();

    /*!
     *  Trim the white space off the back and front of a string
     *  \param  s  String to trim
     */
    static void trim(std::string & s);

    /*!
     * If set to true, whitespaces will be preserved in the parsed
     * character data. Otherwise, it will be trimmed.
     */
    virtual void preserveCharacterData(bool preserve);
    
    /*!
     * If set to true, how std::string values are encoded will be set.
     * 
     * This is a bit goofy to preserve existing behavior: on *ix,
     * XML containing non-ASCII data is lost (it turns into 
     * Windows-1252 on Windows).
     * 
     * When set, there won't be any change on Windows.  However,
     * on *ix, std::string will be encoding as UTF-8 thus preserving
     * the non-ASCII data.
     */
    virtual void storeEncoding(bool value);
    bool storeEncoding() const;

protected:
    void characters(const char* value, int length, const StringEncoding*);

    std::string currentCharacterData;
    std::shared_ptr<const StringEncoding> mpEncoding;
    std::stack<int> bytesForElement;
    std::stack<Element *> nodeStack;
    Document *mDocument;
    bool mOwnDocument;
    bool mPreserveCharData;
    bool mStoreEncoding = false;

 private:
    template <typename T>
    bool characters_(const T* value, size_t length);
    bool call_characters(const std::string& utf8Value);
};
}
}

#endif
