/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_XML_CONTROL_H__
#define __SIX_XML_CONTROL_H__

#include <xml/lite/Document.h>
#include <xml/lite/Validator.h>
#include <logging/Logger.h>
#include <six/Data.h>

namespace six
{

/*!
 *  \class XMLControl
 *  \brief Base class for reading and writing a Data object
 *
 *  This interfaces' implementors are not typically called directly.
 *  Instead, they are used to convert back and forth from XML during
 *  file format reads and writes.
 *
 *  The XMLControl is derived for each Data type that is supported
 *  in this library.  This class does not actually convert data into
 *  a bytes, although the helper functions in the XMLControlFactory do.
 *
 *  Instead, this class defines the interface for going between a data
 *  model, represented by the Data object, to an XML Document Object Model
 *  (DOM), and back.
 *
 *  This interface's implementors are used by the ReadControl and the
 *  WriteControl to bundle the XML content into a file container.
 *
 *  They can also be used to interact with an XML model or a stub XML
 *  file as well.
 */
class XMLControl
{

public:
    //!  Constructor
    XMLControl(logging::Logger* log = NULL, bool ownLog = false);

    //!  Destructor
    virtual ~XMLControl();

    void setLogger(logging::Logger* log, bool ownLog = false);

    /*!
     *  Convert the Data model into an XML DOM.
     *  \param data         Data structure
     *  \param schemaPaths  Directories or files of schema locations
     *  \return An XML DOM
     */
    xml::lite::Document* toXML(const Data* data,
                               const std::vector<std::string>& schemaPaths);

    /*!
     *  Convert a document from a DOM into a Data model
     *  \param doc          XML Document
     *  \param schemaPaths  Directories or files of schema locations
     *  \return a Data model
     */
    Data* fromXML(const xml::lite::Document* doc,
                  const std::vector<std::string>& schemaPaths);

    /*!
     *  Provides a mapping from COMPLEX --> SICD and DERIVED --> SIDD
     */
    static
    std::string dataTypeToString(DataType dataType, bool appendXML = true);

    /*!
     * Split version string into multiple period-separated parts
     * \param versionStr Version string containing (at minimum) major
     *  and minor parts separated by '.'
     * \param[out] Version string components <major, minor, ...>
     */
    static
    void splitVersion(const std::string& versionStr,
                      std::vector<std::string>& version);

protected:
    logging::Logger *mLog;
    bool mOwnLog;

    /*!
     *  Convert a document from a DOM into a Data model
     *  \param doc
     *  \return a Data model
     */
    virtual Data* fromXMLImpl(const xml::lite::Document* doc) = 0;

    /*!
     *  Convert the Data model into an XML DOM.
     *  \param data the Data model
     *  \return An XML DOM
     */
    virtual xml::lite::Document* toXMLImpl(const Data* data) = 0;

    static
    std::string getDefaultURI(const Data& data);

    static
    std::string getVersionFromURI(const xml::lite::Document* doc);

    static
    void getVersionFromURI(const xml::lite::Document* doc,
                           std::vector<std::string>& version);
};

}

#endif

