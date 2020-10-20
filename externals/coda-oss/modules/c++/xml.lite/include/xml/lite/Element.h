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

#ifndef __XML_LITE_ELEMENT_H__
#define __XML_LITE_ELEMENT_H__
#pragma once

#include <io/InputStream.h>
#include <io/OutputStream.h>
#include "xml/lite/XMLException.h"
#include "xml/lite/Attributes.h"

/*!
 * \file  Element.h
 * \brief This contains the xml::lite data type for elements.
 *
 * This class is analogous to its DOM namesake.  It contains all of
 * the information that would be expected of it, including comment data.
 * The API, however, is much simpler and less developed than DOM.
 */

// xml::lite::Element
namespace xml
{
namespace lite
{
/*!
 * \class Element
 * \brief The class defining one element of an XML document
 *
 * This class stores all of the element information about an XML
 * document.
 */
class Element
{
public:
    //! Default constructor
    Element() :
        mParent(NULL)
    {
    }

    /*!
     * Constructor taking the namespace prefix and the local name 
     * \param qname The qname of the object
     * \param uri The uri of the object
     * \param characterData The character data (if any)
     */
    Element(const std::string& qname, const std::string& uri = "",
            std::string characterData = "") :
        mParent(NULL), mName(uri, qname), mCharacterData(characterData)
    {
    }

    //! Destructor
    virtual ~Element()
    {
        destroyChildren();
    }

    //! Destroys any child elements.
    void destroyChildren();

    /*!
     * Copy constructor
     * \param element  Takes an element
     */
    Element(const Element& element);

    /*!
     *  Assignment operator
     *  \param element  Takes an element
     *  \return a reference to *this
     */
    Element& operator=(const Element& element);

    /*!
     *  Clone function performs deep copy
     *  of element
     *  \param element  Takes an element
     */
    void clone(const Element& element);

    /*!
     *  Get the attributes in a non-const way
     *  \return attributes
     */
    Attributes& getAttributes()
    {
        return mAttributes;
    }

    std::string& attribute(const std::string& s)
    {
        return mAttributes[s];
    }

    /*!
     *  Get the attributes in a const way
     *  \return attributes
     */
    const Attributes& getAttributes() const
    {
        return mAttributes;
    }

    /*!
     *  Set the attributes
     *  \param attributes The attributes to set
     */
    void setAttributes(const Attributes& attributes)
    {
        mAttributes.clear();
        for (int i = 0; i < attributes.getLength(); i++)
        {
            mAttributes.add(attributes.getNode(i));
        }
    }

    /*!
     *  Get the elements by tag name
     *  \param qname the QName
     *  \param elements the elements that match the QName
     */
    void getElementsByTagNameNS(const std::string& qname,
                                std::vector<Element*>& elements,
                                bool recurse = false) const;

    /*!
     *  Utility for people that dont like to pass by reference
     *
     */
    std::vector<Element*> getElementsByTagNameNS(const std::string& qname,
                                                 bool recurse = false) const
    {
        std::vector<Element*> v;
        getElementsByTagNameNS(qname, v, recurse);
        return v;
    }

    /*!
     *  Sometimes we dont care about the qname or the uri -- just 
     *  the local name is good enough.  For those times, use this function
     *  \param localName The local name
     *  \param elements The elements
     */
    void getElementsByTagName(const std::string& localName,
                              std::vector<Element*>& elements,
                              bool recurse = false) const;

    /*!
     *  Utility for people that dont like to pass by reference
     */
    std::vector<Element*> getElementsByTagName(const std::string& localName,
                                               bool recurse = false) const
    {
        std::vector<Element*> v;
        getElementsByTagName(localName, v, recurse);
        return v;
    }

    /*!
     *  Get the elements by tag name
     *  \param uri the URI
     *  \param localName the local name
     *  \param elements the elements that match the QName
     */
    void getElementsByTagName(const std::string& uri,
                              const std::string& localName,
                              std::vector<Element*>& elements,
                              bool recurse = false) const;

    /*!
     *  1)  Find this child's attribute and change it
     *  2)  Recursively descend over children and fix all
     *  namespaces below using fixNodeNamespace()
     */
    void setNamespacePrefix(std::string prefix, std::string uri);

