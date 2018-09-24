/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <six/Utilities.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/Metadata.h>

// CPHD Spec is not enforced
#define ENFORCESPEC 0 // TODO: Kill?

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace cphd
{
const char CPHDXMLControl::CPHD10_URI[] = "urn:CPHD:1.0";

CPHDXMLControl::CPHDXMLControl() :
    six::XMLParser(CPHD10_URI, false, new logging::NullLogger(), true),
    mCommon(CPHD10_URI, false, CPHD10_URI, log())
{
}

CPHDXMLControl::CPHDXMLControl(logging::Logger* log, bool ownLog) :
    six::XMLParser(CPHD10_URI, false, log, ownLog),
    mCommon(CPHD10_URI, false, CPHD10_URI, log)
{
}

std::string CPHDXMLControl::getDefaultURI() const
{
    return CPHD10_URI;
}

std::string CPHDXMLControl::getSICommonURI() const
{
    return CPHD10_URI;
}

/*
std::string CPHDXMLControl::toXMLString(const Metadata& metadata)
{
    std::auto_ptr<xml::lite::Document> doc(toXML(metadata));
    io::StringStream ss;
    doc->getRootElement()->print(ss);

    return (std::string("<?xml version=\"1.0\"?>") + ss.stream().str());
}

size_t CPHDXMLControl::getXMLsize(const Metadata& metadata)
{
    return toXMLString(metadata).size();
}
*/

// TODO: Base
std::auto_ptr<Metadata> CPHDXMLControl::fromXML(const std::string& xmlString)
{
    io::StringStream stringStream;
    stringStream.write(xmlString.c_str(), xmlString.size());
    xml::lite::MinidomParser parser;
    parser.parse(stringStream);
    return fromXML(parser.getDocument());
}

std::auto_ptr<Metadata> CPHDXMLControl::fromXML(const xml::lite::Document* doc)
{
    std::auto_ptr<Metadata> cphd(new Metadata());

    XMLElem root = doc->getRootElement();

    XMLElem collectionIDXML   = getFirstAndOnly(root, "CollectionID");
    XMLElem globalXML         = getFirstAndOnly(root, "Global");
    XMLElem sceneCoordsXML    = getFirstAndOnly(root, "SceneCoordinates");
    XMLElem dataXML           = getFirstAndOnly(root, "Data");
    XMLElem channelXML          = getFirstAndOnly(root, "Channel");
    /*
    XMLElem srpXML              = getFirstAndOnly(root, "SRP");
    XMLElem antennaXML          = getOptional(root, "Antenna");
    XMLElem vectorParametersXML = getFirstAndOnly(root, "VectorParameters");
    */

    // Parse XML for each section
    fromXML(collectionIDXML, cphd->collectionID);
    fromXML(globalXML, cphd->global);
    fromXML(sceneCoordsXML, cphd->sceneCoordinates);
    fromXML(dataXML, cphd->data);
    fromXML(channelXML, cphd->channel);
    /*
    fromXML(srpXML, cphd03->srp);

    if (antennaXML != NULL)
    {
        cphd03->antenna.reset(new Antenna());
        fromXML(antennaXML, *cphd03->antenna);
    }

    fromXML(vectorParametersXML, cphd03->vectorParameters);
    */
    return cphd;
}

void CPHDXMLControl::fromXML(const XMLElem collectionIDXML, CollectionID& collectionID)
{
    parseString(getFirstAndOnly(collectionIDXML, "CollectorName"),
                collectionID.collectorName);

    XMLElem element = getOptional(collectionIDXML, "IlluminatorName");
    if (element)
        parseString(element, collectionID.illuminatorName);

    element = getOptional(collectionIDXML, "CoreName");
    if (element)
        parseString(element, collectionID.coreName);

    element = getOptional(collectionIDXML, "CollectType");
    if (element)
        collectionID.collectType
                = six::toType<six::CollectType>(element->getCharacterData());

    XMLElem radarModeXML = getFirstAndOnly(collectionIDXML, "RadarMode");

    collectionID.radarMode
            = six::toType<six::RadarModeType>(getFirstAndOnly(radarModeXML,
                                              "ModeType")->getCharacterData());

    element = getOptional(radarModeXML, "ModeID");
    if (element)
        parseString(element, collectionID.radarModeID);

    std::string classification;
    parseString(getFirstAndOnly(collectionIDXML, "Classification"),
                classification);
    collectionID.setClassificationLevel(classification);

    element = getFirstAndOnly(collectionIDXML, "ReleaseInfo");
    parseString(element, collectionID.releaseInfo);

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

void CPHDXMLControl::fromXML(const XMLElem globalXML, Global& global)
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
    if (tropoXML)
    {
        // Optional
        global.ionoParameters.reset(new IonoParameters());
        parseDouble(getFirstAndOnly(ionoXML, "TECV"), global.ionoParameters->tecv);
        parseDouble(getFirstAndOnly(ionoXML, "F2Height"), global.ionoParameters->f2Height);
    }
}

void CPHDXMLControl::parseVector2D(const XMLElem vecXML, Vector2& vec) const
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
}

