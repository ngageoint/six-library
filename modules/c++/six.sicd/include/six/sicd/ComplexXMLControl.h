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
class ComplexXMLControl : public XMLControl
{

public:
    //!  Constructor
    ComplexXMLControl(logging::Logger* log = NULL) : XMLControl(log)
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

    xml::lite::Element* collectionInfoToXML(CollectionInformation *obj,
                                            xml::lite::Element* parent = NULL);

    xml::lite::Element* imageCreationToXML(ImageCreation *obj,
                                           xml::lite::Element* parent = NULL);

    xml::lite::Element* imageDataToXML(ImageData *obj, xml::lite::Element* parent = NULL);

    xml::lite::Element* geoDataToXML(GeoData *obj, xml::lite::Element* parent =
            NULL);

    xml::lite::Element* geoInfoToXML(GeoInfo *obj, xml::lite::Element* parent =
            NULL);

    xml::lite::Element* gridToXML(Grid *obj, xml::lite::Element* parent = NULL);

    xml::lite::Element* timelineToXML(Timeline *obj,
                                      xml::lite::Element* parent = NULL);

    xml::lite::Element* positionToXML(Position *obj,
                                      xml::lite::Element* parent = NULL);

    xml::lite::Element* radarCollectionToXML(RadarCollection *obj,
                                             xml::lite::Element* parent = NULL);

    xml::lite::Element* imageFormationToXML(ImageFormation *obj,
                                            xml::lite::Element* parent = NULL);

    xml::lite::Element* scpcoaToXML(SCPCOA *obj, xml::lite::Element* parent =
            NULL);

    xml::lite::Element* antennaToXML(Antenna *obj, xml::lite::Element* parent =
            NULL);

    xml::lite::Element* antennaParametersToXML(std::string name,
                                               AntennaParameters *obj,
                                               xml::lite::Element* parent =
                                                       NULL);

    xml::lite::Element
    * matchInfoToXML(MatchInformation *obj, xml::lite::Element* parent = NULL);

    xml::lite::Element* pfaToXML(PFA *obj, xml::lite::Element* parent = NULL);
    xml::lite::Element* rmaToXML(RMA *obj, xml::lite::Element* parent = NULL);
    xml::lite::Element
            * areaLineDirectionParametersToXML(std::string name,
                                               AreaDirectionParameters *obj,
                                               xml::lite::Element* parent =
                                                       NULL);
    xml::lite::Element
            * areaSampleDirectionParametersToXML(std::string name,
                                                 AreaDirectionParameters *obj,
                                                 xml::lite::Element* parent =
                                                         NULL);

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

    void
            xmlToMatchInfo(xml::lite::Element* matchInfoXML,
                           MatchInformation *obj);

    void xmlToPFA(xml::lite::Element* pfaXML, PFA *obj);
    void xmlToRMA(xml::lite::Element* rmaXML, RMA *obj);

    void parseFootprint(xml::lite::Element* footprint, std::string cornerName,
                        std::vector<LatLon>& value, bool alt);

    xml::lite::Element* createFFTSign(std::string name, six::FFTSign sign,
                                      xml::lite::Element* parent = NULL);
    xml::lite::Element* createFootprint(std::string name,
                                        std::string cornerName,
                                        const std::vector<LatLon>& corners,
                                        bool alt = false,
                                        xml::lite::Element* parent = NULL);

};

}
}
#endif
