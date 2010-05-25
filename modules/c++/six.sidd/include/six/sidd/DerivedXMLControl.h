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

    DerivedXMLControl(logging::Logger* log = NULL) : XMLControl(log)
    {
        mURI = "urn:SIDD:0.0.4";
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
    //    virtual xml::lite::Element* createLUT3(
    //            std::string name, char *lutTable, size_t numElems, xml::lite::Element* parent = NULL);
    virtual xml::lite::Element* createLUT(std::string name, LUT *lut,
                                          xml::lite::Element* parent = NULL);

    xml::lite::Element* productCreationToXML(ProductCreation* productCreation,
                                             xml::lite::Element* parent = NULL);

    xml::lite::Element
            * productProcessingToXML(ProductProcessing* productProcessing,
                                     xml::lite::Element* parent = NULL);
    void xmlToProductProcessing(xml::lite::Element* elem,
                                ProductProcessing* productProcessing);

    xml::lite::Element
            * processingModuleToXML(ProcessingModule* procMod,
                                    xml::lite::Element* parent = NULL);
    void xmlToProcessingModule(xml::lite::Element* elem,
                               ProcessingModule* procMod);

    xml::lite::Element
            * downstreamReprocessingToXML(
                                          DownstreamReprocessing* downstreamReproc,
                                          xml::lite::Element* parent = NULL);
    void xmlToDownstreamReprocessing(xml::lite::Element* elem,
                                     DownstreamReprocessing* downstreamReproc);

    xml::lite::Element* displayToXML(Display* display,
                                     xml::lite::Element* parent = NULL);

    xml::lite::Element
            * geographicAndTargetToXML(
                                       GeographicAndTarget* geographicAndTarget,
                                       xml::lite::Element* parent = NULL);

    xml::lite::Element
            * geographicCoverageToXML(GeographicCoverage* geographicCoverage,
                                      xml::lite::Element* parent = NULL);

    xml::lite::Element* measurementToXML(Measurement* measurement,
                                         xml::lite::Element* parent = NULL);

    xml::lite::Element
            * exploitationFeaturesToXML(
                                        ExploitationFeatures* exploitationFeatures,
                                        xml::lite::Element* parent = NULL);

    void xmlToProductCreation(xml::lite::Element* productCreationXML,
                              ProductCreation* productCreation);

    void xmlToDisplay(xml::lite::Element* displayXML, Display* display);

    void xmlToMeasurement(xml::lite::Element* measurementXML,
                          Measurement* measurement);

    void xmlToGeographicAndTarget(xml::lite::Element* geographicAndTargetXML,
                                  GeographicAndTarget* geographicAndTarget);

    void xmlToGeographicCoverage(xml::lite::Element* geographicCoverageXML,
                                 GeographicCoverage* geographicCoverage);

    void xmlToExploitationFeatures(xml::lite::Element* exploitationFeaturesXML,
                                   ExploitationFeatures* exploitationFeatures);
};

}
}
#endif
