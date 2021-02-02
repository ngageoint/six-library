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
#ifndef __SIX_SICD_COMPLEX_XML_PARSER_H__
#define __SIX_SICD_COMPLEX_XML_PARSER_H__

#include <memory>

#include <six/XMLParser.h>
#include <six/SICommonXMLParser.h>
#include <six/sicd/ComplexData.h>
#include <xml/lite/Document.h>

namespace six
{
namespace sicd
{
class ComplexXMLParser : public XMLParser
{
public:
    ComplexXMLParser(const std::string& version,
                     bool addClassAttributes,
                     std::unique_ptr<SICommonXMLParser>&& comParser,
                     logging::Logger* log = nullptr,
                     bool ownLog = false);
#if !CODA_OSS_cpp17
        ComplexXMLParser(const std::string& version,
                     bool addClassAttributes,
                     std::auto_ptr<SICommonXMLParser> comParser,
                     logging::Logger* log = nullptr,
                     bool ownLog = false);
#endif

    xml::lite::Document* toXML(const ComplexData* data) const;

    ComplexData* fromXML(const xml::lite::Document* doc) const;

protected:

    virtual XMLElem convertGeoInfoToXML(const GeoInfo *obj,
                                        XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertWeightTypeToXML(const WeightType& obj,
                          XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertRadarCollectionToXML(const RadarCollection *radar,
                                                XMLElem parent) const = 0;

    virtual XMLElem convertImageFormationToXML(const ImageFormation *obj,
                                       const RadarCollection& radarCollection,
                                       XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertImageFormationAlgoToXML(const PFA* pfa, const RMA* rma,
                                                   const RgAzComp* rgAzComp,
                                                   XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertRMAToXML(const RMA* obj, XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertRMATToXML(const RMAT* obj, XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertHPBWToXML(const HalfPowerBeamwidths* obj,
                                     XMLElem parent = nullptr) const = 0;

    virtual XMLElem convertAntennaParamArrayToXML(
        const std::string& name,
        const GainAndPhasePolys* obj,
        XMLElem parent = nullptr) const = 0;

    virtual void parseWeightTypeFromXML(const XMLElem gridRowColXML,
                        mem::ScopedCopyablePtr<WeightType>& obj) const = 0;

    virtual void parsePolarizationCalibrationFromXML(const XMLElem polCalXML,
                         six::sicd::PolarizationCalibration* obj) const = 0;

    virtual void parseTxRcvPolFromXML(const XMLElem parent,
                         six::DualPolarizationType& obj) const = 0;


    virtual void parseRMATFromXML(const XMLElem rmatElem, RMAT* obj) const = 0;

    virtual void parseRMCRFromXML(const XMLElem rmcrElem, RMCR* obj) const = 0;


    virtual void parseAntennaParamArrayFromXML(
        const XMLElem antennaParamsXML,
        six::sicd::AntennaParameters* obj) const = 0;

protected:

    const six::SICommonXMLParser& common() const
    {
        return *(mCommon.get());
    }

    XMLElem createTxFrequency(const RadarCollection* radar,
                              XMLElem parent = nullptr) const;

    XMLElem createTxSequence(const RadarCollection* radar,
                             XMLElem parent = nullptr) const;

    XMLElem createWaveform(const RadarCollection* radar,
                           XMLElem parent = nullptr) const;

    XMLElem createArea(const RadarCollection* radar,
                       XMLElem parent = nullptr) const;

    XMLElem convertPFAToXML(const PFA *obj, XMLElem parent = nullptr) const;
    XMLElem convertRgAzCompToXML(const RgAzComp *obj, XMLElem parent = nullptr) const; //! added in 1.0.0
    XMLElem convertRMCRToXML(const RMCR* obj, XMLElem parent = nullptr) const; //! added in 1.0.0
    XMLElem convertINCAToXML(const INCA* obj, XMLElem parent = nullptr) const;
    XMLElem convertRcvChanProcToXML(const std::string& version,
                                    const RcvChannelProcessed* obj,
                                    XMLElem parent = nullptr) const;
    XMLElem convertDistortionToXML(const std::string& version,
                                   const Distortion* obj,
                                   XMLElem parent = nullptr) const;

    virtual XMLElem convertSCPCOAToXML(const SCPCOA *obj,
                                       XMLElem parent = nullptr) const;
    virtual void convertDRateSFPolyToXML(const INCA* inca, XMLElem incaElem) const;

    virtual XMLElem convertMatchInformationToXML(const MatchInformation& obj,
                                                 XMLElem parent = nullptr) const;

    virtual void parseSCPCOAFromXML(const XMLElem scpcoaXML, SCPCOA *obj) const;
    virtual void parseDRateSFPolyFromXML(const XMLElem incaElem, INCA* inca) const;
    virtual void parseMatchInformationFromXML(const XMLElem matchInfoXML,
                                              MatchInformation *obj) const;

    virtual void parseRadarCollectionFromXML(const XMLElem radarCollectionXML,
                                             RadarCollection *obj) const = 0;

    void parseWaveformFromXML(
            const XMLElem waveformXML,
            std::vector<mem::ScopedCloneablePtr<WaveformParameters> >&
                    waveform) const;

    void parseAreaFromXML(const XMLElem areaXML,
                          bool cornersRequired,
                          bool planeOrientationRequired,
                          mem::ScopedCloneablePtr<Area>& area) const;

    void parseTxSequenceFromXML(
            const XMLElem txSequenceXML,
            std::vector<mem::ScopedCloneablePtr<TxStep> >& steps) const;

    static
    std::string versionToURI(const std::string& version)
    {
        return ("urn:SICD:" + version);
    }

private:
    XMLElem convertImageCreationToXML(const ImageCreation *obj,
                                      XMLElem parent = nullptr) const;
    XMLElem convertImageDataToXML(const ImageData *obj,
                                  XMLElem parent = nullptr) const;
    XMLElem convertGeoDataToXML(const GeoData *obj,
                                XMLElem parent = nullptr) const;
    XMLElem convertGridToXML(const Grid *obj,
                             XMLElem parent = nullptr) const;
    XMLElem convertTimelineToXML(const Timeline *obj,
                                 XMLElem parent = nullptr) const;
    XMLElem convertPositionToXML(const Position *obj,
                                 XMLElem parent = nullptr) const;
    XMLElem convertAntennaToXML(const Antenna *obj,
                                XMLElem parent = nullptr) const;
    XMLElem convertAntennaParametersToXML(const std::string& name,
                                          AntennaParameters *ap,
                                          XMLElem parent = nullptr) const;

    XMLElem areaLineDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters *obj,
            XMLElem parent = nullptr) const;
    XMLElem areaSampleDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters *obj,
            XMLElem parent = nullptr) const;

    void parseImageCreationFromXML(const XMLElem imageCreationXML,
                                   ImageCreation *obj) const;
    void parseImageDataFromXML(const XMLElem imageDataXML,
                               ImageData *obj) const;
    void parseGeoDataFromXML(const XMLElem geoDataXML, GeoData *obj) const;
    void parseGridFromXML(const XMLElem gridXML, Grid *obj) const;
    void parseTimelineFromXML(const XMLElem timelineXML, Timeline *obj) const;
    void parsePositionFromXML(const XMLElem positionXML, Position *obj) const;
    void parseImageFormationFromXML(const XMLElem imageFormationXML,
                                    const RadarCollection& radarCollection,
                                    ImageFormation *obj) const;
    void parseAntennaFromXML(const XMLElem antennaXML, Antenna *obj) const;
    void parseAntennaParametersFromXML(const XMLElem antennaParamsXML,
                                       AntennaParameters* params) const;
    void parsePFAFromXML(const XMLElem pfaXML, PFA *obj) const;
    void parseRMAFromXML(const XMLElem rmaXML, RMA *obj) const;
    void parseRgAzCompFromXML(const XMLElem rgAzCompXML, RgAzComp *obj) const;
    void parseINCAFromXML(const XMLElem incaElem, INCA* obj) const;

    void parseSideOfTrackType(const XMLElem element, SideOfTrackType& value) const;

    XMLElem createFFTSign(const std::string& name, six::FFTSign sign,
                          XMLElem parent = nullptr) const;

    XMLElem createLatLonFootprint(const std::string& name,
                                  const std::string& cornerName,
                                  const LatLonCorners& corners,
                                  XMLElem parent = nullptr) const;

    XMLElem createLatLonAltFootprint(const std::string& name,
                                     const std::string& cornerName,
                                     const LatLonAltCorners& corners,
                                     XMLElem parent = nullptr) const;

    XMLElem createSideOfTrackType(const std::string& name,
                                  const SideOfTrackType& value, XMLElem parent =
                                          nullptr) const;

private:
    std::unique_ptr<SICommonXMLParser> mCommon;
};

}
}

#endif

