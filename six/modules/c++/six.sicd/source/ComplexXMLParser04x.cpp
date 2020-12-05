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
#include <six/sicd/ComplexXMLParser04x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{
ComplexXMLParser04x::ComplexXMLParser04x(
    const std::string& version,
    bool addClassAttributes,
    std::unique_ptr<SICommonXMLParser>&& comParser,
    logging::Logger* log,
    bool ownLog) :
    ComplexXMLParser(version, addClassAttributes, std::move(comParser), log, ownLog)
{
}

XMLElem ComplexXMLParser04x::convertGeoInfoToXML(
    const GeoInfo *geoInfo,
    XMLElem parent) const
{
    //! 0.4.x has ordering (1. GeoInfo, 2. Desc, 3. choice)
    XMLElem geoInfoXML = newElement("GeoInfo", parent);
    if (!geoInfo->name.empty())
        setAttribute(geoInfoXML, "name", geoInfo->name);

    for (size_t ii = 0; ii < geoInfo->geoInfos.size(); ++ii)
    {
        convertGeoInfoToXML(geoInfo->geoInfos[ii].get(), geoInfoXML);
    }

    common().addParameters("Desc", geoInfo->desc, geoInfoXML);

    const size_t numLatLons = geoInfo->geometryLatLon.size();
    if (numLatLons == 1)
    {
        common().createLatLon("Point", geoInfo->geometryLatLon[0], geoInfoXML);
    }
    else if (numLatLons >= 2)
    {
        XMLElem linePolyXML = newElement(numLatLons == 2 ? "Line" : "Polygon",
                                         geoInfoXML);
        setAttribute(linePolyXML, "size", str::toString(numLatLons));

        for (size_t ii = 0; ii < numLatLons; ++ii)
        {
            XMLElem v = common().createLatLon(numLatLons == 2 ? "Endpoint" : "Vertex",
                         geoInfo->geometryLatLon[ii], linePolyXML);
            setAttribute(v, "index", str::toString(ii + 1));
        }
    }
    return geoInfoXML;
}

XMLElem ComplexXMLParser04x::convertWeightTypeToXML(
    const WeightType& obj,
    XMLElem parent) const
{
    return createString("WgtType", obj.windowName, parent);
}

XMLElem ComplexXMLParser04x::convertRadarCollectionToXML(
    const RadarCollection *radar,
    XMLElem parent) const
{
    XMLElem radarXML = newElement("RadarCollection", parent);

    if (!Init::isUndefined(radar->refFrequencyIndex))
    {
        createInt("RefFreqIndex", radar->refFrequencyIndex, radarXML);
    }

    createTxFrequency(radar, radarXML);

    if (radar->txPolarization != PolarizationSequenceType::NOT_SET)
    {
        // In SICD 0.4, this is not allowed to contain UNKNOWN or SEQUENCE
        createString(
                "TxPolarization",
                six::toString(PolarizationType(radar->txPolarization.value)),
                radarXML);
    }

    if (!Init::isUndefined(radar->polarizationHVAnglePoly))
    {
        common().createPoly1D("PolarizationHVAnglePoly",
                              radar->polarizationHVAnglePoly,
                              radarXML);
    }

    createTxSequence(radar, radarXML);
    createWaveform(radar, radarXML);
    createRcvChannels(radar, radarXML);
    createArea(radar, radarXML);
    common().addParameters("Parameter", radar->parameters, radarXML);

    return radarXML;
}

