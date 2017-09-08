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

#include <six/sicd/ComplexXMLParser.h>
#include <six/sicd/ComplexDataBuilder.h>
#include <six/Utilities.h>


namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{
ComplexXMLParser::ComplexXMLParser(const std::string& version,
                                   bool addClassAttributes,
                                   std::auto_ptr<six::SICommonXMLParser> comParser,
                                   logging::Logger* log,
                                   bool ownLog) :
    XMLParser(versionToURI(version), addClassAttributes, log, ownLog),
    mCommon(comParser.release())
{
}

ComplexData* ComplexXMLParser::fromXML(const xml::lite::Document* doc) const
{
    ComplexDataBuilder builder;
    ComplexData *sicd = builder.steal();

    XMLElem root = doc->getRootElement();

    XMLElem collectionInfoXML  = getFirstAndOnly(root, "CollectionInfo");
    XMLElem imageCreationXML   = getOptional(root, "ImageCreation");
    XMLElem imageDataXML       = getFirstAndOnly(root, "ImageData");
    XMLElem geoDataXML         = getFirstAndOnly(root, "GeoData");
    XMLElem gridXML            = getFirstAndOnly(root, "Grid");
    XMLElem timelineXML        = getFirstAndOnly(root, "Timeline");
    XMLElem positionXML        = getFirstAndOnly(root, "Position");
    XMLElem radarCollectionXML = getFirstAndOnly(root, "RadarCollection");
    XMLElem imageFormationXML  = getFirstAndOnly(root, "ImageFormation");
    XMLElem scpcoaXML          = getFirstAndOnly(root, "SCPCOA");
    XMLElem radiometricXML     = getOptional(root, "Radiometric");
    XMLElem antennaXML         = getOptional(root, "Antenna");
    XMLElem errorStatisticsXML = getOptional(root, "ErrorStatistics");
    XMLElem matchInfoXML       = getOptional(root, "MatchInfo");
    XMLElem pfaXML             = getOptional(root, "PFA");
    XMLElem rmaXML             = getOptional(root, "RMA");
    XMLElem rgAzCompXML        = getOptional(root, "RGAZCOMP");
    if (rgAzCompXML != NULL)
    {
        // In 0.5, the element was in all caps and contained additional
        // elements that disappeared in 1.0.  For the time being at least,
        // don't support this.
        throw except::Exception(Ctxt(
                "SIX library does not support RGAZCOMP element"));
    }
    rgAzCompXML                = getOptional(root, "RgAzComp"); // added in 1.0.0

    parseCollectionInformationFromXML(collectionInfoXML, sicd->collectionInformation.get());

    if (imageCreationXML != NULL)
    {
        builder.addImageCreation();
        parseImageCreationFromXML(imageCreationXML, sicd->imageCreation.get());
    }

    parseImageDataFromXML(imageDataXML, sicd->imageData.get());
    parseGeoDataFromXML(geoDataXML, sicd->geoData.get());
    parseGridFromXML(gridXML, sicd->grid.get());
    parseTimelineFromXML(timelineXML, sicd->timeline.get());
    parsePositionFromXML(positionXML, sicd->position.get());
    parseRadarCollectionFromXML(radarCollectionXML, sicd->radarCollection.get());
    parseImageFormationFromXML(imageFormationXML, *sicd->radarCollection, sicd->imageFormation.get());
    parseSCPCOAFromXML(scpcoaXML, sicd->scpcoa.get());

    if (radiometricXML != NULL)
    {
        builder.addRadiometric();
        common().parseRadiometryFromXML(radiometricXML,
                                       sicd->radiometric.get());
    }

    if (antennaXML != NULL)
    {
        builder.addAntenna();
        parseAntennaFromXML(antennaXML, sicd->antenna.get());
    }

    if (errorStatisticsXML != NULL)
    {
        builder.addErrorStatistics();
        common().parseErrorStatisticsFromXML(errorStatisticsXML,
                                            sicd->errorStatistics.get());
    }

    if (matchInfoXML != NULL)
    {
        builder.addMatchInformation();
        parseMatchInformationFromXML(matchInfoXML, sicd->matchInformation.get());
    }

    if (pfaXML != NULL)
    {
        sicd->pfa.reset(new PFA());
        parsePFAFromXML(pfaXML, sicd->pfa.get());
    }
    if (rmaXML != NULL)
    {
        sicd->rma.reset(new RMA());
        parseRMAFromXML(rmaXML, sicd->rma.get());
    }
    if (rgAzCompXML != NULL)
    {
        sicd->rgAzComp.reset(new RgAzComp());
        parseRgAzCompFromXML(rgAzCompXML, sicd->rgAzComp.get());
    }
    return sicd;
}

xml::lite::Document* ComplexXMLParser::toXML(const ComplexData* sicd) const
{
    xml::lite::Document* doc = new xml::lite::Document();

    XMLElem root = newElement("SICD");
    doc->setRootElement(root);

    convertCollectionInformationToXML(sicd->collectionInformation.get(), root);
    if (sicd->imageCreation.get())
    {
        convertImageCreationToXML(sicd->imageCreation.get(), root);
    }
    convertImageDataToXML(sicd->imageData.get(), root);
    convertGeoDataToXML(sicd->geoData.get(), root);
    convertGridToXML(sicd->grid.get(), root);
    convertTimelineToXML(sicd->timeline.get(), root);
    convertPositionToXML(sicd->position.get(), root);
    convertRadarCollectionToXML(sicd->radarCollection.get(), root);
    convertImageFormationToXML(sicd->imageFormation.get(), *sicd->radarCollection, root);
    convertSCPCOAToXML(sicd->scpcoa.get(), root);
    if (sicd->radiometric.get())
    {
        common().convertRadiometryToXML(sicd->radiometric.get(), root);
    }
    if (sicd->antenna.get())
    {
        convertAntennaToXML(sicd->antenna.get(), root);
    }
    if (sicd->errorStatistics.get())
    {
        common().convertErrorStatisticsToXML(sicd->errorStatistics.get(), root);
    }
    if (sicd->matchInformation.get() && !sicd->matchInformation->types.empty())
        convertMatchInformationToXML(sicd->matchInformation.get(), root);

    // parse the choice per version
    convertImageFormationAlgoToXML(sicd->pfa.get(), sicd->rma.get(), sicd->rgAzComp.get(), root);

    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());
    //        root->setNamespacePrefix("si", common().getSICommonURI());

    return doc;
}

XMLElem ComplexXMLParser::createFFTSign(const std::string& name, six::FFTSign sign,
                                         XMLElem parent) const
{
    return createInt(name, getDefaultURI(),
                     (sign == FFTSign::NEG) ? "-1" : "+1", parent);
}

XMLElem ComplexXMLParser::convertCollectionInformationToXML(
    const CollectionInformation *collInfo,
    XMLElem parent) const
{
    XMLElem collInfoXML = newElement("CollectionInfo", parent);

    const std::string si = common().getSICommonURI();

    createString("CollectorName", si, collInfo->collectorName, collInfoXML);
    if (!collInfo->illuminatorName.empty())
        createString("IlluminatorName", si, collInfo->illuminatorName,
                     collInfoXML);
    createString("CoreName", si, collInfo->coreName, collInfoXML);
    if (!Init::isUndefined(collInfo->collectType))
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
    common().addParameters("Parameter", si, collInfo->parameters, collInfoXML);
    return collInfoXML;
}

