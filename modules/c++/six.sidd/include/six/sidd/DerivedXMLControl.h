/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_DERIVED_XML_CONTROL_H__
#define __SIX_DERIVED_XML_CONTROL_H__

#include "six/XMLControl.h"
#include "six/sidd/DerivedData.h"
#include "six/sidd/SFA.h"
#include <import/xml/lite.h>

namespace six
{
namespace sidd
{
/*!
 *  \class DerivedXMLControl
 *  \brief Turns an DerivedData object into XML and vice versa
 *
 *  Derived XMLControl object for reading and writing DerivedData*
 *  The XMLFactory should be used to create this object, and only
 *  if necessary.  A best practice is to use the six::toXMLCharArray
 *  and six::toXMLString functions to turn Data* objects into XML
 */
class DerivedXMLControl : public XMLControl
{

public:

    DerivedXMLControl(logging::Logger* log = NULL) :
        XMLControl(log)
    {
    }

    virtual ~DerivedXMLControl()
    {
    }

    /*!
     *  Returns a new allocated DOM document, created from the DerivedData*
     */
    virtual xml::lite::Document* toXML(const Data* data);
    /*!
     *  Returns a new allocated DerivedData*, created from the DOM Document*
     *
     */
    virtual Data* fromXML(const xml::lite::Document* doc);

protected:
    typedef xml::lite::Element* XMLElem;

    static const char SIDD_URI[];
    static const char SI_COMMON_URI[];
    static const char SFA_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    std::string getSFAURI() const;

    virtual XMLElem createLUT(std::string name, const LUT *l, XMLElem parent = NULL);
    XMLElem toXML(const ProductCreation* productCreation, XMLElem parent = NULL);
    XMLElem toXML(const ProductProcessing* productProcessing, XMLElem parent = NULL);
    void fromXML(const XMLElem elem, ProductProcessing* productProcessing);
    XMLElem toXML(const ProcessingModule* procMod, XMLElem parent = NULL);
    void fromXML(const XMLElem elem, ProcessingModule* procMod);
    XMLElem toXML(const DownstreamReprocessing* d, XMLElem parent = NULL);
    void fromXML(const XMLElem elem, DownstreamReprocessing* downstreamReproc);
    XMLElem toXML(const Display* display, XMLElem parent = NULL);
    XMLElem toXML(const GeographicAndTarget* g, XMLElem parent = NULL);
    XMLElem toXML(const GeographicCoverage* g, XMLElem parent = NULL);
    XMLElem toXML(const Measurement* measurement, XMLElem parent = NULL);
    XMLElem toXML(const ExploitationFeatures* exFeatures, XMLElem parent = NULL);
    XMLElem toXML(const Annotation *a, XMLElem parent = NULL);
    void fromXML(const XMLElem productCreationXML, ProductCreation* productCreation);
    void fromXML(const XMLElem displayXML, Display* display);
    void fromXML(const XMLElem measurementXML, Measurement* measurement);
    void fromXML(const XMLElem elem, GeographicAndTarget* geographicAndTarget);
    void fromXML(const XMLElem elem, GeographicCoverage* geoCoverage);
    void fromXML(const XMLElem elem, ExploitationFeatures* exFeatures);
    void fromXML(const XMLElem annotationXML, Annotation *a);
    void fromXML(const XMLElem elem, SFAGeometry *g);
    XMLElem toXML(const SFAGeometry *g, std::string useName, XMLElem parent = NULL);
};

}
}
#endif
