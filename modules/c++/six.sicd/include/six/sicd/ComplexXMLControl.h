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
    ComplexXMLControl(logging::Logger* log = NULL) :
        XMLControl(log)
    {
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

    typedef xml::lite::Element* XMLElem;

    static const char SICD_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    XMLElem collectionInfoToXML(CollectionInformation *obj, XMLElem parent =
            NULL);

    XMLElem imageCreationToXML(ImageCreation *obj, XMLElem parent = NULL);

    XMLElem imageDataToXML(ImageData *obj, XMLElem parent = NULL);

    XMLElem geoDataToXML(GeoData *obj, XMLElem parent = NULL);

    XMLElem geoInfoToXML(GeoInfo *obj, XMLElem parent = NULL);

    XMLElem gridToXML(Grid *obj, XMLElem parent = NULL);

    XMLElem timelineToXML(Timeline *obj, XMLElem parent = NULL);

    XMLElem positionToXML(Position *obj, XMLElem parent = NULL);

    XMLElem radarCollectionToXML(RadarCollection *obj, XMLElem parent = NULL);

    XMLElem imageFormationToXML(ImageFormation *obj, XMLElem parent = NULL);

    XMLElem scpcoaToXML(SCPCOA *obj, XMLElem parent = NULL);

    XMLElem antennaToXML(Antenna *obj, XMLElem parent = NULL);

    XMLElem antennaParametersToXML(std::string name, AntennaParameters *obj,
                                   XMLElem parent = NULL);

    XMLElem matchInfoToXML(MatchInformation *obj, XMLElem parent = NULL);

    XMLElem pfaToXML(PFA *obj, XMLElem parent = NULL);
    XMLElem rmaToXML(RMA *obj, XMLElem parent = NULL);
    XMLElem areaLineDirectionParametersToXML(std::string name,
                                             AreaDirectionParameters *obj,
                                             XMLElem parent = NULL);
    XMLElem areaSampleDirectionParametersToXML(std::string name,
                                               AreaDirectionParameters *obj,
                                               XMLElem parent = NULL);

    void xmlToCollectionInfo(XMLElem collectionInfoXML,
                             CollectionInformation *obj);
    void xmlToImageCreation(XMLElem imageCreationXML, ImageCreation *obj);
    void xmlToImageData(XMLElem imageDataXML, ImageData *obj);
    void xmlToGeoData(XMLElem geoDataXML, GeoData *obj);
    void xmlToGeoInfo(XMLElem geoInfoXML, GeoInfo *obj);
    void xmlToGrid(XMLElem gridXML, Grid *obj);
    void xmlToTimeline(XMLElem timelineXML, Timeline *obj);
    void xmlToPosition(XMLElem positionXML, Position *obj);
    void xmlToRadarCollection(XMLElem radarCollectionXML, RadarCollection *obj);
    void xmlToImageFormation(XMLElem imageFormationXML, ImageFormation *obj);
    void xmlToSCPCOA(XMLElem scpcoaXML, SCPCOA *obj);
    void xmlToAntenna(XMLElem antennaXML, Antenna *obj);
    void
    xmlToAntennaParams(XMLElem antennaParamsXML, AntennaParameters* params);

    void
    xmlToMatchInfo(XMLElem matchInfoXML, MatchInformation *obj);

    void xmlToPFA(XMLElem pfaXML, PFA *obj);
    void xmlToRMA(XMLElem rmaXML, RMA *obj);

    void parseFootprint(XMLElem footprint, std::string cornerName, std::vector<
            LatLon>& value, bool alt);

    XMLElem createFFTSign(std::string name, six::FFTSign sign, XMLElem parent =
            NULL);
    XMLElem createFootprint(std::string name, std::string cornerName,
                            const std::vector<LatLon>& corners, bool alt =
                                    false, XMLElem parent = NULL);

    // Overridden methods from the base XMLControl, for adding attributes
    XMLElem createString(std::string name, std::string uri, std::string p = "",
                         XMLElem parent = NULL);
    XMLElem createInt(std::string name, std::string uri, int p = 0,
                      XMLElem parent = NULL);
    XMLElem createDouble(std::string name, std::string uri, double p = 0,
                         XMLElem parent = NULL);
    XMLElem createBooleanType(std::string name, std::string uri, BooleanType b,
                              XMLElem parent = NULL);
    XMLElem createDateTime(std::string name, std::string uri, std::string s,
                           XMLElem parent = NULL);
    XMLElem createDateTime(std::string name, std::string uri, DateTime p,
                           XMLElem parent = NULL);
    XMLElem createDate(std::string name, std::string uri, DateTime p,
                       XMLElem parent = NULL);
    XMLElem createString(std::string name, std::string p = "", XMLElem parent =
            NULL);
    XMLElem createInt(std::string name, int p = 0, XMLElem parent = NULL);
    XMLElem createDouble(std::string name, double p = 0, XMLElem parent = NULL);
    XMLElem createBooleanType(std::string name, BooleanType b, XMLElem parent =
            NULL);
    XMLElem createDateTime(std::string name, std::string s, XMLElem parent =
            NULL);
    XMLElem createDateTime(std::string name, DateTime p, XMLElem parent = NULL);
    XMLElem createDate(std::string name, DateTime p, XMLElem parent = NULL);

};

}
}
#endif