    void setNamespaceURI(std::string prefix, std::string uri);

    /*!
     *  Prints the element to the specified OutputStream
     *  \param stream the OutputStream to write to
     *  \param formatter  The formatter
     *  \todo Add format capability
     */
    void print(io::OutputStream& stream, const io::TextEncoding* pEncoding = nullptr) const;

    void prettyPrint(io::OutputStream& stream, const std::string& formatter, io::TextEncoding encoding) const
    {
        prettyPrint(stream, formatter, &encoding);
    }
    void prettyPrint(io::OutputStream& stream, const std::string& formatter = "    ") const
    {
        prettyPrint(stream, formatter, nullptr /*pEncoding*/);
    }
    void prettyPrint(io::OutputStream& stream, io::TextEncoding encoding) const
    {
        prettyPrint(stream, "    ", encoding);
    }

    /*!
     *  Determines if a child element exists
     *  \param localName the local name to search for
     *  \return true if it exists, false if not
     */
    bool hasElement(const std::string& localName) const;

    /*!
     *  Determines if a child element exists
     *  \param uri the URI to search for
     *  \param localName the local name to search for
     *  \return true if it exists, false if not
     */
    bool hasElement(const std::string& uri,
                    const std::string& localName) const;

    /*!
     *  Returns the character data of this element.
     *  \return the charater data
     */
    std::string getCharacterData() const
    {
        return mCharacterData;
    }

    /*!
     *  Sets the character data for this element.
     *  \param characters The data to add to this element
     */
    void setCharacterData(const std::string& characters)
    {
        mCharacterData = characters;
    }

    /*!
     *  Sets the local name for this element.
     *  \param localName the data to add to this element
     */
    void setLocalName(const std::string& localName)
    {
        mName.setName(localName);
    }

    /*!
     *  Returns the local name of this element.
     *  \return the local name
     */
    std::string getLocalName() const
    {
        return mName.getName();
    }

    /*!
     *  Sets the QName for this element.
     *  \param qname the data to add to this element
     */
    void setQName(const std::string& qname)
    {
        mName.setQName(qname);
    }

    /*!
     *  Returns the QName of this element.
     *  \return the QName
     */
    std::string getQName() const
    {
        return mName.toString();
    }

    /*!
     *  Sets the URI for this element.
     *  \param uri the data to add to this element
     */
    void setUri(const std::string& uri)
    {
        mName.setAssociatedUri(uri);
    }

    /*!
     *  Returns the URI of this element.
     *  \return the URI
     */
    std::string getUri() const
    {
        return mName.getAssociatedUri();
    }

    /*!
     *  Adds a child element to this element
     *  \param node the child element to add
     */
    virtual void addChild(Element * node);

    /*!
     *  Adds a child element to this element
     *  \param node the child element to add
     */
    virtual void addChild(std::unique_ptr <Element>&& node);

    /*!
     *  Returns all of the children of this element
     *  \return the children of this element
     */
    std::vector<Element*>& getChildren()
    {
        return mChildren;
    }

    /*!
     *  Returns all of the children of this element
     *  \return the children of this element
     */
    const std::vector<Element*>& getChildren() const
    {
        return mChildren;
    }

    Element* getParent() const
    {
        return mParent;
    }

    void setParent(Element* parent)
    {
        mParent = parent;
    }

protected:

    void changePrefix(Element* element,
                      const std::string& prefix,
                      const std::string& uri);

    void changeURI(Element* element,
                   const std::string& prefix,
                   const std::string& uri);

    void depthPrint(io::OutputStream& stream, int depth,
                    const std::string& formatter, const io::TextEncoding* pEncoding = nullptr) const;
    
    Element* mParent;
    //! The children of this element
    std::vector<Element*> mChildren;
    xml::lite::QName mName;
    //! The attributes for this element
    xml::lite::Attributes mAttributes;
    //! The character data
    std::string mCharacterData;

private:
    void prettyPrint(io::OutputStream& stream, const std::string& formatter, const io::TextEncoding* pEncoding) const;

};
}
}

#endif