XMLElem ComplexXMLParser04x::convertImageFormationToXML(
    const ImageFormation* imageFormation,
    const RadarCollection& radarCollection,
    XMLElem parent) const
{
    //! this segment was recreated completely because the ordering of
    //! a lot of the variables has been updated
    XMLElem imageFormationXML = newElement("ImageFormation", parent);

    if (radarCollection.area.get() != nullptr &&
        radarCollection.area->plane.get() != nullptr &&
        !radarCollection.area->plane->segmentList.empty() &&
        imageFormation->segmentIdentifier.empty())
    {
        throw except::Exception(Ctxt(
            "ImageFormation.SegmentIdentifier must be included when a "
            "RadarCollection.Area.Plane.SegmentList is included."));
    }

    if (!imageFormation->segmentIdentifier.empty())
        createString("SegmentIdentifier", imageFormation->segmentIdentifier,
            imageFormationXML);
    convertRcvChanProcToXML("0.4", imageFormation->rcvChannelProcessed.get(),
                            imageFormationXML);

    if (imageFormation->txRcvPolarizationProc != DualPolarizationType::NOT_SET)
    {
        createString("TxRcvPolarizationProc",
                     six::toString(imageFormation->txRcvPolarizationProc),
                     imageFormationXML);
    }

    createString("ImageFormAlgo",
                 six::toString(imageFormation->imageFormationAlgorithm),
                 imageFormationXML);

    createDouble("TStartProc", imageFormation->tStartProc, imageFormationXML);
    createDouble("TEndProc", imageFormation->tEndProc, imageFormationXML);

    XMLElem txFreqXML = newElement("TxFrequencyProc", imageFormationXML);
    createDouble("MinProc", imageFormation->txFrequencyProcMin, txFreqXML);
    createDouble("MaxProc", imageFormation->txFrequencyProcMax, txFreqXML);

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

    for (size_t i = 0; i < imageFormation->processing.size(); ++i)
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

        require(createBooleanType(
            "HVAngleCompApplied",
            imageFormation->polarizationCalibration->hvAngleCompensationApplied,
            pcXML), "HVAngleCompApplied");

        require(createBooleanType(
            "DistortionCorrectionApplied",
            imageFormation ->polarizationCalibration->distortionCorrectionApplied,
            pcXML), "DistortionCorrectionApplied");

        convertDistortionToXML("0.4",
            imageFormation->polarizationCalibration->distortion.get(),
            pcXML);
    }
    return imageFormationXML;
}

XMLElem ComplexXMLParser04x::convertImageFormationAlgoToXML(
    const PFA* pfa, const RMA* rma,
    const RgAzComp* rgAzComp, XMLElem parent) const
{
    if (rgAzComp)
    {
        throw except::Exception(Ctxt(
            "RgAzComp cannot be defined in SICD 0.4"));
    }
    else if (pfa && !rma)
    {
        return convertPFAToXML(pfa, parent);
    }
    else if (!pfa && rma)
    {
        return convertRMAToXML(rma, parent);
    }
    else
    {
        //! The 0.4.0 schema does not support the absence of a PFA or RMA
        //  image formation algorithm node. So we must throw here. In
        //  the version 0.4.1 ComplexXMLParser, there is a specialization
        //  of this function to allow OTHER as a valid type.
        throw except::Exception(Ctxt(
            "Only one PFA or RMA can be defined in SICD 0.4"));
    }
}

XMLElem ComplexXMLParser04x::convertRMAToXML(
    const RMA* rma,
    XMLElem parent) const
{
    XMLElem rmaXML = newElement("RMA", parent);

    createString("RMAlgoType", six::toString<six::RMAlgoType>(rma->algoType),
                 rmaXML);

    if (rma->rmcr.get())
    {
        throw except::Exception(Ctxt(
            "RMCR cannot be defined in SICD 0.4"));
    }
    else if (rma->rmat.get() && !rma->inca.get())
    {
        convertRMATToXML(rma->rmat.get(), rmaXML);
    }
    else if (!rma->rmat.get() && rma->inca.get())
    {
        convertINCAToXML(rma->inca.get(), rmaXML);
    }
    else
    {
        throw except::Exception(Ctxt(
            "RMAT or INCA must be defined in SICD 0.4"));
    }

    return rmaXML;
}


XMLElem ComplexXMLParser04x::convertRMATToXML(
    const RMAT* rmat,
    XMLElem rmaXML) const
{
    createString("ImageType", "RMAT", rmaXML);

    XMLElem rmatXML = newElement("RMAT", rmaXML);

    createDouble("RefTime", rmat->refTime, rmatXML);
    common().createVector3D("PosRef", rmat->refPos, rmatXML);
    common().createVector3D("UnitVelRef", rmat->refVel, rmatXML);
    common().createPoly1D("DistRLPoly", rmat->distRefLinePoly, rmatXML);
    common().createPoly2D("CosDCACOAPoly", rmat->cosDCACOAPoly, rmatXML);
    createDouble("Kx1", rmat->kx1, rmatXML);
    createDouble("Kx2", rmat->kx2, rmatXML);
    createDouble("Ky1", rmat->ky1, rmatXML);
    createDouble("Ky2", rmat->ky2, rmatXML);

    return rmatXML;
}