XMLElem ComplexXMLParser::convertImageCreationToXML(
    const ImageCreation *imageCreation,
    XMLElem parent) const
{
    XMLElem imageCreationXML = newElement("ImageCreation", parent);

    const std::string si = common().getSICommonURI();

    if (!imageCreation->application.empty())
        createString("Application", si, imageCreation->application,
                     imageCreationXML);
    if (!Init::isUndefined(imageCreation->dateTime))
        createDateTime("DateTime", si, imageCreation->dateTime,
                       imageCreationXML);
    if (!imageCreation->site.empty())
        createString("Site", si, imageCreation->site, imageCreationXML);
    if (!imageCreation->profile.empty())
        createString("Profile", si, imageCreation->profile, imageCreationXML);
    return imageCreationXML;
}

XMLElem ComplexXMLParser::convertImageDataToXML(
    const ImageData *imageData, XMLElem parent) const
{
    XMLElem imageDataXML = newElement("ImageData", parent);

    createString("PixelType", six::toString(imageData->pixelType), imageDataXML);
    if (imageData->amplitudeTable.get())
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
    createInt("NumRows", static_cast<int>(imageData->numRows), imageDataXML);
    createInt("NumCols", static_cast<int>(imageData->numCols), imageDataXML);
    createInt("FirstRow", static_cast<int>(imageData->firstRow), imageDataXML);
    createInt("FirstCol", static_cast<int>(imageData->firstCol), imageDataXML);

    common().createRowCol("FullImage", "NumRows", "NumCols", imageData->fullImage,
                 imageDataXML);
    common().createRowCol("SCPPixel", imageData->scpPixel, imageDataXML);

    //only if 3+ vertices
    const size_t numVertices = imageData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", imageDataXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML = common().createRowCol("Vertex", imageData->validData[ii],
                                                      vXML);
            setAttribute(vertexXML, "index", str::toString(ii + 1));
        }
    }
    return imageDataXML;
}

XMLElem ComplexXMLParser::convertGeoDataToXML(
    const GeoData *geoData, XMLElem parent) const
{
    XMLElem geoDataXML = newElement("GeoData", parent);

    createEarthModelType("EarthModel", geoData->earthModel, geoDataXML);

    XMLElem scpXML = newElement("SCP", geoDataXML);
    common().createVector3D("ECF", geoData->scp.ecf, scpXML);
    common().createLatLonAlt("LLH", geoData->scp.llh, scpXML);

    createLatLonFootprint("ImageCorners", "ICP", geoData->imageCorners, geoDataXML);

    //only if 3+ vertices
    const size_t numVertices = geoData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", geoDataXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML = common().createLatLon("Vertex", geoData->validData[ii],
                                                      vXML);
            setAttribute(vertexXML, "index", str::toString(ii + 1));
        }
    }

    for (size_t ii = 0; ii < geoData->geoInfos.size(); ++ii)
    {
        convertGeoInfoToXML(geoData->geoInfos[ii].get(), geoDataXML);
    }

    return geoDataXML;
}

