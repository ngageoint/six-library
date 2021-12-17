/* =========================================================================
 * This file is part of six.convert-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.convert-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_BASE_CONVERTER_H__
#define __SIX_BASE_CONVERTER_H__

#include <string>
#include <memory>

#include <six/sicd/ComplexData.h>
#include <six/XMLParser.h>
#include <xml/lite/Document.h>

namespace six
{
namespace convert
{
struct BaseConverter : protected six::XMLParser
{
    /*!
     * \class BaseConverter
     *
     * Base object for parsing vendor XML to create ComplexData
     */
    BaseConverter();

    BaseConverter(const BaseConverter&) = delete;
    BaseConverter& operator=(const BaseConverter&) = delete;

    /*!
     * Create a ComplexData object from source filetype
     *
     * \return ComplexData corresponding to given file
     */
    virtual mem::auto_ptr<six::sicd::ComplexData> convert() const = 0;

protected:
    static mem::auto_ptr<xml_lite::Document>
            readXML(const std::string& xmlPathname);

    XMLElem findUniqueElement(const xml_lite::Element* root,
            const std::string& xmlPath) const;
    std::vector<XMLElem> findElements(const xml_lite::Element* root,
            const std::string& xmlPath) const;
    size_t findIndex(const std::vector<XMLElem>& elements,
            const std::string& xmlPath, const std::string& value) const;
    size_t findIndexByAttributes(const std::vector<XMLElem>& elements,
            const std::vector<std::string>& attributes,
            const std::vector<std::string>& values) const;
    size_t findIndexByAttribute(const std::vector<XMLElem>& elements,
            const std::string& attribute, const std::string& value) const;

    std::string parseStringFromPath(const xml_lite::Element* root,
            const std::string& xmlPath) const;
    double parseDoubleFromPath(const xml_lite::Element* root,
            const std::string& xmlPath) const;
    size_t parseUIntFromPath(const xml_lite::Element* root,
            const std::string& xmlPath) const;
    six::DateTime parseDateTimeFromPath(const xml_lite::Element* root,
            const std::string& xmlPath) const;

    double sumOverElements(const std::vector<XMLElem>& elements,
            const std::string& xmlPath) const;
};

}
}
#endif

