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

#include <six/Utilities.h>
#include <six/SICommonXMLParser10x.h>
#include <six/sicd/ComplexXMLParser10x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{
ComplexXMLParser10x::ComplexXMLParser10x(const std::string& version,
                                         logging::Logger* log,
                                         bool ownLog) :
    ComplexXMLParser(version, false, std::unique_ptr<six::SICommonXMLParser>(
                     new six::SICommonXMLParser10x(
                        versionToURI(version), false,
                        versionToURI(version), log)),
                     log, ownLog)
{
}

XMLElem ComplexXMLParser10x::convertGeoInfoToXML(
    const GeoInfo *geoInfo,
    XMLElem parent) const
{
    //! 1.0.x has ordering (1. Desc, 2. choice, 3. GeoInfo)
    return common().convertGeoInfoToXML(*geoInfo, false, parent);
}

XMLElem ComplexXMLParser10x::convertWeightTypeToXML(
    const WeightType& obj,
    XMLElem parent) const
{
    XMLElem weightTypeXML = newElement("WgtType", parent);
    createString("WindowName", obj.windowName, weightTypeXML);

    common().addParameters("Parameter", obj.parameters, weightTypeXML);

    return weightTypeXML;
}

XMLElem ComplexXMLParser10x::convertRadarCollectionToXML(
    const RadarCollection *radar,
    XMLElem parent) const
{
    XMLElem radarXML = newElement("RadarCollection", parent);

    createTxFrequency(radar, radarXML);

    if (!Init::isUndefined(radar->refFrequencyIndex))
    {
        createInt("RefFreqIndex", radar->refFrequencyIndex, radarXML);
    }

    // SICD v1.0 removes PolarizationHVAnglePoly

    createWaveform(radar, radarXML);

    //! required in 1.0
    createString("TxPolarization", six::toString(radar->txPolarization),
                 radarXML);

    createTxSequence(radar, radarXML);
    createRcvChannels(radar, radarXML);
    createArea(radar, radarXML);

    common().addParameters("Parameter", radar->parameters, radarXML);
    return radarXML;
}

XMLElem ComplexXMLParser10x::convertImageFormationToXML(
    const ImageFormation* imageFormation,
    const RadarCollection& radarCollection,
    XMLElem parent) const
{
    //! this segment was recreated completely because the ordering of
    //! a lot of the variables has been updated
    XMLElem imageFormationXML = newElement("ImageFormation", parent);

    convertRcvChanProcToXML("1.0", imageFormation->rcvChannelProcessed.get(),
                            imageFormationXML);

    createString("TxRcvPolarizationProc",
                 six::toString(imageFormation->txRcvPolarizationProc),
                 imageFormationXML);

    createDouble("TStartProc", imageFormation->tStartProc, imageFormationXML);
    createDouble("TEndProc", imageFormation->tEndProc, imageFormationXML);

    XMLElem txFreqXML = newElement("TxFrequencyProc", imageFormationXML);
    createDouble("MinProc", imageFormation->txFrequencyProcMin, txFreqXML);
    createDouble("MaxProc", imageFormation->txFrequencyProcMax, txFreqXML);

    if (radarCollection.area.get() != nullptr &&
        radarCollection.area->plane.get() != nullptr &&
        !radarCollection.area->plane->segmentList.empty() &&
        imageFormation->segmentIdentifier.empty())
    {
        throw except::Exception(Ctxt(
            "ImageFormation.SegmentIdentifier must be included when a "
            "RadarCollection.Area.Plane.SegmentList is included."));
    }

    //! updated location in 1.0.0
    if (!imageFormation->segmentIdentifier.empty())
        createString("SegmentIdentifier", imageFormation->segmentIdentifier,
                     imageFormationXML);

    createString("ImageFormAlgo",
                 six::toString(imageFormation->imageFormationAlgorithm),
                 imageFormationXML);

    createString("STBeamComp",
                 six::toString(imageFormation->slowTimeBeamCompensation),
                 imageFormationXML);
    createString("ImageBeamComp",
                 six::toString(imageFormation->imageBeamCompensation),
                 imageFormationXML);
    createString("AzAutofocus",
                 six::toString(imageFormation->azimuthAutofocus),
                 imageFormationXML);
    createString("RgAutofocus", six::toString(imageFormation->rangeAutofocus),
                 imageFormationXML);

    for (unsigned int i = 0; i < imageFormation->processing.size(); ++i)
    {
        const Processing* proc = &imageFormation->processing[i];

        XMLElem procXML = newElement("Processing", imageFormationXML);

        createString("Type", proc->type, procXML);
        require(createBooleanType("Applied", proc->applied, procXML), "Applied");
        common().addParameters("Parameter", proc->parameters, procXML);
    }

    if (imageFormation->polarizationCalibration.get())
    {
        XMLElem pcXML =
                newElement("PolarizationCalibration", imageFormationXML);

        require(createBooleanType("DistortCorrectionApplied",
                                  imageFormation ->polarizationCalibration->
                                  distortionCorrectionApplied,
                                  pcXML), "DistortCorrectionApplied");

        convertDistortionToXML("1.0",
            imageFormation->polarizationCalibration->distortion.get(),
            pcXML);
    }
    return imageFormationXML;
}


