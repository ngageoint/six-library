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
                     mem::auto_ptr<SICommonXMLParser> comParser,
                     logging::Logger* log = nullptr,
                     bool ownLog = false);
#endif

        ComplexXMLParser(const ComplexXMLParser&) = delete;
        ComplexXMLParser& operator=(const ComplexXMLParser&) = delete;

    xml::lite::Document* toXML(const ComplexData* data) const;
    std::unique_ptr<xml::lite::Document> toXML(const ComplexData&) const;

    ComplexData* fromXML(const xml::lite::Document* doc) const;
    std::unique_ptr<ComplexData> fromXML(const xml::lite::Document&) const;

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

    virtual void parseWeightTypeFromXML(const xml::lite::Element* gridRowColXML,
                        mem::ScopedCopyablePtr<WeightType>& obj) const = 0;

    virtual void parsePolarizationCalibrationFromXML(const xml::lite::Element* polCalXML,
                         six::sicd::PolarizationCalibration* obj) const = 0;

    virtual void parseTxRcvPolFromXML(const xml::lite::Element* parent,
                         six::DualPolarizationType& obj) const = 0;


    virtual void parseRMATFromXML(const xml::lite::Element* rmatElem, RMAT* obj) const = 0;

    virtual void parseRMCRFromXML(const xml::lite::Element* rmcrElem, RMCR* obj) const = 0;


    virtual void parseAntennaParamArrayFromXML(
        const xml::lite::Element* antennaParamsXML,
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

    virtual void parseSCPCOAFromXML(const xml::lite::Element* scpcoaXML, SCPCOA *obj) const;
    virtual void parseDRateSFPolyFromXML(const xml::lite::Element* incaElem, INCA* inca) const;
    virtual void parseMatchInformationFromXML(const xml::lite::Element* matchInfoXML,
                                              MatchInformation *obj) const;

    virtual void parseRadarCollectionFromXML(const xml::lite::Element* radarCollectionXML,
                                             RadarCollection *obj) const = 0;

    void parseWaveformFromXML(
            const xml::lite::Element* waveformXML,
            std::vector<mem::ScopedCloneablePtr<WaveformParameters> >&
                    waveform) const;

    void parseAreaFromXML(const xml::lite::Element* areaXML,
                          bool cornersRequired,
                          bool planeOrientationRequired,
                          mem::ScopedCloneablePtr<Area>& area) const;

    void parseTxSequenceFromXML(
            const xml::lite::Element* txSequenceXML,
            std::vector<mem::ScopedCloneablePtr<TxStep> >& steps) const;

    static
    std::string versionToURI(const std::string& strVersion)
    {
        return ("urn:SICD:" + strVersion);
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
                                          const AntennaParameters *ap,
                                          XMLElem parent = nullptr) const;

    XMLElem areaLineDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters *obj,
            XMLElem parent = nullptr) const;
    XMLElem areaSampleDirectionParametersToXML(const std::string& name,
            const AreaDirectionParameters *obj,
            XMLElem parent = nullptr) const;

    void parseImageCreationFromXML(const xml::lite::Element* imageCreationXML,
                                   ImageCreation *obj) const;
    void parseImageDataFromXML(const xml::lite::Element* imageDataXML,
                               ImageData *obj) const;
    void parseGeoDataFromXML(const xml::lite::Element* geoDataXML, GeoData *obj) const;
    void parseGridFromXML(const xml::lite::Element* gridXML, Grid *obj) const;
    void parseTimelineFromXML(const xml::lite::Element* timelineXML, Timeline *obj) const;
    void parsePositionFromXML(const xml::lite::Element* positionXML, Position *obj) const;
    void parseImageFormationFromXML(const xml::lite::Element* imageFormationXML,
                                    const RadarCollection& radarCollection,
                                    ImageFormation *obj) const;
    void parseAntennaFromXML(const xml::lite::Element* antennaXML, Antenna *obj) const;
    void parseAntennaParametersFromXML(const xml::lite::Element* antennaParamsXML,
                                       AntennaParameters* params) const;
    void parsePFAFromXML(const xml::lite::Element* pfaXML, PFA *obj) const;
    void parseRMAFromXML(const xml::lite::Element* rmaXML, RMA *obj) const;
    void parseRgAzCompFromXML(const xml::lite::Element* rgAzCompXML, RgAzComp *obj) const;
    void parseINCAFromXML(const xml::lite::Element* incaElem, INCA* obj) const;

    void parseSideOfTrackType(const xml::lite::Element* element, SideOfTrackType& value) const;

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

