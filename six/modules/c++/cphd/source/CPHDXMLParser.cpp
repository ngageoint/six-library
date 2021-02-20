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
#include <set>
#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <mem/ScopedCopyablePtr.h>
#include <str/Convert.h>
#include <six/Utilities.h>
#include <six/SICommonXMLParser.h>
#include <cphd/CPHDXMLParser.h>
#include <cphd/Enums.h>
#include <cphd/Metadata.h>
#include <cphd/Types.h>


#define ENFORCESPEC 0

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace cphd
{

CPHDXMLParser::CPHDXMLParser(
        const std::string& uri,
        bool addClassAttributes,
        logging::Logger* log,
        bool ownLog) :
    six::XMLParser(uri, addClassAttributes, log, ownLog),
    mCommon(getDefaultURI(), addClassAttributes, getDefaultURI(), log)
{
}

/*
 * TO XML
 */
std::unique_ptr<xml::lite::Document> CPHDXMLParser::toXML(
        const Metadata& metadata)
{
    std::unique_ptr<xml::lite::Document> doc(new xml::lite::Document());

    XMLElem root = newElement("CPHD");
    doc->setRootElement(root);

    toXML(metadata.collectionID, root);
    toXML(metadata.global, root);
    toXML(metadata.sceneCoordinates, root);
    toXML(metadata.data, root);
    toXML(metadata.channel, root);
    toXML(metadata.pvp, root);
    if (metadata.supportArray.get())
    {
        toXML(*(metadata.supportArray), root);
    }
    toXML(metadata.dwell, root);
    toXML(metadata.referenceGeometry, root);
    if (metadata.antenna.get())
    {
        toXML(*(metadata.antenna), root);
    }
    if (metadata.txRcv.get())
    {
        toXML(*(metadata.txRcv), root);
    }
    if (metadata.errorParameters.get())
    {
        toXML(*(metadata.errorParameters), root);
    }
    if (metadata.productInfo.get())
    {
        toXML(*(metadata.productInfo), root);
    }
    for (size_t ii = 0; ii < metadata.geoInfo.size(); ++ii)
    {
        toXML(metadata.geoInfo[ii], root);
    }
    if (metadata.matchInfo.get())
    {
        toXML(*(metadata.matchInfo), root);
    }
    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());

    return doc;
}

XMLElem CPHDXMLParser::toXML(const CollectionInformation& collectionID, XMLElem parent)
{
    XMLElem collectionXML = newElement("CollectionID", parent);

    createString("CollectorName", collectionID.collectorName, collectionXML);
    if(!six::Init::isUndefined(collectionID.illuminatorName))
    {
        createString("IlluminatorName", collectionID.illuminatorName, collectionXML);
    }
    createString("CoreName", collectionID.coreName, collectionXML);
    createString("CollectType", collectionID.collectType, collectionXML);

    // RadarMode
    XMLElem radarModeXML = newElement("RadarMode", collectionXML);
    createSixString("ModeType", collectionID.radarMode, radarModeXML);
    if(!six::Init::isUndefined(collectionID.radarModeID))
    {
        createString("ModeID", collectionID.radarModeID, radarModeXML);
    }
    createString("Classification", collectionID.getClassificationLevel(), collectionXML);
    createString("ReleaseInfo", collectionID.releaseInfo, collectionXML);
    if (!collectionID.countryCodes.empty())
    {
        std::string countryCodes = str::join(collectionID.countryCodes, ",");
        createString("CountryCode", countryCodes, collectionXML);
    }
    mCommon.addParameters("Parameter", getDefaultURI(), collectionID.parameters, collectionXML);
    return collectionXML;
}

XMLElem CPHDXMLParser::toXML(const Global& global, XMLElem parent)
{
    XMLElem globalXML = newElement("Global", parent);
    createString("DomainType", global.domainType, globalXML);
    createString("SGN", global.sgn, globalXML);

    //Timeline
    XMLElem timelineXML = newElement("Timeline", globalXML);
    createDateTime("CollectionStart", global.timeline.collectionStart, timelineXML);
    if (!six::Init::isUndefined(global.timeline.rcvCollectionStart))
    {
        createDateTime("RcvCollectionStart", global.timeline.rcvCollectionStart, timelineXML);
    }
    createDouble("TxTime1", global.timeline.txTime1, timelineXML);
    createDouble("TxTime2", global.timeline.txTime2, timelineXML);

    XMLElem fxBandXML = newElement("FxBand", globalXML);
    createDouble("FxMin", global.fxBand.fxMin, fxBandXML);
    createDouble("FxMax", global.fxBand.fxMax, fxBandXML);

    XMLElem toaSwathXML = newElement("TOASwath", globalXML);
    createDouble("TOAMin", global.toaSwath.toaMin, toaSwathXML);
    createDouble("TOAMax", global.toaSwath.toaMax, toaSwathXML);

    if (global.tropoParameters.get())
    {
        XMLElem tropoXML = newElement("TropoParameters", globalXML);
        createDouble("N0", global.tropoParameters->n0, tropoXML);
        createString("RefHeight", global.tropoParameters->refHeight, tropoXML);
    }
    if (global.ionoParameters.get())
    {
        XMLElem ionoXML = newElement("IonoParameters", globalXML);
        createDouble("TECV", global.ionoParameters->tecv, ionoXML);
        if (!six::Init::isUndefined(global.ionoParameters->f2Height))
        {
            createDouble("F2Height", global.ionoParameters->f2Height, ionoXML);
        }
    }
    return globalXML;
}

XMLElem CPHDXMLParser::toXML(const SceneCoordinates& sceneCoords, XMLElem parent)
{
    XMLElem sceneCoordsXML = newElement("SceneCoordinates", parent);
    createString("EarthModel", sceneCoords.earthModel, sceneCoordsXML);

    XMLElem iarpXML = newElement("IARP", sceneCoordsXML);
    mCommon.createVector3D("ECF", sceneCoords.iarp.ecf, iarpXML);
    mCommon.createLatLonAlt("LLH", sceneCoords.iarp.llh, iarpXML);

    XMLElem refSurfXML = newElement("ReferenceSurface", sceneCoordsXML);
    if (sceneCoords.referenceSurface.planar.get())
    {
        XMLElem planarXML = newElement("Planar", refSurfXML);
        mCommon.createVector3D("uIAX", sceneCoords.referenceSurface.planar->uIax, planarXML);
        mCommon.createVector3D("uIAY", sceneCoords.referenceSurface.planar->uIay, planarXML);
    }
    else if (sceneCoords.referenceSurface.hae.get())
    {
        XMLElem haeXML = newElement("HAE", refSurfXML);
        mCommon.createLatLon("uIAXLL", sceneCoords.referenceSurface.hae->uIax, haeXML);
        mCommon.createLatLon("uIAYLL", sceneCoords.referenceSurface.hae->uIay, haeXML);
    }
    else
    {
        throw except::Exception(Ctxt(
                "Reference Surface must be one of two types"));
    }

    XMLElem imageAreaXML = newElement("ImageArea", sceneCoordsXML);
    mCommon.createVector2D("X1Y1", sceneCoords.imageArea.x1y1, imageAreaXML);
    mCommon.createVector2D("X2Y2", sceneCoords.imageArea.x2y2, imageAreaXML);

    if (!sceneCoords.imageArea.polygon.empty())
    {
        XMLElem polygonXML = newElement("Polygon", imageAreaXML);
        setAttribute(polygonXML, "size", sceneCoords.imageArea.polygon.size());
        for (size_t ii = 0; ii < sceneCoords.imageArea.polygon.size(); ++ii)
        {
            XMLElem vertexXML = mCommon.createVector2D("Vertex", sceneCoords.imageArea.polygon[ii], polygonXML);
            setAttribute(vertexXML, "index", ii+1);
        }
    }
    createLatLonFootprint("ImageAreaCornerPoints", "IACP", sceneCoords.imageAreaCorners, sceneCoordsXML);

    // Extended Area (Optional)
    if(sceneCoords.extendedArea.get())
    {
        XMLElem extendedAreaXML = newElement("ExtendedArea", sceneCoordsXML);
        mCommon.createVector2D("X1Y1", sceneCoords.extendedArea->x1y1, extendedAreaXML);
        mCommon.createVector2D("X2Y2", sceneCoords.extendedArea->x2y2, extendedAreaXML);

        if (!sceneCoords.extendedArea->polygon.empty())
        {
            XMLElem polygonXML = newElement("Polygon", sceneCoordsXML);
            setAttribute(polygonXML, "size", sceneCoords.extendedArea->polygon.size());
            for (size_t ii = 0; ii < sceneCoords.extendedArea->polygon.size(); ++ii)
            {
                XMLElem vertexXML = mCommon.createVector2D("Vertex", sceneCoords.extendedArea->polygon[ii], polygonXML);
                setAttribute(vertexXML, "index", ii+1);
            }
        }
    }

    // ImageGrid (Optional)
    if(sceneCoords.imageGrid.get())
    {
        XMLElem imageGridXML = newElement("ImageGrid", sceneCoordsXML);
        if(!six::Init::isUndefined(sceneCoords.imageGrid->identifier))
        {
            createString("Identifier", sceneCoords.imageGrid->identifier, imageGridXML);
        }
        XMLElem iarpLocationXML = newElement("IARPLocation", imageGridXML);
        createDouble("Line", sceneCoords.imageGrid->iarpLocation.line, iarpLocationXML);
        createDouble("Sample", sceneCoords.imageGrid->iarpLocation.sample, iarpLocationXML);

        XMLElem iaxExtentXML = newElement("IAXExtent", imageGridXML);
        createDouble("LineSpacing", sceneCoords.imageGrid->xExtent.lineSpacing, iaxExtentXML);
        createInt("FirstLine", sceneCoords.imageGrid->xExtent.firstLine, iaxExtentXML);
        createInt("NumLines", sceneCoords.imageGrid->xExtent.numLines, iaxExtentXML);

        XMLElem iayExtentXML = newElement("IAYExtent", imageGridXML);
        createDouble("SampleSpacing", sceneCoords.imageGrid->yExtent.sampleSpacing, iayExtentXML);
        createInt("FirstSample", sceneCoords.imageGrid->yExtent.firstSample, iayExtentXML);
        createInt("NumSamples", sceneCoords.imageGrid->yExtent.numSamples, iayExtentXML);

        if (!sceneCoords.imageGrid->segments.empty())
        {
            XMLElem segmentListXML = newElement("SegmentList", imageGridXML);
            createInt("NumSegments", sceneCoords.imageGrid->segments.size(), segmentListXML);

            for (size_t ii = 0; ii < sceneCoords.imageGrid->segments.size(); ++ii)
            {
                XMLElem segmentXML = newElement("Segment", segmentListXML);
                createString("Identifier", sceneCoords.imageGrid->segments[ii].identifier, segmentXML);
                createInt("StartLine", sceneCoords.imageGrid->segments[ii].startLine, segmentXML);
                createInt("StartSample", sceneCoords.imageGrid->segments[ii].startSample, segmentXML);
                createInt("EndLine", sceneCoords.imageGrid->segments[ii].endLine, segmentXML);
                createInt("EndSample", sceneCoords.imageGrid->segments[ii].endSample, segmentXML);

                if (!sceneCoords.imageGrid->segments[ii].polygon.empty())
                {
                    XMLElem polygonXML = newElement("SegmentPolygon", segmentXML);
                    setAttribute(polygonXML, "size", sceneCoords.imageGrid->segments[ii].polygon.size());
                    for (size_t jj = 0; jj < sceneCoords.imageGrid->segments[ii].polygon.size(); ++jj)
                    {
                        XMLElem svXML = newElement("SV", polygonXML);
                        setAttribute(svXML, "index", sceneCoords.imageGrid->segments[ii].polygon[jj].getIndex());
                        createDouble("Line", sceneCoords.imageGrid->segments[ii].polygon[jj].line, svXML);
                        createDouble("Sample", sceneCoords.imageGrid->segments[ii].polygon[jj].sample, svXML);
                    }
                }
            }
        }
    }
    return sceneCoordsXML;
}

XMLElem CPHDXMLParser::toXML(const Data& data, XMLElem parent)
{
    XMLElem dataXML = newElement("Data", parent);
    createString("SignalArrayFormat", data.signalArrayFormat, dataXML);
    createInt("NumBytesPVP", data.numBytesPVP, dataXML);
    createInt("NumCPHDChannels", data.channels.size(), dataXML);
    if (!six::Init::isUndefined(data.signalCompressionID))
    {
        createString("SignalCompressionID", data.signalCompressionID, dataXML);
    }

    for (size_t ii = 0; ii < data.channels.size(); ++ii)
    {
        XMLElem channelXML = newElement("Channel", dataXML);
        createString("Identifier", data.channels[ii].identifier, channelXML);
        createInt("NumVectors", data.channels[ii].numVectors, channelXML);
        createInt("NumSamples", data.channels[ii].numSamples, channelXML);
        createInt("SignalArrayByteOffset", data.channels[ii].signalArrayByteOffset, channelXML);
        createInt("PVPArrayByteOffset", data.channels[ii].pvpArrayByteOffset, channelXML);
        if(!six::Init::isUndefined(data.channels[ii].compressedSignalSize))
        {
            createInt("CompressedSignalSize", data.channels[ii].compressedSignalSize, channelXML);
        }
    }
    createInt("NumSupportArrays", data.supportArrayMap.size(), dataXML);
    for (auto it = data.supportArrayMap.begin(); it != data.supportArrayMap.end(); ++it)
    {
        XMLElem supportArrayXML = newElement("SupportArray", dataXML);
        createString("Identifier", it->second.identifier, supportArrayXML);
        createInt("NumRows", it->second.numRows, supportArrayXML);
        createInt("NumCols", it->second.numCols, supportArrayXML);
        createInt("BytesPerElement", it->second.bytesPerElement, supportArrayXML);
        createInt("ArrayByteOffset", it->second.arrayByteOffset, supportArrayXML);
    }
    return dataXML;
}

