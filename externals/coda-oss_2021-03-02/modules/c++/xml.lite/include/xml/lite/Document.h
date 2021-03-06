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

#ifndef __XML_LITE_DOM_DOCUMENT_H__
#define __XML_LITE_DOM_DOCUMENT_H__
#pragma once

/*!
 * \file  Document.h
 * \brief File describing the tree API
 *
 * This is a simple alternative to the DOM API.  It is very similar,
 * but is a solution intended more for speed and simplicity.  The
 * DOM API offers far more functionality, where this version implements
 * only the widely used features.  In this file, we are only concerned
 * with the data structure for implementing the XML reader, not the reader
 * itself.
 */

#include "xml/lite/Element.h"

namespace xml
{
namespace lite
{
/*!
 * \class Document
 * \brief This class is the organizer for a tree of elements.
 *
 * Use the Document to access the Element nodes contained within.
 * The DocumentParser will build a tree that you can use.
 */
class Document
{
public:
    //! Constructor
    Document(Element* rootNode = NULL, bool own = true) :
        mRootNode(rootNode), mOwnRoot(own)
    {
    }

    /*!
     * Destroy the xml tree.  This deletes the nodes if they exist
     * Careful, this may delete your copy if you are not careful
     */
    virtual ~Document()
    {
        destroy();
    }

    virtual Document* clone() const
    {
        Document* doc = new Document();

        Element* cloneRoot = new Element();
        cloneRoot->clone(*mRootNode);
        doc->setRootElement(cloneRoot);
        return doc;
    }

    /*!
     * Factory-type method for creating a new Element
     * \param qname The qname of the new element
     * \param uri The URI of the new element
     * \param characterData The character data (if any)
     * \return A new element
     */
    virtual Element *createElement(const std::string & qname,
                                   const std::string & uri,
                                   std::string characterData = "");
    virtual Element *createElement(const std::string & qname,
                                   const std::string & uri,
                                   const std::string& characterData, string_encoding);
    virtual Element* createElement(const std::string& qname,
                                   const std::string& uri,
                                   const sys::U8string& characterData);

    /*!
     * Blanket destructor.  This thing deletes everything
     */
    void destroy();

    /*!
     * Insert an element under this element.  Secretly, this
     * tree does not really care whether or not the element in
     * question is added to this tree or another tree, or just
     * an element.  It simply connects the two items
     * \param element Element to add
     * \param underThis Element to add element to
     */
    virtual void insert(Element * element, Element * underThis);

    /*!
     * Remove an element from the tree, starting at the root
     * \param toDelete The node to delete (This DOES do deletion)
     */
    virtual void remove(Element * toDelete);

    /*!
     * Remove an element from the tree, starting at the second param
     * \param toDelete The node to delete (This DOES do deletion)
     * \param fromHere The place to start looking for.  This could
     * be an optimization depending on the task, so I allow it to remain
     * public
     */
    virtual void remove(Element * toDelete, Element * fromHere);

    /*!
     * Sets the internal root element
     * \param element The node to set.
     */
    void setRootElement(Element * element, bool own = true);

    /*!
     * Retrieves the internal root element
     * \return The root node
     */
    Element *getRootElement(bool steal = false)
    {
        if (steal)
            mOwnRoot = false;
        return mRootNode;
    }

    Element *getRootElement() const
    {
        return mRootNode;
    }

protected:
    //! Copy constructor
    Document(const Document&);

    //! Assignment operator
    Document& operator=(const Document&);

    //! The root node element
    Element *mRootNode;
    bool mOwnRoot;
};
}
}

#endif
