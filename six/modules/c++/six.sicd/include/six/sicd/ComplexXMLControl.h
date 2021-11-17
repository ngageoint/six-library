/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_COMPLEX_XML_CONTROL_H__
#define __SIX_COMPLEX_XML_CONTROL_H__

#include <memory>

#include <six/XMLControl.h>
#include <six/Enums.h>

#include <six/sicd/ComplexXMLParser.h>

namespace six
{
namespace sicd
{
/*!
 *  \class ComplexXMLControl
 *  \brief Turn a ComplexData object into XML or vice-versa
 *
 *  This class converts a ComplexData* object into a SICD XML
 *  Document Object Model (DOM).  The XMLControlFactory can be used
 *  to produce either SICD XML or SIDD XML for each Data* within a node.
 *
 *  Direct use of this class is discouraged, but publicly available.
 *  The best practice is to call six::toXMLString().
 *
 *  To use this class generally, consider using the 
 *  XMLControlFactory::newXMLControl() methods
 *  to create this object.
 */
struct ComplexXMLControl : public XMLControl
{
    //!  Constructor
    ComplexXMLControl(logging::Logger* log = nullptr, bool ownLog = false);
    ComplexXMLControl(const ComplexXMLControl&) = delete;
    ComplexXMLControl& operator=(const ComplexXMLControl&) = delete;

    static const six::DataType dataType;

protected:
    /*!
     *  This function takes in a ComplexData object and converts
     *  it to a new-allocated XML DOM.
     *
     *  \param data A ComplexData object
     *  \return An XML DOM
     */
    virtual xml::lite::Document* toXMLImpl(const Data* data);

    /*!
     *  Function takes a DOM Document* node and creates a new-allocated
     *  ComplexData* populated by the DOM.  
     *
     *  
     */
    virtual Data* fromXMLImpl(const xml::lite::Document* doc);

private:
    std::unique_ptr<ComplexXMLParser>
    getParser(const std::string& version) const;
};
}
}

#endif

