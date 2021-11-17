/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SICD_COMPLEX_XML_PARSER_04X_H__
#define __SIX_SICD_COMPLEX_XML_PARSER_04X_H__

#include <six/sicd/ComplexXMLParser.h>

namespace six
{
namespace sicd
{
struct ComplexXMLParser04x : public ComplexXMLParser
{
    ComplexXMLParser04x(const std::string& version,
                        bool addClassAttributes,
                        std::unique_ptr<SICommonXMLParser>&& comParser,
                        logging::Logger* log = nullptr,
                        bool ownLog = false);
#if !CODA_OSS_cpp17
    ComplexXMLParser04x(const std::string& version,
                        bool addClassAttributes,
                        mem::auto_ptr<SICommonXMLParser> comParser,
                        logging::Logger* log = nullptr,
                        bool ownLog = false);
#endif

    ComplexXMLParser04x(const ComplexXMLParser04x&) = delete;
    ComplexXMLParser04x& operator=(const ComplexXMLParser04x&) = delete;

protected:

    virtual XMLElem convertRMATToXML(const RMAT* obj, 
                                     XMLElem parent = nullptr) const = 0;
    virtual void parseRMATFromXML(const xml::lite::Element* rmatElem, RMAT* obj) const = 0;

protected:

    virtual XMLElem convertGeoInfoToXML(const GeoInfo *obj,
                                        XMLElem parent = nullptr) const;
    virtual XMLElem convertWeightTypeToXML(const WeightType& obj, 
                                           XMLElem parent = nullptr) const;
    virtual XMLElem convertRadarCollectionToXML(const RadarCollection *radar,
                                                XMLElem parent) const;
    virtual XMLElem convertImageFormationToXML(const ImageFormation *obj,
                                       const RadarCollection& radarCollection,
                                       XMLElem parent = nullptr) const;
    virtual XMLElem convertImageFormationAlgoToXML(
        const PFA* pfa, const RMA* rma, 
        const RgAzComp* rgAzComp, 
        XMLElem parent = nullptr) const;
    virtual XMLElem convertRMAToXML(const RMA *obj, 
                                    XMLElem parent = nullptr) const;
    virtual XMLElem convertHPBWToXML(const HalfPowerBeamwidths* obj, 
                                     XMLElem parent = nullptr) const;
    virtual XMLElem convertAntennaParamArrayToXML(
        const std::string& name,
        const GainAndPhasePolys* obj, 
        XMLElem parent = nullptr) const;

    virtual void parseWeightTypeFromXML(const xml::lite::Element* gridRowColXML,
        mem::ScopedCopyablePtr<WeightType>& obj) const;
    virtual void parsePolarizationCalibrationFromXML(
        const xml::lite::Element* polCalXML,
        six::sicd::PolarizationCalibration* obj) const;
    virtual void parseTxRcvPolFromXML(const xml::lite::Element* parent,
                         six::DualPolarizationType& obj) const;
    virtual void parseRMCRFromXML(const xml::lite::Element* rmcrElem, RMCR* obj) const;
    virtual void parseAntennaParamArrayFromXML(
        const xml::lite::Element* antennaParamsXML, 
        six::sicd::AntennaParameters* obj) const;
    virtual void parseRadarCollectionFromXML(const xml::lite::Element* radarCollectionXML,
                                             RadarCollection *obj) const;

private:
    XMLElem createRcvChannels(const RadarCollection* radar,
                              XMLElem parent = nullptr) const;
};
}
}

#endif