XMLElem CPHDXMLParser::toXML(const Channel& channel, XMLElem parent)
{
    XMLElem channelXML = newElement("Channel", parent);
    createString("RefChId", channel.refChId, channelXML);
    createBooleanType("FXFixedCPHD", channel.fxFixedCphd, channelXML);
    createBooleanType("TOAFixedCPHD", channel.toaFixedCphd, channelXML);
    createBooleanType("SRPFixedCPHD", channel.srpFixedCphd, channelXML);

    for (size_t ii = 0; ii < channel.parameters.size(); ++ii)
    {
        XMLElem parametersXML = newElement("Parameters", channelXML);
        createString("Identifier", channel.parameters[ii].identifier, parametersXML);
        createInt("RefVectorIndex", channel.parameters[ii].refVectorIndex, parametersXML);
        createBooleanType("FXFixed", channel.parameters[ii].fxFixed, parametersXML);
        createBooleanType("TOAFixed", channel.parameters[ii].toaFixed, parametersXML);
        createBooleanType("SRPFixed", channel.parameters[ii].srpFixed, parametersXML);
        if (!six::Init::isUndefined(channel.parameters[ii].signalNormal))
        {
            createBooleanType("SignalNormal", channel.parameters[ii].signalNormal, parametersXML);
        }
        XMLElem polXML = newElement("Polarization", parametersXML);
        createString("TxPol", channel.parameters[ii].polarization.txPol, polXML);
        createString("RcvPol", channel.parameters[ii].polarization.rcvPol, polXML);
        createDouble("FxC", channel.parameters[ii].fxC, parametersXML);
        createDouble("FxBW", channel.parameters[ii].fxBW, parametersXML);
        if(!six::Init::isUndefined(channel.parameters[ii].fxBWNoise))
        {
            createDouble("FxBWNoise", channel.parameters[ii].fxBWNoise, parametersXML);
        }
        createDouble("TOASaved", channel.parameters[ii].toaSaved, parametersXML);

        if(channel.parameters[ii].toaExtended.get())
        {
            XMLElem toaExtendedXML = newElement("TOAExtended", parametersXML);
            createDouble("TOAExtSaved", channel.parameters[ii].toaExtended->toaExtSaved, toaExtendedXML);
            if(channel.parameters[ii].toaExtended->lfmEclipse.get())
            {
                XMLElem lfmEclipseXML = newElement("LFMEclipse", toaExtendedXML);
                createDouble("FxEarlyLow", channel.parameters[ii].toaExtended->lfmEclipse->fxEarlyLow, lfmEclipseXML);
                createDouble("FxEarlyHigh", channel.parameters[ii].toaExtended->lfmEclipse->fxEarlyHigh, lfmEclipseXML);
                createDouble("FxLateLow", channel.parameters[ii].toaExtended->lfmEclipse->fxLateLow, lfmEclipseXML);
                createDouble("FxLateHigh", channel.parameters[ii].toaExtended->lfmEclipse->fxLateHigh, lfmEclipseXML);
            }
        }
        XMLElem dwellTimesXML = newElement("DwellTimes", parametersXML);
        createString("CODId", channel.parameters[ii].dwellTimes.codId, dwellTimesXML);
        createString("DwellId", channel.parameters[ii].dwellTimes.dwellId, dwellTimesXML);
        if(!six::Init::isUndefined(channel.parameters[ii].imageArea))
        {
            XMLElem imageAreaXML = newElement("ImageArea", parametersXML);
            mCommon.createVector2D("X1Y1", channel.parameters[ii].imageArea.x1y1, imageAreaXML);
            mCommon.createVector2D("X2Y2", channel.parameters[ii].imageArea.x2y2, imageAreaXML);
            if(!channel.parameters[ii].imageArea.polygon.empty())
            {
                XMLElem polygonXML = newElement("Polygon", imageAreaXML);
                setAttribute(polygonXML, "size", channel.parameters[ii].imageArea.polygon.size());
                for (size_t jj = 0; jj < channel.parameters[ii].imageArea.polygon.size(); ++jj)
                {
                    XMLElem vertexXML = mCommon.createVector2D("Vertex", channel.parameters[ii].imageArea.polygon[jj], polygonXML);
                    setAttribute(vertexXML, "index", jj+1);
                }
            }
        }
        if(channel.parameters[ii].antenna.get())
        {
            XMLElem antennaXML = newElement("Antenna", parametersXML);
            createString("TxAPCId", channel.parameters[ii].antenna->txAPCId, antennaXML);
            createString("TxAPATId", channel.parameters[ii].antenna->txAPATId, antennaXML);
            createString("RcvAPCId", channel.parameters[ii].antenna->rcvAPCId, antennaXML);
            createString("RcvAPATId", channel.parameters[ii].antenna->rcvAPATId, antennaXML);
        }
        if(channel.parameters[ii].txRcv.get())
        {
            XMLElem txRcvXML = newElement("TxRcv", parametersXML);
            for (size_t jj = 0; jj < channel.parameters[ii].txRcv->txWFId.size(); ++jj)
            {
                createString("TxWFId", channel.parameters[ii].txRcv->txWFId[jj], txRcvXML);
            }
            for (size_t jj = 0; jj < channel.parameters[ii].txRcv->rcvId.size(); ++jj)
            {
                createString("RcvId", channel.parameters[ii].txRcv->rcvId[jj], txRcvXML);
            }
        }
        if(channel.parameters[ii].tgtRefLevel.get())
        {
            XMLElem tgtRefXML = newElement("TgtRefLevel", parametersXML);
            createDouble("PTRef", channel.parameters[ii].tgtRefLevel->ptRef, tgtRefXML);
        }
        if(channel.parameters[ii].noiseLevel.get())
        {
            XMLElem noiseLevelXML = newElement("NoiseLevel", parametersXML);
            createDouble("PNRef", channel.parameters[ii].noiseLevel->pnRef, noiseLevelXML);
            createDouble("BNRef", channel.parameters[ii].noiseLevel->bnRef, noiseLevelXML);
            if(channel.parameters[ii].noiseLevel->fxNoiseProfile.get())
            {
                XMLElem fxNoiseProfileXML = newElement("FxNoiseProfile", noiseLevelXML);
                for (size_t jj = 0; jj < channel.parameters[ii].noiseLevel->fxNoiseProfile->point.size(); ++jj)
                {
                    XMLElem pointXML = newElement("Point", fxNoiseProfileXML);
                    createDouble("Fx", channel.parameters[ii].noiseLevel->fxNoiseProfile->point[jj].fx, pointXML);
                    createDouble("PN", channel.parameters[ii].noiseLevel->fxNoiseProfile->point[jj].pn, pointXML);
                }
            }
        }
    }
    if(!channel.addedParameters.empty())
    {
        XMLElem addedParamsXML = newElement("AddedParameters", channelXML);
        mCommon.addParameters("Parameter", getDefaultURI(), channel.addedParameters, addedParamsXML);
    }
    return channelXML;
}

XMLElem CPHDXMLParser::toXML(const Pvp& pvp, XMLElem parent)
{
    XMLElem pvpXML = newElement("PVP", parent);
    createPVPType("TxTime", pvp.txTime, pvpXML);
    createPVPType("TxPos", pvp.txPos, pvpXML);
    createPVPType("TxVel", pvp.txVel, pvpXML);
    createPVPType("RcvTime", pvp.rcvTime, pvpXML);
    createPVPType("RcvPos", pvp.rcvPos, pvpXML);
    createPVPType("RcvVel", pvp.rcvVel, pvpXML);
    createPVPType("SRPPos", pvp.srpPos, pvpXML);
    if (!six::Init::isUndefined<size_t>(pvp.ampSF.getOffset()))
    {
        createPVPType("AmpSF", pvp.ampSF, pvpXML);
    }
    createPVPType("aFDOP", pvp.aFDOP, pvpXML);
    createPVPType("aFRR1", pvp.aFRR1, pvpXML);
    createPVPType("aFRR2", pvp.aFRR2, pvpXML);
    createPVPType("FX1", pvp.fx1, pvpXML);
    createPVPType("FX2", pvp.fx2, pvpXML);
    if (!six::Init::isUndefined<size_t>(pvp.fxN1.getOffset()))
    {
        createPVPType("FXN1", pvp.fxN1, pvpXML);
    }
    if (!six::Init::isUndefined<size_t>(pvp.fxN2.getOffset()))
    {
        createPVPType("FXN2", pvp.fxN2, pvpXML);
    }
    createPVPType("TOA1", pvp.toa1, pvpXML);
    createPVPType("TOA2", pvp.toa2, pvpXML);
    if (!six::Init::isUndefined<size_t>(pvp.toaE1.getOffset()))
    {
        createPVPType("TOAE1", pvp.toaE1, pvpXML);
    }
    if (!six::Init::isUndefined<size_t>(pvp.toaE2.getOffset()))
    {
        createPVPType("TOAE2", pvp.toaE2, pvpXML);
    }
    createPVPType("TDTropoSRP", pvp.tdTropoSRP, pvpXML);
    if (!six::Init::isUndefined<size_t>(pvp.tdIonoSRP.getOffset()))
    {
        createPVPType("TDIonoSRP", pvp.tdIonoSRP, pvpXML);
    }
    createPVPType("SC0", pvp.sc0, pvpXML);
    createPVPType("SCSS", pvp.scss, pvpXML);
    if (!six::Init::isUndefined<size_t>(pvp.signal.getOffset()))
    {
        createPVPType("SIGNAL", pvp.signal, pvpXML);
    }
    for (auto it = pvp.addedPVP.begin(); it != pvp.addedPVP.end(); ++it)
    {
        createAPVPType("AddedPVP", it->second, pvpXML);
    }

    return pvpXML;
}

//Assumes optional handled by caller
XMLElem CPHDXMLParser::toXML(const SupportArray& supports, XMLElem parent)
{
    XMLElem supportsXML = newElement("SupportArray", parent);
    if (!supports.iazArray.empty())
    {
        for (size_t ii = 0; ii < supports.iazArray.size(); ++ii)
        {
            XMLElem iazArrayXML = newElement("IAZArray", supportsXML);
            createInt("Identifier", supports.iazArray[ii].getIdentifier(), iazArrayXML);
            createString("ElementFormat", supports.iazArray[ii].elementFormat, iazArrayXML);
            createDouble("X0", supports.iazArray[ii].x0, iazArrayXML);
            createDouble("Y0", supports.iazArray[ii].y0, iazArrayXML);
            createDouble("XSS", supports.iazArray[ii].xSS, iazArrayXML);
            createDouble("YSS", supports.iazArray[ii].ySS, iazArrayXML);
        }
    }
    if (!supports.antGainPhase.empty())
    {
        for (size_t ii = 0; ii < supports.antGainPhase.size(); ++ii)
        {
            XMLElem antGainPhaseXML = newElement("AntGainPhase", supportsXML);
            createInt("Identifier", supports.antGainPhase[ii].getIdentifier(), antGainPhaseXML);
            createString("ElementFormat", supports.antGainPhase[ii].elementFormat, antGainPhaseXML);
            createDouble("X0", supports.antGainPhase[ii].x0, antGainPhaseXML);
            createDouble("Y0", supports.antGainPhase[ii].y0, antGainPhaseXML);
            createDouble("XSS", supports.antGainPhase[ii].xSS, antGainPhaseXML);
            createDouble("YSS", supports.antGainPhase[ii].ySS, antGainPhaseXML);
        }
    }
    if (!supports.addedSupportArray.empty())
    {
        for (auto it = supports.addedSupportArray.begin(); it != supports.addedSupportArray.end(); ++it)
        {
            XMLElem addedSupportArrayXML = newElement("AddedSupportArray", supportsXML);
            createString("Identifier", it->first, addedSupportArrayXML);
            createString("ElementFormat", it->second.elementFormat, addedSupportArrayXML);
            createDouble("X0", it->second.x0, addedSupportArrayXML);
            createDouble("Y0", it->second.y0, addedSupportArrayXML);
            createDouble("XSS", it->second.xSS, addedSupportArrayXML);
            createDouble("YSS", it->second.ySS, addedSupportArrayXML);
            createString("XUnits", it->second.xUnits, addedSupportArrayXML);
            createString("YUnits", it->second.yUnits, addedSupportArrayXML);
            createString("ZUnits", it->second.zUnits, addedSupportArrayXML);
            mCommon.addParameters("Parameter", getDefaultURI(), it->second.parameter, addedSupportArrayXML);
        }
    }
    return supportsXML;
}

XMLElem CPHDXMLParser::toXML(const Dwell& dwell, XMLElem parent)
{
    XMLElem dwellXML = newElement("Dwell", parent);
    createInt("NumCODTimes", dwell.cod.size(), dwellXML);

    for (size_t ii = 0; ii < dwell.cod.size(); ++ii)
    {
        XMLElem codTimeXML = newElement("CODTime", dwellXML);
        createString("Identifier", dwell.cod[ii].identifier, codTimeXML);
        mCommon.createPoly2D("CODTimePoly", dwell.cod[ii].codTimePoly, codTimeXML);
    }
    createInt("NumDwellTimes", dwell.dtime.size(), dwellXML);
    for (size_t ii = 0; ii < dwell.dtime.size(); ++ii)
    {
        XMLElem dwellTimeXML = newElement("DwellTime", dwellXML);
        createString("Identifier", dwell.dtime[ii].identifier, dwellTimeXML);
        mCommon.createPoly2D("DwellTimePoly", dwell.dtime[ii].dwellTimePoly, dwellTimeXML);
    }
    return dwellXML;
}

