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
    std::unique_ptr<xml::lite::Document> toXML(
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
    std::unique_ptr<Metadata> fromXML(
            const xml::lite::Document* doc);

private:
    typedef xml::lite::Element*  XMLElem;

private:

    //! Write to XML object
    XMLElem toXML(const CollectionInformation& obj, XMLElem parent);
    XMLElem toXML(const Global& obj, XMLElem parent);
    XMLElem toXML(const SceneCoordinates& obj, XMLElem parent);
    XMLElem toXML(const Data& obj, XMLElem parent);
    XMLElem toXML(const six::XmlOptionalElement<PolRef>&, xml::lite::Element& parent);
    XMLElem toXML(const Polarization&, xml::lite::Element& parent);
    XMLElem toXML(const Channel& obj, XMLElem parent);
    XMLElem toXML(const Pvp& obj, XMLElem parent);
    XMLElem toXML(const SupportArray& obj, XMLElem parent);
    XMLElem toXML(const Dwell& obj, XMLElem parent);
    XMLElem toXML(const ReferenceGeometry& obj, XMLElem parent);
    XMLElem toXML(const six::XmlOptionalElement<AntPattern::EBFreqShiftSF>&, xml::lite::Element& parent);
    XMLElem toXML(const six::XmlOptionalElement<AntPattern::MLFreqDilationSF>&, xml::lite::Element& parent);
    XMLElem toXML(const six::XmlOptionalElement<AntPattern::AntPolRef>&, xml::lite::Element& parent);
    XMLElem toXML(const Antenna& obj, XMLElem parent);
    XMLElem toXML(const TxRcv& obj, XMLElem parent);
    XMLElem toXML(const ErrorParameters& obj, XMLElem parent);
    XMLElem toXML(const ProductInfo& obj, XMLElem parent);
    XMLElem toXML(const GeoInfo& obj, XMLElem parent);
    XMLElem toXML(const MatchInformation& obj, XMLElem parent);

    //! Read from XML object
    void fromXML(const xml::lite::Element* collectionIDXML, CollectionInformation& collectionID);
    void fromXML(const xml::lite::Element* globalXML, Global& global);
    void fromXML(const xml::lite::Element* sceneCoordsXML, SceneCoordinates& scene);
    void fromXML(const xml::lite::Element* dataXML, Data& data);
    void fromXML(const xml::lite::Element* channelXML, Channel& channel);
    void fromXML(const xml::lite::Element* pvpXML, Pvp& pvp);
    void fromXML(const xml::lite::Element* DwellXML, Dwell& dwell);
    void fromXML(const xml::lite::Element* refGeoXML, ReferenceGeometry& refGeo);
    void fromXML(const xml::lite::Element* supportArrayXML, SupportArray& supportArray);
    void fromXML(const xml::lite::Element* antennaXML, Antenna& antenna);
    void fromXML(const xml::lite::Element* txRcvXML, TxRcv& txRcv);
    void fromXML(const xml::lite::Element* errParamXML, ErrorParameters& errParam);
    void fromXML(const xml::lite::Element* productInfoXML, ProductInfo& productInfo);
    void fromXML(const xml::lite::Element* geoInfoXML, GeoInfo& geoInfo);
    void fromXML(const xml::lite::Element* matchInfoXML, MatchInformation& matchInfo);


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
    XMLElem createDecorrType(const std::string& name, const six::DecorrType& dt,
        XMLElem parent) const;
    XMLElem createDecorrType(const std::string& name, const std::optional<six::DecorrType>& dt,
        XMLElem parent) const;

    //! Parse helper functions
    void parseAreaType(const xml::lite::Element* areaXML, AreaType& area) const;
    void parseLineSample(const xml::lite::Element* lsXML, LineSample& ls) const;
    void parseIAExtent(const xml::lite::Element* extentXML, ImageAreaXExtent& extent) const;
    void parseIAExtent(const xml::lite::Element* extentXML, ImageAreaYExtent& extent) const;
    void parseChannelParameters(const xml::lite::Element* paramXML,
                                ChannelParameter& param) const;
    void parsePVPType(Pvp& p, const xml::lite::Element* paramXML, PVPType& param) const;
    void parsePVPType(Pvp& p, const xml::lite::Element* paramXML) const;
    bool parseOptionalPVPType(const xml::lite::Element* parent, const std::string& tag, Pvp& p, PVPType& param) const;
    void parsePlatformParams(const xml::lite::Element* platXML, Bistatic::PlatformParams& plat) const;
    void parseCommon(const xml::lite::Element* imgTypeXML, ImagingType* imgType) const;
    void parsePosVelErr(const xml::lite::Element* posVelErrXML, six::PosVelError& posVelErr) const;
    void parsePlatform(const xml::lite::Element* platXML,  ErrorParameters::Bistatic::Platform& plat) const;
    void parseSupportArrayParameter(const xml::lite::Element* paramXML, SupportArrayParameter& param,
                                    bool additionalFlag) const;
    void parseTxRcvParameter(const xml::lite::Element* paramXML, ParameterType& param) const;
    void parsePolarization(const xml::lite::Element& polarizationXML, Polarization&) const;

    void parse(const xml::lite::Element&, six::XmlOptionalElement<PolRef>&) const;
    void parse(const xml::lite::Element&, six::XmlOptionalElement<AntPattern::EBFreqShiftSF>&) const;
    void parse(const xml::lite::Element&, six::XmlOptionalElement<AntPattern::MLFreqDilationSF>&) const;
    void parse(const xml::lite::Element&, six::XmlOptionalElement<AntPattern::AntPolRef>&) const;

private:
    six::SICommonXMLParser10x mCommon;
};
}

#endif
