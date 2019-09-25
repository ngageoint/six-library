/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD_CPHD_XML_CONTROL_H__
#define __CPHD_CPHD_XML_CONTROL_H__

#include <memory>

#include <mem/SharedPtr.h>
#include <logging/Logger.h>
#include <xml/lite/Element.h>
#include <xml/lite/Document.h>
#include <six/XMLParser.h>
#include <six/SICommonXMLParser10x.h>
#include <cphd/CollectionID.h>
#include <cphd/SceneCoordinates.h>
#include <cphd/Data.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/ReferenceGeometry.h>

namespace cphd
{
//TODO: Seperate From XML and To XML files?

/*!
 *  This class converts a Metadata object into a CPHD XML
 *  Document Object Model (DOM).
 */
class CPHDXMLControl : public six::XMLParser
{
public:
    //!  Constructor
    CPHDXMLControl();

    //! Constructor
    CPHDXMLControl(logging::Logger* log, bool ownLog);

    //! Constructor
    CPHDXMLControl(logging::Logger* log, bool ownLog, std::vector<std::string>& schemaPaths);

    // To string
    std::string toXMLString(const Metadata& metadata);

    std::auto_ptr<Metadata> fromXML(const xml::lite::Document* doc);

    //! (Testing) Parses specifc XML blocks specified by nodeNames vector
    std::auto_ptr<Metadata> fromXML(const xml::lite::Document* doc, const std::vector<std::string>& nodeNames);

    std::auto_ptr<Metadata> fromXML(const std::string& xmlString);

    std::auto_ptr<xml::lite::Document> toXML(const Metadata& metadata);


    //getter
    const std::vector<std::string>& getSchemaPaths()
    {
        return mSchemaPaths;
    }


    //! Validate the xml and log any errors
    //  NOTE: Errors are treated as detriments to valid processing
    //        and fail accordingly
    static void validate(const xml::lite::Document* doc,
              const std::vector<std::string>& schemaPaths,
              logging::Logger* log);

    // size_t getXMLsize(const Metadata& metadata);

private:
    typedef xml::lite::Element* XMLElem;

private:
    static const char CPHD10_URI[];

    //! Returns the default URI
    std::string getDefaultURI() const;

    //! Returns the URI to use with SI Common types
    std::string getSICommonURI() const;

    // Write to XML object
    XMLElem toXML(const CollectionID& obj, XMLElem parent = NULL);
    XMLElem toXML(const Global& obj, XMLElem parent = NULL);
    XMLElem toXML(const SceneCoordinates& obj, XMLElem parent = NULL);
    XMLElem toXML(const Data& obj, XMLElem parent = NULL);
    XMLElem toXML(const Channel& obj, XMLElem parent = NULL);
    XMLElem toXML(const Pvp& obj, XMLElem parent = NULL);
    XMLElem toXML(const SupportArray& obj, XMLElem parent = NULL);
    XMLElem toXML(const Dwell& obj, XMLElem parent = NULL);
    XMLElem toXML(const ReferenceGeometry& obj, XMLElem parent = NULL);
    XMLElem toXML(const Antenna& obj, XMLElem parent = NULL);
    XMLElem toXML(const TxRcv& obj, XMLElem parent = NULL);
    XMLElem toXML(const ErrorParameters& obj, XMLElem parent = NULL);
    XMLElem toXML(const ProductInfo& obj, XMLElem parent = NULL);
    XMLElem toXML(const GeoInfo& obj, XMLElem parent = NULL);
    XMLElem toXML(const MatchInfo& obj, XMLElem parent = NULL);

    // Read from XML object
    void fromXML(const XMLElem collectionIDXML, CollectionID& collectionID);
    void fromXML(const XMLElem globalXML, Global& global);
    void fromXML(const XMLElem sceneCoordsXML, SceneCoordinates& scene);
    void fromXML(const XMLElem dataXML, Data& data);
    void fromXML(const XMLElem channelXML, Channel& channel);
    void fromXML(const XMLElem pvpXML, Pvp& pvp);
    void fromXML(const XMLElem DwellXML, Dwell& dwell);
    void fromXML(const XMLElem refGeoXML, ReferenceGeometry& refGeo);
    void fromXML(const XMLElem supportArrayXML, SupportArray& supportArray);
    void fromXML(const XMLElem antennaXML, Antenna& antenna);
    void fromXML(const XMLElem txRcvXML, TxRcv& txRcv);
    void fromXML(const XMLElem errParamXML, ErrorParameters& errParam);
    void fromXML(const XMLElem productInfoXML, ProductInfo& productInfo);
    void fromXML(const XMLElem geoInfoXML, GeoInfo& geoInfo);
    void fromXML(const XMLElem matchInfoXML, MatchInfo& matchInfo);


    // Create helper functions
    void createParameterCollection(const std::string& name, 
                                   six::ParameterCollection& ParameterCollection,
                                   XMLElem parent) const;
    XMLElem createVector2D(const std::string& name,
                           Vector2 p,
                           XMLElem parent) const;
    XMLElem createLatLonFootprint(const std::string& name,
                                  const std::string& cornerName,
                                  const cphd::LatLonCorners& corners,
                                  XMLElem parent) const;
    XMLElem createPVPType(const std::string& name,
                          PVPType p,
                          XMLElem parent) const;
    XMLElem createAPVPType(const std::string& name,
                           APVPType p,
                           XMLElem parent) const;

    XMLElem createErrorParamPlatform(const std::string& name,
                                     ErrorParameters::Bistatic::Platform p,
                                     XMLElem parent) const;

    // Parse helper functions
    void parseVector2D(const XMLElem vecXML, Vector2& vec) const;
    void parseAreaType(const XMLElem areaXML, AreaType& area) const;
    void parseLineSample(const XMLElem lsXML, LineSample& ls) const;
    void parseIAExtent(const XMLElem extentXML, ImageAreaXExtent& extent) const;
    void parseIAExtent(const XMLElem extentXML, ImageAreaYExtent& extent) const;
    void parseChannelParameters(const XMLElem paramXML,
                                ChannelParameter& param) const;
    void parsePVPType(Pvp& p, const XMLElem paramXML, PVPType& param) const;
    void parsePVPType(Pvp& p, const XMLElem paramXML, size_t idx) const;
    void parsePlatformParams(const XMLElem platXML, Bistatic::PlatformParams& plat) const;
    void parseCommon(const XMLElem imgTypeXML, ImagingType* imgType) const;
    void parseDecorr(const XMLElem decorrXML, Decorr& decorr) const;
    void parsePosVelErr(const XMLElem posVelErrXML, PosVelErr& posVelErr) const;
    void parsePlatform(const XMLElem platXML,  ErrorParameters::Bistatic::Platform& plat) const;
    void parseSupportArrayParameter(const XMLElem paramXML, SupportArrayParameter& param,
                                    bool additionalFlag) const;
    void parseTxRcvParameter(const XMLElem paramXML, ParameterType& param) const;

private:
    six::SICommonXMLParser10x mCommon;
    std::vector<std::string> mSchemaPaths;
};
}

#endif
