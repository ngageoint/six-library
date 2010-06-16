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
        mURI = "urn:SIDD:0.2";
    }

    virtual ~DerivedXMLControl()
    {
    }

    /*!
     *  Returns a new allocated DOM document, created from the DerivedData*
     */
    virtual xml::lite::Document* toXML(Data* data);
    /*!
     *  Returns a new allocated DerivedData*, created from the DOM Document*
     *
     */
    virtual Data* fromXML(xml::lite::Document* doc);

protected:
    typedef xml::lite::Element* XMLElem;

    virtual XMLElem createLUT(std::string name, LUT *l, XMLElem parent = NULL);
    XMLElem toXML(ProductCreation* productCreation, XMLElem parent = NULL);
    XMLElem toXML(ProductProcessing* productProcessing, XMLElem parent = NULL);
    void fromXML(XMLElem elem, ProductProcessing* productProcessing);
    XMLElem toXML(ProcessingModule* procMod, XMLElem parent = NULL);
    void fromXML(XMLElem elem, ProcessingModule* procMod);
    XMLElem toXML(DownstreamReprocessing* d, XMLElem parent = NULL);
    void fromXML(XMLElem elem, DownstreamReprocessing* downstreamReproc);
    XMLElem toXML(Display* display, XMLElem parent = NULL);
    XMLElem toXML(GeographicAndTarget* g, XMLElem parent = NULL);
    XMLElem toXML(GeographicCoverage* g, XMLElem parent = NULL);
    XMLElem toXML(Measurement* measurement, XMLElem parent = NULL);
    XMLElem toXML(ExploitationFeatures* exFeatures, XMLElem parent = NULL);
    XMLElem toXML(Annotation *a, XMLElem parent = NULL);
    void fromXML(XMLElem productCreationXML, ProductCreation* productCreation);
    void fromXML(XMLElem displayXML, Display* display);
    void fromXML(XMLElem measurementXML, Measurement* measurement);
    void fromXML(XMLElem elem, GeographicAndTarget* geographicAndTarget);
    void fromXML(XMLElem elem, GeographicCoverage* geoCoverage);
    void fromXML(XMLElem elem, ExploitationFeatures* exFeatures);
    void fromXML(XMLElem annotationXML, Annotation *a);
    void fromXML(XMLElem elem, SFAGeometry *g);
    XMLElem toXML(SFAGeometry *g, std::string useName, XMLElem parent = NULL);
};

}
}
#endif