XMLElem CPHDXMLParser::toXML(const ReferenceGeometry& refGeo, XMLElem parent)
{
    XMLElem refGeoXML = newElement("ReferenceGeometry", parent);
    XMLElem srpXML = newElement("SRP", refGeoXML);
    mCommon.createVector3D("ECF", refGeo.srp.ecf, srpXML);
    mCommon.createVector3D("IAC", refGeo.srp.iac, srpXML);
    createDouble("ReferenceTime", refGeo.referenceTime, refGeoXML);
    createDouble("SRPCODTime", refGeo.srpCODTime, refGeoXML);
    createDouble("SRPDwellTime", refGeo.srpDwellTime, refGeoXML);

    if (refGeo.monostatic.get())
    {
        XMLElem monoXML = newElement("Monostatic", refGeoXML);
        mCommon.createVector3D("ARPPos", refGeo.monostatic->arpPos, monoXML);
        mCommon.createVector3D("ARPVel", refGeo.monostatic->arpVel, monoXML);
        std::string side = refGeo.monostatic->sideOfTrack;
        createString("SideOfTrack", (side == "LEFT" ? "L" : "R"), monoXML);
        createDouble("SlantRange", refGeo.monostatic->slantRange, monoXML);
        createDouble("GroundRange", refGeo.monostatic->groundRange, monoXML);
        createDouble("DopplerConeAngle", refGeo.monostatic->dopplerConeAngle, monoXML);
        createDouble("GrazeAngle", refGeo.monostatic->grazeAngle, monoXML);
        createDouble("IncidenceAngle", refGeo.monostatic->incidenceAngle, monoXML);
        createDouble("AzimuthAngle", refGeo.monostatic->azimuthAngle, monoXML);
        createDouble("TwistAngle", refGeo.monostatic->twistAngle, monoXML);
        createDouble("SlopeAngle", refGeo.monostatic->slopeAngle, monoXML);
        createDouble("LayoverAngle", refGeo.monostatic->layoverAngle, monoXML);
    }
    else if(refGeo.bistatic.get())
    {
        XMLElem biXML = newElement("Bistatic", refGeoXML);
        createDouble("AzimuthAngle", refGeo.bistatic->azimuthAngle, biXML);
        createDouble("AzimuthAngleRate", refGeo.bistatic->azimuthAngleRate, biXML);
        createDouble("BistaticAngle", refGeo.bistatic->bistaticAngle, biXML);
        createDouble("BistaticAngleRate", refGeo.bistatic->bistaticAngleRate, biXML);
        createDouble("GrazeAngle", refGeo.bistatic->grazeAngle, biXML);
        createDouble("TwistAngle", refGeo.bistatic->twistAngle, biXML);
        createDouble("SlopeAngle", refGeo.bistatic->slopeAngle, biXML);
        createDouble("LayoverAngle", refGeo.bistatic->layoverAngle, biXML);
        XMLElem txPlatXML = newElement("TxPlatform", biXML);
        createDouble("Time", refGeo.bistatic->txPlatform.time, txPlatXML);
        mCommon.createVector3D("Pos", refGeo.bistatic->txPlatform.pos, txPlatXML);
        mCommon.createVector3D("Vel", refGeo.bistatic->txPlatform.vel, txPlatXML);

        {
            const std::string side = refGeo.bistatic->txPlatform.sideOfTrack;
            createString("SideOfTrack", (side == "LEFT" ? "L" : "R"), txPlatXML);
        }
        createDouble("SlantRange", refGeo.bistatic->txPlatform.slantRange, txPlatXML);
        createDouble("GroundRange", refGeo.bistatic->txPlatform.groundRange, txPlatXML);
        createDouble("DopplerConeAngle", refGeo.bistatic->txPlatform.dopplerConeAngle, txPlatXML);
        createDouble("GrazeAngle", refGeo.bistatic->txPlatform.grazeAngle, txPlatXML);
        createDouble("IncidenceAngle", refGeo.bistatic->txPlatform.incidenceAngle, txPlatXML);
        createDouble("AzimuthAngle", refGeo.bistatic->txPlatform.azimuthAngle, txPlatXML);
        XMLElem rcvPlatXML = newElement("RcvPlatform", biXML);
        createDouble("Time", refGeo.bistatic->rcvPlatform.time, rcvPlatXML);
        mCommon.createVector3D("Pos", refGeo.bistatic->rcvPlatform.pos, rcvPlatXML);
        mCommon.createVector3D("Vel", refGeo.bistatic->rcvPlatform.vel, rcvPlatXML);

        {
            const std::string side = refGeo.bistatic->rcvPlatform.sideOfTrack;
            createString("SideOfTrack", (side == "LEFT" ? "L" : "R"), rcvPlatXML);
        }
        createDouble("SlantRange", refGeo.bistatic->rcvPlatform.slantRange, rcvPlatXML);
        createDouble("GroundRange", refGeo.bistatic->rcvPlatform.groundRange, rcvPlatXML);
        createDouble("DopplerConeAngle", refGeo.bistatic->rcvPlatform.dopplerConeAngle, rcvPlatXML);
        createDouble("GrazeAngle", refGeo.bistatic->rcvPlatform.grazeAngle, rcvPlatXML);
        createDouble("IncidenceAngle", refGeo.bistatic->rcvPlatform.incidenceAngle, rcvPlatXML);
        createDouble("AzimuthAngle", refGeo.bistatic->rcvPlatform.azimuthAngle, rcvPlatXML);
    }
    return refGeoXML;
}

XMLElem CPHDXMLParser::toXML(const Antenna& antenna, XMLElem parent)
{
    XMLElem antennaXML = newElement("Antenna", parent);
    createInt("NumACFs", antenna.antCoordFrame.size(), antennaXML);
    createInt("NumAPCs", antenna.antPhaseCenter.size(), antennaXML);
    createInt("NumAntPats", antenna.antPattern.size(), antennaXML);
    for (size_t ii = 0; ii < antenna.antCoordFrame.size(); ++ii)
    {
        XMLElem antCoordFrameXML = newElement("AntCoordFrame", antennaXML);
        createString("Identifier", antenna.antCoordFrame[ii].identifier, antCoordFrameXML);
        mCommon.createPolyXYZ("XAxisPoly", antenna.antCoordFrame[ii].xAxisPoly, antCoordFrameXML);
        mCommon.createPolyXYZ("YAxisPoly", antenna.antCoordFrame[ii].yAxisPoly, antCoordFrameXML);
    }
    for (size_t ii = 0; ii < antenna.antPhaseCenter.size(); ++ii)
    {
        XMLElem antPhaseCenterXML = newElement("AntPhaseCenter", antennaXML);
        createString("Identifier", antenna.antPhaseCenter[ii].identifier, antPhaseCenterXML);
        createString("ACFId", antenna.antPhaseCenter[ii].acfId, antPhaseCenterXML);
        mCommon.createVector3D("APCXYZ", antenna.antPhaseCenter[ii].apcXYZ, antPhaseCenterXML);
    }
    for (size_t ii = 0; ii < antenna.antPattern.size(); ++ii)
    {
        XMLElem antPatternXML = newElement("AntPattern", antennaXML);
        createString("Identifier", antenna.antPattern[ii].identifier, antPatternXML);
        createDouble("FreqZero", antenna.antPattern[ii].freqZero, antPatternXML);
        if (!six::Init::isUndefined(antenna.antPattern[ii].gainZero))
        {
            createDouble("GainZero", antenna.antPattern[ii].gainZero, antPatternXML);
        }
        if (!six::Init::isUndefined(antenna.antPattern[ii].ebFreqShift))
        {
            createBooleanType("EBFreqShift", antenna.antPattern[ii].ebFreqShift, antPatternXML);
        }
        if (!six::Init::isUndefined(antenna.antPattern[ii].mlFreqDilation))
        {
            createBooleanType("MLFreqDilation", antenna.antPattern[ii].mlFreqDilation, antPatternXML);
        }
        if (!six::Init::isUndefined(antenna.antPattern[ii].gainBSPoly))
        {
            mCommon.createPoly1D("GainBSPoly", antenna.antPattern[ii].gainBSPoly, antPatternXML);
        }
        XMLElem ebXML = newElement("EB", antPatternXML);
        mCommon.createPoly1D("DCXPoly", antenna.antPattern[ii].eb.dcxPoly, ebXML);
        mCommon.createPoly1D("DCYPoly", antenna.antPattern[ii].eb.dcyPoly, ebXML);
        XMLElem arrayXML = newElement("Array", antPatternXML);
        mCommon.createPoly2D("GainPoly", antenna.antPattern[ii].array.gainPoly, arrayXML);
        mCommon.createPoly2D("PhasePoly", antenna.antPattern[ii].array.phasePoly, arrayXML);
        XMLElem elementXML = newElement("Element", antPatternXML);
        mCommon.createPoly2D("GainPoly", antenna.antPattern[ii].element.gainPoly, elementXML);
        mCommon.createPoly2D("PhasePoly", antenna.antPattern[ii].element.phasePoly, elementXML);
        for (size_t jj = 0; jj < antenna.antPattern[ii].gainPhaseArray.size(); ++jj)
        {
            XMLElem gainPhaseArrayXML = newElement("GainPhaseArray", antPatternXML);
            createDouble("Freq", antenna.antPattern[ii].gainPhaseArray[jj].freq, gainPhaseArrayXML);
            createString("ArrayId", antenna.antPattern[ii].gainPhaseArray[jj].arrayId, gainPhaseArrayXML);
            if (!six::Init::isUndefined(antenna.antPattern[ii].gainPhaseArray[jj].elementId))
            {
                createString("ElementId", antenna.antPattern[ii].gainPhaseArray[jj].elementId, gainPhaseArrayXML);
            }
        }
    }
    return antennaXML;
}

XMLElem CPHDXMLParser::toXML(const TxRcv& txRcv, XMLElem parent)
{
    XMLElem txRcvXML = newElement("TxRcv", parent);
    createInt("NumTxWFs", txRcv.txWFParameters.size(), txRcvXML);
    for (size_t ii = 0; ii < txRcv.txWFParameters.size(); ++ii)
    {
        XMLElem txWFParamsXML = newElement("TxWFParameters", txRcvXML);
        createString("Identifier", txRcv.txWFParameters[ii].identifier, txWFParamsXML);
        createDouble("PulseLength", txRcv.txWFParameters[ii].pulseLength, txWFParamsXML);
        createDouble("RFBandwidth", txRcv.txWFParameters[ii].rfBandwidth, txWFParamsXML);
        createDouble("FreqCenter", txRcv.txWFParameters[ii].freqCenter, txWFParamsXML);
        if (!six::Init::isUndefined(txRcv.txWFParameters[ii].lfmRate))
        {
            createDouble("LFMRate", txRcv.txWFParameters[ii].lfmRate, txWFParamsXML);
        }
        createString("Polarization", txRcv.txWFParameters[ii].polarization, txWFParamsXML);
        if (!six::Init::isUndefined(txRcv.txWFParameters[ii].power))
        {
            createDouble("Power", txRcv.txWFParameters[ii].power, txWFParamsXML);
        }
    }
    createInt("NumRcvs", txRcv.rcvParameters.size(), txRcvXML);
    for (size_t ii = 0; ii < txRcv.rcvParameters.size(); ++ii)
    {
        XMLElem rcvParamsXML = newElement("RcvParameters", txRcvXML);
        createString("Identifier", txRcv.rcvParameters[ii].identifier, rcvParamsXML);
        createDouble("WindowLength", txRcv.rcvParameters[ii].windowLength, rcvParamsXML);
        createDouble("SampleRate", txRcv.rcvParameters[ii].sampleRate, rcvParamsXML);
        createDouble("IFFilterBW", txRcv.rcvParameters[ii].ifFilterBW, rcvParamsXML);
        createDouble("FreqCenter", txRcv.rcvParameters[ii].freqCenter, rcvParamsXML);
        if (!six::Init::isUndefined(txRcv.rcvParameters[ii].lfmRate))
        {
            createDouble("LFMRate", txRcv.rcvParameters[ii].lfmRate, rcvParamsXML);
        }
        createString("Polarization", txRcv.rcvParameters[ii].polarization, rcvParamsXML);
        if (!six::Init::isUndefined(txRcv.rcvParameters[ii].pathGain))
        {
            createDouble("PathGain", txRcv.rcvParameters[ii].pathGain, rcvParamsXML);
        }
    }
    return txRcvXML;
}

