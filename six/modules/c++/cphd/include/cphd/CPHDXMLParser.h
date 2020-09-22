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

#ifndef __CPHD_CPHD_XML_PARSER_H__
#define __CPHD_CPHD_XML_PARSER_H__

#include <memory>

#include <mem/SharedPtr.h>
#include <logging/Logger.h>
#include <xml/lite/Element.h>
#include <xml/lite/Document.h>
#include <six/XMLParser.h>
#include <six/SICommonXMLParser10x.h>
#include <cphd/SceneCoordinates.h>
#include <cphd/Data.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/ReferenceGeometry.h>
#include <cphd/Types.h>

namespace cphd
{
/*!
 *  \class CPHDXMLParser
 *
 *  \brief This class converts a Metadata object into a CPHD XML
 *  Document Object Model (DOM) and vise-versa.
 */
class CPHDXMLParser : public six::XMLParser
{
public:
    /*!
     *  \func CPHDXMLParser
     *  \brief Constructor
     *
     *  \param uri CPHD file uri string
     *  \param addClassAttributes flag indicates whether to add as attribute
     *  \param log provide logger object
     *  \param ownLog flag indicates if log should be deleted
     */
    CPHDXMLParser(const std::string& uri,
                  bool addClassAttributes,
                  logging::Logger* log = nullptr,
                  bool ownLog = false);

    /*!
     *  \func toXML
     *
     *  \brief Convert metadata to XML document object
     *
     *  \param metadata Valid CPHD metadata object
     *  \param schemaPaths Vector of XML Schema for validation
     *  \return pointer to xml Document object
     */
    std::auto_ptr<xml::lite::Document> toXML(
            const Metadata& metadata);
    std::unique_ptr<xml::lite::Document> toXML(std::nullptr_t,
            const Metadata& metadata);

    /*!
     *  \func fromXML
     *
     *  \brief Parse XML document to Metadata object
     *
     *  \param doc XML document object of CPHD
     *  \param schemaPaths Vector of XML Schema for validation
     *
     *  \return pointer to metadata object
     */
    std::auto_ptr<Metadata> fromXML(
            const xml::lite::Document* doc);
    std::unique_ptr<Metadata> fromXML(std::nullptr_t,
            const xml::lite::Document* doc);

private:
    typedef xml::lite::Element*  XMLElem;

private:

    //! Write to XML object
    XMLElem toXML(const CollectionInformation& obj, XMLElem parent);
    XMLElem toXML(const Global& obj, XMLElem parent);
    XMLElem toXML(const SceneCoordinates& obj, XMLElem parent);
    XMLElem toXML(const Data& obj, XMLElem parent);
    XMLElem toXML(const Channel& obj, XMLElem parent);
    XMLElem toXML(const Pvp& obj, XMLElem parent);
    XMLElem toXML(const SupportArray& obj, XMLElem parent);
    XMLElem toXML(const Dwell& obj, XMLElem parent);
    XMLElem toXML(const ReferenceGeometry& obj, XMLElem parent);
    XMLElem toXML(const Antenna& obj, XMLElem parent);
    XMLElem toXML(const TxRcv& obj, XMLElem parent);
    XMLElem toXML(const ErrorParameters& obj, XMLElem parent);
    XMLElem toXML(const ProductInfo& obj, XMLElem parent);
    XMLElem toXML(const GeoInfo& obj, XMLElem parent);
    XMLElem toXML(const MatchInformation& obj, XMLElem parent);

    //! Read from XML object
    void fromXML(const XMLElem collectionIDXML, CollectionInformation& collectionID);
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
    void fromXML(const XMLElem matchInfoXML, MatchInformation& matchInfo);


    //! Create helper functions
    XMLElem createLatLonFootprint(const std::string& name,
                                  const std::string& cornerName,
                                  const cphd::LatLonCorners& corners,
                                  XMLElem parent) const;
    XMLElem createPVPType(const std::string& name,
                          const PVPType& p,
                          XMLElem parent) const;
    XMLElem createAPVPType(const std::string& name,
                           const APVPType& p,
                           XMLElem parent) const;

    XMLElem createErrorParamPlatform(const std::string& name,
                                     const ErrorParameters::Bistatic::Platform p,
                                     XMLElem parent) const;

    //! Parse helper functions
    void parseAreaType(const XMLElem areaXML, AreaType& area) const;
    void parseLineSample(const XMLElem lsXML, LineSample& ls) const;
    void parseIAExtent(const XMLElem extentXML, ImageAreaXExtent& extent) const;
    void parseIAExtent(const XMLElem extentXML, ImageAreaYExtent& extent) const;
    void parseChannelParameters(const XMLElem paramXML,
                                ChannelParameter& param) const;
    void parsePVPType(Pvp& p, const XMLElem paramXML, PVPType& param) const;
    void parsePVPType(Pvp& p, const XMLElem paramXML) const;
    void parsePlatformParams(const XMLElem platXML, Bistatic::PlatformParams& plat) const;
    void parseCommon(const XMLElem imgTypeXML, ImagingType* imgType) const;
    void parsePosVelErr(const XMLElem posVelErrXML, six::PosVelError& posVelErr) const;
    void parsePlatform(const XMLElem platXML,  ErrorParameters::Bistatic::Platform& plat) const;
    void parseSupportArrayParameter(const XMLElem paramXML, SupportArrayParameter& param,
                                    bool additionalFlag) const;
    void parseTxRcvParameter(const XMLElem paramXML, ParameterType& param) const;

private:
    six::SICommonXMLParser10x mCommon;
};
}

#endif