XMLElem ComplexXMLParser::convertGridToXML(
    const Grid *grid, XMLElem parent) const
{
    XMLElem gridXML = newElement("Grid", parent);

    createString("ImagePlane", six::toString(grid->imagePlane), gridXML);
    createString("Type", six::toString(grid->type), gridXML);
    common().createPoly2D("TimeCOAPoly", grid->timeCOAPoly, gridXML);

    XMLElem rowDirXML = newElement("Row", gridXML);

    common().createVector3D("UVectECF", grid->row->unitVector, rowDirXML);
    createDouble("SS", grid->row->sampleSpacing, rowDirXML);
    createDouble("ImpRespWid", grid->row->impulseResponseWidth, rowDirXML);
    createFFTSign("Sgn", grid->row->sign, rowDirXML);
    createDouble("ImpRespBW", grid->row->impulseResponseBandwidth, rowDirXML);
    createDouble("KCtr", grid->row->kCenter, rowDirXML);
    createDouble("DeltaK1", grid->row->deltaK1, rowDirXML);
    createDouble("DeltaK2", grid->row->deltaK2, rowDirXML);

    if (!Init::isUndefined(grid->row->deltaKCOAPoly))
    {
        common().createPoly2D("DeltaKCOAPoly", grid->row->deltaKCOAPoly, rowDirXML);
    }

    if (grid->row->weightType.get())
    {
        convertWeightTypeToXML(*grid->row->weightType, rowDirXML);
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

    common().createVector3D("UVectECF", grid->col->unitVector, colDirXML);
    createDouble("SS", grid->col->sampleSpacing, colDirXML);
    createDouble("ImpRespWid", grid->col->impulseResponseWidth, colDirXML);
    createFFTSign("Sgn", grid->col->sign, colDirXML);
    createDouble("ImpRespBW", grid->col->impulseResponseBandwidth, colDirXML);
    createDouble("KCtr", grid->col->kCenter, colDirXML);
    createDouble("DeltaK1", grid->col->deltaK1, colDirXML);
    createDouble("DeltaK2", grid->col->deltaK2, colDirXML);

    if (!Init::isUndefined(grid->col->deltaKCOAPoly))
    {
        common().createPoly2D("DeltaKCOAPoly", grid->col->deltaKCOAPoly, colDirXML);
    }

    if (grid->col->weightType.get())
    {
        convertWeightTypeToXML(*grid->col->weightType, colDirXML);
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

XMLElem ComplexXMLParser::convertTimelineToXML(
    const Timeline *timeline, XMLElem parent) const
{
    XMLElem timelineXML = newElement("Timeline", parent);

    createDateTime("CollectStart", timeline->collectStart, timelineXML);
    createDouble("CollectDuration", timeline->collectDuration, timelineXML);

    if (timeline->interPulsePeriod.get())
    {
        XMLElem ippXML = newElement("IPP", timelineXML);
        size_t setSize = timeline->interPulsePeriod->sets.size();
        ippXML->attribute("size") = str::toString<size_t>(setSize);

        for (size_t i = 0; i < setSize; ++i)
        {
            const TimelineSet& timelineSet = timeline->interPulsePeriod->sets[i];
            XMLElem setXML = newElement("Set", ippXML);
            setXML->attribute("index") = str::toString<size_t>(i + 1);

            createDouble("TStart", timelineSet.tStart, setXML);
            createDouble("TEnd", timelineSet.tEnd, setXML);
            createInt("IPPStart", timelineSet.interPulsePeriodStart, setXML);
            createInt("IPPEnd", timelineSet.interPulsePeriodEnd, setXML);
            common().createPoly1D("IPPPoly", timelineSet.interPulsePeriodPoly, setXML);
        }
    }

    return timelineXML;
}

XMLElem ComplexXMLParser::convertPositionToXML(
    const Position *position, XMLElem parent) const
{
    XMLElem positionXML = newElement("Position", parent);

    common().createPolyXYZ("ARPPoly", position->arpPoly, positionXML);
    if (!Init::isUndefined(position->grpPoly))
        common().createPolyXYZ("GRPPoly", position->grpPoly, positionXML);
    if (!Init::isUndefined(position->txAPCPoly))
        common().createPolyXYZ("TxAPCPoly", position->txAPCPoly, positionXML);
    if (position->rcvAPC.get() && !position->rcvAPC->rcvAPCPolys.empty())
    {
        size_t numPolys = position->rcvAPC->rcvAPCPolys.size();
        XMLElem rcvXML = newElement("RcvAPC", positionXML);
        setAttribute(rcvXML, "size", str::toString(numPolys));

        for (size_t i = 0; i < numPolys; ++i)
        {
            PolyXYZ xyz = position->rcvAPC->rcvAPCPolys[i];
            XMLElem xyzXML = common().createPolyXYZ("RcvAPCPoly", xyz, rcvXML);
            setAttribute(xyzXML, "index", str::toString(i + 1));
        }
    }
    return positionXML;
}

XMLElem ComplexXMLParser::createTxFrequency(const RadarCollection* radar,
                                            XMLElem parent) const
{
    XMLElem txFreqXML = newElement("TxFrequency", parent);
    createDouble("Min", radar->txFrequencyMin, txFreqXML);
    createDouble("Max", radar->txFrequencyMax, txFreqXML);
    return txFreqXML;
}

XMLElem ComplexXMLParser::createTxSequence(const RadarCollection* radar,
                                           XMLElem parent) const
{
    if (radar->txSequence.empty())
    {
        return NULL;
    }
    else
    {
        XMLElem txSeqXML = newElement("TxSequence", parent);
        setAttribute(txSeqXML, "size", str::toString(radar->txSequence.size()));

        for (size_t ii = 0; ii < radar->txSequence.size(); ++ii)
        {
            const TxStep* const tx = radar->txSequence[ii].get();

            XMLElem txStepXML = newElement("TxStep", txSeqXML);
            setAttribute(txStepXML, "index", str::toString(ii + 1));

            if (!Init::isUndefined(tx->waveformIndex))
            {
                createInt("WFIndex", tx->waveformIndex, txStepXML);
            }
            if (tx->txPolarization != PolarizationType::NOT_SET)
            {
                createString("TxPolarization",
                             six::toString(tx->txPolarization), txStepXML);
            }
        }

        return txSeqXML;
    }
}

XMLElem ComplexXMLParser::createWaveform(const RadarCollection* radar,
                                         XMLElem parent) const
{
    if (radar->waveform.empty())
    {
        return NULL;
    }
    else
    {
        const size_t numWaveforms = radar->waveform.size();
        XMLElem wfXML = newElement("Waveform", parent);
        setAttribute(wfXML, "size", str::toString(numWaveforms));

        for (size_t ii = 0; ii < numWaveforms; ++ii)
        {
            const WaveformParameters* const wf = radar->waveform[ii].get();

            XMLElem wfpXML = newElement("WFParameters", wfXML);
            setAttribute(wfpXML, "index", str::toString(ii + 1));

            if (!Init::isUndefined(wf->txPulseLength))
                createDouble("TxPulseLength", wf->txPulseLength, wfpXML);
            if (!Init::isUndefined(wf->txRFBandwidth))
                createDouble("TxRFBandwidth", wf->txRFBandwidth, wfpXML);
            if (!Init::isUndefined(wf->txFrequencyStart))
                createDouble("TxFreqStart", wf->txFrequencyStart, wfpXML);
            if (!Init::isUndefined(wf->txFMRate))
                createDouble("TxFMRate", wf->txFMRate, wfpXML);
            if (wf->rcvDemodType != DemodType::NOT_SET)
                createString("RcvDemodType", six::toString(wf->rcvDemodType),
                             wfpXML);
            if (!Init::isUndefined(wf->rcvWindowLength))
                createDouble("RcvWindowLength", wf->rcvWindowLength, wfpXML);
            if (!Init::isUndefined(wf->adcSampleRate))
                createDouble("ADCSampleRate", wf->adcSampleRate, wfpXML);
            if (!Init::isUndefined(wf->rcvIFBandwidth))
                createDouble("RcvIFBandwidth", wf->rcvIFBandwidth, wfpXML);
            if (!Init::isUndefined(wf->rcvFrequencyStart))
                createDouble("RcvFreqStart", wf->rcvFrequencyStart, wfpXML);
            if (!Init::isUndefined(wf->rcvFMRate))
                createDouble("RcvFMRate", wf->rcvFMRate, wfpXML);
        }

        return wfXML;
    }
}

XMLElem ComplexXMLParser::createArea(const RadarCollection* radar,
                                     XMLElem parent) const
{
    if (radar->area.get() == NULL)
    {
        return NULL;
    }
    else
    {
        XMLElem areaXML = newElement("Area", parent);
        const Area* const area = radar->area.get();

        bool haveACPCorners = true;

        for (size_t ii = 0; ii < LatLonAltCorners::NUM_CORNERS; ++ii)
        {
            if (Init::isUndefined(area->acpCorners.getCorner(ii)))
            {
                haveACPCorners = false;
                break;
            }
        }

        if (haveACPCorners)
        {
            createLatLonAltFootprint("Corner", "ACP", area->acpCorners, areaXML);
        }

        const AreaPlane* const plane = area->plane.get();
        if (plane)
        {
            XMLElem planeXML = newElement("Plane", areaXML);
            XMLElem refPtXML = newElement("RefPt", planeXML);

            ReferencePoint refPt = plane->referencePoint;
            if (!refPt.name.empty())
                setAttribute(refPtXML, "name", refPt.name);

            common().createVector3D("ECF", refPt.ecef, refPtXML);
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

            if (!Init::isUndefined(plane->orientation))
            {
                createString("Orientation",
                             six::toString<OrientationType>(plane->orientation),
                             planeXML);
            }
        }

        return areaXML;
    }
}

XMLElem ComplexXMLParser::areaLineDirectionParametersToXML(
    const std::string& name,
    const AreaDirectionParameters *adp,
    XMLElem parent) const
{
    XMLElem adpXML = newElement(name, parent);
    common().createVector3D("UVectECF", adp->unitVector, adpXML);
    createDouble("LineSpacing", adp->spacing, adpXML);
    createInt("NumLines", static_cast<int>(adp->elements), adpXML);
    createInt("FirstLine", static_cast<int>(adp->first), adpXML);
    return adpXML;
}

XMLElem ComplexXMLParser::areaSampleDirectionParametersToXML(
    const std::string& name,
    const AreaDirectionParameters *adp,
    XMLElem parent) const
{
    XMLElem adpXML = newElement(name, parent);
    common().createVector3D("UVectECF", adp->unitVector, adpXML);
    createDouble("SampleSpacing", adp->spacing, adpXML);
    createInt("NumSamples", static_cast<int>(adp->elements), adpXML);
    createInt("FirstSample", static_cast<int>(adp->first), adpXML);
    return adpXML;
}

XMLElem ComplexXMLParser::convertSCPCOAToXML(
    const SCPCOA *scpcoa,
    XMLElem parent) const
{
    XMLElem scpcoaXML = newElement("SCPCOA", parent);
    createDouble("SCPTime", scpcoa->scpTime, scpcoaXML);
    common().createVector3D("ARPPos", scpcoa->arpPos, scpcoaXML);
    common().createVector3D("ARPVel", scpcoa->arpVel, scpcoaXML);
    common().createVector3D("ARPAcc", scpcoa->arpAcc, scpcoaXML);
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

XMLElem ComplexXMLParser::convertAntennaToXML(
    const Antenna *antenna,
    XMLElem parent) const
{
    XMLElem antennaXML = newElement("Antenna", parent);

    if (antenna->tx.get())
    {
        convertAntennaParametersToXML("Tx", antenna->tx.get(), antennaXML);
    }
    if (antenna->rcv.get())
    {
        convertAntennaParametersToXML("Rcv", antenna->rcv.get(), antennaXML);
    }
    if (antenna->twoWay.get())
    {
        convertAntennaParametersToXML("TwoWay", antenna->twoWay.get(), antennaXML);
    }
    return antennaXML;
}

XMLElem ComplexXMLParser::convertAntennaParametersToXML(
    const std::string& name,
    AntennaParameters *params,
    XMLElem parent) const
{
    XMLElem apXML = newElement(name, parent);

    common().createPolyXYZ("XAxisPoly", params->xAxisPoly, apXML);
    common().createPolyXYZ("YAxisPoly", params->yAxisPoly, apXML);
    createDouble("FreqZero", params->frequencyZero, apXML);

    if (params->electricalBoresight.get())
    {
        XMLElem ebXML = newElement("EB", apXML);
        common().createPoly1D("DCXPoly", params->electricalBoresight->dcxPoly, ebXML);
        common().createPoly1D("DCYPoly", params->electricalBoresight->dcyPoly, ebXML);
    }

    this->convertHPBWToXML(params->halfPowerBeamwidths.get(), apXML);

    this->convertAntennaParamArrayToXML(name, params->array.get(), apXML);

    if (params->element.get())
    {
        XMLElem elemXML = newElement("Elem", apXML);
        common().createPoly2D("GainPoly", params->element->gainPoly, elemXML);
        common().createPoly2D("PhasePoly", params->element->phasePoly, elemXML);
    }
    if (!params->gainBSPoly.empty())
    {
        common().createPoly1D("GainBSPoly", params->gainBSPoly, apXML);
    }

    createBooleanType("EBFreqShift", params->electricalBoresightFrequencyShift,
                      apXML);
    createBooleanType("MLFreqDilation", params->mainlobeFrequencyDilation,
                      apXML);

    return apXML;
}

XMLElem ComplexXMLParser::convertPFAToXML(
    const PFA *pfa,
    XMLElem parent) const
{
    XMLElem pfaXML = newElement("PFA", parent);

    common().createVector3D("FPN", pfa->focusPlaneNormal, pfaXML);
    common().createVector3D("IPN", pfa->imagePlaneNormal, pfaXML);
    createDouble("PolarAngRefTime", pfa->polarAngleRefTime, pfaXML);
    common().createPoly1D("PolarAngPoly", pfa->polarAnglePoly, pfaXML);
    common().createPoly1D("SpatialFreqSFPoly", pfa->spatialFrequencyScaleFactorPoly,
                 pfaXML);
    createDouble("Krg1", pfa->krg1, pfaXML);
    createDouble("Krg2", pfa->krg2, pfaXML);
    createDouble("Kaz1", pfa->kaz1, pfaXML);
    createDouble("Kaz2", pfa->kaz2, pfaXML);
    if (pfa->slowTimeDeskew.get())
    {
        XMLElem stdXML = newElement("STDeskew", pfaXML);
        require(createBooleanType("Applied", pfa->slowTimeDeskew->applied,
                                  stdXML), "Applied");

        common().createPoly2D("STDSPhasePoly",
                     pfa->slowTimeDeskew->slowTimeDeskewPhasePoly, stdXML);
    }

    return pfaXML;
}

void ComplexXMLParser::convertDRateSFPolyToXML(
    const INCA* inca, XMLElem incaXML) const
{
    //! Poly2D in 0.4.1
    common().createPoly2D("DRateSFPoly",
        inca->dopplerRateScaleFactorPoly, incaXML);
}

XMLElem ComplexXMLParser::convertRMCRToXML(
    const RMCR* rmcr,
    XMLElem rmaXML) const
{
    createString("ImageType", "RMCR", rmaXML);

    XMLElem rmcrXML = newElement("RMCR", rmaXML);

    common().createVector3D("PosRef", rmcr->refPos, rmcrXML);
    common().createVector3D("VelRef", rmcr->refVel, rmcrXML);
    createDouble("DopConeAngRef", rmcr->dopConeAngleRef, rmcrXML);

    return rmcrXML;
}

XMLElem ComplexXMLParser::convertINCAToXML(
    const INCA* inca,
    XMLElem rmaXML) const
{
    createString("ImageType", "INCA", rmaXML);

    XMLElem incaXML = newElement("INCA", rmaXML);

    common().createPoly1D("TimeCAPoly", inca->timeCAPoly, incaXML);
    createDouble("R_CA_SCP", inca->rangeCA, incaXML);
    createDouble("FreqZero", inca->freqZero, incaXML);

    convertDRateSFPolyToXML(inca, incaXML);

    if (!inca->dopplerCentroidPoly.empty())
    {
        common().createPoly2D("DopCentroidPoly",
                        inca->dopplerCentroidPoly,
                        incaXML);
    }

    if (!Init::isUndefined(inca->dopplerCentroidCOA))
    {
        createBooleanType("DopCentroidCOA",
                          inca->dopplerCentroidCOA,
                          incaXML);
    }

    return incaXML;
}

XMLElem ComplexXMLParser::convertRcvChanProcToXML(
    const std::string& version,
    const RcvChannelProcessed* rcvChanProc,
    XMLElem imageFormationXML) const
{
    if (!rcvChanProc)
    {
        throw except::Exception(Ctxt(FmtX(
            "[RcvChanProc] is a manditory field in ImageFormation in %s",
            version.c_str())));
    }
    if (rcvChanProc->channelIndex.empty())
    {
        throw except::Exception(Ctxt(
                "ImageFormation.RcvChanProc must have at least one ChanIndex"));
    }
    XMLElem rcvChanXML = newElement("RcvChanProc", imageFormationXML);
    createInt("NumChanProc",
              rcvChanProc->numChannelsProcessed,
              rcvChanXML);
    if (!Init::isUndefined(rcvChanProc->prfScaleFactor))
    {
        createDouble("PRFScaleFactor",
                rcvChanProc->prfScaleFactor,
                rcvChanXML);
    }

    for (std::vector<int>::const_iterator it =
            rcvChanProc->channelIndex.begin();
            it != rcvChanProc->channelIndex.end(); ++it)
    {
        createInt("ChanIndex", *it, rcvChanXML);
    }
    return rcvChanXML;
}

XMLElem ComplexXMLParser::convertDistortionToXML(
    const std::string& version,
    const Distortion* distortion,
    XMLElem pcXML) const
{
    if (distortion)
    {
        XMLElem distortionXML = newElement("Distortion", pcXML);

        //This should be optionally added...
        createDateTime("CalibrationDate", distortion->calibrationDate,
                        distortionXML);
        createDouble("A", distortion->a, distortionXML);
        common().createComplex("F1", distortion->f1, distortionXML);
        common().createComplex("Q1", distortion->q1, distortionXML);
        common().createComplex("Q2", distortion->q2, distortionXML);
        common().createComplex("F2", distortion->f2, distortionXML);
        common().createComplex("Q3", distortion->q3, distortionXML);
        common().createComplex("Q4", distortion->q4, distortionXML);

        if (!Init::isUndefined(distortion->gainErrorA))
            createDouble("GainErrorA", distortion->gainErrorA,
                            distortionXML);
        if (!Init::isUndefined(distortion->gainErrorF1))
            createDouble("GainErrorF1", distortion->gainErrorF1,
                            distortionXML);
        if (!Init::isUndefined(distortion->gainErrorF2))
            createDouble("GainErrorF2", distortion->gainErrorF2,
                            distortionXML);
        if (!Init::isUndefined(distortion->phaseErrorF1))
            createDouble("PhaseErrorF1", distortion->phaseErrorF1,
                            distortionXML);
        if (!Init::isUndefined(distortion->phaseErrorF2))
            createDouble("PhaseErrorF2", distortion->phaseErrorF2,
                            distortionXML);

        return distortionXML;
    }
    else
    {
        throw except::Exception(Ctxt(FmtX(
            "[Distortion] is a maditory field of ImageFormation in %s",
            version.c_str())));
    }
}

XMLElem ComplexXMLParser::convertRgAzCompToXML(
    const RgAzComp* rgAzComp,
    XMLElem parent) const
{
    XMLElem rgAzCompXML = newElement("RgAzComp", parent);

    createDouble("AzSF", rgAzComp->azSF, rgAzCompXML);
    common().createPoly1D("KazPoly", rgAzComp->kazPoly, rgAzCompXML);

    return rgAzCompXML;
}

void ComplexXMLParser::parseDRateSFPolyFromXML(
    const XMLElem incaElem, INCA* inca) const
{
    //! Poly2D in 0.4.1
    common().parsePoly2D(getFirstAndOnly(incaElem, "DRateSFPoly"),
                         inca->dopplerRateScaleFactorPoly);
}

void ComplexXMLParser::parseCollectionInformationFromXML(
    const XMLElem collectionInfoXML,
    CollectionInformation *collInfo) const
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
            = six::toType<RadarModeType>(getFirstAndOnly(radarModeXML,
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
    common().parseParameters(collectionInfoXML, "Parameter", collInfo->parameters);
}

void ComplexXMLParser::parseImageCreationFromXML(
    const XMLElem imageCreationXML,
    ImageCreation *imageCreation) const
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

void ComplexXMLParser::parseImageDataFromXML(
    const XMLElem imageDataXML,
    ImageData *imageData) const
{
    imageData->pixelType
            = six::toType<PixelType>(
                    getFirstAndOnly(imageDataXML,
                                    "PixelType")->getCharacterData());

    XMLElem ampTableXML = getOptional(imageDataXML, "AmpTable");

    if (ampTableXML != NULL)
    {
        std::vector < XMLElem > ampsXML;
        ampTableXML->getElementsByTagName("Amplitude", ampsXML);
        imageData->amplitudeTable.reset(new AmplitudeTable());

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
                    log()->warn(Ctxt(
                            "Unable to parse ampTable value - invalid index: " +
                            str::toString(index)));
                }
                else
                {
                    parseDouble(*it, *(double*) ampTable[index]);
                }
            }
            else
            {
                log()->warn(Ctxt(
                        "Unable to parse ampTable value - no index provided"));
            }
        }
    }

    parseUInt(getFirstAndOnly(imageDataXML, "NumRows"), imageData->numRows);
    parseUInt(getFirstAndOnly(imageDataXML, "NumCols"), imageData->numCols);
    parseUInt(getFirstAndOnly(imageDataXML, "FirstRow"), imageData->firstRow);
    parseUInt(getFirstAndOnly(imageDataXML, "FirstCol"), imageData->firstCol);

    common().parseRowColInt(getFirstAndOnly(imageDataXML, "FullImage"), "NumRows",
                   "NumCols", imageData->fullImage);

    common().parseRowColInt(getFirstAndOnly(imageDataXML, "SCPPixel"), "Row", "Col",
                   imageData->scpPixel);

    XMLElem validDataXML = getOptional(imageDataXML, "ValidData");
    if (validDataXML)
    {
        common().parseRowColInts(validDataXML, "Vertex", imageData->validData);
    }
}

void ComplexXMLParser::parseGeoDataFromXML(
    const XMLElem geoDataXML,
    GeoData *geoData) const
{
    parseEarthModelType(getFirstAndOnly(geoDataXML, "EarthModel"),
                        geoData->earthModel);

    XMLElem tmpElem = getFirstAndOnly(geoDataXML, "SCP");
    common().parseVector3D(getFirstAndOnly(tmpElem, "ECF"), geoData->scp.ecf);
    common().parseLatLonAlt(getFirstAndOnly(tmpElem, "LLH"), geoData->scp.llh);

    common().parseFootprint(getFirstAndOnly(geoDataXML, "ImageCorners"), "ICP",
                   geoData->imageCorners);

    tmpElem = getOptional(geoDataXML, "ValidData");
    if (tmpElem != NULL)
    {
        common().parseLatLons(tmpElem, "Vertex", geoData->validData);
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
        parseGeoInfoFromXML(*it, geoData->geoInfos[idx].get());
    }

}

void ComplexXMLParser::parseGeoInfoFromXML(const XMLElem geoInfoXML, GeoInfo* geoInfo) const
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
        parseGeoInfoFromXML(*it, geoInfo->geoInfos[idx].get());
    }

    //optional
    common().parseParameters(geoInfoXML, "Desc", geoInfo->desc);

    XMLElem tmpElem = getOptional(geoInfoXML, "Point");
    if (tmpElem)
    {
        LatLon ll;
        common().parseLatLon(tmpElem, ll);
        geoInfo->geometryLatLon.push_back(ll);
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
            common().parseLatLons(tmpElem, pointName, geoInfo->geometryLatLon);
        }
    }
}

