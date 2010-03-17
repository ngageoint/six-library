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
#ifndef __SIX_COMPLEX_XML_CONTROL_H__
#define __SIX_COMPLEX_XML_CONTROL_H__

#include "six/XMLControl.h"
#include "six/sicd/ComplexData.h"
#include <import/xml/lite.h>

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
 *  The best practice is to call six::toXMLCharArray() or six::toXMLString().
 *
 *  To use this class generally, consider using the 
 *  XMLControlFactory::newXMLControl() methods
 *  to create this object.
 */
class ComplexXMLControl: public XMLControl
{

public:
    //!  Constructor
    ComplexXMLControl()
    {
        mURI = "urn:SICD:0.3.1";
    }

    //!  Destructor
    virtual ~ComplexXMLControl()
    {
    }


    /*!
     *  This function takes in a ComplexData object and converts
     *  it to a new-allocated XML DOM.
     *
     *  \param data Either a ComplexData or DerivedData object
     *  \return An XML DOM
     */
    virtual xml::lite::Document* toXML(Data* data);

    /*!
     *  Function takes a DOM Document* node and creates a new-allocated
     *  ComplexData* populated by the DOM.  
     *
     *  
     */
    virtual Data* fromXML(xml::lite::Document* doc);

protected:

    xml::lite::Element* collectionInfoToXML(xml::lite::Document* doc,
            CollectionInformation *obj);

    xml::lite::Element* imageCreationToXML(xml::lite::Document* doc,
            ImageCreation *obj);

    xml::lite::Element
    * imageDataToXML(xml::lite::Document* doc, ImageData *obj);

    xml::lite::Element* geoDataToXML(xml::lite::Document* doc, GeoData *obj);

    xml::lite::Element* geoInfoToXML(xml::lite::Document* doc, GeoInfo *obj);

    xml::lite::Element* gridToXML(xml::lite::Document* doc, Grid *obj);

    xml::lite::Element* timelineToXML(xml::lite::Document* doc, Timeline *obj);

    xml::lite::Element* positionToXML(xml::lite::Document* doc, Position *obj);

    xml::lite::Element* radarCollectionToXML(xml::lite::Document* doc,
            RadarCollection *obj);

    xml::lite::Element* imageFormationToXML(xml::lite::Document* doc,
            ImageFormation *obj);

    xml::lite::Element* scpcoaToXML(xml::lite::Document* doc, SCPCOA *obj);

    xml::lite::Element* antennaToXML(xml::lite::Document* doc, Antenna *obj);

    xml::lite::Element* antennaParametersToXML(xml::lite::Document* doc,
            std::string name, AntennaParameters *obj);

    xml::lite::Element
    * matchInfoToXML(xml::lite::Document* doc, MatchInformation *obj);

    xml::lite::Element* pfaToXML(xml::lite::Document* doc, PFA *obj);
    xml::lite::Element* areaLineDirectionParametersToXML(xml::lite::Document* doc,
            std::string name, AreaDirectionParameters *obj);
    xml::lite::Element* areaSampleDirectionParametersToXML(xml::lite::Document* doc,
            std::string name, AreaDirectionParameters *obj);

    void xmlToCollectionInfo(xml::lite::Element* collectionInfoXML,
            CollectionInformation *obj);
    void xmlToImageCreation(xml::lite::Element* imageCreationXML,
            ImageCreation *obj);
    void xmlToImageData(xml::lite::Element* imageDataXML, ImageData *obj);
    void xmlToGeoData(xml::lite::Element* geoDataXML, GeoData *obj);
    void xmlToGeoInfo(xml::lite::Element* geoInfoXML, GeoInfo *obj);
    void xmlToGrid(xml::lite::Element* gridXML, Grid *obj);
    void xmlToTimeline(xml::lite::Element* timelineXML, Timeline *obj);
    void xmlToPosition(xml::lite::Element* positionXML, Position *obj);
    void xmlToRadarCollection(xml::lite::Element* radarCollectionXML,
            RadarCollection *obj);
    void xmlToImageFormation(xml::lite::Element* imageFormationXML,
            ImageFormation *obj);
    void xmlToSCPCOA(xml::lite::Element* scpcoaXML, SCPCOA *obj);
    void xmlToAntenna(xml::lite::Element* antennaXML, Antenna *obj);
    void xmlToAntennaParams(xml::lite::Element* antennaParamsXML,
                            AntennaParameters* params);

    void xmlToMatchInfo(xml::lite::Element* matchInfoXML,
                        MatchInformation *obj);
    
    void xmlToPFA(xml::lite::Element* pfaXML, PFA *obj);

    std::vector<LatLon> parseFootprint(xml::lite::Element* footprint,
                                       std::string cornerName, 
                                       bool alt);

    xml::lite::Element* createFootprint(xml::lite::Document* doc,
                                        std::string name, 
                                        std::string cornerName, 
                                        const std::vector<LatLon>& corners, 
                                        bool alt = false);


};

}
}
#endif
