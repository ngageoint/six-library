/* =========================================================================
 * This file is part of six.sicd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include "six/sicd/ComplexXMLControl.h"
#include "six/sicd/ComplexData.h"
#include "six/sicd/ComplexDataBuilder.h"
#include "six/Types.h"
#include "six/Utilities.h"
#include <import/str.h>
#include <iostream>

using namespace six;
using namespace six::sicd;

typedef xml::lite::Element* XMLElem;

const char ComplexXMLControl::SICD_URI[] = "urn:SICD:1.0.0";

std::string ComplexXMLControl::getDefaultURI() const
{
    return SICD_URI;
}

std::string ComplexXMLControl::getSICommonURI() const
{
    return SICD_URI;
}

Data* ComplexXMLControl::fromXML(const xml::lite::Document* doc)
{
    ComplexDataBuilder builder;
    ComplexData *sicd = builder.steal();

    XMLElem root = doc->getRootElement();

    XMLElem collectionInfoXML = getFirstAndOnly(root, "CollectionInfo");

    XMLElem imageCreationXML = NULL;

    imageCreationXML = getOptional(root, "ImageCreation");

    XMLElem imageDataXML = getFirstAndOnly(root, "ImageData");
    XMLElem geoDataXML = getFirstAndOnly(root, "GeoData");
    XMLElem gridXML = getFirstAndOnly(root, "Grid");
    XMLElem timelineXML = getFirstAndOnly(root, "Timeline");
    XMLElem positionXML = getFirstAndOnly(root, "Position");
    XMLElem radarCollectionXML = getFirstAndOnly(root, "RadarCollection");
    XMLElem imageFormationXML = getFirstAndOnly(root, "ImageFormation");
    XMLElem scpcoaXML = getFirstAndOnly(root, "SCPCOA");

    XMLElem radiometricXML = NULL;
    radiometricXML = getOptional(root, "Radiometric");

    XMLElem antennaXML = getOptional(root, "Antenna");

    XMLElem errorStatisticsXML = getOptional(root, "ErrorStatistics");

    XMLElem matchInfoXML = getOptional(root, "MatchInfo");
    XMLElem pfaXML = getOptional(root, "PFA");
    XMLElem rmaXML = getOptional(root, "RMA");

    fromXML(collectionInfoXML, sicd->collectionInformation);

    if (imageCreationXML != NULL)
    {
        builder.addImageCreation();
        fromXML(imageCreationXML, sicd->imageCreation);
    }

    fromXML(imageDataXML, sicd->imageData);
    fromXML(geoDataXML, sicd->geoData);
    fromXML(gridXML, sicd->grid);
    fromXML(timelineXML, sicd->timeline);
    fromXML(positionXML, sicd->position);
    fromXML(radarCollectionXML, sicd->radarCollection);
    fromXML(imageFormationXML, sicd->imageFormation);
    fromXML(scpcoaXML, sicd->scpcoa);

    if (radiometricXML != NULL)
    {
        builder.addRadiometric();
        XMLControl::fromXML(radiometricXML, sicd->radiometric);
    }

    if (antennaXML != NULL)
    {
        builder.addAntenna();
        fromXML(antennaXML, sicd->antenna);
    }

    if (errorStatisticsXML != NULL)
    {
        builder.addErrorStatistics();
        XMLControl::fromXML(errorStatisticsXML, sicd->errorStatistics);
    }

    if (matchInfoXML != NULL)
    {
        builder.addMatchInformation();
        fromXML(matchInfoXML, sicd->matchInformation);
    }

    if (pfaXML != NULL)
    {
        sicd->pfa = new PFA();
        fromXML(pfaXML, sicd->pfa);
    }
    if (rmaXML != NULL)
    {
        sicd->rma = new RMA();
        fromXML(rmaXML, sicd->rma);
    }

    return sicd;
}

xml::lite::Document* ComplexXMLControl::toXML(const Data *data)
{
    if (data->getDataType() != DataType::COMPLEX)
    {
        throw except::Exception(Ctxt("Data must be SICD"));
    }
    xml::lite::Document* doc = new xml::lite::Document();

    XMLElem root = newElement("SICD");
    doc->setRootElement(root);

    const ComplexData *sicd = (const ComplexData*) data;

    toXML(sicd->collectionInformation, root);
    if (sicd->imageCreation)
    {
        toXML(sicd->imageCreation, root);
    }
    toXML(sicd->imageData, root);
    toXML(sicd->geoData, root);
    toXML(sicd->grid, root);
    toXML(sicd->timeline, root);
    toXML(sicd->position, root);
    toXML(sicd->radarCollection, root);
    toXML(sicd->imageFormation, root);
    toXML(sicd->scpcoa, root);
    if (sicd->radiometric)
        XMLControl::toXML(sicd->radiometric, root);
    if (sicd->antenna)
        toXML(sicd->antenna, root);
    if (sicd->errorStatistics)
        XMLControl::toXML(sicd->errorStatistics, root);
    if (sicd->matchInformation && !sicd->matchInformation->collects.empty())
        toXML(sicd->matchInformation, root);

    if (sicd->pfa && sicd->rma)
    {
        throw except::Exception(Ctxt("Only one of PFA and RMA can be defined"));
    }

    if (sicd->pfa)
        toXML(sicd->pfa, root);
    else if (sicd->rma)
        toXML(sicd->rma, root);

    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());
    //        root->setNamespacePrefix("si", getSICommonURI());

    return doc;
}

XMLElem ComplexXMLControl::createFFTSign(const std::string& name, six::FFTSign sign,
                                         XMLElem parent)
{
    std::string charData = (sign == FFTSign::NEG) ? ("-1") : ("+1");
    XMLElem e = newElement(name, getDefaultURI(), charData, parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(getDefaultURI());
    node.setValue("xs:int");
    e->getAttributes().add(node);
    return e;
}

XMLElem ComplexXMLControl::toXML(const CollectionInformation *collInfo,
                                 XMLElem parent)
{
    XMLElem collInfoXML = newElement("CollectionInfo", parent);

    std::string si = getSICommonURI();

    createString("CollectorName", si, collInfo->collectorName, collInfoXML);
    if (!collInfo->illuminatorName.empty())
        createString("IlluminatorName", si, collInfo->illuminatorName,
                     collInfoXML);
    createString("CoreName", si, collInfo->coreName, collInfoXML);
    if (!Init::isUndefined<CollectType>(collInfo->collectType))
        createString("CollectType", si,
                     six::toString<six::CollectType>(collInfo->collectType),
                     collInfoXML);

    XMLElem radarModeXML = newElement("RadarMode", si, collInfoXML);
    createString("ModeType", si, six::toString(collInfo->radarMode),
                 radarModeXML);
    if (!collInfo->radarModeID.empty())
        createString("ModeID", si, collInfo->radarModeID, radarModeXML);

    //TODO maybe add more class. info in the future
    createString("Classification", si, collInfo->classification.level,
                 collInfoXML);

    for (std::vector<std::string>::const_iterator it =
            collInfo->countryCodes.begin(); it != collInfo->countryCodes.end(); ++it)
    {
        createString("CountryCode", si, *it, collInfoXML);
    }
    addParameters("Parameter", si, collInfo->parameters, collInfoXML);
    return collInfoXML;
}

XMLElem ComplexXMLControl::toXML(const ImageCreation *imageCreation,
                                 XMLElem parent)
{
    XMLElem imageCreationXML = newElement("ImageCreation", parent);

    std::string si = getSICommonURI();

    if (!imageCreation->application.empty())
        createString("Application", si, imageCreation->application,
                     imageCreationXML);
    if (!Init::isUndefined<DateTime>(imageCreation->dateTime))
        createDateTime("DateTime", si, imageCreation->dateTime,
                       imageCreationXML);
    if (!imageCreation->site.empty())
        createString("Site", si, imageCreation->site, imageCreationXML);
    if (!imageCreation->profile.empty())
        createString("Profile", si, imageCreation->profile, imageCreationXML);
    return imageCreationXML;
}

XMLElem ComplexXMLControl::toXML(const ImageData *imageData, XMLElem parent)
{
    XMLElem imageDataXML = newElement("ImageData", parent);

    createString("PixelType", six::toString(imageData->pixelType), imageDataXML);
    if (imageData->amplitudeTable)
    {
        AmplitudeTable& ampTable = *(imageData->amplitudeTable);
        XMLElem ampTableXML = newElement("AmpTable", imageDataXML);
        setAttribute(ampTableXML, "size", str::toString(ampTable.numEntries));
        for (unsigned int i = 0; i < ampTable.numEntries; ++i)
        {
            XMLElem ampXML = createDouble("Amplitude", *(double*) ampTable[i],
                                          ampTableXML);
            setAttribute(ampXML, "index", str::toString(i));
        }
    }
    createInt("NumRows", imageData->numRows, imageDataXML);
    createInt("NumCols", imageData->numCols, imageDataXML);
    createInt("FirstRow", imageData->firstRow, imageDataXML);
    createInt("FirstCol", imageData->firstCol, imageDataXML);

    createRowCol("FullImage", "NumRows", "NumCols", imageData->fullImage,
                 imageDataXML);
    createRowCol("SCPPixel", imageData->scpPixel, imageDataXML);

    //only if 3+ vertices
    unsigned int numVertices = imageData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", imageDataXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            XMLElem vertexXML = createRowCol("Vertex", imageData->validData[i],
                                             vXML);
            setAttribute(vertexXML, "index", str::toString(i + 1));
        }
    }
    return imageDataXML;
}

XMLElem ComplexXMLControl::toXML(const GeoData *geoData, XMLElem parent)
{
    XMLElem geoDataXML = newElement("GeoData", parent);

    createEarthModelType("EarthModel", geoData->earthModel, geoDataXML);

    XMLElem scpXML = newElement("SCP", geoDataXML);
    createVector3D("ECF", geoData->scp.ecf, scpXML);
    createLatLonAlt("LLH", geoData->scp.llh, scpXML);

    //createFootprint
    createFootprint("ImageCorners", "ICP", geoData->imageCorners, geoDataXML);

    //only if 3+ vertices
    const size_t numVertices = geoData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", geoDataXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML = createLatLon("Vertex", geoData->validData[ii],
                                             vXML);
            setAttribute(vertexXML, "index", 
                    str::toString(geoData->validDataIdx[ii]));
        }
    }

    for (size_t ii = 0; ii < geoData->geoInfos.size(); ++ii)
    {
        toXML(geoData->geoInfos[ii].get(), geoDataXML);
    }

    return geoDataXML;
}

XMLElem ComplexXMLControl::toXML(const GeoInfo *geoInfo, XMLElem parent)
{
    XMLElem geoInfoXML = newElement("GeoInfo", parent);
    if (!geoInfo->name.empty())
        setAttribute(geoInfoXML, "name", geoInfo->name);

    for (size_t ii = 0; ii < geoInfo->geoInfos.size(); ++ii)
    {
        toXML(geoInfo->geoInfos[ii].get(), geoInfoXML);
    }

    addParameters("Desc", geoInfo->desc, geoInfoXML);

    const size_t numLatLons = geoInfo->geometryLatLon.size();
    if (numLatLons == 1)
    {
        createLatLon("Point", geoInfo->geometryLatLon[0], geoInfoXML);
    }
    else if (numLatLons >= 2)
    {
        XMLElem linePolyXML = newElement(numLatLons == 2 ? "Line" : "Polygon",
                                         geoInfoXML);
        setAttribute(linePolyXML, "size", str::toString(numLatLons));

        for (int i = 0; i < numLatLons; ++i)
        {
            XMLElem v = createLatLon(numLatLons == 2 ? "Endpoint" : "Vertex",
                         geoInfo->geometryLatLon[i], linePolyXML);
            setAttribute(v, "index", 
                    str::toString(geoInfo->geometryLatLonIdx[i]));
        }
    }
    return geoInfoXML;
}

XMLElem ComplexXMLControl::toXML(const Grid *grid, XMLElem parent)
{
    XMLElem gridXML = newElement("Grid", parent);

    createString("ImagePlane", six::toString(grid->imagePlane), gridXML);
    createString("Type", six::toString(grid->type), gridXML);
    createPoly2D("TimeCOAPoly", grid->timeCOAPoly, gridXML);

    XMLElem rowDirXML = newElement("Row", gridXML);

    createVector3D("UVectECF", grid->row->unitVector, rowDirXML);
    createDouble("SS", grid->row->sampleSpacing, rowDirXML);
    createDouble("ImpRespWid", grid->row->impulseResponseWidth, rowDirXML);
    createFFTSign("Sgn", grid->row->sign, rowDirXML);
    createDouble("ImpRespBW", grid->row->impulseResponseBandwidth, rowDirXML);
    createDouble("KCtr", grid->row->kCenter, rowDirXML);
    createDouble("DeltaK1", grid->row->deltaK1, rowDirXML);
    createDouble("DeltaK2", grid->row->deltaK2, rowDirXML);

    if (!Init::isUndefined<Poly2D>(grid->row->deltaKCOAPoly))
    {
        createPoly2D("DeltaKCOAPoly", grid->row->deltaKCOAPoly, rowDirXML);
    }

    if (!Init::isUndefined<std::string>(grid->row->weightType))
    {
        createString("WgtType", grid->row->weightType, rowDirXML);
    }

    size_t numWeights = grid->row->weights.size();
    if (numWeights > 0)
    {
        XMLElem wgtFuncXML = newElement("WgtFunct", rowDirXML);
        setAttribute(wgtFuncXML, "size", str::toString(numWeights));

        for (size_t i = 1; i <= numWeights; ++i)
        {
            XMLElem wgtXML = createDouble("Wgt", grid->row->weights[i - 1],
                                          wgtFuncXML);
            setAttribute(wgtXML, "index", str::toString(i));
        }
    }

    XMLElem colDirXML = newElement("Col", gridXML);

    createVector3D("UVectECF", grid->col->unitVector, colDirXML);
    createDouble("SS", grid->col->sampleSpacing, colDirXML);
    createDouble("ImpRespWid", grid->col->impulseResponseWidth, colDirXML);
    createFFTSign("Sgn", grid->col->sign, colDirXML);
    createDouble("ImpRespBW", grid->col->impulseResponseBandwidth, colDirXML);
    createDouble("KCtr", grid->col->kCenter, colDirXML);
    createDouble("DeltaK1", grid->col->deltaK1, colDirXML);
    createDouble("DeltaK2", grid->col->deltaK2, colDirXML);

    if (!Init::isUndefined<Poly2D>(grid->col->deltaKCOAPoly))
    {
        createPoly2D("DeltaKCOAPoly", grid->col->deltaKCOAPoly, colDirXML);
    }

    if (!Init::isUndefined<std::string>(grid->col->weightType))
    {
        createString("WgtType", grid->col->weightType, colDirXML);
    }

    numWeights = grid->col->weights.size();
    if (numWeights > 0)
    {
        XMLElem wgtFuncXML = newElement("WgtFunct", colDirXML);
        setAttribute(wgtFuncXML, "size", str::toString(numWeights));

        for (size_t i = 1; i <= numWeights; ++i)
        {
            XMLElem wgtXML = createDouble("Wgt", grid->col->weights[i - 1],
                                          wgtFuncXML);
            setAttribute(wgtXML, "index", str::toString(i));
        }
    }

    return gridXML;
}

XMLElem ComplexXMLControl::toXML(const Timeline *timeline, XMLElem parent)
{
    XMLElem timelineXML = newElement("Timeline", parent);

    createDateTime("CollectStart", timeline->collectStart, timelineXML);
    createDouble("CollectDuration", timeline->collectDuration, timelineXML);

    if (timeline->interPulsePeriod)
    {
        XMLElem ippXML = newElement("IPP", timelineXML);
        unsigned int setSize = timeline->interPulsePeriod->sets.size();
        ippXML->attribute("size") = str::toString<int>(setSize);

        for (unsigned int i = 0; i < setSize; ++i)
        {
            TimelineSet* timelineSet = timeline->interPulsePeriod->sets[i];
            XMLElem setXML = newElement("Set", ippXML);
            setXML->attribute("index") = str::toString<int>(i + 1);

            createDouble("TStart", timelineSet->tStart, setXML);
            createDouble("TEnd", timelineSet->tEnd, setXML);
            createInt("IPPStart", timelineSet->interPulsePeriodStart, setXML);
            createInt("IPPEnd", timelineSet->interPulsePeriodEnd, setXML);
            createPoly1D("IPPPoly", timelineSet->interPulsePeriodPoly, setXML);
        }
    }

    return timelineXML;
}

XMLElem ComplexXMLControl::toXML(const Position *position, XMLElem parent)
{
    XMLElem positionXML = newElement("Position", parent);

    createPolyXYZ("ARPPoly", position->arpPoly, positionXML);
    if (!Init::isUndefined<PolyXYZ>(position->grpPoly))
        createPolyXYZ("GRPPoly", position->grpPoly, positionXML);
    if (!Init::isUndefined<PolyXYZ>(position->txAPCPoly))
        createPolyXYZ("TxAPCPoly", position->txAPCPoly, positionXML);
    if (position->rcvAPC && !position->rcvAPC->rcvAPCPolys.empty())
    {
        unsigned int numPolys = position->rcvAPC->rcvAPCPolys.size();
        XMLElem rcvXML = newElement("RcvAPC", positionXML);
        setAttribute(rcvXML, "size", str::toString(numPolys));

        for (unsigned int i = 0; i < numPolys; ++i)
        {
            PolyXYZ xyz = position->rcvAPC->rcvAPCPolys[0];
            XMLElem xyzXML = createPolyXYZ("RcvAPCPoly", xyz, rcvXML);
            setAttribute(xyzXML, "index", str::toString(i + 1));
        }
    }
    return positionXML;
}

XMLElem ComplexXMLControl::toXML(const RadarCollection *radar, XMLElem parent)
{
    XMLElem radarXML = newElement("RadarCollection", parent);

    if (!Init::isUndefined<int>(radar->refFrequencyIndex))
    {
        createInt("RefFreqIndex", radar->refFrequencyIndex, radarXML);
    }

    XMLElem txFreqXML = newElement("TxFrequency", radarXML);
    createDouble("Min", radar->txFrequencyMin, txFreqXML);
    createDouble("Max", radar->txFrequencyMax, txFreqXML);

    if (radar->txPolarization != PolarizationType::NOT_SET)
    {
        createString("TxPolarization", six::toString(radar->txPolarization),
                     radarXML);
    }

    if (!Init::isUndefined<Poly1D>(radar->polarizationHVAnglePoly))
    {
        createPoly1D("PolarizationHVAnglePoly", radar->polarizationHVAnglePoly,
                     radarXML);
    }

    if (!radar->txSequence.empty())
    {
        XMLElem txSeqXML = newElement("TxSequence", radarXML);
        setAttribute(txSeqXML, "size", str::toString(radar->txSequence.size()));

        for (size_t ii = 0; ii < radar->txSequence.size(); ++ii)
        {
            const TxStep* const tx = radar->txSequence[ii].get();

            XMLElem txStepXML = newElement("TxStep", txSeqXML);
            setAttribute(txStepXML, "index", str::toString(ii + 1));

            if (!Init::isUndefined<int>(tx->waveformIndex))
            {
                createInt("WFIndex", tx->waveformIndex, txStepXML);
            }
            if (tx->txPolarization != PolarizationType::NOT_SET)
            {
                createString("TxPolarization",
                             six::toString(tx->txPolarization), txStepXML);
            }
        }
    }

    if (!radar->waveform.empty())
    {
        const size_t numWaveforms = radar->waveform.size();
        XMLElem wfXML = newElement("Waveform", radarXML);
        setAttribute(wfXML, "size", str::toString(numWaveforms));

        for (size_t ii = 0; ii < numWaveforms; ++ii)
        {
            const WaveformParameters* const wf = radar->waveform[ii].get();

            XMLElem wfpXML = newElement("WFParameters", wfXML);
            setAttribute(wfpXML, "index", str::toString(ii + 1));

            if (!Init::isUndefined<double>(wf->txPulseLength))
                createDouble("TxPulseLength", wf->txPulseLength, wfpXML);
            if (!Init::isUndefined<double>(wf->txRFBandwidth))
                createDouble("TxRFBandwidth", wf->txRFBandwidth, wfpXML);
            if (!Init::isUndefined<double>(wf->txFrequencyStart))
                createDouble("TxFreqStart", wf->txFrequencyStart, wfpXML);
            if (!Init::isUndefined<double>(wf->txFMRate))
                createDouble("TxFMRate", wf->txFMRate, wfpXML);
            if (wf->rcvDemodType != DemodType::NOT_SET)
                createString("RcvDemodType", six::toString(wf->rcvDemodType),
                             wfpXML);
            if (!Init::isUndefined<double>(wf->rcvWindowLength))
                createDouble("RcvWindowLength", wf->rcvWindowLength, wfpXML);
            if (!Init::isUndefined<double>(wf->adcSampleRate))
                createDouble("ADCSampleRate", wf->adcSampleRate, wfpXML);
            if (!Init::isUndefined<double>(wf->rcvIFBandwidth))
                createDouble("RcvIFBandwidth", wf->rcvIFBandwidth, wfpXML);
            if (!Init::isUndefined<double>(wf->rcvFrequencyStart))
                createDouble("RcvFreqStart", wf->rcvFrequencyStart, wfpXML);
            if (!Init::isUndefined<double>(wf->rcvFMRate))
                createDouble("RcvFMRate", wf->rcvFMRate, wfpXML);
        }
    }

    const size_t numChannels = radar->rcvChannels.size();
    XMLElem rcvChanXML = newElement("RcvChannels", radarXML);
    setAttribute(rcvChanXML, "size", str::toString(numChannels));
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        const ChannelParameters* const cp = radar->rcvChannels[ii].get();
        XMLElem cpXML = newElement("ChanParameters", rcvChanXML);
        setAttribute(cpXML, "index", str::toString(ii + 1));

        if (!Init::isUndefined<int>(cp->rcvAPCIndex))
            createInt("RcvAPCIndex", cp->rcvAPCIndex, cpXML);

        if (cp->txRcvPolarization != DualPolarizationType::NOT_SET)
        {
            createString("TxRcvPolarization", six::toString<
                    DualPolarizationType>(cp->txRcvPolarization), cpXML);
        }
    }

    if (radar->area.get())
    {
        XMLElem areaXML = newElement("Area", radarXML);
        const Area* const area = radar->area.get();

        bool haveACPCorners = true;

        for (size_t ii = 0; ii < LatLonAltCorners::NUM_CORNERS; ++ii)
        {
            if (Init::isUndefined<LatLonAlt>(area->acpCorners.getCorner(ii)))
            {
                haveACPCorners = false;
                break;
            }
        }

        if (haveACPCorners)
        {
            createFootprint("Corner", "ACP", area->acpCorners, areaXML);
        }

        const AreaPlane* const plane = area->plane.get();
        if (plane)
        {
            XMLElem planeXML = newElement("Plane", areaXML);
            XMLElem refPtXML = newElement("RefPt", planeXML);

            ReferencePoint refPt = plane->referencePoint;
            if (!refPt.name.empty())
                setAttribute(refPtXML, "name", refPt.name);

            createVector3D("ECF", refPt.ecef, refPtXML);
            createDouble("Line", refPt.rowCol.row, refPtXML);
            createDouble("Sample", refPt.rowCol.col, refPtXML);

            areaLineDirectionParametersToXML("XDir", plane->xDirection.get(),
                                             planeXML);
            areaSampleDirectionParametersToXML("YDir", plane->yDirection.get(),
                                               planeXML);

            if (!plane->segmentList.empty())
            {
                XMLElem segListXML = newElement("SegmentList", planeXML);
                setAttribute(segListXML, "size",
                             str::toString(plane->segmentList.size()));

                for (size_t ii = 0; ii < plane->segmentList.size(); ++ii)
                {
                    const Segment* const segment = plane->segmentList[ii].get();
                    XMLElem segXML = newElement("Segment", segListXML);
                    setAttribute(segXML, "index", str::toString(ii + 1));

                    createInt("StartLine", segment->startLine, segXML);
                    createInt("StartSample", segment->startSample, segXML);
                    createInt("EndLine", segment->endLine, segXML);
                    createInt("EndSample", segment->endSample, segXML);
                    createString("Identifier", segment->identifier, segXML);
                }
            }

            createString("Orientation",
                         six::toString<OrientationType>(plane->orientation),
                         planeXML);
        }
    }

    addParameters("Parameter", radar->parameters, radarXML);
    return radarXML;
}

XMLElem ComplexXMLControl::areaLineDirectionParametersToXML(
                                                            const std::string& name,
                                                            const AreaDirectionParameters *adp,
                                                            XMLElem parent)
{
    XMLElem adpXML = newElement(name, parent);
    createVector3D("UVectECF", adp->unitVector, adpXML);
    createDouble("LineSpacing", adp->spacing, adpXML);
    createInt("NumLines", adp->elements, adpXML);
    createInt("FirstLine", adp->first, adpXML);
    return adpXML;
}

XMLElem ComplexXMLControl::areaSampleDirectionParametersToXML(
                                                              const std::string& name,
                                                              const AreaDirectionParameters *adp,
                                                              XMLElem parent)
{
    XMLElem adpXML = newElement(name, parent);
    createVector3D("UVectECF", adp->unitVector, adpXML);
    createDouble("SampleSpacing", adp->spacing, adpXML);
    createInt("NumSamples", adp->elements, adpXML);
    createInt("FirstSample", adp->first, adpXML);
    return adpXML;
}

XMLElem ComplexXMLControl::toXML(const ImageFormation *imageFormation,
                                 XMLElem parent)
{
    XMLElem imageFormationXML = newElement("ImageFormation", parent);

    if (!imageFormation->segmentIdentifier.empty())
        createString("SegmentIdentifier", imageFormation->segmentIdentifier,
                     imageFormationXML);

    //TODO this is actually required by the schema, but I don't want to seg fault
    if (imageFormation->rcvChannelProcessed)
    {
        XMLElem rcvChanXML = newElement("RcvChanProc", imageFormationXML);
        createInt("NumChanProc",
                  imageFormation->rcvChannelProcessed->numChannelsProcessed,
                  rcvChanXML);
        if (!Init::isUndefined<double>(
                                       imageFormation->rcvChannelProcessed->prfScaleFactor))
            createDouble("PRFScaleFactor",
                         imageFormation->rcvChannelProcessed->prfScaleFactor,
                         rcvChanXML);

        for (std::vector<int>::const_iterator it =
                imageFormation->rcvChannelProcessed->channelIndex.begin(); it
                != imageFormation->rcvChannelProcessed->channelIndex.end(); ++it)
        {
            createInt("ChanIndex", *it, rcvChanXML);
        }
    }
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

    for (unsigned int i = 0; i < imageFormation->processing.size(); ++i)
    {
        const Processing* proc = &imageFormation->processing[i];

        XMLElem procXML = newElement("Processing", imageFormationXML);

        createString("Type", proc->type, procXML);
        require(createBooleanType("Applied", proc->applied, procXML), "Applied");
        addParameters("Parameter", proc->parameters, procXML);
    }

    if (imageFormation->polarizationCalibration)
    {
        XMLElem pcXML =
                newElement("PolarizationCalibration", imageFormationXML);

        require(
                createBooleanType(
                                  "HVAngleCompApplied",
                                  imageFormation->polarizationCalibration->hvAngleCompensationApplied,
                                  pcXML), "HVAngleCompApplied");

        require(
                createBooleanType(
                                  "DistortionCorrectionApplied",
                                  imageFormation ->polarizationCalibration->distortionCorrectionApplied,
                                  pcXML), "DistortionCorrectionApplied");

        //TODO this is required, but doing this for safety - once we decide on a policy, maybe throw an exception
        Distortion *distortion =
                imageFormation->polarizationCalibration->distortion;
        if (distortion)
        {
            XMLElem distortionXML = newElement("Distortion", pcXML);

            //This should be optionally added...
            createDateTime("CalibrationDate", distortion->calibrationDate,
                           distortionXML);
            createDouble("A", distortion->a, distortionXML);
            createComplex("F1", distortion->f1, distortionXML);
            createComplex("Q1", distortion->q1, distortionXML);
            createComplex("Q2", distortion->q2, distortionXML);
            createComplex("F2", distortion->f2, distortionXML);
            createComplex("Q3", distortion->q3, distortionXML);
            createComplex("Q4", distortion->q4, distortionXML);

            if (!Init::isUndefined<double>(distortion->gainErrorA))
                createDouble("GainErrorA", distortion->gainErrorA,
                             distortionXML);
            if (!Init::isUndefined<double>(distortion->gainErrorF1))
                createDouble("GainErrorF1", distortion->gainErrorF1,
                             distortionXML);
            if (!Init::isUndefined<double>(distortion->gainErrorF2))
                createDouble("GainErrorF2", distortion->gainErrorF2,
                             distortionXML);
            if (!Init::isUndefined<double>(distortion->phaseErrorF1))
                createDouble("PhaseErrorF1", distortion->phaseErrorF1,
                             distortionXML);
            if (!Init::isUndefined<double>(distortion->phaseErrorF2))
                createDouble("PhaseErrorF2", distortion->phaseErrorF2,
                             distortionXML);
        }
    }
    return imageFormationXML;
}

XMLElem ComplexXMLControl::toXML(const SCPCOA *scpcoa, XMLElem parent)
{
    XMLElem scpcoaXML = newElement("SCPCOA", parent);
    createDouble("SCPTime", scpcoa->scpTime, scpcoaXML);
    createVector3D("ARPPos", scpcoa->arpPos, scpcoaXML);
    createVector3D("ARPVel", scpcoa->arpVel, scpcoaXML);
    createVector3D("ARPAcc", scpcoa->arpAcc, scpcoaXML);
    createString("SideOfTrack", six::toString(scpcoa->sideOfTrack), scpcoaXML);
    createDouble("SlantRange", scpcoa->slantRange, scpcoaXML);
    createDouble("GroundRange", scpcoa->groundRange, scpcoaXML);
    createDouble("DopplerConeAng", scpcoa->dopplerConeAngle, scpcoaXML);
    createDouble("GrazeAng", scpcoa->grazeAngle, scpcoaXML);
    createDouble("IncidenceAng", scpcoa->incidenceAngle, scpcoaXML);
    createDouble("TwistAng", scpcoa->twistAngle, scpcoaXML);
    createDouble("SlopeAng", scpcoa->slopeAngle, scpcoaXML);
    return scpcoaXML;
}

XMLElem ComplexXMLControl::toXML(const Antenna *antenna, XMLElem parent)
{
    XMLElem antennaXML = newElement("Antenna", parent);

    if (antenna->tx)
        toXML("Tx", antenna->tx, antennaXML);
    if (antenna->rcv)
        toXML("Rcv", antenna->rcv, antennaXML);
    if (antenna->twoWay)
        toXML("TwoWay", antenna->twoWay, antennaXML);

    return antennaXML;
}

XMLElem ComplexXMLControl::toXML(const std::string& name,
                                 AntennaParameters *params, XMLElem parent)
{
    XMLElem apXML = newElement(name, parent);

    createPolyXYZ("XAxisPoly", params->xAxisPoly, apXML);
    createPolyXYZ("YAxisPoly", params->yAxisPoly, apXML);
    createDouble("FreqZero", params->frequencyZero, apXML);

    if (params->electricalBoresight)
    {
        XMLElem ebXML = newElement("EB", apXML);
        createPoly1D("DCXPoly", params->electricalBoresight->dcxPoly, ebXML);
        createPoly1D("DCYPoly", params->electricalBoresight->dcyPoly, ebXML);
    }
    if (params->halfPowerBeamwidths)
    {
        XMLElem hpXML = newElement("HPBW", apXML);
        createDouble("DCX", params->halfPowerBeamwidths->dcx, hpXML);
        createDouble("DCY", params->halfPowerBeamwidths->dcy, hpXML);
    }
    if (params->array)
    {
        XMLElem arrXML = newElement("Array", apXML);
        createPoly2D("GainPoly", params->array->gainPoly, arrXML);
        createPoly2D("PhasePoly", params->array->phasePoly, arrXML);
    }
    if (params->element)
    {
        XMLElem elemXML = newElement("Elem", apXML);
        createPoly2D("GainPoly", params->element->gainPoly, elemXML);
        createPoly2D("PhasePoly", params->element->phasePoly, elemXML);
    }
    if (!params->gainBSPoly.empty())
    {
        createPoly1D("GainBSPoly", params->gainBSPoly, apXML);
    }

    createBooleanType("EBFreqShift", params->electricalBoresightFrequencyShift,
                      apXML);
    createBooleanType("MLFreqDilation", params->mainlobeFrequencyDilation,
                      apXML);

    return apXML;
}

XMLElem ComplexXMLControl::toXML(const MatchInformation *matchInfo,
                                 XMLElem parent)
{
    XMLElem matchInfoXML = newElement("MatchInfo", parent);

    for (unsigned int i = 0; i < matchInfo->collects.size(); ++i)
    {

        const MatchCollection* const mc = matchInfo->collects[i].get();
        XMLElem mcXML = newElement("Collect", matchInfoXML);
        setAttribute(mcXML, "index", str::toString(i + 1));

        createString("CollectorName", mc->collectorName, mcXML);
        if (!mc->illuminatorName.empty())
            createString("IlluminatorName", mc->illuminatorName, mcXML);
        createString("CoreName", mc->coreName, mcXML);

        for (std::vector<std::string>::const_iterator it =
                mc->matchType.begin(); it != mc->matchType.end(); ++it)
        {
            createString("MatchType", *it, mcXML);
        }
        addParameters("Parameter", mc->parameters, mcXML);
    }

    return matchInfoXML;
}

XMLElem ComplexXMLControl::toXML(const PFA *pfa, XMLElem parent)
{
    XMLElem pfaXML = newElement("PFA", parent);

    createVector3D("FPN", pfa->focusPlaneNormal, pfaXML);
    createVector3D("IPN", pfa->imagePlaneNormal, pfaXML);
    createDouble("PolarAngRefTime", pfa->polarAngleRefTime, pfaXML);
    createPoly1D("PolarAngPoly", pfa->polarAnglePoly, pfaXML);
    createPoly1D("SpatialFreqSFPoly", pfa->spatialFrequencyScaleFactorPoly,
                 pfaXML);
    createDouble("Krg1", pfa->krg1, pfaXML);
    createDouble("Krg2", pfa->krg2, pfaXML);
    createDouble("Kaz1", pfa->kaz1, pfaXML);
    createDouble("Kaz2", pfa->kaz2, pfaXML);
    if (pfa->slowTimeDeskew)
    {
        XMLElem stdXML = newElement("STDeskew", pfaXML);
        require(createBooleanType("Applied", pfa->slowTimeDeskew->applied,
                                  stdXML), "Applied");

        createPoly2D("STDSPhasePoly",
                     pfa->slowTimeDeskew->slowTimeDeskewPhasePoly, stdXML);
    }

    return pfaXML;
}

XMLElem ComplexXMLControl::toXML(const RMA *rma, XMLElem parent)
{
    XMLElem rmaXML = newElement("RMA", parent);

    createString("RMAlgoType", six::toString<six::RMAlgoType>(rma->algoType),
                 rmaXML);

    if (rma->rmat)
    {
        createString("ImageType", "RMAT", rmaXML);

        XMLElem rmatXML = newElement("RMAT", rmaXML);
        RMAT* rmat = rma->rmat;

        createDouble("RefTime", rmat->refTime, rmatXML);
        createVector3D("PosRef", rmat->refPos, rmatXML);
        createVector3D("UnitVelRef", rmat->refVel, rmatXML);
        createPoly1D("DistRLPoly", rmat->distRefLinePoly, rmatXML);
        createPoly2D("CosDCACOAPoly", rmat->cosDCACOAPoly, rmatXML);
        createDouble("Kx1", rmat->kx1, rmatXML);
        createDouble("Kx2", rmat->kx2, rmatXML);
        createDouble("Ky1", rmat->ky1, rmatXML);
        createDouble("Ky2", rmat->ky2, rmatXML);
    }
    else if (rma->inca)
    {
        createString("ImageType", "INCA", rmaXML);

        XMLElem incaXML = newElement("INCA", rmaXML);
        INCA* inca = rma->inca;

        createPoly1D("TimeCAPoly", inca->timeCAPoly, incaXML);
        createDouble("R_CA_SCP", inca->rangeCA, incaXML);
        createDouble("FreqZero", inca->freqZero, incaXML);
        createPoly2D("DRateSFPoly", inca->dopplerRateScaleFactorPoly, incaXML);

        if (!inca->dopplerCentroidPoly.empty())
        {
            createPoly2D("DopCentroidPoly",
                         inca->dopplerCentroidPoly,
                         incaXML);
        }

        if (!Init::isUndefined<BooleanType>(inca->dopplerCentroidCOA))
        {
            createBooleanType("DopCentroidCOA",
                              inca->dopplerCentroidCOA,
                              incaXML);
        }
    }
    else
    {
        throw except::Exception(Ctxt(
                                     "One of RMAT and INCA must be defined -- both "
                                         "are undefined."));
    }

    return rmaXML;
}

void ComplexXMLControl::fromXML(const XMLElem collectionInfoXML,
                                CollectionInformation *collInfo)
{
    parseString(getFirstAndOnly(collectionInfoXML, "CollectorName"),
                collInfo->collectorName);

    XMLElem element = getOptional(collectionInfoXML, "IlluminatorName");
    if (element)
        parseString(element, collInfo->illuminatorName);

    element = getOptional(collectionInfoXML, "CoreName");
    if (element)
        parseString(element, collInfo->coreName);

    element = getOptional(collectionInfoXML, "CollectType");
    if (element)
        collInfo->collectType
                = six::toType<six::CollectType>(element->getCharacterData());

    XMLElem radarModeXML = getFirstAndOnly(collectionInfoXML, "RadarMode");

    collInfo->radarMode
            = six::toType<RadarModeType>(
                                         getFirstAndOnly(radarModeXML,
                                                         "ModeType")->getCharacterData());

    element = getOptional(radarModeXML, "ModeID");
    if (element)
        parseString(element, collInfo->radarModeID);

    parseString(getFirstAndOnly(collectionInfoXML, "Classification"),
                collInfo->classification.level);

    std::vector < XMLElem > countryCodeXML;
    collectionInfoXML->getElementsByTagName("CountryCode", countryCodeXML);

    //optional
    for (std::vector<XMLElem>::const_iterator it = countryCodeXML.begin(); it
            != countryCodeXML.end(); ++it)
    {
        std::string cc;

        parseString(*it, cc);
        collInfo->countryCodes.push_back(cc);
    }

    //optional
    parseParameters(collectionInfoXML, "Parameter", collInfo->parameters);
}

void ComplexXMLControl::fromXML(const XMLElem imageCreationXML,
                                ImageCreation *imageCreation)
{
    // Optional
    XMLElem element = getOptional(imageCreationXML, "Application");
    if (element)
        parseString(element, imageCreation->application);

    element = getOptional(imageCreationXML, "DateTime");
    if (element)
        parseDateTime(element, imageCreation->dateTime);

    element = getOptional(imageCreationXML, "Site");
    if (element)
        parseString(element, imageCreation->site);

    element = getOptional(imageCreationXML, "Profile");
    if (element)
        parseString(element, imageCreation->profile);
}

void ComplexXMLControl::fromXML(const XMLElem imageDataXML,
                                ImageData *imageData)
{
    imageData->pixelType
            = six::toType<PixelType>(
                                     getFirstAndOnly(imageDataXML, "PixelType")->getCharacterData());

    XMLElem ampTableXML = getOptional(imageDataXML, "AmpTable");

    if (ampTableXML != NULL)
    {
        std::vector < XMLElem > ampsXML;
        ampTableXML->getElementsByTagName("Amplitude", ampsXML);
        imageData->amplitudeTable = new AmplitudeTable();

        AmplitudeTable& ampTable = *(imageData->amplitudeTable);
        for (std::vector<XMLElem>::const_iterator it = ampsXML.begin(); it
                != ampsXML.end(); ++it)
        {
            XMLElem ampXML = *it;
            xml::lite::Attributes atts = ampXML->getAttributes();
            if (atts.contains("index"))
            {
                int index = str::toType<int>(atts.getValue("index"));
                if (index < 0 || index > 255)
                {
                    mLog->warn(
                               Ctxt(
                                    FmtX(
                                         "Unable to parse ampTable value - invalid index: %d",
                                         index)));
                }
                else
                {
                    parseDouble(*it, *(double*) ampTable[index]);
                }
            }
            else
            {
                mLog->warn(
                           Ctxt(
                                FmtX(
                                     "Unable to parse ampTable value - no index provided")));
            }
        }
    }

    parseUInt(getFirstAndOnly(imageDataXML, "NumRows"), imageData->numRows);
    parseUInt(getFirstAndOnly(imageDataXML, "NumCols"), imageData->numCols);
    parseUInt(getFirstAndOnly(imageDataXML, "FirstRow"), imageData->firstRow);
    parseUInt(getFirstAndOnly(imageDataXML, "FirstCol"), imageData->firstCol);

    parseRowColInt(getFirstAndOnly(imageDataXML, "FullImage"), "NumRows",
                   "NumCols", imageData->fullImage);

    parseRowColInt(getFirstAndOnly(imageDataXML, "SCPPixel"), "Row", "Col",
                   imageData->scpPixel);

    XMLElem validDataXML = getOptional(imageDataXML, "ValidData");
    if (validDataXML)
    {
        std::vector < XMLElem > verticesXML;
        validDataXML->getElementsByTagName("Vertex", verticesXML);

        //TODO make sure there are at least 3
        for (std::vector<XMLElem>::const_iterator it = verticesXML.begin(); it
                != verticesXML.end(); ++it)
        {
            RowColInt rowCol;

            parseRowColInt(*it, "Row", "Col", rowCol);
            imageData->validData.push_back(rowCol);
        }
    }

}

void ComplexXMLControl::fromXML(const XMLElem geoDataXML, GeoData *geoData)
{
    parseEarthModelType(getFirstAndOnly(geoDataXML, "EarthModel"),
                        geoData->earthModel);

    XMLElem tmpElem = getFirstAndOnly(geoDataXML, "SCP");
    parseVector3D(getFirstAndOnly(tmpElem, "ECF"), geoData->scp.ecf);
    parseLatLonAlt(getFirstAndOnly(tmpElem, "LLH"), geoData->scp.llh);

    parseFootprint(getFirstAndOnly(geoDataXML, "ImageCorners"), "ICP",
                   geoData->imageCorners);

    tmpElem = getOptional(geoDataXML, "ValidData");
    if (tmpElem != NULL)
    {
        parseLatLons(tmpElem, "Vertex", geoData->validData, 
                geoData->validDataIdx);
    }

    std::vector < XMLElem > geoInfosXML;
    geoDataXML->getElementsByTagName("GeoInfo", geoInfosXML);

    //optional
    size_t idx(geoData->geoInfos.size());
    geoData->geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoData->geoInfos[idx].reset(new GeoInfo());
        fromXML(*it, geoData->geoInfos[idx].get());
    }

}

void ComplexXMLControl::fromXML(const XMLElem geoInfoXML, GeoInfo* geoInfo)
{
    std::vector < XMLElem > geoInfosXML;
    geoInfoXML->getElementsByTagName("GeoInfo", geoInfosXML);
    geoInfo->name = geoInfoXML->getAttributes().getValue("name");

    //optional
    size_t idx(geoInfo->geoInfos.size());
    geoInfo->geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoInfo->geoInfos[idx].reset(new GeoInfo());
        fromXML(*it, geoInfo->geoInfos[idx].get());
    }

    //optional
    parseParameters(geoInfoXML, "Desc", geoInfo->desc);

    XMLElem tmpElem = getOptional(geoInfoXML, "Point");
    if (tmpElem)
    {
        LatLon ll;
        parseLatLon(tmpElem, ll);
        geoInfo->geometryLatLon.push_back(ll);
        geoInfo->geometryLatLonIdx.push_back(0);
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
            parseLatLons(tmpElem, pointName, geoInfo->geometryLatLon, 
                    geoInfo->geometryLatLonIdx);
        }
    }
}

void ComplexXMLControl::fromXML(const XMLElem gridXML, Grid *grid)
{
    grid->imagePlane
            = six::toType<ComplexImagePlaneType>(
                                                 getFirstAndOnly(gridXML,
                                                                 "ImagePlane")->getCharacterData());
    grid->type
            = six::toType<ComplexImageGridType>(
                                                getFirstAndOnly(gridXML, "Type")->getCharacterData());

    XMLElem tmpElem = getFirstAndOnly(gridXML, "TimeCOAPoly");
    parsePoly2D(tmpElem, grid->timeCOAPoly);

    tmpElem = getFirstAndOnly(gridXML, "Row");
    parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->row->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->row->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
                grid->row->impulseResponseWidth);
    grid->row->sign
            = six::toType<six::FFTSign>(
                                        getFirstAndOnly(tmpElem, "Sgn")->getCharacterData());
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespBW"),
                grid->row->impulseResponseBandwidth);
    parseDouble(getFirstAndOnly(tmpElem, "KCtr"), grid->row->kCenter);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK1"), grid->row->deltaK1);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK2"), grid->row->deltaK2);

    XMLElem optElem = getOptional(tmpElem, "DeltaKCOAPoly");
    if (optElem)
    {
        //optional
        parsePoly2D(optElem, grid->row->deltaKCOAPoly);
    }

    optElem = getOptional(tmpElem, "WgtType");
    if (optElem)
    {
        //optional
        parseString(optElem, grid->row->weightType);
    }

    XMLElem weightFuncXML = getOptional(tmpElem, "WgtFunct");
    if (weightFuncXML)
    {
        //optional
        //TODO make sure there is at least one and not more than 512 wgts
        std::vector < XMLElem > weightsXML;
        weightFuncXML->getElementsByTagName("Wgt", weightsXML);
        for (std::vector<XMLElem>::const_iterator it = weightsXML.begin(); it
                != weightsXML.end(); ++it)
        {
            double value;
            parseDouble(*it, value);
            grid->row->weights.push_back(value);
        }
    }

    tmpElem = getFirstAndOnly(gridXML, "Col");
    parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->col->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->col->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
                grid->col->impulseResponseWidth);
    grid->col->sign
            = six::toType<six::FFTSign>(
                                        getFirstAndOnly(tmpElem, "Sgn")->getCharacterData());
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespBW"),
                grid->col->impulseResponseBandwidth);
    parseDouble(getFirstAndOnly(tmpElem, "KCtr"), grid->col->kCenter);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK1"), grid->col->deltaK1);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK2"), grid->col->deltaK2);

    optElem = getOptional(tmpElem, "DeltaKCOAPoly");
    if (optElem)
    {
        //optional
        parsePoly2D(optElem, grid->col->deltaKCOAPoly);
    }

    optElem = getOptional(tmpElem, "WgtType");
    if (optElem)
    {
        //optional
        parseString(optElem, grid->col->weightType);
    }

    weightFuncXML = getOptional(tmpElem, "WgtFunct");
    if (weightFuncXML)
    {
        //optional

        //TODO make sure there is at least one and not more than 512 wgts
        std::vector < XMLElem > weightsXML;
        weightFuncXML->getElementsByTagName("Wgt", weightsXML);
        for (std::vector<XMLElem>::const_iterator it = weightsXML.begin(); it
                != weightsXML.end(); ++it)
        {
            double value;
            parseDouble(*it, value);
            grid->col->weights.push_back(value);
        }
    }
}

void ComplexXMLControl::fromXML(const XMLElem timelineXML, Timeline *timeline)
{
    parseDateTime(getFirstAndOnly(timelineXML, "CollectStart"),
                  timeline->collectStart);
    parseDouble(getFirstAndOnly(timelineXML, "CollectDuration"),
                timeline->collectDuration);

    XMLElem ippXML = getOptional(timelineXML, "IPP");
    if (ippXML)
    {
        timeline->interPulsePeriod = new InterPulsePeriod();
        //TODO make sure there is at least one
        std::vector < XMLElem > setsXML;
        ippXML->getElementsByTagName("Set", setsXML);
        for (std::vector<XMLElem>::const_iterator it = setsXML.begin(); it
                != setsXML.end(); ++it)
        {
            // Use the first set that is already available.
            TimelineSet* ts = new TimelineSet();
            parseDouble(getFirstAndOnly(*it, "TStart"), ts->tStart);
            parseDouble(getFirstAndOnly(*it, "TEnd"), ts->tEnd);
            parseInt(getFirstAndOnly(*it, "IPPStart"),
                     ts->interPulsePeriodStart);
            parseInt(getFirstAndOnly(*it, "IPPEnd"), ts->interPulsePeriodEnd);
            parsePoly1D(getFirstAndOnly(*it, "IPPPoly"),
                        ts->interPulsePeriodPoly);
            timeline->interPulsePeriod->sets.push_back(ts);
        }

        // Required to have at least one timeline set.
        if (timeline->interPulsePeriod->sets.size() == 0)
            timeline->interPulsePeriod->sets.push_back(new TimelineSet());
    }
}

void ComplexXMLControl::fromXML(const XMLElem positionXML, Position *position)
{
    XMLElem tmpElem = getFirstAndOnly(positionXML, "ARPPoly");
    parsePolyXYZ(tmpElem, position->arpPoly);

    tmpElem = getOptional(positionXML, "GRPPoly");
    if (tmpElem)
    {
        //optional
        parsePolyXYZ(tmpElem, position->grpPoly);
    }

    tmpElem = getOptional(positionXML, "TxAPCPoly");
    if (tmpElem)
    {
        //optional
        parsePolyXYZ(tmpElem, position->txAPCPoly);
    }

    tmpElem = getOptional(positionXML, "RcvAPC");
    if (tmpElem)
    {
        //optional
        position->rcvAPC = new RcvAPC();

        //TODO make sure there is at least one
        std::vector < XMLElem > polysXML;
        tmpElem->getElementsByTagName("RcvAPCPoly", polysXML);
        for (std::vector<XMLElem>::const_iterator it = polysXML.begin(); it
                != polysXML.end(); ++it)
        {
            PolyXYZ p;
            parsePolyXYZ(*it, p);
            position->rcvAPC->rcvAPCPolys.push_back(p);
        }
    }
}

void ComplexXMLControl::fromXML(const XMLElem radarCollectionXML,
                                RadarCollection *radarCollection)
{
    XMLElem tmpElem = NULL;
    XMLElem optElem = NULL;

    tmpElem = getOptional(radarCollectionXML, "RefFreqIndex");
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
        radarCollection->txPolarization
                = six::toType<PolarizationType>(tmpElem->getCharacterData());
    }

    tmpElem = getOptional(radarCollectionXML, "PolarizationHVAnglePoly");
    if (tmpElem)
    {
        //optional
        parsePoly1D(tmpElem, radarCollection->polarizationHVAnglePoly);
    }

    tmpElem = getOptional(radarCollectionXML, "TxSequence");
    if (tmpElem)
    {
        //optional
        //TODO make sure there is at least one
        std::vector < XMLElem > txStepsXML;
        tmpElem->getElementsByTagName("TxStep", txStepsXML);
        for (std::vector<XMLElem>::const_iterator it = txStepsXML.begin(); it
                != txStepsXML.end(); ++it)
        {
            radarCollection->txSequence.resize(
                radarCollection->txSequence.size() + 1);
            radarCollection->txSequence.back().reset(new TxStep());
            TxStep* const step = radarCollection->txSequence.back().get();

            optElem = getOptional(*it, "WFIndex");
            if (optElem)
            {
                //optional
                parseInt(optElem, step->waveformIndex);
            }

            optElem = getOptional(*it, "TxPolarization");
            if (optElem)
            {
                //optional
                step->txPolarization
                        = six::toType<PolarizationType>(
                                                        optElem->getCharacterData());
            }
        }
    }

    tmpElem = getOptional(radarCollectionXML, "Waveform");
    if (tmpElem)
    {
        //optional
        //TODO make sure there is at least one
        std::vector < XMLElem > wfParamsXML;
        tmpElem->getElementsByTagName("WFParameters", wfParamsXML);
        for (std::vector<XMLElem>::const_iterator it = wfParamsXML.begin(); it
                != wfParamsXML.end(); ++it)
        {
            radarCollection->waveform.resize(
                radarCollection->waveform.size() + 1);
            radarCollection->waveform.back().reset(new WaveformParameters());
            WaveformParameters* const wfParams =
                radarCollection->waveform.back().get();

            optElem = getOptional(*it, "TxPulseLength");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->txPulseLength);
            }

            optElem = getOptional(*it, "TxRFBandwidth");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->txRFBandwidth);
            }

            optElem = getOptional(*it, "TxFreqStart");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->txFrequencyStart);
            }

            optElem = getOptional(*it, "TxFMRate");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->txFMRate);
            }

            optElem = getOptional(*it, "RcvDemodType");
            if (optElem)
            {
                //optional
                wfParams->rcvDemodType
                        = six::toType<DemodType>(optElem->getCharacterData());
            }

            optElem = getOptional(*it, "RcvWindowLength");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->rcvWindowLength);
            }

            optElem = getOptional(*it, "ADCSampleRate");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->adcSampleRate);
            }

            optElem = getOptional(*it, "RcvIFBandwidth");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->rcvIFBandwidth);
            }

            optElem = getOptional(*it, "RcvFreqStart");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->rcvFrequencyStart);
            }

            optElem = getOptional(*it, "RcvFMRate");
            if (optElem)
            {
                //optional
                parseDouble(optElem, wfParams->rcvFMRate);
            }
        }
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
        radarCollection->area.reset(new Area());

        optElem = getOptional(areaXML, "Corner");
        if (optElem)
        {
            //optional
            parseFootprint(optElem, "ACP", radarCollection->area->acpCorners);
        }

        XMLElem planeXML = getOptional(areaXML, "Plane");
        if (planeXML)
        {
            //optional
            radarCollection->area->plane.reset(new AreaPlane());

            XMLElem refPtXML = getFirstAndOnly(planeXML, "RefPt");
            try
            {
                radarCollection->area->plane->referencePoint.name
                        = refPtXML->getAttributes().getValue("name");
            }
            catch (except::Exception &ex)
            {
            }

            parseVector3D(getFirstAndOnly(refPtXML, "ECF"),
                          radarCollection->area->plane->referencePoint.ecef);
            parseDouble(getFirstAndOnly(refPtXML, "Line"),
                        radarCollection->area->plane->referencePoint.rowCol.row);
            parseDouble(getFirstAndOnly(refPtXML, "Sample"),
                        radarCollection->area->plane->referencePoint.rowCol.col);

            XMLElem dirXML = getFirstAndOnly(planeXML, "XDir");
            parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                          radarCollection->area->plane->xDirection->unitVector);
            parseDouble(getFirstAndOnly(dirXML, "LineSpacing"),
                        radarCollection->area->plane->xDirection->spacing);
            parseUInt(getFirstAndOnly(dirXML, "NumLines"),
                      radarCollection->area->plane->xDirection->elements);
            parseUInt(getFirstAndOnly(dirXML, "FirstLine"),
                      radarCollection->area->plane->xDirection->first);

            dirXML = getFirstAndOnly(planeXML, "YDir");
            parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                          radarCollection->area->plane->yDirection->unitVector);
            parseDouble(getFirstAndOnly(dirXML, "SampleSpacing"),
                        radarCollection->area->plane->yDirection->spacing);
            parseUInt(getFirstAndOnly(dirXML, "NumSamples"),
                      radarCollection->area->plane->yDirection->elements);
            parseUInt(getFirstAndOnly(dirXML, "FirstSample"),
                      radarCollection->area->plane->yDirection->first);

            XMLElem segmentListXML = getOptional(planeXML, "SegmentList");
            if (segmentListXML != NULL)
            {
                //TODO make sure there is at least one
                std::vector < XMLElem > segmentsXML;
                segmentListXML->getElementsByTagName("Segment", segmentsXML);

                for (std::vector<XMLElem>::const_iterator it =
                        segmentsXML.begin(); it != segmentsXML.end(); ++it)
                {
                    radarCollection->area->plane->segmentList.resize(
                        radarCollection->area->plane->segmentList.size() + 1);
                    radarCollection->area->plane->segmentList.back().reset(
                        new Segment());
                    Segment* const seg =
                        radarCollection->area->plane->segmentList.back().get();

                    parseInt(getFirstAndOnly(*it, "StartLine"), seg->startLine);
                    parseInt(getFirstAndOnly(*it, "StartSample"),
                             seg->startSample);
                    parseInt(getFirstAndOnly(*it, "EndLine"), seg->endLine);
                    parseInt(getFirstAndOnly(*it, "EndSample"), seg->endSample);
                    parseString(getFirstAndOnly(*it, "Identifier"),
                                seg->identifier);
                }
            }

            // TODO:  This is required for 0.4.x, but treat as
            // optional to support 0.3.1 data.
            if (tmpElem = getOptional(planeXML, "Orientation"))
                radarCollection->area->plane->orientation = six::toType<
                        OrientationType>(tmpElem->getCharacterData());
        }
    }

    parseParameters(radarCollectionXML, "Parameter",
                    radarCollection->parameters);
}

void ComplexXMLControl::fromXML(const XMLElem imageFormationXML,
                                ImageFormation *imageFormation)
{
    XMLElem tmpElem = getOptional(imageFormationXML, "SegmentIdentifier");
    if (tmpElem)
    {
        //optional
        parseString(tmpElem, imageFormation->segmentIdentifier);
    }

    tmpElem = getFirstAndOnly(imageFormationXML, "RcvChanProc");

    parseUInt(getFirstAndOnly(tmpElem, "NumChanProc"),
              imageFormation->rcvChannelProcessed->numChannelsProcessed);

    XMLElem prfXML = getOptional(tmpElem, "PRFScaleFactor");
    if (prfXML)
    {
        //optional
        parseDouble(prfXML, imageFormation->rcvChannelProcessed->prfScaleFactor);
    }

    //TODO make sure there is at least one
    std::vector < XMLElem > chansXML;
    tmpElem->getElementsByTagName("ChanIndex", chansXML);
    for (std::vector<XMLElem>::const_iterator it = chansXML.begin(); it
            != chansXML.end(); ++it)
    {
        int value;
        parseInt(*it, value);
        imageFormation->rcvChannelProcessed->channelIndex.push_back(value);
    }

    tmpElem = getOptional(imageFormationXML, "TxRcvPolarizationProc");

    if (tmpElem)
    {
        imageFormation->txRcvPolarizationProc = six::toType<
                DualPolarizationType>(tmpElem->getCharacterData());
    }

    imageFormation->imageFormationAlgorithm
            = six::toType<ImageFormationType>(
                                              getFirstAndOnly(
                                                              imageFormationXML,
                                                              "ImageFormAlgo")->getCharacterData());

    parseDouble(getFirstAndOnly(imageFormationXML, "TStartProc"),
                imageFormation->tStartProc);

    parseDouble(getFirstAndOnly(imageFormationXML, "TEndProc"),
                imageFormation->tEndProc);

    tmpElem = getFirstAndOnly(imageFormationXML, "TxFrequencyProc");

    parseDouble(getFirstAndOnly(tmpElem, "MinProc"),
                imageFormation->txFrequencyProcMin);

    parseDouble(getFirstAndOnly(tmpElem, "MaxProc"),
                imageFormation->txFrequencyProcMax);

    imageFormation->slowTimeBeamCompensation
            = six::toType<SlowTimeBeamCompensationType>(
                                                        getFirstAndOnly(
                                                                        imageFormationXML,
                                                                        "STBeamComp")->getCharacterData());

    imageFormation->imageBeamCompensation
            = six::toType<ImageBeamCompensationType>(
                                                     getFirstAndOnly(
                                                                     imageFormationXML,
                                                                     "ImageBeamComp")->getCharacterData());

    imageFormation->azimuthAutofocus
            = six::toType<AutofocusType>(
                                         getFirstAndOnly(imageFormationXML,
                                                         "AzAutofocus")->getCharacterData());

    imageFormation->rangeAutofocus
            = six::toType<AutofocusType>(
                                         getFirstAndOnly(imageFormationXML,
                                                         "RgAutofocus")->getCharacterData());

    std::vector < XMLElem > procXML;
    imageFormationXML->getElementsByTagName("Processing", procXML);

    for (unsigned int i = 0; i < procXML.size(); ++i)
    {
        Processing* proc = new Processing();

        parseString(getFirstAndOnly(procXML[i], "Type"), proc->type);
        parseBooleanType(getFirstAndOnly(procXML[i], "Applied"), proc->applied);
        parseParameters(procXML[i], "Parameter", proc->parameters);

        imageFormation->processing.push_back(*proc);
    }

    XMLElem polCalXML = getOptional(imageFormationXML,
                                    "PolarizationCalibration");
    if (polCalXML)
    {
        //optional
        imageFormation->polarizationCalibration = new PolarizationCalibration();
        imageFormation->polarizationCalibration->distortion = new Distortion();

        parseBooleanType(
                         getFirstAndOnly(polCalXML, "HVAngleCompApplied"),
                         imageFormation->polarizationCalibration ->hvAngleCompensationApplied);

        parseBooleanType(
                         getFirstAndOnly(polCalXML,
                                         "DistortionCorrectionApplied"),
                         imageFormation->polarizationCalibration ->distortionCorrectionApplied);

        XMLElem distortionXML = getFirstAndOnly(polCalXML, "Distortion");

        XMLElem calibDateXML = getOptional(distortionXML, "CalibrationDate");
        if (calibDateXML)
        {
            parseDateTime(
                          calibDateXML,
                          imageFormation->polarizationCalibration ->distortion->calibrationDate);
        }

        parseDouble(getFirstAndOnly(distortionXML, "A"),
                    imageFormation->polarizationCalibration->distortion->a);

        parseComplex(getFirstAndOnly(distortionXML, "F1"),
                     imageFormation->polarizationCalibration->distortion->f1);
        parseComplex(getFirstAndOnly(distortionXML, "Q1"),
                     imageFormation->polarizationCalibration->distortion->q1);
        parseComplex(getFirstAndOnly(distortionXML, "Q2"),
                     imageFormation->polarizationCalibration->distortion->q2);
        parseComplex(getFirstAndOnly(distortionXML, "F2"),
                     imageFormation->polarizationCalibration->distortion->f2);
        parseComplex(getFirstAndOnly(distortionXML, "Q3"),
                     imageFormation->polarizationCalibration->distortion->q3);
        parseComplex(getFirstAndOnly(distortionXML, "Q4"),
                     imageFormation->polarizationCalibration->distortion->q4);

        XMLElem gainErrorXML = getOptional(distortionXML, "GainErrorA");
        if (gainErrorXML)
        {
            parseDouble(
                        gainErrorXML,
                        imageFormation->polarizationCalibration ->distortion->gainErrorA);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF1");
        if (gainErrorXML)
        {
            parseDouble(
                        gainErrorXML,
                        imageFormation->polarizationCalibration ->distortion->gainErrorF1);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF2");
        if (gainErrorXML)
        {
            parseDouble(
                        gainErrorXML,
                        imageFormation->polarizationCalibration ->distortion->gainErrorF2);
        }

        XMLElem phaseErrorXML = getOptional(distortionXML, "PhaseErrorF1");
        if (phaseErrorXML)
        {
            parseDouble(
                        phaseErrorXML,
                        imageFormation->polarizationCalibration ->distortion->phaseErrorF1);
        }

        phaseErrorXML = getOptional(distortionXML, "PhaseErrorF2");
        if (phaseErrorXML)
        {
            parseDouble(
                        phaseErrorXML,
                        imageFormation->polarizationCalibration ->distortion->phaseErrorF2);
        }

    }
}

void ComplexXMLControl::fromXML(const XMLElem scpcoaXML, SCPCOA *scpcoa)
{
    parseDouble(getFirstAndOnly(scpcoaXML, "SCPTime"), scpcoa->scpTime);

    parseVector3D(getFirstAndOnly(scpcoaXML, "ARPPos"), scpcoa->arpPos);
    parseVector3D(getFirstAndOnly(scpcoaXML, "ARPVel"), scpcoa->arpVel);
    parseVector3D(getFirstAndOnly(scpcoaXML, "ARPAcc"), scpcoa->arpAcc);

    parseSideOfTrackType(getFirstAndOnly(scpcoaXML, "SideOfTrack"),
                         scpcoa->sideOfTrack);
    parseDouble(getFirstAndOnly(scpcoaXML, "SlantRange"), scpcoa->slantRange);
    parseDouble(getFirstAndOnly(scpcoaXML, "GroundRange"), scpcoa->groundRange);
    parseDouble(getFirstAndOnly(scpcoaXML, "DopplerConeAng"),
                scpcoa->dopplerConeAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "GrazeAng"), scpcoa->grazeAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "IncidenceAng"),
                scpcoa->incidenceAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "TwistAng"), scpcoa->twistAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "SlopeAng"), scpcoa->slopeAngle);
}

void ComplexXMLControl::fromXML(const XMLElem antennaParamsXML,
                                AntennaParameters* params)
{
    parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "XAxisPoly"),
                 params->xAxisPoly);
    parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "YAxisPoly"),
                 params->yAxisPoly);
    parseDouble(getFirstAndOnly(antennaParamsXML, "FreqZero"),
                params->frequencyZero);

    XMLElem tmpElem = getOptional(antennaParamsXML, "EB");
    if (tmpElem)
    {
        params->electricalBoresight = new ElectricalBoresight();
        parsePoly1D(getFirstAndOnly(tmpElem, "DCXPoly"),
                    params->electricalBoresight->dcxPoly);
        parsePoly1D(getFirstAndOnly(tmpElem, "DCYPoly"),
                    params->electricalBoresight->dcyPoly);
    }

    tmpElem = getOptional(antennaParamsXML, "HPBW");
    if (tmpElem)
    {
        params->halfPowerBeamwidths = new HalfPowerBeamwidths();
        parseDouble(getFirstAndOnly(tmpElem, "DCX"),
                    params->halfPowerBeamwidths->dcx);
        parseDouble(getFirstAndOnly(tmpElem, "DCY"),
                    params->halfPowerBeamwidths->dcy);
    }

    tmpElem = getOptional(antennaParamsXML, "Array");
    if (tmpElem)
    {
        params->array = new GainAndPhasePolys();
        parsePoly2D(getFirstAndOnly(tmpElem, "GainPoly"),
                    params->array->gainPoly);
        parsePoly2D(getFirstAndOnly(tmpElem, "PhasePoly"),
                    params->array->phasePoly);
    }

    tmpElem = getOptional(antennaParamsXML, "Elem");
    if (tmpElem)
    {
        params->element = new GainAndPhasePolys();
        parsePoly2D(getFirstAndOnly(tmpElem, "GainPoly"),
                    params->element->gainPoly);
        parsePoly2D(getFirstAndOnly(tmpElem, "PhasePoly"),
                    params->element->phasePoly);
    }

    tmpElem = getOptional(antennaParamsXML, "GainBSPoly");
    if (tmpElem)
    {
        //optional
        parsePoly1D(tmpElem, params->gainBSPoly);
    }

    tmpElem = getOptional(antennaParamsXML, "EBFreqShift");
    if (tmpElem)
    {
        //optional
        parseBooleanType(tmpElem, params->electricalBoresightFrequencyShift);
    }

    tmpElem = getOptional(antennaParamsXML, "MLFreqDilation");
    if (tmpElem)
    {
        //optional
        parseBooleanType(tmpElem, params->mainlobeFrequencyDilation);
    }
}

void ComplexXMLControl::fromXML(const XMLElem antennaXML, Antenna *antenna)
{
    XMLElem antennaParamsXML = getOptional(antennaXML, "Tx");
    if (antennaParamsXML)
    {
        antenna->tx = new AntennaParameters();
        fromXML(antennaParamsXML, antenna->tx);
    }

    antennaParamsXML = getOptional(antennaXML, "Rcv");
    if (antennaParamsXML)
    {
        antenna->rcv = new AntennaParameters();
        fromXML(antennaParamsXML, antenna->rcv);
    }

    antennaParamsXML = getOptional(antennaXML, "TwoWay");
    if (antennaParamsXML)
    {
        antenna->twoWay = new AntennaParameters();
        fromXML(antennaParamsXML, antenna->twoWay);
    }
}

void ComplexXMLControl::fromXML(const XMLElem matchInfoXML,
                                MatchInformation *matchInfo)
{
    XMLElem optElem = NULL;

    //TODO make sure there is at least one
    std::vector < XMLElem > collectsXML;
    matchInfoXML->getElementsByTagName("Collect", collectsXML);
    for (std::vector<XMLElem>::const_iterator it = collectsXML.begin(); it
            != collectsXML.end(); ++it)
    {
        // The MatchInformation object was given a MatchCollection when
        // it was instantiated.  The first time through, just populate it.

        MatchCollection* coll;
        if (it == collectsXML.begin())
        {
            coll = matchInfo->collects[0].get();
        }
        else
        {
            matchInfo->collects.resize(matchInfo->collects.size() + 1);
            matchInfo->collects.back().reset(new MatchCollection());
            coll = matchInfo->collects.back().get();
        }

        parseString(getFirstAndOnly(*it, "CollectorName"), coll->collectorName);

        optElem = getOptional(*it, "IlluminatorName");
        if (optElem)
        {
            //optional
            parseString(optElem, coll->illuminatorName);
        }

        parseString(getFirstAndOnly(*it, "CoreName"), coll->coreName);

        //optional
        std::vector < XMLElem > matchTypesXML;
        (*it)->getElementsByTagName("MatchType", matchTypesXML);
        for (std::vector<XMLElem>::const_iterator it2 = matchTypesXML.begin(); it2
                != matchTypesXML.end(); ++it2)
        {
            std::string value;

            parseString(*it2, value);
            coll->matchType.push_back(value);
        }

        //optional
        parseParameters(*it, "Parameter", coll->parameters);
    }
}

void ComplexXMLControl::fromXML(const XMLElem pfaXML, PFA *pfa)
{
    parseVector3D(getFirstAndOnly(pfaXML, "FPN"), pfa->focusPlaneNormal);
    parseVector3D(getFirstAndOnly(pfaXML, "IPN"), pfa->imagePlaneNormal);
    parseDouble(getFirstAndOnly(pfaXML, "PolarAngRefTime"),
                pfa->polarAngleRefTime);
    parsePoly1D(getFirstAndOnly(pfaXML, "PolarAngPoly"), pfa->polarAnglePoly);
    parsePoly1D(getFirstAndOnly(pfaXML, "SpatialFreqSFPoly"),
                pfa->spatialFrequencyScaleFactorPoly);
    parseDouble(getFirstAndOnly(pfaXML, "Krg1"), pfa->krg1);
    parseDouble(getFirstAndOnly(pfaXML, "Krg2"), pfa->krg2);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz1"), pfa->kaz1);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz2"), pfa->kaz2);

    XMLElem deskewXML = getOptional(pfaXML, "STDeskew");
    if (deskewXML)
    {
        pfa->slowTimeDeskew = new SlowTimeDeskew();
        parseBooleanType(getFirstAndOnly(deskewXML, "Applied"),
                         pfa->slowTimeDeskew->applied);

        parsePoly2D(getFirstAndOnly(deskewXML, "STDSPhasePoly"),
                    pfa->slowTimeDeskew->slowTimeDeskewPhasePoly);
    }
}

void ComplexXMLControl::fromXML(const XMLElem rmaXML, RMA* rma)
{
    rma->algoType
            = six::toType<RMAlgoType>(
                                      getFirstAndOnly(rmaXML, "RMAlgoType")->getCharacterData());

    XMLElem rmatElem = getOptional(rmaXML, "RMAT");

    if (rmatElem)
    {
        rma->rmat = new RMAT();
        RMAT* rmat = rma->rmat;

        parseDouble(getFirstAndOnly(rmatElem, "RefTime"), rmat->refTime);
        parseVector3D(getFirstAndOnly(rmatElem, "PosRef"), rmat->refPos);
        parseVector3D(getFirstAndOnly(rmatElem, "UnitVelRef"), rmat->refVel);
        parsePoly1D(getFirstAndOnly(rmatElem, "DistRLPoly"),
                    rmat->distRefLinePoly);
        parsePoly2D(getFirstAndOnly(rmatElem, "CosDCACOAPoly"),
                    rmat->cosDCACOAPoly);
        parseDouble(getFirstAndOnly(rmatElem, "Kx1"), rmat->kx1);
        parseDouble(getFirstAndOnly(rmatElem, "Kx2"), rmat->kx2);
        parseDouble(getFirstAndOnly(rmatElem, "Ky1"), rmat->ky1);
        parseDouble(getFirstAndOnly(rmatElem, "Ky2"), rmat->ky2);
    }

    XMLElem incaElem = getOptional(rmaXML, "INCA");

    if (incaElem)
    {
        rma->inca = new INCA();
        INCA* inca = rma->inca;

        parsePoly1D(getFirstAndOnly(incaElem, "TimeCAPoly"), inca->timeCAPoly);
        parseDouble(getFirstAndOnly(incaElem, "R_CA_SCP"), inca->rangeCA);
        parseDouble(getFirstAndOnly(incaElem, "FreqZero"), inca->freqZero);

        // NOTE - the following lines should be replaced with the just the
        // 0.4.1 path - or, once we come up with a better way to track previous
        // versions this should get reworked

        // the dopplerRateScaleFactorPoly changed from 1D to 2D between 0.4.0 and 0.4.1
        XMLElem drateSFPoly = getFirstAndOnly(incaElem, "DRateSFPoly");
        if (!drateSFPoly->getAttributes().contains("order2"))
        {
            Poly1D oldPoly;
            parsePoly1D(drateSFPoly, oldPoly);
            // set x order -> 0, y order -> oldPoly
            size_t yOrder = oldPoly.order();
            inca->dopplerRateScaleFactorPoly = Poly2D(0, yOrder);

            double* yVals = inca->dopplerRateScaleFactorPoly[0];
            for (size_t i = 0; i <= yOrder; ++i)
            {
                yVals[i] = oldPoly[i];
            }
            mLog->warn(
                       Ctxt(
                            "Parsing DRateSFPoly as Poly1D to support version 0.4.0"));
        }
        else
        {
            // 0.4.1 - Poly2D
            parsePoly2D(drateSFPoly, inca->dopplerRateScaleFactorPoly);
        }

        XMLElem tmpElem = getOptional(incaElem, "DopCentroidPoly");
        if (tmpElem)
        {
            parsePoly2D(tmpElem, inca->dopplerCentroidPoly);
        }

        tmpElem = getOptional(incaElem, "DopCentroidCOA");
        if (tmpElem)
        {
            parseBooleanType(tmpElem, inca->dopplerCentroidCOA);
        }
    }
}

XMLElem ComplexXMLControl::createFootprint(const std::string& name,
                                           const std::string& cornerName,
                                           const LatLonCorners& corners,
                                           XMLElem parent)
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex = createLatLon(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1:FRFC");

    vertex = createLatLon(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2:FRLC");

    vertex = createLatLon(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3:LRLC");

    vertex = createLatLon(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4:LRFC");

    return footprint;
}

XMLElem ComplexXMLControl::createFootprint(const std::string& name,
                                           const std::string& cornerName,
                                           const LatLonAltCorners& corners,
                                           XMLElem parent)
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex =
        createLatLonAlt(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1:FRFC");

    vertex = createLatLonAlt(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2:FRLC");

    vertex = createLatLonAlt(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3:LRLC");

    vertex = createLatLonAlt(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4:LRFC");

    return footprint;
}

XMLElem ComplexXMLControl::createString(const std::string& name, const std::string& uri,
                                        const std::string& p, XMLElem parent)
{
    XMLElem e = XMLControl::createString(name, uri, p, parent);
    if (e)
    {
        // add the class attribute
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:string");
        e->getAttributes().add(node);
    }
    return e;
}

XMLElem ComplexXMLControl::createInt(const std::string& name, const std::string& uri, int p,
                                     XMLElem parent)
{
    XMLElem e = XMLControl::createInt(name, uri, p, parent);
    if (e)
    {
        // add the class attribute
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:int");
        e->getAttributes().add(node);
    }
    return e;
}

XMLElem ComplexXMLControl::createDouble(const std::string& name, const std::string& uri,
                                        double p, XMLElem parent)
{
    XMLElem e = XMLControl::createDouble(name, uri, p, parent);
    if (e)
    {
        // add the class attribute
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:double");
        e->getAttributes().add(node);
    }
    return e;
}

XMLElem ComplexXMLControl::createBooleanType(const std::string& name, const std::string& uri,
                                             BooleanType p, XMLElem parent)
{
    XMLElem e = XMLControl::createBooleanType(name, uri, p, parent);
    if (e)
    {
        // add the class attribute
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:boolean");
        e->getAttributes().add(node);
    }
    return e;
}

XMLElem ComplexXMLControl::createDateTime(const std::string& name, const std::string& uri,
                                          const std::string& s, XMLElem parent)
{
    XMLElem e = XMLControl::createDateTime(name, uri, s, parent);
    if (e)
    {
        // add the class attribute
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:dateTime");
        e->getAttributes().add(node);
    }
    return e;
}

XMLElem ComplexXMLControl::createDateTime(const std::string& name, const std::string& uri,
                                          DateTime p, XMLElem parent)
{
    std::string s = six::toString<DateTime>(p);
    return createDateTime(name, uri, s, parent);
}

XMLElem ComplexXMLControl::createDate(const std::string& name, const std::string& uri,
                                      DateTime p, XMLElem parent)
{
    XMLElem e = XMLControl::createDate(name, uri, p, parent);
    if (e)
    {
        // add the class attribute
        xml::lite::AttributeNode node;
        node.setQName("class");
        node.setUri(getDefaultURI());
        node.setValue("xs:date");
        e->getAttributes().add(node);
    }
    return e;
}

XMLElem ComplexXMLControl::createString(const std::string& name, const std::string& p,
                                        XMLElem parent)
{
    return createString(name, getDefaultURI(), p, parent);
}

XMLElem ComplexXMLControl::createInt(const std::string& name, int p, XMLElem parent)
{
    return createInt(name, getDefaultURI(), p, parent);
}

XMLElem ComplexXMLControl::createDouble(const std::string& name, double p,
                                        XMLElem parent)
{
    return createDouble(name, getDefaultURI(), p, parent);
}

XMLElem ComplexXMLControl::createBooleanType(const std::string& name, BooleanType p,
                                             XMLElem parent)
{
    return createBooleanType(name, getDefaultURI(), p, parent);
}

XMLElem ComplexXMLControl::createDateTime(const std::string& name, const std::string& s,
                                          XMLElem parent)
{
    return createDateTime(name, getDefaultURI(), s, parent);
}

XMLElem ComplexXMLControl::createDateTime(const std::string& name, DateTime p,
                                          XMLElem parent)
{
    return createDateTime(name, getDefaultURI(), p, parent);
}

XMLElem ComplexXMLControl::createDate(const std::string& name, DateTime p,
                                      XMLElem parent)
{
    return createDate(name, getDefaultURI(), p, parent);
}

XMLElem ComplexXMLControl::createEarthModelType(const std::string& name,
                                                const EarthModelType& value,
                                                XMLElem parent)
{
    return createString(name, six::toString(value), parent);
}

XMLElem ComplexXMLControl::createSideOfTrackType(const std::string& name,
                                                 const SideOfTrackType& value,
                                                 XMLElem parent)
{
    return createString(name, six::toString(value), parent);
}

void ComplexXMLControl::parseEarthModelType(XMLElem element,
                                            EarthModelType& value)
{
    value = six::toType<EarthModelType>(element->getCharacterData());
}

void ComplexXMLControl::parseSideOfTrackType(XMLElem element,
                                             SideOfTrackType& value)
{
    value = six::toType<SideOfTrackType>(element->getCharacterData());
}