XMLElem CPHDXMLParser::toXML(const ErrorParameters& errParams, XMLElem parent)
{
    XMLElem errParamsXML = newElement("ErrorParameters", parent);
    if (errParams.monostatic.get())
    {
        XMLElem monoXML = newElement("Monostatic", errParamsXML);
        XMLElem posVelErrXML = newElement("PosVelErr", monoXML);
        createString("Frame", errParams.monostatic->posVelErr.frame, posVelErrXML);
        createDouble("P1", errParams.monostatic->posVelErr.p1, posVelErrXML);
        createDouble("P2", errParams.monostatic->posVelErr.p2, posVelErrXML);
        createDouble("P3", errParams.monostatic->posVelErr.p3, posVelErrXML);
        createDouble("V1", errParams.monostatic->posVelErr.v1, posVelErrXML);
        createDouble("V2", errParams.monostatic->posVelErr.v2, posVelErrXML);
        createDouble("V3", errParams.monostatic->posVelErr.v3, posVelErrXML);
        if(errParams.monostatic->posVelErr.corrCoefs.get())
        {
            XMLElem corrCoefsXML = newElement("CorrCoefs", posVelErrXML);
            createDouble("P1P2", errParams.monostatic->posVelErr.corrCoefs->p1p2, corrCoefsXML);
            createDouble("P1P3", errParams.monostatic->posVelErr.corrCoefs->p1p3, corrCoefsXML);
            createDouble("P1V1", errParams.monostatic->posVelErr.corrCoefs->p1v1, corrCoefsXML);
            createDouble("P1V2", errParams.monostatic->posVelErr.corrCoefs->p1v2, corrCoefsXML);
            createDouble("P1V3", errParams.monostatic->posVelErr.corrCoefs->p1v3, corrCoefsXML);
            createDouble("P2P3", errParams.monostatic->posVelErr.corrCoefs->p2p3, corrCoefsXML);
            createDouble("P2V1", errParams.monostatic->posVelErr.corrCoefs->p2v1, corrCoefsXML);
            createDouble("P2V2", errParams.monostatic->posVelErr.corrCoefs->p2v2, corrCoefsXML);
            createDouble("P2V3", errParams.monostatic->posVelErr.corrCoefs->p2v3, corrCoefsXML);
            createDouble("P3V1", errParams.monostatic->posVelErr.corrCoefs->p3v1, corrCoefsXML);
            createDouble("P3V2", errParams.monostatic->posVelErr.corrCoefs->p3v2, corrCoefsXML);
            createDouble("P3V3", errParams.monostatic->posVelErr.corrCoefs->p3v3, corrCoefsXML);
            createDouble("V1V2", errParams.monostatic->posVelErr.corrCoefs->v1v2, corrCoefsXML);
            createDouble("V1V3", errParams.monostatic->posVelErr.corrCoefs->v1v3, corrCoefsXML);
            createDouble("V2V3", errParams.monostatic->posVelErr.corrCoefs->v2v3, corrCoefsXML);
        }
        if(errParams.monostatic->posVelErr.PositionDecorr().has())
        {
            XMLElem positionDecorrXML = newElement("PositionDecorr", posVelErrXML);
            createDouble("CorrCoefZero", errParams.monostatic->posVelErr.PositionDecorr().get().corrCoefZero, positionDecorrXML);
            createDouble("DecorrRate", errParams.monostatic->posVelErr.PositionDecorr().get().decorrRate, positionDecorrXML);
        }
        // RadarSensor
        XMLElem radarXML = newElement("RadarSensor", monoXML);
        createDouble("RangeBias", errParams.monostatic->radarSensor.rangeBias, radarXML);
        if (!six::Init::isUndefined(errParams.monostatic->radarSensor.clockFreqSF))
        {
            createDouble("ClockFreqSF", errParams.monostatic->radarSensor.clockFreqSF, radarXML);
        }
        if (!six::Init::isUndefined(errParams.monostatic->radarSensor.collectionStartTime))
        {
            createDouble("CollectionStartTime", errParams.monostatic->radarSensor.collectionStartTime, radarXML);
        }
        if (errParams.monostatic->radarSensor.rangeBiasDecorr.get())
        {
            XMLElem rangeBiasDecorrXML = newElement("RangeBiasDecorr", radarXML);
            createDouble("CorrCoefZero", errParams.monostatic->radarSensor.rangeBiasDecorr->corrCoefZero, rangeBiasDecorrXML);
            createDouble("DecorrRate", errParams.monostatic->radarSensor.rangeBiasDecorr->decorrRate, rangeBiasDecorrXML);
        }

        if (errParams.monostatic->tropoError.get())
        {
            XMLElem tropoXML = newElement("TropoError", monoXML);
            if (errParams.monostatic->tropoError->TropoRangeVertical().has())
            {
                createDouble("TropoRangeVertical", errParams.monostatic->tropoError->TropoRangeVertical().get(), tropoXML);
            }
            if (errParams.monostatic->tropoError->TropoRangeSlant().has())
            {
                createDouble("TropoRangeSlant", errParams.monostatic->tropoError->TropoRangeSlant().get(), tropoXML);
            }
            if (errParams.monostatic->tropoError->TropoRangeDecorr().has())
            {
                XMLElem tropoDecorrXML = newElement("TropoRangeDecorr", tropoXML);
                createDouble("CorrCoefZero", errParams.monostatic->tropoError->TropoRangeDecorr().get().corrCoefZero, tropoDecorrXML);
                createDouble("DecorrRate", errParams.monostatic->tropoError->TropoRangeDecorr().get().decorrRate, tropoDecorrXML);
            }
        }
        if (errParams.monostatic->ionoError.get())
        {
            XMLElem ionoXML = newElement("IonoError", monoXML);
            createDouble("IonoRangeVertical", errParams.monostatic->ionoError->IonoRangeVertical().get(), ionoXML);
            if (errParams.monostatic->ionoError->IonoRangeRateVertical().has())
            {
                createDouble("IonoRangeRateVertical", errParams.monostatic->ionoError->IonoRangeRateVertical().get(), ionoXML);
            }
            if (errParams.monostatic->ionoError->IonoRgRgRateCC().has())
            {
                createDouble("IonoRgRgRateCC", errParams.monostatic->ionoError->IonoRgRgRateCC().get(), ionoXML);
            }
            if (errParams.monostatic->ionoError->IonoRangeVertDecorr().has())
            {
                XMLElem ionoDecorrXML = newElement("IonoRangeVertDecorr", ionoXML);
                createDouble("CorrCoefZero", errParams.monostatic->ionoError->IonoRangeVertDecorr().get().corrCoefZero, ionoDecorrXML);
                createDouble("DecorrRate", errParams.monostatic->ionoError->IonoRangeVertDecorr().get().decorrRate, ionoDecorrXML);
            }
        }
        if (errParams.monostatic->parameter.size() > 0)
        {
            XMLElem addedParamsXML = newElement("AddedParameters", monoXML);
            mCommon.addParameters("Parameter", getDefaultURI(), errParams.monostatic->parameter, addedParamsXML);
        }
    }
    else if (errParams.bistatic.get())
    {
        XMLElem biXML = newElement("Bistatic", errParamsXML);
        XMLElem txPlatXML = newElement("TxPlatform", biXML);
        createErrorParamPlatform("TxPlatform", errParams.bistatic->txPlatform, txPlatXML);
        XMLElem radarTxXML = newElement("RadarSensor", txPlatXML);
        if(!six::Init::isUndefined(errParams.bistatic->txPlatform.radarSensor.clockFreqSF))
        {
            createDouble("ClockFreqSF", errParams.bistatic->txPlatform.radarSensor.clockFreqSF, radarTxXML);
        }
        createDouble("CollectionStartTime", errParams.bistatic->txPlatform.radarSensor.collectionStartTime, radarTxXML);

        XMLElem rcvPlatXML = newElement("RcvPlatform", biXML);
        createErrorParamPlatform("RcvPlatform", errParams.bistatic->rcvPlatform, rcvPlatXML);
        XMLElem radarRcvXML = newElement("RadarSensor", rcvPlatXML);
        if(!six::Init::isUndefined(errParams.bistatic->rcvPlatform.radarSensor.clockFreqSF))
        {
            createDouble("ClockFreqSF", errParams.bistatic->rcvPlatform.radarSensor.clockFreqSF, radarRcvXML);
        }
        createDouble("CollectionStartTime", errParams.bistatic->rcvPlatform.radarSensor.collectionStartTime, radarRcvXML);

        if (errParams.bistatic->parameter.size() > 0)
        {
            XMLElem addedParamsXML = newElement("AddedParameters", biXML);
            mCommon.addParameters("Parameter",  getDefaultURI(), errParams.bistatic->parameter, addedParamsXML);
        }
    }

    return errParamsXML;
}

XMLElem CPHDXMLParser::toXML(const ProductInfo& productInfo, XMLElem parent)
{
    XMLElem productInfoXML = newElement("ProductInfo", parent);
    if(!six::Init::isUndefined(productInfo.profile))
    {
        createString("Profile", productInfo.profile, productInfoXML);
    }
    for (size_t ii = 0; ii < productInfo.creationInfo.size(); ++ii)
    {
        XMLElem creationInfoXML = newElement("CreationInfo", productInfoXML);
        if(!six::Init::isUndefined(productInfo.creationInfo[ii].application))
        {
            createString("Application", productInfo.creationInfo[ii].application, creationInfoXML);
        }
        createDateTime("DateTime", productInfo.creationInfo[ii].dateTime, creationInfoXML);
        if (!six::Init::isUndefined(productInfo.creationInfo[ii].site))
        {
            createString("Site", productInfo.creationInfo[ii].site, creationInfoXML);
        }
        mCommon.addParameters("Parameter", getDefaultURI(), productInfo.creationInfo[ii].parameter, creationInfoXML);
    }
    mCommon.addParameters("Parameter", getDefaultURI(), productInfo.parameter, productInfoXML);
    return productInfoXML;
}

XMLElem CPHDXMLParser::toXML(const GeoInfo& geoInfo, XMLElem parent)
{
    XMLElem geoInfoXML = newElement("GeoInfo", parent);

    mCommon.addParameters("Desc", geoInfo.desc, geoInfoXML);

    const size_t numLatLons = geoInfo.geometryLatLon.size();
    if (numLatLons == 1)
    {
        mCommon.createLatLon("Point", geoInfo.geometryLatLon[0], geoInfoXML);
    }
    else if (numLatLons >= 2)
    {
        XMLElem linePolyXML = newElement(numLatLons == 2 ? "Line" : "Polygon",
                                         geoInfoXML);
        setAttribute(linePolyXML, "size", numLatLons);

        for (size_t ii = 0; ii < numLatLons; ++ii)
        {
            XMLElem v = mCommon.createLatLon(numLatLons == 2 ? "Endpoint" : "Vertex",
                         geoInfo.geometryLatLon[ii], linePolyXML);
            setAttribute(v, "index", ii + 1);
        }
    }

    if (!geoInfo.name.empty())
        setAttribute(geoInfoXML, "name", geoInfo.name);

    for (size_t ii = 0; ii < geoInfo.geoInfos.size(); ++ii)
    {
        toXML(*geoInfo.geoInfos[ii], geoInfoXML);
    }

    return geoInfoXML;
}

XMLElem CPHDXMLParser::toXML(const MatchInformation& matchInfo, XMLElem parent)
{
    return mCommon.convertMatchInformationToXML(matchInfo, parent);
}

/*
 * FROM XML
 */

std::unique_ptr<Metadata> CPHDXMLParser::fromXML(
        const xml::lite::Document* doc)
{
    std::unique_ptr<Metadata> cphd(new Metadata());

    XMLElem root = doc->getRootElement();

    XMLElem collectionIDXML   = getFirstAndOnly(root, "CollectionID");
    XMLElem globalXML         = getFirstAndOnly(root, "Global");
    XMLElem sceneCoordsXML    = getFirstAndOnly(root, "SceneCoordinates");
    XMLElem dataXML           = getFirstAndOnly(root, "Data");
    XMLElem channelXML        = getFirstAndOnly(root, "Channel");
    XMLElem pvpXML            = getFirstAndOnly(root, "PVP");
    XMLElem dwellXML          = getFirstAndOnly(root, "Dwell");
    XMLElem refGeoXML         = getFirstAndOnly(root, "ReferenceGeometry");
    XMLElem supportArrayXML   = getOptional(root, "SupportArray");
    XMLElem antennaXML        = getOptional(root, "Antenna");
    XMLElem txRcvXML          = getOptional(root, "TxRcv");
    XMLElem errParamXML       = getOptional(root, "ErrorParameters");
    XMLElem productInfoXML    = getOptional(root, "ProductInfo");
    XMLElem matchInfoXML      = getOptional(root, "MatchInfo");

    std::vector<XMLElem> geoInfoXMLVec;
    root->getElementsByTagName("GeoInfo", geoInfoXMLVec);
    cphd->geoInfo.resize(geoInfoXMLVec.size());

    // Parse XML for each section
    fromXML(collectionIDXML, cphd->collectionID);
    fromXML(globalXML, cphd->global);
    fromXML(sceneCoordsXML, cphd->sceneCoordinates);
    fromXML(dataXML, cphd->data);
    fromXML(channelXML, cphd->channel);
    fromXML(pvpXML, cphd->pvp);
    fromXML(dwellXML, cphd->dwell);
    fromXML(refGeoXML, cphd->referenceGeometry);
    if(supportArrayXML)
    {
        cphd->supportArray.reset(new SupportArray());
        fromXML(supportArrayXML, *(cphd->supportArray));
    }
    if(antennaXML)
    {
        cphd->antenna.reset(new Antenna());
        fromXML(antennaXML, *(cphd->antenna));
    }
    if(txRcvXML)
    {
        cphd->txRcv.reset(new TxRcv());
        fromXML(txRcvXML, *(cphd->txRcv));
    }
    if(errParamXML)
    {
        cphd->errorParameters.reset(new ErrorParameters());
        fromXML(errParamXML, *(cphd->errorParameters));
    }
    if(productInfoXML)
    {
        cphd->productInfo.reset(new ProductInfo());
        fromXML(productInfoXML, *(cphd->productInfo));
    }
    if(matchInfoXML)
    {
        cphd->matchInfo.reset(new MatchInformation());
        fromXML(matchInfoXML, *(cphd->matchInfo));
    }
    for (size_t ii = 0; ii < geoInfoXMLVec.size(); ++ii)
    {
        fromXML(geoInfoXMLVec[ii], cphd->geoInfo[ii]);
    }

    return cphd;
}

void CPHDXMLParser::fromXML(const XMLElem collectionIDXML, CollectionInformation& collectionID)
{
    parseString(getFirstAndOnly(collectionIDXML, "CollectorName"),
                collectionID.collectorName);

    XMLElem element = getOptional(collectionIDXML, "IlluminatorName");
    if (element)
    {
        parseString(element, collectionID.illuminatorName);
    }

    element = getOptional(collectionIDXML, "CoreName");
    if (element)
    {
        parseString(element, collectionID.coreName);
    }

    element = getOptional(collectionIDXML, "CollectType");
    if (element)
    {
        collectionID.collectType
                = six::toType<six::CollectType>(element->getCharacterData());
    }

    XMLElem radarModeXML = getFirstAndOnly(collectionIDXML, "RadarMode");

    collectionID.radarMode
            = six::toType<RadarModeType>(getFirstAndOnly(radarModeXML,
                                         "ModeType")->getCharacterData());

    element = getOptional(radarModeXML, "ModeID");
    if (element)
    {
        parseString(element, collectionID.radarModeID);
    }

    element = getFirstAndOnly(collectionIDXML, "ReleaseInfo");
    parseString(element, collectionID.releaseInfo);

    std::string classification;
    parseString(getFirstAndOnly(collectionIDXML, "Classification"),
                classification);
    collectionID.setClassificationLevel(classification);

    // Optional
    std::vector<std::string> countryCodes;
    element = getOptional(collectionIDXML, "CountryCode");
    if (element)
    {
        std::string countryCodeStr;
        parseString(element, countryCodeStr);
        collectionID.countryCodes = str::split(countryCodeStr, ",");
        for (size_t ii = 0; ii < collectionID.countryCodes.size(); ++ii)
        {
            str::trim(collectionID.countryCodes[ii]);
        }
    }

    //optional
    mCommon.parseParameters(collectionIDXML, "Parameter", collectionID.parameters);
}

void CPHDXMLParser::fromXML(const XMLElem globalXML, Global& global)
{
    global.domainType = DomainType(
            getFirstAndOnly(globalXML, "DomainType")->getCharacterData());
    global.sgn = PhaseSGN(
            getFirstAndOnly(globalXML, "SGN")->getCharacterData());

    // Timeline
    const XMLElem timelineXML = getFirstAndOnly(globalXML, "Timeline");
    parseDateTime(
            getFirstAndOnly(timelineXML, "CollectionStart"),
            global.timeline.collectionStart);

    // Optional
    const XMLElem rcvCollectionXML = getOptional(timelineXML,
                                                 "RcvCollectionStart");
    if (rcvCollectionXML)
    {
        parseDateTime(rcvCollectionXML,
                      global.timeline.rcvCollectionStart);
    }

    parseDouble(
            getFirstAndOnly(timelineXML, "TxTime1"), global.timeline.txTime1);
    parseDouble(
            getFirstAndOnly(timelineXML, "TxTime2"), global.timeline.txTime2);

    // FxBand
    const XMLElem fxBandXML = getFirstAndOnly(globalXML, "FxBand");
    parseDouble(getFirstAndOnly(fxBandXML, "FxMin"), global.fxBand.fxMin);
    parseDouble(getFirstAndOnly(fxBandXML, "FxMax"), global.fxBand.fxMax);

    // TOASwath
    const XMLElem toaSwathXML = getFirstAndOnly(globalXML, "TOASwath");
    parseDouble(getFirstAndOnly(toaSwathXML, "TOAMin"), global.toaSwath.toaMin);
    parseDouble(getFirstAndOnly(toaSwathXML, "TOAMax"), global.toaSwath.toaMax);

    // TropoParameters
    const XMLElem tropoXML = getOptional(globalXML, "TropoParameters");
    if (tropoXML)
    {
        // Optional
        global.tropoParameters.reset(new TropoParameters());
        parseDouble(getFirstAndOnly(tropoXML, "N0"), global.tropoParameters->n0);
        global.tropoParameters->refHeight =
                getFirstAndOnly(tropoXML, "RefHeight")->getCharacterData();
    }

    // IonoParameters
    const XMLElem ionoXML = getOptional(globalXML, "IonoParameters");
    if (ionoXML)
    {
        // Optional
        global.ionoParameters.reset(new IonoParameters());
        parseDouble(getFirstAndOnly(ionoXML, "TECV"), global.ionoParameters->tecv);
        parseDouble(getFirstAndOnly(ionoXML, "F2Height"), global.ionoParameters->f2Height);
    }
}