void ComplexXMLParser::parseGridFromXML(const XMLElem gridXML, Grid *grid) const
{
    grid->imagePlane = six::toType<ComplexImagePlaneType>(
        getFirstAndOnly(gridXML, "ImagePlane")->getCharacterData());
    grid->type = six::toType<ComplexImageGridType>(
        getFirstAndOnly(gridXML, "Type")->getCharacterData());

    XMLElem tmpElem = getFirstAndOnly(gridXML, "TimeCOAPoly");
    common().parsePoly2D(tmpElem, grid->timeCOAPoly);

    tmpElem = getFirstAndOnly(gridXML, "Row");
    common().parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->row->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->row->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
                grid->row->impulseResponseWidth);
    grid->row->sign = six::toType<six::FFTSign>(
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
        common().parsePoly2D(optElem, grid->row->deltaKCOAPoly);
    }

    parseWeightTypeFromXML(tmpElem, grid->row->weightType);

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
    common().parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->col->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->col->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
                grid->col->impulseResponseWidth);
    grid->col->sign = six::toType<six::FFTSign>(
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
        common().parsePoly2D(optElem, grid->col->deltaKCOAPoly);
    }

    parseWeightTypeFromXML(tmpElem, grid->col->weightType);

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

void ComplexXMLParser::parseTimelineFromXML(
    const XMLElem timelineXML,
    Timeline *timeline) const
{
    parseDateTime(getFirstAndOnly(timelineXML, "CollectStart"),
                  timeline->collectStart);
    parseDouble(getFirstAndOnly(timelineXML, "CollectDuration"),
                timeline->collectDuration);

    XMLElem ippXML = getOptional(timelineXML, "IPP");
    if (ippXML)
    {
        timeline->interPulsePeriod.reset(new InterPulsePeriod());
        //TODO make sure there is at least one
        std::vector < XMLElem > setsXML;
        ippXML->getElementsByTagName("Set", setsXML);
        for (std::vector<XMLElem>::const_iterator it = setsXML.begin(); it
                != setsXML.end(); ++it)
        {
            // Use the first set that is already available.
            timeline->interPulsePeriod->sets.resize(
                    timeline->interPulsePeriod->sets.size() + 1);
            TimelineSet& ts(timeline->interPulsePeriod->sets.back());

            parseDouble(getFirstAndOnly(*it, "TStart"), ts.tStart);
            parseDouble(getFirstAndOnly(*it, "TEnd"), ts.tEnd);
            parseInt(getFirstAndOnly(*it, "IPPStart"),
                     ts.interPulsePeriodStart);
            parseInt(getFirstAndOnly(*it, "IPPEnd"), ts.interPulsePeriodEnd);
            common().parsePoly1D(getFirstAndOnly(*it, "IPPPoly"),
                        ts.interPulsePeriodPoly);
        }

        // Required to have at least one timeline set.
        // TODO: Does it really make sense to do this?
        if (timeline->interPulsePeriod->sets.empty())
        {
            timeline->interPulsePeriod->sets.resize(1);
        }
    }
}