XMLElem ComplexXMLParser10x::convertImageFormationAlgoToXML(
    const PFA* pfa, const RMA* rma,
    const RgAzComp* rgAzComp, XMLElem parent) const
{
    if (pfa && !rma && !rgAzComp)
    {
        return convertPFAToXML(pfa, parent);
    }
    else if (!pfa && rma && !rgAzComp)
    {
        return convertRMAToXML(rma, parent);
    }
    else if (!pfa && !rma && rgAzComp)
    {
        return convertRgAzCompToXML(rgAzComp, parent);
    }
    else if (!pfa && !rma && !rgAzComp)
    {
        //! This will occur when set to OTHER. We do not want to include
        //  a specialized image formation algorithm so we return nullptr.
        return nullptr;
    }
    else
    {
        throw except::Exception(Ctxt(
            "Only one PFA, RMA, or RgAzComp can be defined in SICD 1.0"));
    }
}

XMLElem ComplexXMLParser10x::convertSCPCOAToXML(
    const SCPCOA* scpcoa, XMLElem parent) const
{
    XMLElem scpcoaXML = ComplexXMLParser::convertSCPCOAToXML(scpcoa, parent);

    //! Added in 1.0.0
    createDouble("AzimAng", scpcoa->azimAngle, scpcoaXML);
    createDouble("LayoverAng", scpcoa->layoverAngle, scpcoaXML);
    return scpcoaXML;
}


XMLElem ComplexXMLParser10x::convertRMAToXML(
    const RMA* rma,
    XMLElem parent) const
{
    XMLElem rmaXML = newElement("RMA", parent);

    createString("RMAlgoType", six::toString<six::RMAlgoType>(rma->algoType),
                 rmaXML);

    if (rma->rmat.get() && !rma->rmcr.get() && !rma->inca.get())
    {
        convertRMATToXML(rma->rmat.get(), rmaXML);
    }
    else if (!rma->rmat.get() && rma->rmcr.get() && !rma->inca.get())
    {
        convertRMCRToXML(rma->rmcr.get(), rmaXML);
    }
    else if (!rma->rmat.get() && !rma->rmcr.get() && rma->inca.get())
    {
        convertINCAToXML(rma->inca.get(), rmaXML);
    }
    else
    {
        throw except::Exception(Ctxt(
            "One of RMAT, RMCR or INCA must be defined in SICD 1.0."));
    }

    return rmaXML;
}


XMLElem ComplexXMLParser10x::convertRMATToXML(
    const RMAT* rmat,
    XMLElem rmaXML) const
{
    createString("ImageType", "RMAT", rmaXML);

    XMLElem rmatXML = newElement("RMAT", rmaXML);

    common().createVector3D("PosRef", rmat->refPos, rmatXML);
    common().createVector3D("VelRef", rmat->refVel, rmatXML);
    createDouble("DopConeAngRef", rmat->dopConeAngleRef, rmatXML);

    return rmatXML;
}