void CPHDXMLParser::fromXML(const XMLElem sceneCoordsXML,
                             SceneCoordinates& scene)
{
    scene.earthModel = EarthModelType(
            getFirstAndOnly(sceneCoordsXML, "EarthModel")->getCharacterData());

    // IARP
    const XMLElem iarpXML = getFirstAndOnly(sceneCoordsXML, "IARP");
    mCommon.parseVector3D(getFirstAndOnly(iarpXML, "ECF"), scene.iarp.ecf);
    mCommon.parseLatLonAlt(getFirstAndOnly(iarpXML, "LLH"), scene.iarp.llh);

    // ReferenceSurface
    const XMLElem surfaceXML = getFirstAndOnly(sceneCoordsXML, "ReferenceSurface");
    const XMLElem planarXML = getOptional(surfaceXML, "Planar");
    const XMLElem haeXML = getOptional(surfaceXML, "HAE");
    if (planarXML && !haeXML)
    {
        // Choice type
        scene.referenceSurface.planar.reset(new Planar());
        mCommon.parseVector3D(getFirstAndOnly(planarXML, "uIAX"),
                              scene.referenceSurface.planar->uIax);
        mCommon.parseVector3D(getFirstAndOnly(planarXML, "uIAY"),
                              scene.referenceSurface.planar->uIay);
    }
    else if (haeXML && !planarXML)
    {
        // Choice type
        scene.referenceSurface.hae.reset(new HAE());
        mCommon.parseLatLon(getFirstAndOnly(haeXML, "uIAXLL"),
                            scene.referenceSurface.hae->uIax);
        mCommon.parseLatLon(getFirstAndOnly(haeXML, "uIAYLL"),
                            scene.referenceSurface.hae->uIay);
    }
    else
    {
        throw except::Exception(Ctxt(
                "ReferenceSurface must exactly one of Planar or HAE element"));
    }

    // ImageArea
    const XMLElem imageAreaXML = getFirstAndOnly(sceneCoordsXML, "ImageArea");
    parseAreaType(imageAreaXML, scene.imageArea);

    // ImageAreaCorners
    const XMLElem cornersXML = getFirstAndOnly(sceneCoordsXML,
                                               "ImageAreaCornerPoints");
    mCommon.parseFootprint(cornersXML, "IACP", scene.imageAreaCorners);

    // Extended Area
    const XMLElem extendedAreaXML = getOptional(sceneCoordsXML, "ExtendedArea");
    if (extendedAreaXML)
    {
        scene.extendedArea.reset(new AreaType());
        parseAreaType(extendedAreaXML, *scene.extendedArea);
    }

    // Image Grid
    const XMLElem gridXML = getOptional(sceneCoordsXML, "ImageGrid");
    if (gridXML)
    {
        // Optional
        scene.imageGrid.reset(new ImageGrid());
        const XMLElem identifierXML = getOptional(gridXML, "Identifier");
        if (identifierXML)
        {
            parseString(identifierXML, scene.imageGrid->identifier);
        }
        parseLineSample(getFirstAndOnly(gridXML, "IARPLocation"),
                        scene.imageGrid->iarpLocation);
        parseIAExtent(getFirstAndOnly(gridXML, "IAXExtent"),
                      scene.imageGrid->xExtent);
        parseIAExtent(getFirstAndOnly(gridXML, "IAYExtent"),
                      scene.imageGrid->yExtent);

        // Segment List
        const XMLElem segListXML = getOptional(gridXML, "SegmentList");
        if (segListXML)
        {
            // Optional
            size_t numSegments;
            parseUInt(getFirstAndOnly(segListXML, "NumSegments"), numSegments);
            scene.imageGrid->segments.resize(numSegments);

            std::vector<XMLElem> segmentsXML;
            segListXML->getElementsByTagName("Segment", segmentsXML);

            for (size_t ii = 0; ii < segmentsXML.size(); ++ii)
            {
                const XMLElem segmentXML = segmentsXML[ii];
                parseString(getFirstAndOnly(segmentXML, "Identifier"),
                         scene.imageGrid->segments[ii].identifier);
                parseInt(getFirstAndOnly(segmentXML, "StartLine"),
                         scene.imageGrid->segments[ii].startLine);
                parseInt(getFirstAndOnly(segmentXML, "StartSample"),
                         scene.imageGrid->segments[ii].startSample);
                parseInt(getFirstAndOnly(segmentXML, "EndLine"),
                         scene.imageGrid->segments[ii].endLine);
                parseInt(getFirstAndOnly(segmentXML, "EndSample"),
                         scene.imageGrid->segments[ii].endSample);

                const XMLElem polygonXML = getOptional(segmentXML,
                                                       "SegmentPolygon");
                if (polygonXML)
                {
                    // Optional
                    size_t polygonSize = 0;
                    sscanf(polygonXML->attribute("size").c_str(), "%zu", &polygonSize);
                    scene.imageGrid->segments[ii].polygon.resize(polygonSize);
                    std::vector<XMLElem> polyVerticesXMLVec;
                    polygonXML->getElementsByTagName("SV", polyVerticesXMLVec);
                    if (scene.imageGrid->segments[ii].polygon.size() != polyVerticesXMLVec.size())
                    {
                        throw except::Exception(Ctxt(
                                "Incorrect polygon vertices provided"));
                    }
                    if (polyVerticesXMLVec.size() < 3)
                    {
                        throw except::Exception(Ctxt(
                                "Polygon must have at least 3 vertices"));
                    }
                    std::vector<LineSample>& vertices =
                            scene.imageGrid->segments[ii].polygon;
                    vertices.resize(polyVerticesXMLVec.size());
                    for (size_t jj = 0; jj < polyVerticesXMLVec.size(); ++jj)
                    {
                        size_t tempIdx;
                        sscanf(polyVerticesXMLVec[jj]->attribute("index").c_str(), "%zu", &tempIdx);
                        vertices[jj].setIndex(tempIdx);
                        parseLineSample(polyVerticesXMLVec[jj], vertices[jj]);
                    }
                }
            }
        }
    }
}

void CPHDXMLParser::fromXML(const XMLElem dataXML, Data& data)
{
    const XMLElem signalXML = getFirstAndOnly(dataXML, "SignalArrayFormat");
    data.signalArrayFormat = SignalArrayFormat(signalXML->getCharacterData());

    size_t numBytesPVP_temp = 0;
    XMLElem numBytesPVPXML = getFirstAndOnly(dataXML, "NumBytesPVP");
    parseUInt(numBytesPVPXML, numBytesPVP_temp);
    if(numBytesPVP_temp % 8 != 0)
    {
        throw except::Exception(Ctxt(
                "Number of bytes must be multiple of 8"));
    }
    data.numBytesPVP = numBytesPVP_temp;

    // Channels
    std::vector<XMLElem> channelsXML;
    dataXML->getElementsByTagName("Channel", channelsXML);
    data.channels.resize(channelsXML.size());
    for (size_t ii = 0; ii < channelsXML.size(); ++ii)
    {
        parseString(getFirstAndOnly(channelsXML[ii], "Identifier"),
                    data.channels[ii].identifier);
        parseUInt(getFirstAndOnly(channelsXML[ii], "NumVectors"),
                  data.channels[ii].numVectors);
        parseUInt(getFirstAndOnly(channelsXML[ii], "NumSamples"),
                  data.channels[ii].numSamples);
        parseUInt(getFirstAndOnly(channelsXML[ii], "SignalArrayByteOffset"),
                  data.channels[ii].signalArrayByteOffset);
        parseUInt(getFirstAndOnly(channelsXML[ii], "PVPArrayByteOffset"),
                  data.channels[ii].pvpArrayByteOffset);
        XMLElem compressionXML = getOptional(channelsXML[ii], "CompressedSignalSize");
        if (compressionXML)
        {
            parseUInt(compressionXML, data.channels[ii].compressedSignalSize);
        }
    }
    XMLElem compressionXML = getOptional(dataXML, "SignalCompressionID");
    if (compressionXML)
    {
        parseString(compressionXML,
                    data.signalCompressionID);
    }

    // Support Arrays
    std::vector<XMLElem> supportsXML;
    dataXML->getElementsByTagName("SupportArray", supportsXML);
    for (size_t ii = 0; ii < supportsXML.size(); ++ii)
    {
        std::string id;
        size_t offset;
        size_t numRows;
        size_t numCols;
        size_t numBytes;
        parseString(getFirstAndOnly(supportsXML[ii], "Identifier"), id);
        parseUInt(getFirstAndOnly(supportsXML[ii], "ArrayByteOffset"), offset);
        parseUInt(getFirstAndOnly(supportsXML[ii], "NumRows"), numRows);
        parseUInt(getFirstAndOnly(supportsXML[ii], "NumCols"), numCols);
        parseUInt(getFirstAndOnly(supportsXML[ii], "BytesPerElement"), numBytes);
        data.setSupportArray(id, numRows, numCols, numBytes, offset);
    }
}

void CPHDXMLParser::fromXML(const XMLElem channelXML, Channel& channel)
{
    parseString(getFirstAndOnly(channelXML, "RefChId"), channel.refChId);
    parseBooleanType(getFirstAndOnly(channelXML, "FXFixedCPHD"),
                     channel.fxFixedCphd);
    parseBooleanType(getFirstAndOnly(channelXML, "TOAFixedCPHD"),
                     channel.toaFixedCphd);
    parseBooleanType(getFirstAndOnly(channelXML, "SRPFixedCPHD"),
                     channel.srpFixedCphd);

    std::vector<XMLElem> parametersXML;
    channelXML->getElementsByTagName("Parameters", parametersXML);
    channel.parameters.resize(parametersXML.size());
    for (size_t ii = 0; ii < parametersXML.size(); ++ii)
    {
        parseChannelParameters(parametersXML[ii], channel.parameters[ii]);
    }

    XMLElem addedParametersXML = getOptional(channelXML, "AddedParameters");
    if(addedParametersXML)
    {
        mCommon.parseParameters(addedParametersXML, "Parameter", channel.addedParameters);
    }
}

void CPHDXMLParser::fromXML(const XMLElem pvpXML, Pvp& pvp)
{
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "TxTime"), pvp.txTime);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "TxPos"), pvp.txPos);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "TxVel"), pvp.txVel);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "RcvTime"), pvp.rcvTime);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "RcvPos"), pvp.rcvPos);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "RcvVel"), pvp.rcvVel);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "SRPPos"), pvp.srpPos);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "aFDOP"), pvp.aFDOP);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "aFRR1"), pvp.aFRR1);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "aFRR2"), pvp.aFRR2);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "FX1"), pvp.fx1);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "FX2"), pvp.fx2);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "TOA1"), pvp.toa1);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "TOA2"), pvp.toa2);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "TDTropoSRP"), pvp.tdTropoSRP);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "SC0"), pvp.sc0);
    parsePVPType(pvp, getFirstAndOnly(pvpXML, "SCSS"), pvp.scss);

    const XMLElem AmpSFXML = getOptional(pvpXML, "AmpSF");
    if (AmpSFXML)
    {
        parsePVPType(pvp, AmpSFXML, pvp.ampSF);
    }

    const XMLElem FXN1XML = getOptional(pvpXML, "FXN1");
    if (FXN1XML)
    {
        parsePVPType(pvp, FXN1XML, pvp.fxN1);
    }

    const XMLElem FXN2XML = getOptional(pvpXML, "FXN2");
    if (FXN2XML)
    {
        parsePVPType(pvp, FXN2XML, pvp.fxN2);
    }

    const XMLElem TOAE1XML = getOptional(pvpXML, "TOAE1");
    if (TOAE1XML)
    {
        parsePVPType(pvp, TOAE1XML, pvp.toaE1);
    }

    const XMLElem TOAE2XML = getOptional(pvpXML, "TOAE2");
    if (TOAE2XML)
    {
        parsePVPType(pvp, TOAE2XML, pvp.toaE2);
    }

    const XMLElem TDIonoSRPXML = getOptional(pvpXML, "TDIonoSRP");
    if (TDIonoSRPXML)
    {
        parsePVPType(pvp, TDIonoSRPXML, pvp.tdIonoSRP);
    }

    const XMLElem SIGNALXML = getOptional(pvpXML, "SIGNAL");
    if (SIGNALXML)
    {
        parsePVPType(pvp, SIGNALXML, pvp.signal);
    }

    std::vector<XMLElem> addedParamsXML;
    const std::string str = "AddedPVP";
    pvpXML->getElementsByTagName(str, addedParamsXML);
    if(addedParamsXML.empty())
    {
        return;
    }
    for (size_t ii = 0; ii < addedParamsXML.size(); ++ii)
    {
        parsePVPType(pvp, addedParamsXML[ii]);
    }
}

void CPHDXMLParser::fromXML(const XMLElem DwellXML,
                             Dwell& dwell)
{
    // CODTime
    size_t numCODTimes;
    parseUInt(getFirstAndOnly(DwellXML, "NumCODTimes"), numCODTimes);
    dwell.cod.resize(numCODTimes);

    std::vector<XMLElem> codXMLVec;
    DwellXML->getElementsByTagName("CODTime", codXMLVec);
    for(size_t ii = 0; ii < codXMLVec.size(); ++ii)
    {
        parseString(getFirstAndOnly(codXMLVec[ii], "Identifier"), dwell.cod[ii].identifier);
        mCommon.parsePoly2D(getFirstAndOnly(codXMLVec[ii], "CODTimePoly"), dwell.cod[ii].codTimePoly);
    }

    // DwellTime
    size_t numDwellTimes = 0;
    parseUInt(getFirstAndOnly(DwellXML, "NumDwellTimes"), numDwellTimes);
    dwell.dtime.resize(numDwellTimes);

    std::vector<XMLElem> dtimeXMLVec;
    DwellXML->getElementsByTagName("DwellTime", dtimeXMLVec);
    for(size_t ii = 0; ii < dtimeXMLVec.size(); ++ii)
    {
        parseString(getFirstAndOnly(dtimeXMLVec[ii], "Identifier"), dwell.dtime[ii].identifier);
        mCommon.parsePoly2D(getFirstAndOnly(dtimeXMLVec[ii], "DwellTimePoly"), dwell.dtime[ii].dwellTimePoly);
    }
}