void ComplexXMLParser::parsePositionFromXML(
    const XMLElem positionXML,
    Position *position) const
{
    XMLElem tmpElem = getFirstAndOnly(positionXML, "ARPPoly");
    common().parsePolyXYZ(tmpElem, position->arpPoly);

    tmpElem = getOptional(positionXML, "GRPPoly");
    if (tmpElem)
    {
        //optional
        common().parsePolyXYZ(tmpElem, position->grpPoly);
    }

    tmpElem = getOptional(positionXML, "TxAPCPoly");
    if (tmpElem)
    {
        //optional
        common().parsePolyXYZ(tmpElem, position->txAPCPoly);
    }

    tmpElem = getOptional(positionXML, "RcvAPC");
    if (tmpElem)
    {
        //optional
        position->rcvAPC.reset(new RcvAPC());

        //TODO make sure there is at least one
        std::vector < XMLElem > polysXML;
        tmpElem->getElementsByTagName("RcvAPCPoly", polysXML);
        for (std::vector<XMLElem>::const_iterator it = polysXML.begin(); it
                != polysXML.end(); ++it)
        {
            PolyXYZ p;
            common().parsePolyXYZ(*it, p);
            position->rcvAPC->rcvAPCPolys.push_back(p);
        }
    }
}

void ComplexXMLParser::parseImageFormationFromXML(
    const XMLElem imageFormationXML,
    const RadarCollection& radarCollection,
    ImageFormation *imageFormation) const
{
    XMLElem tmpElem = getOptional(imageFormationXML, "SegmentIdentifier");
    if (radarCollection.area.get() != NULL &&
        radarCollection.area->plane.get() != NULL &&
        !radarCollection.area->plane->segmentList.empty() &&
        !tmpElem)
    {
        throw except::Exception(Ctxt(
            "ImageFormation.SegmentIdentifier must be included when a "
            "RadarCollection.Area.Plane.SegmentList is included."));
    }

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

    //! version specific TxRcvPolarization
    parseTxRcvPolFromXML(imageFormationXML,
                         imageFormation->txRcvPolarizationProc);

    imageFormation->imageFormationAlgorithm = six::toType<ImageFormationType>(
        getFirstAndOnly(imageFormationXML,
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

    imageFormation->slowTimeBeamCompensation =
        six::toType<SlowTimeBeamCompensationType>(getFirstAndOnly(
            imageFormationXML, "STBeamComp")->getCharacterData());

    imageFormation->imageBeamCompensation =
        six::toType<ImageBeamCompensationType>(getFirstAndOnly(
            imageFormationXML, "ImageBeamComp")->getCharacterData());

    imageFormation->azimuthAutofocus = six::toType<AutofocusType>(
        getFirstAndOnly(imageFormationXML,
                        "AzAutofocus")->getCharacterData());

    imageFormation->rangeAutofocus = six::toType<AutofocusType>(
        getFirstAndOnly(imageFormationXML,
                        "RgAutofocus")->getCharacterData());

    std::vector < XMLElem > procXML;
    imageFormationXML->getElementsByTagName("Processing", procXML);

    for (unsigned int i = 0; i < procXML.size(); ++i)
    {
        Processing* proc = new Processing();

        parseString(getFirstAndOnly(procXML[i], "Type"), proc->type);
        parseBooleanType(getFirstAndOnly(procXML[i], "Applied"), proc->applied);
        common().parseParameters(procXML[i], "Parameter", proc->parameters);

        imageFormation->processing.push_back(*proc);
    }

    XMLElem polCalXML = getOptional(imageFormationXML,
                                    "PolarizationCalibration");
    if (polCalXML)
    {
        //optional
        imageFormation->polarizationCalibration.reset(new PolarizationCalibration());
        imageFormation->polarizationCalibration->distortion.reset(new Distortion());

        // parses HVAngleCompApplied and DistortionCorrectionApplied
        parsePolarizationCalibrationFromXML(
            polCalXML, imageFormation->polarizationCalibration.get());

        XMLElem distortionXML = getFirstAndOnly(polCalXML, "Distortion");

        XMLElem calibDateXML = getOptional(distortionXML, "CalibrationDate");
        if (calibDateXML)
        {
            parseDateTime(calibDateXML, imageFormation->
                polarizationCalibration->distortion->calibrationDate);
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
            parseDouble(gainErrorXML, imageFormation->
                polarizationCalibration ->distortion->gainErrorA);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF1");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->
                polarizationCalibration->distortion->gainErrorF1);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF2");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->
                polarizationCalibration ->distortion->gainErrorF2);
        }

        XMLElem phaseErrorXML = getOptional(distortionXML, "PhaseErrorF1");
        if (phaseErrorXML)
        {
            parseDouble(phaseErrorXML, imageFormation->
                polarizationCalibration ->distortion->phaseErrorF1);
        }

        phaseErrorXML = getOptional(distortionXML, "PhaseErrorF2");
        if (phaseErrorXML)
        {
            parseDouble(phaseErrorXML, imageFormation->
                polarizationCalibration ->distortion->phaseErrorF2);
        }

    }
}

