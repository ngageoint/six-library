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

#ifndef CODA_OSS_xml_lite_Doocument_h_INCLUDED_
#define CODA_OSS_xml_lite_Doocument_h_INCLUDED_
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

#include <assert.h>

#include <utility>
#include <std/memory>
#include "coda_oss/string.h"
#include "coda_oss/memory.h"

#include "xml/lite/Element.h"
#include "xml/lite/QName.h"

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
struct Document // SOAPDocument derives :-(
{
    //! Constructor
    Document(Element* rootNode = nullptr, bool own = true) :
        mRootNode(rootNode), mOwnRoot(own)
    {
    }
    #ifndef SWIG // SWIG doesn't like std::unique_ptr
    explicit Document(std::unique_ptr<Element>&& rootNode) : // implicitly own=true
        Document(rootNode.release(), true /*own*/)
    {
    }
    #endif // SWIG

    /*!
     * Destroy the xml tree.  This deletes the nodes if they exist
     * Careful, this may delete your copy if you are not careful
     */
    virtual ~Document() noexcept(false)
    {
        destroy();
    }

    #ifndef SWIG // SWIG doesn't like std::unique_ptr
    std::unique_ptr<Document>& clone(std::unique_ptr<Document>& doc) const
    {
        doc = std::make_unique<Document>();

        auto cloneRoot = std::make_unique<Element>();
        cloneRoot->clone(*mRootNode);
        doc->setRootElement(std::move(cloneRoot));
        return doc;
    }
    Document* clone() const
    {
        std::unique_ptr<Document> doc;
        return clone(doc).release();
    }
    #endif // SWIG

    /*!
     * Factory-type method for creating a new Element
     * \param qname The qname of the new element
     * \param uri The URI of the new element
     * \param characterData The character data (if any)
     * \return A new element
     */
    Element *createElement(const std::string & qname, const std::string & uri, std::string characterData = "");
    #ifndef SWIG // SWIG doesn't like std::unique_ptr
    std::unique_ptr<Element> createElement(const xml::lite::QName&, const std::string& characterData) const;
    std::unique_ptr<Element> createElement(const xml::lite::QName&, const coda_oss::u8string& characterData) const;
    #endif // SWIG

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
    void insert(Element * element, Element * underThis);

    /*!
     * Remove an element from the tree, starting at the root
     * \param toDelete The node to delete (This DOES do deletion)
     */
    void remove(Element * toDelete);

    /*!
     * Remove an element from the tree, starting at the second param
     * \param toDelete The node to delete (This DOES do deletion)
     * \param fromHere The place to start looking for.  This could
     * be an optimization depending on the task, so I allow it to remain
     * public
     */
    void remove(Element * toDelete, Element * fromHere);

    /*!
     * Sets the internal root element
     * \param element The node to set.
     */
    void setRootElement(Element * element, bool own = true);
    #ifndef SWIG // SWIG doesn't like std::unique_ptr
    void setRootElement(std::unique_ptr<Element>&& element) // implicitly own=true
    {
        setRootElement(element.release(), true /*own*/);
    }
    #endif // SWIG

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
    #ifndef SWIG // SWIG doesn't like std::unique_ptr
    std::unique_ptr<Element>& getRootElement(std::unique_ptr<Element>& rootNode) // implicitly steal=true
    {
        rootNode.reset(getRootElement(true /*steal*/));
        return rootNode;
    }
    #endif // SWIG
    Element *getRootElement() const
    {
        return mRootNode;
    }

private:
    Document(const Document&);
    Document& operator=(const Document&);

    //! The root node element
    Element *mRootNode;
    bool mOwnRoot;
};

inline Element& getRootElement(Document& doc)
{
    auto retval = doc.getRootElement();
    assert(retval != nullptr);
    return *retval;
}
inline const Element& getRootElement(const Document& doc)
{
    auto retval = doc.getRootElement();
    assert(retval != nullptr);
    return *retval;
}
inline Element& getRootElement(Document* pDoc)
{
    assert(pDoc != nullptr);
    return getRootElement(*pDoc);
}
inline const Element& getRootElement(const Document* pDoc)
{
    assert(pDoc != nullptr);
    return getRootElement(*pDoc);
}

}
}

#endif // CODA_OSS_xml_lite_Doocument_h_INCLUDED_