void CPHDXMLParser::fromXML(const XMLElem refGeoXML, ReferenceGeometry& refGeo)
{
    XMLElem srpXML = getFirstAndOnly(refGeoXML, "SRP");
    mCommon.parseVector3D(getFirstAndOnly(srpXML, "ECF"), refGeo.srp.ecf);
    mCommon.parseVector3D(getFirstAndOnly(srpXML, "IAC"), refGeo.srp.iac);
    parseDouble(getFirstAndOnly(refGeoXML, "ReferenceTime"), refGeo.referenceTime);
    parseDouble(getFirstAndOnly(refGeoXML, "SRPCODTime"), refGeo.srpCODTime);
    parseDouble(getFirstAndOnly(refGeoXML, "SRPDwellTime"), refGeo.srpDwellTime);

    const XMLElem monoXML = getOptional(refGeoXML, "Monostatic");
    const XMLElem biXML = getOptional(refGeoXML, "Bistatic");
    if (monoXML && !biXML)
    {
        refGeo.monostatic.reset(new Monostatic());
        parseCommon(monoXML, (ImagingType*)refGeo.monostatic.get());
        parseDouble(getFirstAndOnly(monoXML, "SlantRange"), refGeo.monostatic->slantRange);
        parseDouble(getFirstAndOnly(monoXML, "GroundRange"), refGeo.monostatic->groundRange);
        parseDouble(getFirstAndOnly(monoXML, "DopplerConeAngle"), refGeo.monostatic->dopplerConeAngle);
        parseDouble(getFirstAndOnly(monoXML, "IncidenceAngle"), refGeo.monostatic->incidenceAngle);
        mCommon.parseVector3D(getFirstAndOnly(monoXML, "ARPPos"), refGeo.monostatic->arpPos);
        mCommon.parseVector3D(getFirstAndOnly(monoXML, "ARPVel"), refGeo.monostatic->arpVel);
        std::string side = "";
        parseString(getFirstAndOnly(monoXML, "SideOfTrack"), side);
        refGeo.monostatic->sideOfTrack = (side == "L" ? six::SideOfTrackType("LEFT") : six::SideOfTrackType("RIGHT"));

    }
    else if (!monoXML && biXML)
    {
        refGeo.bistatic.reset(new Bistatic());
        parseCommon(biXML, (ImagingType*)refGeo.bistatic.get());
        parseDouble(getFirstAndOnly(biXML, "AzimuthAngleRate"), refGeo.bistatic->azimuthAngleRate);
        parseDouble(getFirstAndOnly(biXML, "BistaticAngle"), refGeo.bistatic->bistaticAngle);
        parseDouble(getFirstAndOnly(biXML, "BistaticAngleRate"), refGeo.bistatic->bistaticAngleRate);

        parsePlatformParams(getFirstAndOnly(biXML, "TxPlatform"), refGeo.bistatic->txPlatform);
        parsePlatformParams(getFirstAndOnly(biXML, "RcvPlatform"), refGeo.bistatic->rcvPlatform);
    }
    else
    {
        throw except::Exception(Ctxt(
                "One of the two types Mono or Bi must be provided"));
    }
}

void CPHDXMLParser::fromXML(const XMLElem supportArrayXML, SupportArray& supportArray)
{
    std::vector<XMLElem> iazArrayXMLVec;
    supportArrayXML->getElementsByTagName("IAZArray", iazArrayXMLVec);
    supportArray.iazArray.resize(iazArrayXMLVec.size());
    for (size_t ii = 0; ii < iazArrayXMLVec.size(); ++ii)
    {
        parseSupportArrayParameter(iazArrayXMLVec[ii], supportArray.iazArray[ii], false);
    }

    std::vector<XMLElem> antGainPhaseXMLVec;
    supportArrayXML->getElementsByTagName("AntGainPhase", antGainPhaseXMLVec);
    supportArray.antGainPhase.resize(antGainPhaseXMLVec.size());
    for (size_t ii = 0; ii < antGainPhaseXMLVec.size(); ++ii)
    {
        parseSupportArrayParameter(antGainPhaseXMLVec[ii], supportArray.antGainPhase[ii], false);
    }

    std::vector<XMLElem> addedSupportArrayXMLVec;
    supportArrayXML->getElementsByTagName("AddedSupportArray", addedSupportArrayXMLVec);
    for (size_t ii = 0; ii < addedSupportArrayXMLVec.size(); ++ii)
    {
        std::string id;
        parseString(getFirstAndOnly(addedSupportArrayXMLVec[ii], "Identifier"), id);
        if (supportArray.addedSupportArray.count(id) == 0)
        {
            parseSupportArrayParameter(addedSupportArrayXMLVec[ii], supportArray.addedSupportArray[id], true);
            parseString(getFirstAndOnly(addedSupportArrayXMLVec[ii], "XUnits"), supportArray.addedSupportArray[id].xUnits);
            parseString(getFirstAndOnly(addedSupportArrayXMLVec[ii], "YUnits"), supportArray.addedSupportArray[id].yUnits);
            parseString(getFirstAndOnly(addedSupportArrayXMLVec[ii], "ZUnits"), supportArray.addedSupportArray[id].zUnits);
            mCommon.parseParameters(addedSupportArrayXMLVec[ii], "Parameter", supportArray.addedSupportArray[id].parameter);
        }
        else
        {
            throw except::Exception(Ctxt(
                    "Support array identifier for support array is not unique: " + id));
        }
    }
}


void CPHDXMLParser::fromXML(const XMLElem antennaXML, Antenna& antenna)
{
    size_t numACFs = 0;
    size_t numAPCs = 0;
    size_t numAntPats = 0;
    parseUInt(getFirstAndOnly(antennaXML, "NumACFs"), numACFs);
    antenna.antCoordFrame.resize(numACFs);
    parseUInt(getFirstAndOnly(antennaXML, "NumAPCs"), numAPCs);
    antenna.antPhaseCenter.resize(numAPCs);
    parseUInt(getFirstAndOnly(antennaXML, "NumAntPats"), numAntPats);
    antenna.antPattern.resize(numAntPats);

    // Parse AntCoordFrame
    std::vector<XMLElem> antCoordFrameXMLVec;
    antennaXML->getElementsByTagName("AntCoordFrame", antCoordFrameXMLVec);
    if(antenna.antCoordFrame.size() != antCoordFrameXMLVec.size())
    {
        throw except::Exception(Ctxt(
                "Incorrect number of AntCoordFrames provided"));
    }
    for( size_t ii = 0; ii < antCoordFrameXMLVec.size(); ++ii)
    {
        parseString(getFirstAndOnly(antCoordFrameXMLVec[ii], "Identifier"), antenna.antCoordFrame[ii].identifier);
        mCommon.parsePolyXYZ(getFirstAndOnly(antCoordFrameXMLVec[ii], "XAxisPoly"), antenna.antCoordFrame[ii].xAxisPoly);
        mCommon.parsePolyXYZ(getFirstAndOnly(antCoordFrameXMLVec[ii], "YAxisPoly"), antenna.antCoordFrame[ii].yAxisPoly);
    }

    // Parse AntPhaseCenter
    std::vector<XMLElem> antPhaseCenterXMLVec;
    antennaXML->getElementsByTagName("AntPhaseCenter", antPhaseCenterXMLVec);
    if(antenna.antPhaseCenter.size() != antPhaseCenterXMLVec.size())
    {
        throw except::Exception(Ctxt(
                "Incorrect number of AntPhaseCenters provided"));
    }
    for( size_t ii = 0; ii < antPhaseCenterXMLVec.size(); ++ii)
    {
        parseString(getFirstAndOnly(antPhaseCenterXMLVec[ii], "Identifier"), antenna.antPhaseCenter[ii].identifier);
        parseString(getFirstAndOnly(antPhaseCenterXMLVec[ii], "ACFId"), antenna.antPhaseCenter[ii].acfId);
        mCommon.parseVector3D(getFirstAndOnly(antPhaseCenterXMLVec[ii], "APCXYZ"), antenna.antPhaseCenter[ii].apcXYZ);
    }

    std::vector<XMLElem> antPatternXMLVec;
    antennaXML->getElementsByTagName("AntPattern", antPatternXMLVec);
    if(antenna.antPattern.size() != antPatternXMLVec.size())
    {
        throw except::Exception(Ctxt(
                "Incorrect number of AntPatterns provided"));
    }
    for( size_t ii = 0; ii < antPatternXMLVec.size(); ++ii)
    {
        parseString(getFirstAndOnly(antPatternXMLVec[ii], "Identifier"), antenna.antPattern[ii].identifier);
        parseDouble(getFirstAndOnly(antPatternXMLVec[ii], "FreqZero"), antenna.antPattern[ii].freqZero);
        XMLElem gainZeroXML = getOptional(antPatternXMLVec[ii], "GainZero");
        if(gainZeroXML)
        {
            parseDouble(gainZeroXML, antenna.antPattern[ii].gainZero);
        }
        XMLElem ebFreqShiftXML = getOptional(antPatternXMLVec[ii], "EBFreqShift");
        if(ebFreqShiftXML)
        {
            parseBooleanType(ebFreqShiftXML, antenna.antPattern[ii].ebFreqShift);
        }
        XMLElem mlFreqDilationXML = getOptional(antPatternXMLVec[ii], "MLFreqDilation");
        if(mlFreqDilationXML)
        {
            parseBooleanType(mlFreqDilationXML, antenna.antPattern[ii].mlFreqDilation);
        }
        XMLElem gainBSPoly = getOptional(antPatternXMLVec[ii], "GainBSPoly");
        if(gainBSPoly)
        {
            mCommon.parsePoly1D(gainBSPoly, antenna.antPattern[ii].gainBSPoly);
        }

        // Parse EB
        XMLElem ebXML = getFirstAndOnly(antPatternXMLVec[ii], "EB");
        mCommon.parsePoly1D(getFirstAndOnly(ebXML, "DCXPoly"), antenna.antPattern[ii].eb.dcxPoly);
        mCommon.parsePoly1D(getFirstAndOnly(ebXML, "DCYPoly"), antenna.antPattern[ii].eb.dcyPoly);

        // Parse Array
        XMLElem arrayXML = getFirstAndOnly(antPatternXMLVec[ii], "Array");
        mCommon.parsePoly2D(getFirstAndOnly(arrayXML, "GainPoly"), antenna.antPattern[ii].array.gainPoly);
        mCommon.parsePoly2D(getFirstAndOnly(arrayXML, "PhasePoly"), antenna.antPattern[ii].array.phasePoly);

        // Parse Element
        XMLElem elementXML = getFirstAndOnly(antPatternXMLVec[ii], "Element");
        mCommon.parsePoly2D(getFirstAndOnly(elementXML, "GainPoly"), antenna.antPattern[ii].element.gainPoly);
        mCommon.parsePoly2D(getFirstAndOnly(elementXML, "PhasePoly"), antenna.antPattern[ii].element.phasePoly);

        // Parse GainPhaseArray
        std::vector<XMLElem> gainPhaseArrayXMLVec;
        antPatternXMLVec[ii]->getElementsByTagName("GainPhaseArray", gainPhaseArrayXMLVec);
        antenna.antPattern[ii].gainPhaseArray.resize(gainPhaseArrayXMLVec.size());
        for (size_t jj = 0; jj < gainPhaseArrayXMLVec.size(); ++jj)
        {
            parseDouble(getFirstAndOnly(gainPhaseArrayXMLVec[jj], "Freq"),  antenna.antPattern[ii].gainPhaseArray[jj].freq);
            parseString(getFirstAndOnly(gainPhaseArrayXMLVec[jj], "ArrayId"), antenna.antPattern[ii].gainPhaseArray[jj].arrayId);
            XMLElem elementIdXML = getOptional(gainPhaseArrayXMLVec[jj], "ElementId");
            if(elementIdXML)
            {
                parseString(elementIdXML, antenna.antPattern[ii].gainPhaseArray[jj].elementId);
            }

        }
    }
}

void CPHDXMLParser::fromXML(const XMLElem txRcvXML, TxRcv& txRcv)
{
    size_t numTxWFs = 0;
    size_t numRcvs = 0;
    parseUInt(getFirstAndOnly(txRcvXML, "NumTxWFs"), numTxWFs);
    txRcv.txWFParameters.resize(numTxWFs);
    parseUInt(getFirstAndOnly(txRcvXML, "NumRcvs"), numRcvs);
    txRcv.rcvParameters.resize(numRcvs);

    std::vector<XMLElem> txWFXMLVec;
    txRcvXML->getElementsByTagName("TxWFParameters", txWFXMLVec);
    if(txRcv.txWFParameters.size() != txWFXMLVec.size())
    {
        throw except::Exception(Ctxt(
                "Incorrect number of TxWF parameters provided"));
    }
    for(size_t ii = 0; ii < txWFXMLVec.size(); ++ii)
    {
        parseTxRcvParameter(txWFXMLVec[ii], txRcv.txWFParameters[ii]);
        parseDouble(getFirstAndOnly(txWFXMLVec[ii], "PulseLength"), txRcv.txWFParameters[ii].pulseLength);
        parseDouble(getFirstAndOnly(txWFXMLVec[ii], "RFBandwidth"), txRcv.txWFParameters[ii].rfBandwidth);
        XMLElem powerXML = getOptional(txWFXMLVec[ii], "Power");
        if(powerXML)
        {
            parseDouble(powerXML, txRcv.txWFParameters[ii].power);
        }
    }

    std::vector<XMLElem> rcvXMLVec;
    txRcvXML->getElementsByTagName("RcvParameters", rcvXMLVec);
    if(txRcv.rcvParameters.size() != rcvXMLVec.size())
    {
        throw except::Exception(Ctxt(
                "Incorrect number of Rcv parameters provided"));
    }
    for(size_t ii = 0; ii < rcvXMLVec.size(); ++ii)
    {
        parseTxRcvParameter(rcvXMLVec[ii], txRcv.rcvParameters[ii]);
        parseDouble(getFirstAndOnly(rcvXMLVec[ii], "WindowLength"), txRcv.rcvParameters[ii].windowLength);
        parseDouble(getFirstAndOnly(rcvXMLVec[ii], "SampleRate"), txRcv.rcvParameters[ii].sampleRate);
        parseDouble(getFirstAndOnly(rcvXMLVec[ii], "IFFilterBW"), txRcv.rcvParameters[ii].ifFilterBW);
        XMLElem pathGainXML = getOptional(rcvXMLVec[ii], "PathGain");
        if(pathGainXML)
        {
            parseDouble(pathGainXML, txRcv.rcvParameters[ii].pathGain);
        }
    }

}

