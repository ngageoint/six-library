/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#ifndef __XML_LITE_SERIALIZABLE_H__
#define __XML_LITE_SERIALIZABLE_H__

#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "io/Serializable.h"
#include "xml/lite/MinidomParser.h"

/*!
 *  \file Serializable.h
 *  \brief Provides the class for creating and processing XML documents
 *
 *  This file contains the serializable class for XML documents
 */
namespace xml
{
namespace lite
{
/*!
 *  \class Serializable
 *  \brief Document building and processing class
 *  
 *  This class provides a Serializable interface for XML documents.
 *  Internally, it has a parser for XML, and it has a "document"
 *  containing a tree representation for all of its nodes.  Note that
 *  all nodes in a document are dynamically allocated.  They are deleted
 *  by the class at destruction time.
 */
class Serializable : public io::Serializable
{
public:
    //! Constructor
    Serializable() {}

    Serializable(Document* document, bool own = true)
    {
        setDocument(document, own);
    }

    //! Destructor
    virtual ~Serializable() {}

    /*!
     *  Return in the document as it is.  Consider throwing null-pointer
     *  ref upon null.
     *  \return A Document
     */
    virtual Document *getDocument() const
    {
        return mParser.getDocument();
    }

    virtual Document *getDocument(bool steal = false)
    {
        return mParser.getDocument(steal);
    }

    /*!
     *  This is the public interface for resetting the
     *  XML document.  This will call the handler version of this
     *  function, which will delete the old document.
     *
     *  \param newDocument The new document.
     */
    void setDocument(Document* document, bool own = true)
    {
        mParser.setDocument(document, own);
    }

    /*!
     *  Transfer this object into a byte stream
     *  \param os The object to serialize this to
     */
    virtual void serialize(io::OutputStream& os);

    /*!
     * Unpack this input stream to the object
     * \param is  Stream to read object from
     */
    virtual void deserialize(io::InputStream& is);

protected:
    //! The parser
    ::xml::lite::MinidomParser mParser;
};
}
}

#endif