void ComplexXMLParser::parseSCPCOAFromXML(
    const XMLElem scpcoaXML,
    SCPCOA *scpcoa) const
{
    parseDouble(getFirstAndOnly(scpcoaXML, "SCPTime"), scpcoa->scpTime);

    common().parseVector3D(getFirstAndOnly(scpcoaXML, "ARPPos"), scpcoa->arpPos);
    common().parseVector3D(getFirstAndOnly(scpcoaXML, "ARPVel"), scpcoa->arpVel);
    common().parseVector3D(getFirstAndOnly(scpcoaXML, "ARPAcc"), scpcoa->arpAcc);

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

void ComplexXMLParser::parseAntennaParametersFromXML(
    const XMLElem antennaParamsXML,
    AntennaParameters* params) const
{
    common().parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "XAxisPoly"),
                 params->xAxisPoly);
    common().parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "YAxisPoly"),
                 params->yAxisPoly);
    parseDouble(getFirstAndOnly(antennaParamsXML, "FreqZero"),
                params->frequencyZero);

    XMLElem tmpElem = getOptional(antennaParamsXML, "EB");
    if (tmpElem)
    {
        params->electricalBoresight.reset(new ElectricalBoresight());
        common().parsePoly1D(getFirstAndOnly(tmpElem, "DCXPoly"),
                    params->electricalBoresight->dcxPoly);
        common().parsePoly1D(getFirstAndOnly(tmpElem, "DCYPoly"),
                    params->electricalBoresight->dcyPoly);
    }

    //! this field was deprecated in 1.0.0, but we can do this
    //  since it was previously optional
    tmpElem = getOptional(antennaParamsXML, "HPBW");
    if (tmpElem)
    {
        params->halfPowerBeamwidths.reset(new HalfPowerBeamwidths());
        parseDouble(getFirstAndOnly(tmpElem, "DCX"),
                    params->halfPowerBeamwidths->dcx);
        parseDouble(getFirstAndOnly(tmpElem, "DCY"),
                    params->halfPowerBeamwidths->dcy);
    }

    //! this field turned manditory in 1.0.0
    parseAntennaParamArrayFromXML(antennaParamsXML, params);

    tmpElem = getOptional(antennaParamsXML, "Elem");
    if (tmpElem)
    {
        params->element.reset(new GainAndPhasePolys());
        common().parsePoly2D(getFirstAndOnly(tmpElem, "GainPoly"),
                    params->element->gainPoly);
        common().parsePoly2D(getFirstAndOnly(tmpElem, "PhasePoly"),
                    params->element->phasePoly);
    }

    tmpElem = getOptional(antennaParamsXML, "GainBSPoly");
    if (tmpElem)
    {
        //optional
        common().parsePoly1D(tmpElem, params->gainBSPoly);
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

void ComplexXMLParser::parseAntennaFromXML(
    const XMLElem antennaXML,
    Antenna *antenna) const
{
    XMLElem antennaParamsXML = getOptional(antennaXML, "Tx");
    if (antennaParamsXML)
    {
        antenna->tx.reset(new AntennaParameters());
        parseAntennaParametersFromXML(antennaParamsXML, antenna->tx.get());
    }

    antennaParamsXML = getOptional(antennaXML, "Rcv");
    if (antennaParamsXML)
    {
        antenna->rcv.reset(new AntennaParameters());
        parseAntennaParametersFromXML(antennaParamsXML, antenna->rcv.get());
    }

    antennaParamsXML = getOptional(antennaXML, "TwoWay");
    if (antennaParamsXML)
    {
        antenna->twoWay.reset(new AntennaParameters());
        parseAntennaParametersFromXML(antennaParamsXML, antenna->twoWay.get());
    }
}

void ComplexXMLParser::parsePFAFromXML(
    const XMLElem pfaXML,
    PFA *pfa) const
{
    common().parseVector3D(getFirstAndOnly(pfaXML, "FPN"), pfa->focusPlaneNormal);
    common().parseVector3D(getFirstAndOnly(pfaXML, "IPN"), pfa->imagePlaneNormal);
    parseDouble(getFirstAndOnly(pfaXML, "PolarAngRefTime"),
                pfa->polarAngleRefTime);
    common().parsePoly1D(getFirstAndOnly(pfaXML, "PolarAngPoly"), pfa->polarAnglePoly);
    common().parsePoly1D(getFirstAndOnly(pfaXML, "SpatialFreqSFPoly"),
                pfa->spatialFrequencyScaleFactorPoly);
    parseDouble(getFirstAndOnly(pfaXML, "Krg1"), pfa->krg1);
    parseDouble(getFirstAndOnly(pfaXML, "Krg2"), pfa->krg2);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz1"), pfa->kaz1);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz2"), pfa->kaz2);

    XMLElem deskewXML = getOptional(pfaXML, "STDeskew");
    if (deskewXML)
    {
        pfa->slowTimeDeskew.reset(new SlowTimeDeskew());
        parseBooleanType(getFirstAndOnly(deskewXML, "Applied"),
                         pfa->slowTimeDeskew->applied);

        common().parsePoly2D(getFirstAndOnly(deskewXML, "STDSPhasePoly"),
                    pfa->slowTimeDeskew->slowTimeDeskewPhasePoly);
    }
}