void CPHDXMLParser::fromXML(const XMLElem errParamXML, ErrorParameters& errParam)
{
    XMLElem monostaticXML = getOptional(errParamXML, "Monostatic");
    XMLElem bistaticXML = getOptional(errParamXML, "Bistatic");

    if(monostaticXML && !bistaticXML)
    {
        errParam.monostatic.reset(new ErrorParameters::Monostatic());
        parsePosVelErr(getFirstAndOnly(monostaticXML, "PosVelErr"), errParam.monostatic->posVelErr);

        XMLElem radarSensorXML = getFirstAndOnly(monostaticXML, "RadarSensor");
        parseDouble(getFirstAndOnly(radarSensorXML, "RangeBias"), errParam.monostatic->radarSensor.rangeBias);

        XMLElem clockFreqSFXML = getOptional(radarSensorXML, "ClockFreqSF");
        if(clockFreqSFXML)
        {
            parseDouble(clockFreqSFXML, errParam.monostatic->radarSensor.clockFreqSF);
        }

        XMLElem collectionStartTimeXML = getOptional(radarSensorXML, "CollectionStartTime");
        if(collectionStartTimeXML)
        {
            parseDouble(collectionStartTimeXML, errParam.monostatic->radarSensor.collectionStartTime);
        }

        XMLElem rangeBiasDecorrXML = getOptional(radarSensorXML, "RangeBiasDecorr");
        if(rangeBiasDecorrXML)
        {
            errParam.monostatic->radarSensor.rangeBiasDecorr.reset(new six::DecorrType());
            mCommon.parseDecorrType(rangeBiasDecorrXML, *(errParam.monostatic->radarSensor.rangeBiasDecorr));
        }

        XMLElem tropoErrorXML = getFirstAndOnly(monostaticXML, "TropoError");
        if(tropoErrorXML)
        {
            errParam.monostatic->tropoError.reset(new six::TropoError());
            XMLElem verticalXML = getOptional(tropoErrorXML, "TropoRangeVertical");
            double value;
            if(verticalXML)
            {
                if (parseDouble(verticalXML, value))
                {
                    errParam.monostatic->tropoError->TropoRangeVertical().set(value);
                }
            }
            XMLElem slantXML = getOptional(tropoErrorXML, "TropoRangeSlant");
            if(slantXML)
            {
                if (parseDouble(slantXML, value))
                {
                    errParam.monostatic->tropoError->TropoRangeSlant().set(value);
                }
            }
            XMLElem decorrXML = getOptional(tropoErrorXML, "TropoRangeDecorr");
            if(decorrXML)
            {
                six::DecorrType tropoRangeDecorr;
                mCommon.parseDecorrType(decorrXML, tropoRangeDecorr);
                errParam.monostatic->tropoError->TropoRangeDecorr().set(tropoRangeDecorr);
            }
        }

        XMLElem ionoErrorXML = getFirstAndOnly(monostaticXML, "IonoError");
        if(ionoErrorXML)
        {
            errParam.monostatic->ionoError.reset(new six::IonoError());
            double value;
            parseDouble(getFirstAndOnly(ionoErrorXML, "IonoRangeVertical"), value);
            errParam.monostatic->ionoError->IonoRangeVertical().set(value);

            XMLElem rateVerticalXML = getOptional(ionoErrorXML, "IonoRangeRateVertical");
            if(rateVerticalXML)
            {
                if (parseDouble(rateVerticalXML, value))
                {
                    errParam.monostatic->ionoError->IonoRangeRateVertical().set(value);
                }
            }
            XMLElem rgrgRateCCXML = getOptional(ionoErrorXML, "IonoRgRgRateCC");
            if(rgrgRateCCXML)
            {
                if (parseDouble(rgrgRateCCXML, value))
                {
                    errParam.monostatic->ionoError->IonoRgRgRateCC().set(value);
                }
            }
            XMLElem decorrXML = getOptional(ionoErrorXML, "IonoRangeVertDecorr");
            if(decorrXML)
            {
                six::DecorrType ionoRangeVertDecorr;
                mCommon.parseDecorrType(decorrXML, ionoRangeVertDecorr);
                errParam.monostatic->ionoError->IonoRangeVertDecorr().set(ionoRangeVertDecorr);
            }
        }
        mCommon.parseParameters(monostaticXML, "Parameter", errParam.monostatic->parameter);
    }
    else if(!monostaticXML && bistaticXML)
    {
        errParam.bistatic.reset(new ErrorParameters::Bistatic());
        parsePlatform(getFirstAndOnly(bistaticXML, "TxPlatform"), errParam.bistatic->txPlatform);
        parsePlatform(getFirstAndOnly(bistaticXML, "RcvPlatform"), errParam.bistatic->rcvPlatform);
        mCommon.parseParameters(bistaticXML, "Parameter", errParam.bistatic->parameter);
    }
    else
    {
        throw except::Exception(Ctxt(
                "Must be one of monostatic or bistatic"));
    }
}


void CPHDXMLParser::fromXML(const XMLElem productInfoXML, ProductInfo& productInfo)
{
    XMLElem profileXML = getOptional(productInfoXML, "Profile");
    if(profileXML)
    {
        parseString(profileXML, productInfo.profile);
    }

    std::vector<XMLElem> creationInfoXML;
    productInfoXML->getElementsByTagName("CreationInfo", creationInfoXML);
    productInfo.creationInfo.resize(creationInfoXML.size());

    for (size_t ii = 0; ii < creationInfoXML.size(); ++ii)
    {
        XMLElem applicationXML = getOptional(creationInfoXML[ii], "Application");
        if(applicationXML)
        {
            parseString(applicationXML, productInfo.creationInfo[ii].application);
        }

        parseDateTime(getFirstAndOnly(creationInfoXML[ii], "DateTime"), productInfo.creationInfo[ii].dateTime);

        XMLElem siteXML = getOptional(creationInfoXML[ii], "Site");
        if(siteXML)
        {
            parseString(siteXML, productInfo.creationInfo[ii].site);
        }
        mCommon.parseParameters(creationInfoXML[ii], "Parameter", productInfo.creationInfo[ii].parameter);
    }
    mCommon.parseParameters(productInfoXML, "Parameter", productInfo.parameter);

}

void CPHDXMLParser::fromXML(const XMLElem geoInfoXML, GeoInfo& geoInfo)
{
    std::vector < XMLElem > geoInfosXML;
    geoInfoXML->getElementsByTagName("GeoInfo", geoInfosXML);
    geoInfo.name = geoInfoXML->getAttributes().getValue("name");

    //optional
    mCommon.parseParameters(geoInfoXML, "Desc", geoInfo.desc);

    XMLElem tmpElem = getOptional(geoInfoXML, "Point");
    if (tmpElem)
    {
        LatLon ll;
        mCommon.parseLatLon(tmpElem, ll);
        geoInfo.geometryLatLon.push_back(ll);
    }
    else
    {
        std::string pointName = "Endpoint";
        tmpElem = getOptional(geoInfoXML, "Line");
        if (!tmpElem)
        {
            pointName = "Vertex";
            tmpElem = getOptional(geoInfoXML, "Polygon");
        }
        if (tmpElem)
        {
            mCommon.parseLatLons(tmpElem, pointName, geoInfo.geometryLatLon);
        }
    }

    //optional
    size_t idx(geoInfo.geoInfos.size());
    geoInfo.geoInfos.resize(idx + geoInfosXML.size());

    for (auto it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoInfo.geoInfos[idx].reset(new GeoInfo());
        fromXML(*it, *geoInfo.geoInfos[idx]);
    }
}

void CPHDXMLParser::fromXML(const XMLElem matchInfoXML, MatchInformation& matchInfo)
{
    mCommon.parseMatchInformationFromXML(matchInfoXML, &matchInfo);
}