void CPHDXMLControl::parseAreaType(const XMLElem areaXML, AreaType& area) const
{
    parseVector2D(getFirstAndOnly(areaXML, "X1Y1"), area.x1y1);
    parseVector2D(getFirstAndOnly(areaXML, "X2Y2"), area.x2y2);
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
            parseVector2D(vertexXML, vertex);
        }
    }
}

void CPHDXMLControl::parseLineSample(const XMLElem lsXML, LineSample& ls) const
{
    parseDouble(getFirstAndOnly(lsXML, "Line"), ls.line);
    parseDouble(getFirstAndOnly(lsXML, "Sample"), ls.sample);
}

void CPHDXMLControl::parseIAExtent(const XMLElem extentXML,
                                   ImageAreaExtent& extent) const
{
    parseDouble(getFirstAndOnly(extentXML, "LineSpacing"),
                extent.lineSpacing);
    parseInt(getFirstAndOnly(extentXML, "FirstLine"),
             extent.firstLine);
    parseUInt(getFirstAndOnly(extentXML, "NumLines"),
              extent.numLines);
}

void CPHDXMLControl::fromXML(const XMLElem sceneCoordsXML,
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
                    std::vector<XMLElem> polyVertices;
                    polygonXML->getElementsByTagName("SV", polyVertices);
                    if (polyVertices.size() < 3)
                    {
                        throw except::Exception(Ctxt(
                                "Polygon must have at least 3 vertices"));
                    }
                    std::vector<LineSample>& vertices =
                            scene.imageGrid->segments[ii].polygon;
                    vertices.resize(polyVertices.size());
                    for (size_t ii = 0; ii < polyVertices.size(); ++ii)
                    {
                        parseLineSample(polyVertices[ii], vertices[ii]);
                    }
                }
            }
        }
    }
}

void CPHDXMLControl::fromXML(const XMLElem dataXML, Data& data)
{
    const XMLElem signalXML = getFirstAndOnly(dataXML, "SignalArrayFormat");
    data.signalArrayFormat = SignalArrayFormat(signalXML->getCharacterData());
    parseUInt(getFirstAndOnly(dataXML, "NumBytesPVP"), data.numBytesPVP);

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
        const XMLElem compressionSizeXML = getOptional(channelsXML[ii],
                                                       "CompressedSignalSize");
        if (compressionSizeXML)
        {
            // Optional
            parseUInt(compressionSizeXML,
                      data.channels[ii].compressedSignalSize);
        }
    }

    parseString(getFirstAndOnly(dataXML, "SignalCompressionID"),
                data.signalCompressionID);

    // Support Arrays
    std::vector<XMLElem> supportsXML;
    dataXML->getElementsByTagName("SupportArray", supportsXML);
    data.supportArrays.resize(supportsXML.size());
    for (size_t ii = 0; ii < supportsXML.size(); ++ii)
    {
        parseString(getFirstAndOnly(supportsXML[ii], "Identifier"),
                                    data.supportArrays[ii].identifier);
        parseUInt(getFirstAndOnly(supportsXML[ii], "NumRows"),
                                  data.supportArrays[ii].numRows);
        parseUInt(getFirstAndOnly(supportsXML[ii], "NumCols"),
                                  data.supportArrays[ii].numCols);
        parseUInt(getFirstAndOnly(supportsXML[ii], "BytesPerElement"),
                                  data.supportArrays[ii].bytesPerElement);
        parseUInt(getFirstAndOnly(supportsXML[ii], "ArrayByteOffset"),
                                  data.supportArrays[ii].arrayByteOffset);
    }
}

void CPHDXMLControl::parseChannelParameters(
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

}

void CPHDXMLControl::fromXML(const XMLElem channelXML, Channel& channel)
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
}
}
