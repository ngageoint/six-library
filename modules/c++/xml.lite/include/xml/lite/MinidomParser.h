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

#ifndef __XML_LITE_MINIDOM_PARSER_H__
#define __XML_LITE_MINIDOM_PARSER_H__

#include "xml/lite/XMLReader.h"
#include "xml/lite/Document.h"
#include "xml/lite/Element.h"
#include "xml/lite/MinidomHandler.h"

/*!
 * \file MinidomParser.h
 * \brief This is the API for the minidom parser.
 *
 * The MinidomParser class is a simple DOM parser.  Much of the DOM
 * functionality is not present, however, it is sufficient for simple
 * SOAP/WSDL parsing, etc.  The idea is that a parser has a content handler,
 * and a SAX driver underneath, and that we can switch element types by
 * overloading the underlying Document class, and binding the document class
 * in the handler to our new class.
 */

namespace xml
{
namespace lite
{
/*!
 * \class MinidomParser
 * \brief Almost painfully simple DOM parser.
 *
 * This class provides a simple, quick DOM parser, with none of the
 * bloat of the spec.  It was inspired by python's xml.dom.minidom
 * module.
 */
class MinidomParser
{
public:
    /*!
     *  Constructor.  Set our SAX ContentHandler.
     */
    MinidomParser();

    //! Destructor.
    virtual ~MinidomParser()
    {
    }

    /*!
     *  Present our parsing interface.  Similar to DOM, the input
     *  is an InputStream (DOM's is called input source), but it
     *  is even more flexible, and works within the XPC io paradigm.
     *  \param is  This is the input stream to feed the parser
     *  \param size  This is the size of the stream to feed the parser
     */
    virtual void parse(io::InputStream& is, int size = io::InputStream::IS_END);
    

    /*!
     *  This clears the MinidomHandler, killing its underlying Document
     *  tree.  The Document node is preserved, however -- it must
     *  be explicitly reset to another document to change element type.
     */
    virtual void clear();

    /*!
     *  Return a pointer to the document.  Note that its a reference
     *  so you dont get to keep it.
     *  \return Pointer to document.
     */
    virtual Document *getDocument() const;

    virtual Document *getDocument(bool steal = false);

    /*!
     *  Reader accessor
     *  \return The reader by constant reference
     */
    const XMLReader& getReader() const
    {
        return mReader;
    }

    /*!
     *  Reader accessor
     *  \return The reader by reference
     */
    XMLReader& getReader()
    {
        return mReader;
    }

    /*!
     *  This is the public interface for resetting the
     *  XML document.  This will call the handler version of this
     *  function, which will delete the old document.
     *
     *  \param newDocument The new document.
     */
    virtual void setDocument(Document * newDocument, bool own = true);

    /*!
     * @see MinidomHandler::preserveCharacterData
     */
    virtual void preserveCharacterData(bool preserve);

protected:
    MinidomHandler mHandler;
    XMLReader mReader;
};
}
}

#endif