XMLElem CPHDXMLParser::createLatLonFootprint(const std::string& name,
                                              const std::string& cornerName,
                                              const cphd::LatLonCorners& corners,
                                              XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex =
        mCommon.createLatLon(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1");

    vertex = mCommon.createLatLon(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2");

    vertex = mCommon.createLatLon(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3");

    vertex = mCommon.createLatLon(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4");

    return footprint;
}

XMLElem CPHDXMLParser::createPVPType(const std::string& name,
                                      const PVPType& p,
                                      XMLElem parent) const
{
    XMLElem pvpXML = newElement(name, parent);
    createInt("Offset", p.getOffset(), pvpXML);
    createInt("Size", p.getSize(), pvpXML);
    createString("Format", p.getFormat(), pvpXML);
    return pvpXML;
}

XMLElem CPHDXMLParser::createAPVPType(const std::string& name,
                                       const APVPType& p,
                                       XMLElem parent) const
{
    XMLElem apvpXML = newElement(name, parent);
    createString("Name", p.getName(), apvpXML);
    createInt("Offset", p.getOffset(), apvpXML);
    createInt("Size", p.getSize(), apvpXML);
    createString("Format", p.getFormat(), apvpXML);
    return apvpXML;
}

XMLElem CPHDXMLParser::createErrorParamPlatform(
        const std::string& /*name*/,
        const ErrorParameters::Bistatic::Platform p,
        XMLElem parent) const
{
    XMLElem posVelErrXML = newElement("PosVelErr", parent);
    createString("Frame", p.posVelErr.frame, posVelErrXML);
    createDouble("P1", p.posVelErr.p1, posVelErrXML);
    createDouble("P2", p.posVelErr.p2, posVelErrXML);
    createDouble("P3", p.posVelErr.p3, posVelErrXML);
    createDouble("V1", p.posVelErr.v1, posVelErrXML);
    createDouble("V2", p.posVelErr.v2, posVelErrXML);
    createDouble("V3", p.posVelErr.v3, posVelErrXML);
    if(p.posVelErr.corrCoefs.get())
    {
        XMLElem corrCoefsXML = newElement("CorrCoefs", posVelErrXML);
        createDouble("P1P2", p.posVelErr.corrCoefs->p1p2, corrCoefsXML);
        createDouble("P1P3", p.posVelErr.corrCoefs->p1p3, corrCoefsXML);
        createDouble("P1V1", p.posVelErr.corrCoefs->p1v1, corrCoefsXML);
        createDouble("P1V2", p.posVelErr.corrCoefs->p1v2, corrCoefsXML);
        createDouble("P1V3", p.posVelErr.corrCoefs->p1v3, corrCoefsXML);
        createDouble("P2P3", p.posVelErr.corrCoefs->p2p3, corrCoefsXML);
        createDouble("P2V1", p.posVelErr.corrCoefs->p2v1, corrCoefsXML);
        createDouble("P2V2", p.posVelErr.corrCoefs->p2v2, corrCoefsXML);
        createDouble("P2V3", p.posVelErr.corrCoefs->p2v3, corrCoefsXML);
        createDouble("P3V1", p.posVelErr.corrCoefs->p3v1, corrCoefsXML);
        createDouble("P3V2", p.posVelErr.corrCoefs->p3v2, corrCoefsXML);
        createDouble("P3V3", p.posVelErr.corrCoefs->p3v3, corrCoefsXML);
        createDouble("V1V2", p.posVelErr.corrCoefs->v1v2, corrCoefsXML);
        createDouble("V1V3", p.posVelErr.corrCoefs->v1v3, corrCoefsXML);
        createDouble("V2V3", p.posVelErr.corrCoefs->v2v3, corrCoefsXML);
    }
    if(p.posVelErr.PositionDecorr().has())
    {
        XMLElem positionDecorrXML = newElement("PositionDecorr", posVelErrXML);
        createDouble("CorrCoefZero", p.posVelErr.PositionDecorr().get().corrCoefZero, positionDecorrXML);
        createDouble("DecorrRate", p.posVelErr.PositionDecorr().get().decorrRate, positionDecorrXML);
    }
    return posVelErrXML;
}

/*
 * Parser helper functions
 */
void CPHDXMLParser::parseAreaType(const XMLElem areaXML, AreaType& area) const
{
    mCommon.parseVector2D(getFirstAndOnly(areaXML, "X1Y1"), area.x1y1);
    mCommon.parseVector2D(getFirstAndOnly(areaXML, "X2Y2"), area.x2y2);
    const XMLElem polygonXML = getOptional(areaXML, "Polygon");
    if (polygonXML)
    {
        std::vector<XMLElem> verticesXML;
        polygonXML->getElementsByTagName("Vertex", verticesXML);
        if (verticesXML.size() < 3)
        {
            throw except::Exception(Ctxt(
                    "Polygons must have at least 3 sides"));
        }
        area.polygon.resize(verticesXML.size());
        for (size_t ii = 0; ii < area.polygon.size(); ++ii)
        {
            Vector2& vertex = area.polygon[ii];
            const XMLElem vertexXML = verticesXML[ii];
            mCommon.parseVector2D(vertexXML, vertex);
        }
    }
}

void CPHDXMLParser::parseLineSample(const XMLElem lsXML, LineSample& ls) const
{
    parseDouble(getFirstAndOnly(lsXML, "Line"), ls.line);
    parseDouble(getFirstAndOnly(lsXML, "Sample"), ls.sample);
}

void CPHDXMLParser::parseIAExtent(const XMLElem extentXML,
                                   ImageAreaXExtent& extent) const
{
    parseDouble(getFirstAndOnly(extentXML, "LineSpacing"),
                extent.lineSpacing);
    parseInt(getFirstAndOnly(extentXML, "FirstLine"),
             extent.firstLine);
    parseUInt(getFirstAndOnly(extentXML, "NumLines"),
              extent.numLines);
}

void CPHDXMLParser::parseIAExtent(const XMLElem extentXML,
                                   ImageAreaYExtent& extent) const
{
    parseDouble(getFirstAndOnly(extentXML, "SampleSpacing"),
                extent.sampleSpacing);
    parseInt(getFirstAndOnly(extentXML, "FirstSample"),
             extent.firstSample);
    parseUInt(getFirstAndOnly(extentXML, "NumSamples"),
              extent.numSamples);
}

void CPHDXMLParser::parseChannelParameters(
        const XMLElem paramXML, ChannelParameter& param) const
{
    parseString(getFirstAndOnly(paramXML, "Identifier"), param.identifier);
    parseUInt(getFirstAndOnly(paramXML, "RefVectorIndex"), param.refVectorIndex);
    parseBooleanType(getFirstAndOnly(paramXML, "FXFixed"), param.fxFixed);
    parseBooleanType(getFirstAndOnly(paramXML, "TOAFixed"), param.toaFixed);
    parseBooleanType(getFirstAndOnly(paramXML, "SRPFixed"), param.srpFixed);

    XMLElem signalXML = getOptional(paramXML, "SignalNormal");
    if (signalXML)
    {
        parseBooleanType(signalXML, param.signalNormal);
    }

    parseDouble(getFirstAndOnly(paramXML, "FxC"), param.fxC);
    parseDouble(getFirstAndOnly(paramXML, "FxBW"), param.fxBW);
    XMLElem FxBWNoiseXML = getOptional(paramXML, "FxBWNoise");
    if (FxBWNoiseXML)
    {
        parseDouble(FxBWNoiseXML, param.fxBWNoise);
    }
    parseDouble(getFirstAndOnly(paramXML, "TOASaved"), param.toaSaved);

    XMLElem toaExtendedXML = getOptional(paramXML, "TOAExtended");
    if(toaExtendedXML)
    {
        param.toaExtended.reset(new TOAExtended());
        parseDouble(getFirstAndOnly(toaExtendedXML, "TOAExtSaved"), param.toaExtended->toaExtSaved);
        XMLElem lfmEclipseXML = getOptional(toaExtendedXML, "LFMEclipse");
        if(lfmEclipseXML)
        {
            param.toaExtended->lfmEclipse.reset(new TOAExtended::LFMEclipse());
            parseDouble(getFirstAndOnly(lfmEclipseXML, "FxEarlyLow"), param.toaExtended->lfmEclipse->fxEarlyLow);
            parseDouble(getFirstAndOnly(lfmEclipseXML, "FxEarlyHigh"), param.toaExtended->lfmEclipse->fxEarlyHigh);
            parseDouble(getFirstAndOnly(lfmEclipseXML, "FxLateLow"), param.toaExtended->lfmEclipse->fxLateLow);
            parseDouble(getFirstAndOnly(lfmEclipseXML, "FxLateHigh"), param.toaExtended->lfmEclipse->fxLateHigh);
        }
    }

    XMLElem dwellTimesXML = getFirstAndOnly(paramXML, "DwellTimes");
    parseString(getFirstAndOnly(dwellTimesXML, "CODId"), param.dwellTimes.codId);
    parseString(getFirstAndOnly(dwellTimesXML, "DwellId"), param.dwellTimes.dwellId);

    XMLElem imageAreaXML = getOptional(paramXML, "ImageArea");
    if(imageAreaXML)
    {
        parseAreaType(imageAreaXML, param.imageArea);
    }

    XMLElem antennaXML = getOptional(paramXML, "Antenna");
    if(antennaXML)
    {
        param.antenna.reset(new ChannelParameter::Antenna());
        parseString(getFirstAndOnly(antennaXML, "TxAPCId"), param.antenna->txAPCId);
        parseString(getFirstAndOnly(antennaXML, "TxAPATId"), param.antenna->txAPATId);
        parseString(getFirstAndOnly(antennaXML, "RcvAPCId"), param.antenna->rcvAPCId);
        parseString(getFirstAndOnly(antennaXML, "RcvAPATId"), param.antenna->rcvAPATId);
    }

    XMLElem txRcvXML = getOptional(paramXML, "TxRcv");
    if(txRcvXML)
    {
        std::vector<XMLElem> txWFIdXML;
        txRcvXML->getElementsByTagName("TxWFId", txWFIdXML);
        param.txRcv.reset(new ChannelParameter::TxRcv());
        param.txRcv->txWFId.resize(txWFIdXML.size());
        for(size_t ii = 0; ii < txWFIdXML.size(); ++ii)
        {
            parseString(txWFIdXML[ii], param.txRcv->txWFId[ii]);
        }

        std::vector<XMLElem> rcvIdXML;
        txRcvXML->getElementsByTagName("RcvId", rcvIdXML);
        param.txRcv->rcvId.resize(rcvIdXML.size());
        for(size_t ii = 0; ii < rcvIdXML.size(); ++ii)
        {
            parseString(rcvIdXML[ii], param.txRcv->rcvId[ii]);
        }
    }

    XMLElem tgtRefLevelXML = getOptional(paramXML, "TgtRefLevel");
    if(tgtRefLevelXML)
    {
        param.tgtRefLevel.reset(new TgtRefLevel());
        parseDouble(getFirstAndOnly(tgtRefLevelXML, "PTRef"), param.tgtRefLevel->ptRef);
    }

    XMLElem noiseLevelXML = getOptional(paramXML, "NoiseLevel");
    if(noiseLevelXML)
    {
        param.noiseLevel.reset(new NoiseLevel());
        parseDouble(getFirstAndOnly(noiseLevelXML, "PNRef"), param.noiseLevel->pnRef);
        parseDouble(getFirstAndOnly(noiseLevelXML, "BNRef"), param.noiseLevel->bnRef);
        if(!(param.noiseLevel->bnRef > 0 && param.noiseLevel->bnRef <= 1))
        {
            throw except::Exception(Ctxt(
                "Noise equivalent BW value must be > 0.0 and <= 1.0"));
        }

        XMLElem fxNoiseProfileXML = getOptional(noiseLevelXML, "FxNoiseProfile");
        if(fxNoiseProfileXML)
        {
            param.noiseLevel->fxNoiseProfile.reset(new FxNoiseProfile());
            std::vector<XMLElem> pointXMLVec;
            fxNoiseProfileXML->getElementsByTagName("Point", pointXMLVec);
            if(pointXMLVec.size() < 2)
            {
                throw except::Exception(Ctxt(
                    "Atleast 2 noise profile points must be provided"));
            }
            param.noiseLevel->fxNoiseProfile->point.resize(pointXMLVec.size());
            double prevPoint = six::Init::undefined<double>();
            for(size_t ii = 0; ii < pointXMLVec.size(); ++ii)
            {
                double fx;
                parseDouble(getFirstAndOnly(pointXMLVec[ii], "Fx"), fx);
                parseDouble(getFirstAndOnly(pointXMLVec[ii], "PN"), param.noiseLevel->fxNoiseProfile->point[ii].pn);

                if(!six::Init::isUndefined(prevPoint) && fx <= prevPoint)
                {
                    throw except::Exception(Ctxt(
                        "Fx values are strictly increasing"));
                }
                param.noiseLevel->fxNoiseProfile->point[ii].fx = fx;
                prevPoint = fx;
            }
        }
    }

    // Polarization
    std::vector<XMLElem> PolarizationXML;
    paramXML->getElementsByTagName("Polarization", PolarizationXML);
    for (size_t ii = 0; ii < PolarizationXML.size(); ++ii)
    {
        const XMLElem TxPolXML = getFirstAndOnly(PolarizationXML[ii], "TxPol");
        param.polarization.txPol = PolarizationType(TxPolXML->getCharacterData());

        const XMLElem RcvPolXML = getFirstAndOnly(PolarizationXML[ii], "RcvPol");
        param.polarization.rcvPol = PolarizationType(RcvPolXML->getCharacterData());
    }

}

void CPHDXMLParser::parsePVPType(Pvp& pvp, const XMLElem paramXML, PVPType& param) const
{
    size_t size;
    size_t offset;
    std::string format;
    parseUInt(getFirstAndOnly(paramXML, "Size"), size);
    parseUInt(getFirstAndOnly(paramXML, "Offset"), offset);
    parseString(getFirstAndOnly(paramXML, "Format"), format);
    if (param.getSize() != size)
    {
        std::ostringstream ostr;
        ostr << "Specified size: " << size
             << " does not match default size: "
             << param.getSize();
        throw except::Exception(Ctxt(ostr.str()));
    }
    if (param.getFormat() != format)
    {
        std::ostringstream ostr;
        ostr << "Specified format: " << format
             << " does not match default format: "
             << param.getFormat();
        throw except::Exception(Ctxt(ostr.str()));
    }
    pvp.setOffset(offset, param);
}

void CPHDXMLParser::parsePVPType(Pvp& pvp, const XMLElem paramXML) const
{
    std::string name;
    size_t size;
    size_t offset;
    std::string format;
    parseString(getFirstAndOnly(paramXML, "Name"), name);
    parseUInt(getFirstAndOnly(paramXML, "Size"), size);
    parseUInt(getFirstAndOnly(paramXML, "Offset"), offset);
    parseString(getFirstAndOnly(paramXML, "Format"), format);
    pvp.setCustomParameter(size, offset, format, name);
}

void CPHDXMLParser::parsePlatformParams(const XMLElem platXML, Bistatic::PlatformParams& plat) const
{
    parseDouble(getFirstAndOnly(platXML, "Time"), plat.time);
    parseDouble(getFirstAndOnly(platXML, "SlantRange"), plat.slantRange);
    parseDouble(getFirstAndOnly(platXML, "GroundRange"), plat.groundRange);
    parseDouble(getFirstAndOnly(platXML, "DopplerConeAngle"), plat.dopplerConeAngle);
    parseDouble(getFirstAndOnly(platXML, "AzimuthAngle"), plat.azimuthAngle);
    parseDouble(getFirstAndOnly(platXML, "GrazeAngle"), plat.grazeAngle);
    parseDouble(getFirstAndOnly(platXML, "IncidenceAngle"), plat.incidenceAngle);
    mCommon.parseVector3D(getFirstAndOnly(platXML, "Pos"), plat.pos);
    mCommon.parseVector3D(getFirstAndOnly(platXML, "Vel"), plat.vel);
    std::string side = "";
    parseString(getFirstAndOnly(platXML, "SideOfTrack"), side);
    plat.sideOfTrack = (side == "L" ? six::SideOfTrackType("LEFT") : six::SideOfTrackType("RIGHT"));

}

void CPHDXMLParser::parseCommon(const XMLElem imgTypeXML, ImagingType* imgType) const
{
    parseDouble(getFirstAndOnly(imgTypeXML, "TwistAngle"), imgType->twistAngle);
    parseDouble(getFirstAndOnly(imgTypeXML, "SlopeAngle"), imgType->slopeAngle);
    parseDouble(getFirstAndOnly(imgTypeXML, "LayoverAngle"), imgType->layoverAngle);
    parseDouble(getFirstAndOnly(imgTypeXML, "AzimuthAngle"), imgType->azimuthAngle);
    parseDouble(getFirstAndOnly(imgTypeXML, "GrazeAngle"), imgType->grazeAngle);
}

void CPHDXMLParser::parsePosVelErr(const XMLElem posVelErrXML, six::PosVelError& posVelErr) const
{
    std::string frameStr;
    parseString(getFirstAndOnly(posVelErrXML, "Frame"), frameStr);
    posVelErr.frame.mValue = scene::FrameType::fromString(frameStr);
    parseDouble(getFirstAndOnly(posVelErrXML, "P1"), posVelErr.p1);
    parseDouble(getFirstAndOnly(posVelErrXML, "P2"), posVelErr.p2);
    parseDouble(getFirstAndOnly(posVelErrXML, "P3"), posVelErr.p3);
    parseDouble(getFirstAndOnly(posVelErrXML, "V1"), posVelErr.v1);
    parseDouble(getFirstAndOnly(posVelErrXML, "V2"), posVelErr.v2);
    parseDouble(getFirstAndOnly(posVelErrXML, "V3"), posVelErr.v3);

    XMLElem corrCoefsXML = getOptional(posVelErrXML, "CorrCoefs");

    if(corrCoefsXML)
    {
        posVelErr.corrCoefs.reset(new six::CorrCoefs());
        parseDouble(getFirstAndOnly(corrCoefsXML, "P1P2"), posVelErr.corrCoefs->p1p2);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P1P3"), posVelErr.corrCoefs->p1p3);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P1V1"), posVelErr.corrCoefs->p1v1);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P1V2"), posVelErr.corrCoefs->p1v2);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P1V3"), posVelErr.corrCoefs->p1v3);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P2P3"), posVelErr.corrCoefs->p2p3);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P2V1"), posVelErr.corrCoefs->p2v1);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P2V2"), posVelErr.corrCoefs->p2v2);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P2V3"), posVelErr.corrCoefs->p2v3);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P3V1"), posVelErr.corrCoefs->p3v1);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P3V2"), posVelErr.corrCoefs->p3v2);
        parseDouble(getFirstAndOnly(corrCoefsXML, "P3V3"), posVelErr.corrCoefs->p3v3);
        parseDouble(getFirstAndOnly(corrCoefsXML, "V1V2"), posVelErr.corrCoefs->v1v2);
        parseDouble(getFirstAndOnly(corrCoefsXML, "V1V3"), posVelErr.corrCoefs->v1v3);
        parseDouble(getFirstAndOnly(corrCoefsXML, "V2V3"), posVelErr.corrCoefs->v2v3);
    }

    XMLElem posDecorrXML = getOptional(posVelErrXML, "PositionDecorr");
    if(posDecorrXML)
    {
        // posVelErr.positionDecorr.reset(new six::DecorrType());
        six::DecorrType positionDecorr;
        mCommon.parseDecorrType(posDecorrXML, positionDecorr);
        posVelErr.PositionDecorr().set(positionDecorr);
    }
}

void CPHDXMLParser::parsePlatform(XMLElem platXML, ErrorParameters::Bistatic::Platform& plat) const
{
    parsePosVelErr(getFirstAndOnly(platXML, "PosVelErr"), plat.posVelErr);
    XMLElem radarSensorXML = getFirstAndOnly(platXML, "RadarSensor");
    XMLElem clockFreqSFXML = getOptional(radarSensorXML, "ClockFreqSF");
    if(clockFreqSFXML)
    {
        parseDouble(clockFreqSFXML, plat.radarSensor.clockFreqSF);
    }
    parseDouble(getFirstAndOnly(radarSensorXML, "CollectionStartTime"), plat.radarSensor.collectionStartTime);
}

void CPHDXMLParser::parseSupportArrayParameter(const XMLElem paramXML, SupportArrayParameter& param, bool additionalFlag) const
{
    if(!additionalFlag)
    {
        size_t identifierVal;
        parseUInt(getFirstAndOnly(paramXML, "Identifier"), identifierVal);
        param.setIdentifier(identifierVal);
    }
    parseString(getFirstAndOnly(paramXML, "ElementFormat"), param.elementFormat);
    parseDouble(getFirstAndOnly(paramXML, "X0"), param.x0);
    parseDouble(getFirstAndOnly(paramXML, "Y0"), param.y0);
    parseDouble(getFirstAndOnly(paramXML, "XSS"), param.xSS);
    parseDouble(getFirstAndOnly(paramXML, "YSS"), param.ySS);
}

void CPHDXMLParser::parseTxRcvParameter(const XMLElem paramXML, ParameterType& param) const
{
    parseString(getFirstAndOnly(paramXML, "Identifier"), param.identifier);
    parseDouble(getFirstAndOnly(paramXML, "FreqCenter"), param.freqCenter);
    XMLElem lfmRateXML = getOptional(paramXML, "LFMRate");
    if(lfmRateXML)
    {
        parseDouble(lfmRateXML, param.lfmRate);
    }
    param.polarization = PolarizationType(getFirstAndOnly(paramXML, "Polarization")->getCharacterData());
}
}