XMLElem ComplexXMLParser10x::convertHPBWToXML(
    const HalfPowerBeamwidths*,
    XMLElem) const
{
    //! this field was deprecated in 1.0.0
    return nullptr;
}

XMLElem ComplexXMLParser10x::convertAntennaParamArrayToXML(
    const std::string& name,
    const GainAndPhasePolys* array,
    XMLElem apXML) const
{
    //! mandatory field in 1.0
    if (array)
    {
        XMLElem arrXML = newElement("Array", apXML);
        common().createPoly2D("GainPoly", array->gainPoly, arrXML);
        common().createPoly2D("PhasePoly", array->phasePoly, arrXML);
        return arrXML;
    }
    else
    {
        throw except::Exception(Ctxt(FmtX(
            "[Array] is a mandatory field in AntennaParams of [%s] in 1.0",
            name.c_str())));
    }
}


void ComplexXMLParser10x::parseWeightTypeFromXML(
    const XMLElem gridRowColXML,
    mem::ScopedCopyablePtr<WeightType>& obj) const
{
    const XMLElem weightType = getOptional(gridRowColXML, "WgtType");
    if (weightType)
    {
        obj.reset(new WeightType());
        parseString(getFirstAndOnly(weightType, "WindowName"),
                    obj->windowName);
        common().parseParameters(weightType, "Parameter", obj->parameters);
    }
    else
    {
        obj.reset();
    }
}

void ComplexXMLParser10x::parsePolarizationCalibrationFromXML(
    const XMLElem polCalXML,
    six::sicd::PolarizationCalibration* obj) const
{
    // HVAngleCompApplied no longer exists in 1.0.0
    parseBooleanType(getFirstAndOnly(polCalXML, "DistortCorrectionApplied"),
                     obj->distortionCorrectionApplied);
}

void ComplexXMLParser10x::parseTxRcvPolFromXML(
    const XMLElem parent,
    six::DualPolarizationType& txRcvPol) const
{
    //! Required field
    txRcvPol = six::toType<
        DualPolarizationType>(
            getFirstAndOnly(parent, "TxRcvPolarizationProc")->
                getCharacterData());
}

void ComplexXMLParser10x::parseSCPCOAFromXML(
        const XMLElem scpcoaXML, SCPCOA* scpcoa) const
{
    ComplexXMLParser::parseSCPCOAFromXML(scpcoaXML, scpcoa);

    //! Added in 1.0.0
    parseDouble(getFirstAndOnly(scpcoaXML, "AzimAng"), scpcoa->azimAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "LayoverAng"), scpcoa->layoverAngle);
}

void ComplexXMLParser10x::parseRMATFromXML(const XMLElem rmatElem,
                                           RMAT* rmat) const
{
    common().parseVector3D(getFirstAndOnly(rmatElem, "PosRef"), rmat->refPos);
    common().parseVector3D(getFirstAndOnly(rmatElem, "VelRef"), rmat->refVel);
    parseDouble(getFirstAndOnly(rmatElem, "DopConeAngRef"), rmat->dopConeAngleRef);
}


void ComplexXMLParser10x::parseRMCRFromXML(const XMLElem rmcrElem,
                                           RMCR* rmcr) const
{
    common().parseVector3D(getFirstAndOnly(rmcrElem, "PosRef"), rmcr->refPos);
    common().parseVector3D(getFirstAndOnly(rmcrElem, "VelRef"), rmcr->refVel);
    parseDouble(getFirstAndOnly(rmcrElem, "DopConeAngRef"), rmcr->dopConeAngleRef);
}


