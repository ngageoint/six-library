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
     *  \param data A ComplexData object
     *  \return An XML DOM
     */
    virtual xml::lite::Document* toXML(const Data* data);

    /*!
     *  Function takes a DOM Document* node and creates a new-allocated
     *  ComplexData* populated by the DOM.  
     *
     *  
     */
    virtual Data* fromXML(const xml::lite::Document* doc);

protected:

    typedef xml::lite::Element* XMLElem;

    static const char SICD_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    XMLElem toXML(const CollectionInformation *obj, XMLElem parent = NULL);
    XMLElem toXML(const ImageCreation *obj, XMLElem parent = NULL);
    XMLElem toXML(const ImageData *obj, XMLElem parent = NULL);
    XMLElem toXML(const GeoData *obj, XMLElem parent = NULL);
    XMLElem toXML(const GeoInfo *obj, XMLElem parent = NULL);
    XMLElem toXML(const Grid *obj, XMLElem parent = NULL);
    XMLElem toXML(const Timeline *obj, XMLElem parent = NULL);
    XMLElem toXML(const Position *obj, XMLElem parent = NULL);
    XMLElem toXML(const RadarCollection *obj, XMLElem parent = NULL);
    XMLElem toXML(const ImageFormation *obj, XMLElem parent = NULL);
    XMLElem toXML(const SCPCOA *obj, XMLElem parent = NULL);
    XMLElem toXML(const Antenna *obj, XMLElem parent = NULL);
    XMLElem toXML(const std::string& name, AntennaParameters *ap,
            XMLElem parent = NULL);
    XMLElem toXML(const MatchInformation *obj, XMLElem parent = NULL);
    XMLElem toXML(const PFA *obj, XMLElem parent = NULL);
    XMLElem toXML(const RMA *obj, XMLElem parent = NULL);

    XMLElem areaLineDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters *obj,
            XMLElem parent = NULL);
    XMLElem areaSampleDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters *obj,
            XMLElem parent = NULL);

    void fromXML(const XMLElem collectionInfoXML, CollectionInformation *obj);
    void fromXML(const XMLElem imageCreationXML, ImageCreation *obj);
    void fromXML(const XMLElem imageDataXML, ImageData *obj);
    void fromXML(const XMLElem geoDataXML, GeoData *obj);
    void fromXML(const XMLElem geoInfoXML, GeoInfo *obj);
    void fromXML(const XMLElem gridXML, Grid *obj);
    void fromXML(const XMLElem timelineXML, Timeline *obj);
    void fromXML(const XMLElem positionXML, Position *obj);
    void fromXML(const XMLElem radarCollectionXML, RadarCollection *obj);
    void fromXML(const XMLElem imageFormationXML, ImageFormation *obj);
    void fromXML(const XMLElem scpcoaXML, SCPCOA *obj);
    void fromXML(const XMLElem antennaXML, Antenna *obj);
    void fromXML(const XMLElem antennaParamsXML, AntennaParameters* params);
    void fromXML(const XMLElem matchInfoXML, MatchInformation *obj);
    void fromXML(const XMLElem pfaXML, PFA *obj);
    void fromXML(const XMLElem rmaXML, RMA *obj);

    void parseEarthModelType(const XMLElem element, EarthModelType& value);
    void parseSideOfTrackType(const XMLElem element, SideOfTrackType& value);

    XMLElem createFFTSign(const std::string& name, six::FFTSign sign,
            XMLElem parent = NULL);
    XMLElem createFootprint(const std::string& name,
                            const std::string& cornerName,
                            const std::vector<LatLon>& corners,
                            XMLElem parent = NULL);
    XMLElem createFootprint(const std::string& name,
                            const std::string& cornerName,
                            const std::vector<LatLonAlt>& corners,
                            XMLElem parent = NULL);
    XMLElem createEarthModelType(const std::string& name,
                                 const EarthModelType& value,
                                 XMLElem parent = NULL);
    XMLElem createSideOfTrackType(const std::string& name,
                                  const SideOfTrackType& value, XMLElem parent =
                                          NULL);

    // Overridden methods from the base XMLControl, for adding attributes
    XMLElem createString(const std::string& name, const std::string& uri,
                         const std::string& p = "", XMLElem parent = NULL);
    XMLElem createInt(const std::string& name, const std::string& uri,
                      int p = 0, XMLElem parent = NULL);
    XMLElem createDouble(const std::string& name, const std::string& uri,
                         double p = 0, XMLElem parent = NULL);
    XMLElem createBooleanType(const std::string& name, const std::string& uri,
                              BooleanType b, XMLElem parent = NULL);
    XMLElem createDateTime(const std::string& name, const std::string& uri,
                           const std::string& s, XMLElem parent = NULL);
    XMLElem createDateTime(const std::string& name, const std::string& uri,
                           DateTime p, XMLElem parent = NULL);
    XMLElem createDate(const std::string& name, const std::string& uri,
                       DateTime p, XMLElem parent = NULL);
    XMLElem createString(const std::string& name, const std::string& p = "",
                         XMLElem parent = NULL);
    XMLElem createInt(const std::string& name, int p = 0,
                      XMLElem parent = NULL);
    XMLElem createDouble(const std::string& name, double p = 0,
                         XMLElem parent = NULL);
    XMLElem createBooleanType(const std::string& name, BooleanType b,
                              XMLElem parent = NULL);
    XMLElem createDateTime(const std::string& name, const std::string& s,
                           XMLElem parent = NULL);
    XMLElem createDateTime(const std::string& name, DateTime p,
                           XMLElem parent = NULL);
    XMLElem createDate(const std::string& name, DateTime p,
                       XMLElem parent = NULL);

};

}
}
#endif