XMLElem ComplexXMLParser04x::convertHPBWToXML(
    const HalfPowerBeamwidths* halfPowerBeamwidths,
    XMLElem parent) const
{
    if (halfPowerBeamwidths)
    {
        XMLElem hpXML = newElement("HPBW", parent);
        createDouble("DCX", halfPowerBeamwidths->dcx, hpXML);
        createDouble("DCY", halfPowerBeamwidths->dcy, hpXML);
        return hpXML;
    }
    else
    {
        return nullptr;
    }
}

XMLElem ComplexXMLParser04x::convertAntennaParamArrayToXML(
    const std::string& /*name*/,
    const GainAndPhasePolys* array,
    XMLElem apXML) const
{
    //! optional field in 0.4
    if (array)
    {
        XMLElem arrXML = newElement("Array", apXML);
        common().createPoly2D("GainPoly", array->gainPoly, arrXML);
        common().createPoly2D("PhasePoly", array->phasePoly, arrXML);
        return arrXML;
    }
    else
    {
        return nullptr;
    }
}


void ComplexXMLParser04x::parseWeightTypeFromXML(
    const XMLElem gridRowColXML,
    mem::ScopedCopyablePtr<WeightType>& obj) const
{
    const XMLElem weightType = getOptional(gridRowColXML, "WgtType");
    if (weightType)
    {
        obj.reset(new WeightType());
        parseString(weightType, obj->windowName);
    }
    else
    {
        obj.reset();
    }
}

void ComplexXMLParser04x::parsePolarizationCalibrationFromXML(
    const XMLElem polCalXML,
    six::sicd::PolarizationCalibration* obj) const
{
    parseBooleanType(getFirstAndOnly(polCalXML, "HVAngleCompApplied"),
                     obj->hvAngleCompensationApplied);

    parseBooleanType(getFirstAndOnly(polCalXML, "DistortionCorrectionApplied"),
                     obj->distortionCorrectionApplied);
}

void ComplexXMLParser04x::parseTxRcvPolFromXML(
    const XMLElem parent,
    six::DualPolarizationType& txRcvPol) const
{
    //! Optional field
    XMLElem txElem = getOptional(parent, "TxRcvPolarizationProc");
    if (txElem)
    {
        txRcvPol = six::toType<
                DualPolarizationType>(txElem->getCharacterData());
    }
}

void ComplexXMLParser04x::parseRMATFromXML(const XMLElem rmatElem,
                                           RMAT* rmat) const
{
    parseDouble(getFirstAndOnly(rmatElem, "RefTime"), rmat->refTime);
    common().parseVector3D(getFirstAndOnly(rmatElem, "PosRef"), rmat->refPos);
    common().parseVector3D(getFirstAndOnly(rmatElem, "UnitVelRef"), rmat->refVel);
    common().parsePoly1D(getFirstAndOnly(rmatElem, "DistRLPoly"),
                rmat->distRefLinePoly);
    common().parsePoly2D(getFirstAndOnly(rmatElem, "CosDCACOAPoly"),
                rmat->cosDCACOAPoly);
    parseDouble(getFirstAndOnly(rmatElem, "Kx1"), rmat->kx1);
    parseDouble(getFirstAndOnly(rmatElem, "Kx2"), rmat->kx2);
    parseDouble(getFirstAndOnly(rmatElem, "Ky1"), rmat->ky1);
    parseDouble(getFirstAndOnly(rmatElem, "Ky2"), rmat->ky2);
}

void ComplexXMLParser04x::parseRMCRFromXML(const XMLElem,
                                           RMCR*) const
{
    // did not exist in 0.4
    throw except::Exception(Ctxt("RMA=>RMCR is not available in 0.4"));
}

void ComplexXMLParser04x::parseAntennaParamArrayFromXML(
    const XMLElem antennaParamsXML,
    six::sicd::AntennaParameters* params) const
{
    //! this field is optional in 0.4
    XMLElem arrayElem = getOptional(antennaParamsXML, "Array");
    if (arrayElem)
    {
        params->array.reset(new GainAndPhasePolys());
        common().parsePoly2D(getFirstAndOnly(arrayElem, "GainPoly"),
                    params->array->gainPoly);
        common().parsePoly2D(getFirstAndOnly(arrayElem, "PhasePoly"),
                    params->array->phasePoly);
    }
}