void ComplexXMLParser::parseRMAFromXML(
    const XMLElem rmaXML,
    RMA* rma) const
{
    rma->algoType = six::toType<RMAlgoType>(
        getFirstAndOnly(rmaXML, "RMAlgoType")->getCharacterData());

    XMLElem rmatElem = getOptional(rmaXML, "RMAT");
    if (rmatElem)
    {
        rma->rmat.reset(new RMAT());
        parseRMATFromXML(rmatElem, rma->rmat.get());
    }

    XMLElem rmcrElem = getOptional(rmaXML, "RMCR");
    if (rmcrElem)
    {
        rma->rmcr.reset(new RMCR());
        parseRMCRFromXML(rmcrElem, rma->rmcr.get());
    }

    XMLElem incaElem = getOptional(rmaXML, "INCA");
    if (incaElem)
    {
        rma->inca.reset(new INCA());
        parseINCAFromXML(incaElem, rma->inca.get());
    }
}

void ComplexXMLParser::parseINCAFromXML(
    const XMLElem incaElem, INCA* inca) const
{
    common().parsePoly1D(getFirstAndOnly(incaElem, "TimeCAPoly"), inca->timeCAPoly);
    parseDouble(getFirstAndOnly(incaElem, "R_CA_SCP"), inca->rangeCA);
    parseDouble(getFirstAndOnly(incaElem, "FreqZero"), inca->freqZero);

    parseDRateSFPolyFromXML(incaElem, inca);

    XMLElem tmpElem = getOptional(incaElem, "DopCentroidPoly");
    if (tmpElem)
    {
        common().parsePoly2D(tmpElem, inca->dopplerCentroidPoly);
    }

    tmpElem = getOptional(incaElem, "DopCentroidCOA");
    if (tmpElem)
    {
        parseBooleanType(tmpElem, inca->dopplerCentroidCOA);
    }
}

void ComplexXMLParser::parseRgAzCompFromXML(
    const XMLElem rgAzCompXML,
    RgAzComp* rgAzComp) const
{
    parseDouble(getFirstAndOnly(rgAzCompXML, "AzSF"), rgAzComp->azSF);
    common().parsePoly1D(getFirstAndOnly(rgAzCompXML, "KazPoly"), rgAzComp->kazPoly);
}

