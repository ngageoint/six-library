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
    ComplexXMLParser(version, false, std::auto_ptr<six::SICommonXMLParser>(
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
    XMLElem geoInfoXML = newElement("GeoInfo", parent);

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

    if (!geoInfo->name.empty())
        setAttribute(geoInfoXML, "name", geoInfo->name);

    for (size_t ii = 0; ii < geoInfo->geoInfos.size(); ++ii)
    {
        convertGeoInfoToXML(geoInfo->geoInfos[ii].get(), geoInfoXML);
    }

    return geoInfoXML;
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

    if (!Init::isUndefined<int>(radar->refFrequencyIndex))
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

XMLElem ComplexXMLParser10x::convertMatchInformationToXML(
    const MatchInformation* matchInfo, 
    XMLElem parent) const
{
    XMLElem matchInfoXML = newElement("MatchInfo", parent);

    createInt("NumMatchTypes", 
              matchInfo->types.size(),
              matchInfoXML);

    for (size_t i = 0; i < matchInfo->types.size(); ++i)
    {
        const MatchType* mt = matchInfo->types[i].get();
        XMLElem mtXML = newElement("MatchType", matchInfoXML);
        setAttribute(mtXML, "index", str::toString(i + 1));

        createString("TypeID", mt->typeID, mtXML);
        createInt("CurrentIndex", mt->currentIndex, mtXML);
        createInt("NumMatchCollections", mt->matchCollects.size(), mtXML);

        for (size_t j = 0; j < mt->matchCollects.size(); ++j)
        {
            XMLElem mcXML = newElement("MatchCollection", mtXML);
            setAttribute(mcXML, "index", str::toString(j + 1));

            createString("CoreName", mt->matchCollects[j].coreName, mcXML);
            createInt("MatchIndex", mt->matchCollects[j].matchIndex, mcXML);
            common().addParameters("Parameter", 
                mt->matchCollects[j].parameters, mcXML);
        }
    }

    return matchInfoXML;
}

XMLElem ComplexXMLParser10x::convertImageFormationToXML(
    const ImageFormation* imageFormation, 
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
        //  a specialized image formation algorithm so we return NULL.
        return NULL;
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
    const HalfPowerBeamwidths* halfPowerBeamwidths, 
    XMLElem parent) const
{
    //! this field was deprecated in 1.0.0
    return NULL;
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

void ComplexXMLParser10x::parseMatchInformationFromXML(
    const XMLElem matchInfoXML, 
    MatchInformation* matchInfo) const
{
    int numMatchTypes = 0;
    parseInt(getFirstAndOnly(matchInfoXML, "NumMatchTypes"), numMatchTypes);

    //TODO make sure there is at least one
    std::vector < XMLElem > typesXML;
    matchInfoXML->getElementsByTagName("MatchType", typesXML);

    //! validate the numMatchTypes
    if (typesXML.size() != (size_t)numMatchTypes)
    {
        throw except::Exception(
            Ctxt("NumMatchTypes does not match number of MatchType fields"));
    }

    for (size_t i = 0; i < typesXML.size(); i++)
    {
        // The MatchInformation object was given a MatchType when
        // it was instantiated.  The first time through, just populate it.
        if (i != 0)
        {
            matchInfo->types.push_back(
                mem::ScopedCopyablePtr<MatchType>(new MatchType()));
        }
        MatchType* type = matchInfo->types[i].get();

        parseString(getFirstAndOnly(typesXML[i], "TypeID"), type->typeID);

        XMLElem curIndexElem = getOptional(typesXML[i], "CurrentIndex");
        if (curIndexElem)
        {
            //optional
            parseInt(curIndexElem, type->currentIndex);
        }

        int numMatchCollections = 0;
        parseInt(getFirstAndOnly(typesXML[i], "NumMatchCollections"), 
                 numMatchCollections);

        std::vector < XMLElem > matchCollectionsXML;
        typesXML[i]->getElementsByTagName("MatchCollection", matchCollectionsXML);

        //! validate the numMatchTypes
        if (matchCollectionsXML.size() !=
            static_cast<size_t>(numMatchCollections))
        {
            throw except::Exception(
                Ctxt("NumMatchCollections does not match number of " \
                     "MatchCollect fields"));
        }

        // Need to make sure this is resized properly - at MatchType
        // construction time, matchCollects is initialized to size 1, but in
        // SICD 1.1 this entire block may be missing.
        type->matchCollects.resize(matchCollectionsXML.size());
        for (size_t jj = 0; jj < matchCollectionsXML.size(); jj++)
        {
            MatchCollect& collect(type->matchCollects[jj]);

            parseString(getFirstAndOnly(
                matchCollectionsXML[jj], "CoreName"), collect.coreName);

            XMLElem matchIndexXML = 
                getOptional(matchCollectionsXML[jj], "MatchIndex");
            if (matchIndexXML)
            {
                parseInt(matchIndexXML, collect.matchIndex);
            }

            common().parseParameters(
                matchCollectionsXML[jj], "Parameter", collect.parameters);
        }
    }
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
    setAttribute(rcvChanXML, "size", str::toString(numChannels));
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        const ChannelParameters* const cp = radar->rcvChannels[ii].get();
        XMLElem cpXML = newElement("ChanParameters", rcvChanXML);
        setAttribute(cpXML, "index", str::toString(ii + 1));

        //! required in 1.0
        createString(
            "TxRcvPolarization",
            six::toString<DualPolarizationType>(cp->txRcvPolarization),
            cpXML);

        if (!Init::isUndefined<int>(cp->rcvAPCIndex))
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