void ComplexXMLParser10x::parseAntennaParamArrayFromXML(
    const XMLElem antennaParamsXML,
    six::sicd::AntennaParameters* params) const
{
    //! this field is mandatory in 1.0.0
    XMLElem arrayElem = getFirstAndOnly(antennaParamsXML, "Array");
    params->array.reset(new GainAndPhasePolys());
    common().parsePoly2D(getFirstAndOnly(arrayElem, "GainPoly"),
                params->array->gainPoly);
    common().parsePoly2D(getFirstAndOnly(arrayElem, "PhasePoly"),
                params->array->phasePoly);
}

XMLElem ComplexXMLParser10x::createRcvChannels(const RadarCollection* radar,
                                               XMLElem parent) const
{
    const size_t numChannels = radar->rcvChannels.size();
    XMLElem rcvChanXML = newElement("RcvChannels", parent);
    setAttribute(rcvChanXML, "size", numChannels);
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        const ChannelParameters* const cp = radar->rcvChannels[ii].get();
        XMLElem cpXML = newElement("ChanParameters", rcvChanXML);
        setAttribute(cpXML, "index", ii + 1);

        //! required in 1.0
        createString(
            "TxRcvPolarization",
            six::toString<DualPolarizationType>(cp->txRcvPolarization),
            cpXML);

        if (!Init::isUndefined(cp->rcvAPCIndex))
        {
            createInt("RcvAPCIndex", cp->rcvAPCIndex, cpXML);
        }
    }

    return rcvChanXML;
}

void ComplexXMLParser10x::parseRadarCollectionFromXML(
        const XMLElem radarCollectionXML,
        RadarCollection* radarCollection) const
{
    XMLElem tmpElem = getFirstAndOnly(radarCollectionXML, "TxFrequency");
    parseDouble(getFirstAndOnly(tmpElem, "Min"),
                radarCollection->txFrequencyMin);
    parseDouble(getFirstAndOnly(tmpElem, "Max"),
                radarCollection->txFrequencyMax);

    tmpElem = getOptional(radarCollectionXML, "RefFreqIndex");
    if (tmpElem)
    {
        //optional
        parseInt(tmpElem, radarCollection->refFrequencyIndex);
    }

    tmpElem = getOptional(radarCollectionXML, "Waveform");
    if (tmpElem)
    {
        //optional
        parseWaveformFromXML(tmpElem, radarCollection->waveform);
    }

    tmpElem = getFirstAndOnly(radarCollectionXML, "TxPolarization");
    radarCollection->txPolarization = six::toType<PolarizationSequenceType>(
            tmpElem->getCharacterData());

    tmpElem = getOptional(radarCollectionXML, "TxSequence");
    if (tmpElem)
    {
        //optional
        parseTxSequenceFromXML(tmpElem, radarCollection->txSequence);
    }

    tmpElem = getFirstAndOnly(radarCollectionXML, "RcvChannels");

    std::vector<XMLElem> channelsXML;
    tmpElem->getElementsByTagName("ChanParameters", channelsXML);
    if (channelsXML.empty())
    {
        throw except::Exception(Ctxt(
                "Expected at least one ChanParameters element"));
    }

    for (std::vector<XMLElem>::const_iterator it = channelsXML.begin();
         it != channelsXML.end();
         ++it)
    {
        radarCollection->rcvChannels.resize(
                radarCollection->rcvChannels.size() + 1);
        mem::ScopedCloneablePtr<ChannelParameters>& chanParams =
                radarCollection->rcvChannels.back();
        chanParams.reset(new ChannelParameters());

        XMLElem childXML = getFirstAndOnly(*it, "TxRcvPolarization");
        chanParams->txRcvPolarization = six::toType<DualPolarizationType>(
                childXML->getCharacterData());

        childXML = getOptional(*it, "RcvAPCIndex");
        if (childXML)
        {
            parseInt(childXML, chanParams->rcvAPCIndex);
        }
    }

    XMLElem areaXML = getOptional(radarCollectionXML, "Area");
    if (areaXML)
    {
        //optional
        parseAreaFromXML(areaXML, true, false, radarCollection->area);
    }

    common().parseParameters(radarCollectionXML, "Parameter",
                    radarCollection->parameters);
}
}
}