void ComplexXMLParser::parseWaveformFromXML(
    const XMLElem waveformXML,
    std::vector<mem::ScopedCloneablePtr<WaveformParameters> >& waveform) const
{
    std::vector<XMLElem> wfParamsXML;
    waveformXML->getElementsByTagName("WFParameters", wfParamsXML);
    if (wfParamsXML.empty())
    {
        throw except::Exception(Ctxt(
                "Expected at least one WFParameters element"));
    }

    for (std::vector<XMLElem>::const_iterator it = wfParamsXML.begin(); it
            != wfParamsXML.end(); ++it)
    {
        waveform.resize(waveform.size() + 1);
        waveform.back().reset(new WaveformParameters());
        WaveformParameters* const wfParams = waveform.back().get();

        XMLElem optElem = getOptional(*it, "TxPulseLength");
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

void ComplexXMLParser::parseAreaFromXML(
        const XMLElem areaXML,
        bool cornersRequired,
        bool planeOrientationRequired,
        mem::ScopedCloneablePtr<Area>& area) const
{
    area.reset(new Area());

    XMLElem optElem = getOptional(areaXML, "Corner");
    if (optElem)
    {
        //optional
        common().parseFootprint(optElem, "ACP", area->acpCorners);
    }
    else if (cornersRequired)
    {
        throw except::Exception(Ctxt("Corner element required"));
    }

    XMLElem planeXML = getOptional(areaXML, "Plane");
    if (planeXML)
    {
        //optional
        area->plane.reset(new AreaPlane());

        XMLElem refPtXML = getFirstAndOnly(planeXML, "RefPt");
        try
        {
            area->plane->referencePoint.name
                    = refPtXML->getAttributes().getValue("name");
        }
        catch (const except::Exception& /*ex*/)
        {
        }

        common().parseVector3D(getFirstAndOnly(refPtXML, "ECF"),
                      area->plane->referencePoint.ecef);
        parseDouble(getFirstAndOnly(refPtXML, "Line"),
                    area->plane->referencePoint.rowCol.row);
        parseDouble(getFirstAndOnly(refPtXML, "Sample"),
                    area->plane->referencePoint.rowCol.col);

        XMLElem dirXML = getFirstAndOnly(planeXML, "XDir");
        common().parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                      area->plane->xDirection->unitVector);
        parseDouble(getFirstAndOnly(dirXML, "LineSpacing"),
                    area->plane->xDirection->spacing);
        parseUInt(getFirstAndOnly(dirXML, "NumLines"),
                  area->plane->xDirection->elements);
        parseUInt(getFirstAndOnly(dirXML, "FirstLine"),
                  area->plane->xDirection->first);

        dirXML = getFirstAndOnly(planeXML, "YDir");
        common().parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                      area->plane->yDirection->unitVector);
        parseDouble(getFirstAndOnly(dirXML, "SampleSpacing"),
                    area->plane->yDirection->spacing);
        parseUInt(getFirstAndOnly(dirXML, "NumSamples"),
                  area->plane->yDirection->elements);
        parseUInt(getFirstAndOnly(dirXML, "FirstSample"),
                  area->plane->yDirection->first);

        XMLElem segmentListXML = getOptional(planeXML, "SegmentList");
        if (segmentListXML != NULL)
        {
            std::vector<XMLElem> segmentsXML;
            segmentListXML->getElementsByTagName("Segment", segmentsXML);
            if (segmentsXML.empty())
            {
                throw except::Exception(Ctxt(
                        "Expected at least one Segment element"));
            }

            for (std::vector<XMLElem>::const_iterator it =
                    segmentsXML.begin(); it != segmentsXML.end(); ++it)
            {
                area->plane->segmentList.resize(
                    area->plane->segmentList.size() + 1);
                area->plane->segmentList.back().reset(
                    new Segment());
                Segment* const seg =
                    area->plane->segmentList.back().get();

                parseInt(getFirstAndOnly(*it, "StartLine"), seg->startLine);
                parseInt(getFirstAndOnly(*it, "StartSample"),
                         seg->startSample);
                parseInt(getFirstAndOnly(*it, "EndLine"), seg->endLine);
                parseInt(getFirstAndOnly(*it, "EndSample"), seg->endSample);
                parseString(getFirstAndOnly(*it, "Identifier"),
                            seg->identifier);
            }
        }

        XMLElem orientation = getOptional(planeXML, "Orientation");
        if (orientation)
        {
            area->plane->orientation = six::toType<OrientationType>(
                    orientation->getCharacterData());
        }
        else if (planeOrientationRequired)
        {
            throw except::Exception(Ctxt("Orientation element required"));
        }
    }
}

void ComplexXMLParser::parseTxSequenceFromXML(
        const XMLElem txSequenceXML,
        std::vector<mem::ScopedCloneablePtr<TxStep> >& steps) const
{
    std::vector<XMLElem> txStepsXML;
    txSequenceXML->getElementsByTagName("TxStep", txStepsXML);
    if (txStepsXML.empty())
    {
        throw except::Exception(Ctxt(
                "Expected at least one TxStep element"));
    }

    for (std::vector<XMLElem>::const_iterator it = txStepsXML.begin(); it
            != txStepsXML.end(); ++it)
    {
        steps.resize(steps.size() + 1);
        mem::ScopedCloneablePtr<TxStep>& step(steps.back());
        step.reset(new TxStep());

        XMLElem optElem = getOptional(*it, "WFIndex");
        if (optElem)
        {
            //optional
            parseInt(optElem, step->waveformIndex);
        }

        optElem = getOptional(*it, "TxPolarization");
        if (optElem)
        {
            //optional
            step->txPolarization = six::toType<PolarizationType>(
                    optElem->getCharacterData());
        }
    }
}

XMLElem ComplexXMLParser::createLatLonFootprint(const std::string& name,
                                                const std::string& cornerName,
                                                const LatLonCorners& corners,
                                                XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex = common().createLatLon(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1:FRFC");

    vertex = common().createLatLon(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2:FRLC");

    vertex = common().createLatLon(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3:LRLC");

    vertex = common().createLatLon(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4:LRFC");

    return footprint;
}

XMLElem ComplexXMLParser::createLatLonAltFootprint(const std::string& name,
                                                   const std::string& cornerName,
                                                   const LatLonAltCorners& corners,
                                                   XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex =
        common().createLatLonAlt(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1");

    vertex = common().createLatLonAlt(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2");

    vertex = common().createLatLonAlt(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3");

    vertex = common().createLatLonAlt(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4");

    return footprint;
}


XMLElem ComplexXMLParser::createEarthModelType(const std::string& name,
                                               const EarthModelType& value,
                                               XMLElem parent) const
{
    return createString(name, six::toString(value), parent);
}

XMLElem ComplexXMLParser::createSideOfTrackType(const std::string& name,
                                                const SideOfTrackType& value,
                                                XMLElem parent) const
{
    return createString(name, six::toString(value), parent);
}

void ComplexXMLParser::parseEarthModelType(XMLElem element,
                                           EarthModelType& value) const
{
    value = six::toType<EarthModelType>(element->getCharacterData());
}

void ComplexXMLParser::parseSideOfTrackType(XMLElem element,
                                            SideOfTrackType& value) const
{
    value = six::toType<SideOfTrackType>(element->getCharacterData());
}

}
}