XMLElem ComplexXMLParser04x::createRcvChannels(const RadarCollection* radar,
                                               XMLElem parent) const
{
    const size_t numChannels = radar->rcvChannels.size();
    XMLElem rcvChanXML = newElement("RcvChannels", parent);
    setAttribute(rcvChanXML, "size", str::toString(numChannels));
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        const ChannelParameters* const cp = radar->rcvChannels[ii].get();
        XMLElem cpXML = newElement("ChanParameters", rcvChanXML);
        setAttribute(cpXML, "index", str::toString(ii + 1));

        if (!Init::isUndefined(cp->rcvAPCIndex))
            createInt("RcvAPCIndex", cp->rcvAPCIndex, cpXML);

        if (cp->txRcvPolarization != DualPolarizationType::NOT_SET)
        {
            createString("TxRcvPolarization", six::toString<
                    DualPolarizationType>(cp->txRcvPolarization), cpXML);
        }
    }

    return rcvChanXML;
}

void ComplexXMLParser04x::parseRadarCollectionFromXML(
    const XMLElem radarCollectionXML,
    RadarCollection* radarCollection) const
{
    XMLElem tmpElem = getOptional(radarCollectionXML, "RefFreqIndex");
    if (tmpElem)
    {
        //optional
        parseInt(tmpElem, radarCollection->refFrequencyIndex);
    }

    tmpElem = getFirstAndOnly(radarCollectionXML, "TxFrequency");
    parseDouble(getFirstAndOnly(tmpElem, "Min"),
                radarCollection->txFrequencyMin);
    parseDouble(getFirstAndOnly(tmpElem, "Max"),
                radarCollection->txFrequencyMax);

    tmpElem = getOptional(radarCollectionXML, "TxPolarization");
    if (tmpElem)
    {
        //optional
        // Note that in SICD 0.4, UNKNOWN and SEQUENCE are not allowed here
        radarCollection->txPolarization
                = six::toType<PolarizationType>(tmpElem->getCharacterData());
    }

    tmpElem = getOptional(radarCollectionXML, "PolarizationHVAnglePoly");
    if (tmpElem)
    {
        //optional
        common().parsePoly1D(tmpElem, radarCollection->polarizationHVAnglePoly);
    }

    tmpElem = getOptional(radarCollectionXML, "TxSequence");
    if (tmpElem)
    {
        //optional
        parseTxSequenceFromXML(tmpElem, radarCollection->txSequence);
    }

    tmpElem = getOptional(radarCollectionXML, "Waveform");
    if (tmpElem)
    {
        //optional
        parseWaveformFromXML(tmpElem, radarCollection->waveform);
    }

    tmpElem = getFirstAndOnly(radarCollectionXML, "RcvChannels");

    //optional
    std::vector < XMLElem > channelsXML;
    tmpElem->getElementsByTagName("ChanParameters", channelsXML);
    for (std::vector<XMLElem>::const_iterator it = channelsXML.begin(); it
            != channelsXML.end(); ++it)
    {
        radarCollection->rcvChannels.resize(
            radarCollection->rcvChannels.size() + 1);
        radarCollection->rcvChannels.back().reset(new ChannelParameters());
        ChannelParameters* const chanParams =
            radarCollection->rcvChannels.back().get();

        XMLElem childXML = getOptional(*it, "RcvAPCIndex");
        if (childXML)
        {
            parseInt(childXML, chanParams->rcvAPCIndex);
        }

        childXML = getOptional(*it, "TxRcvPolarization");
        if (childXML)
        {
            //optional
            chanParams->txRcvPolarization
                    = six::toType<DualPolarizationType>(
                                                        childXML->getCharacterData());
        }
    }

    XMLElem areaXML = getOptional(radarCollectionXML, "Area");
    if (areaXML)
    {
        //optional
        parseAreaFromXML(areaXML, false, true, radarCollection->area);
    }

    common().parseParameters(radarCollectionXML, "Parameter",
                    radarCollection->parameters);
}
}
}
