/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

Data* ComplexXMLControl::fromXML(xml::lite::Document* doc)
{
    ComplexDataBuilder builder;
    ComplexData *sicd = builder.steal();

    xml::lite::Element *root = doc->getRootElement();

    xml::lite::Element *collectionInfoXML = getFirstAndOnly(root,
            "CollectionInfo");

    xml::lite::Element *imageCreationXML = NULL;

    imageCreationXML = getOptional(root, "ImageCreation");

    xml::lite::Element *imageDataXML = getFirstAndOnly(root, "ImageData");
    xml::lite::Element *geoDataXML = getFirstAndOnly(root, "GeoData");
    xml::lite::Element *gridXML = getFirstAndOnly(root, "Grid");
    xml::lite::Element *timelineXML = getFirstAndOnly(root, "Timeline");
    xml::lite::Element *positionXML = getFirstAndOnly(root, "Position");
    xml::lite::Element *radarCollectionXML = getFirstAndOnly(root,
            "RadarCollection");
    xml::lite::Element *imageFormationXML = getFirstAndOnly(root,
            "ImageFormation");
    xml::lite::Element *scpcoaXML = getFirstAndOnly(root, "SCPCOA");

    xml::lite::Element *radiometricXML = NULL;
    radiometricXML = getOptional(root, "Radiometric");

    xml::lite::Element *antennaXML = getOptional(root, "Antenna");

    xml::lite::Element *errorStatisticsXML = getOptional(root,
            "ErrorStatistics");

    xml::lite::Element *matchInfoXML = getOptional(root, "MatchInfo");
    xml::lite::Element *pfaXML = getOptional(root, "PFA");

    xmlToCollectionInfo(collectionInfoXML, sicd->collectionInformation);

    if (imageCreationXML != NULL)
    {
        builder.addImageCreation();
        xmlToImageCreation(imageCreationXML, sicd->imageCreation);
    }

    xmlToImageData(imageDataXML, sicd->imageData);
    xmlToGeoData(geoDataXML, sicd->geoData);
    xmlToGrid(gridXML, sicd->grid);
    xmlToTimeline(timelineXML, sicd->timeline);
    xmlToPosition(positionXML, sicd->position);
    xmlToRadarCollection(radarCollectionXML, sicd->radarCollection);
    xmlToImageFormation(imageFormationXML, sicd->imageFormation);
    xmlToSCPCOA(scpcoaXML, sicd->scpcoa);

    if (radiometricXML != NULL)
    {
        builder.addRadiometric();
        xmlToRadiometric(radiometricXML, sicd->radiometric);
    }

    if (antennaXML != NULL)
    {
        builder.addAntenna();
        xmlToAntenna(antennaXML, sicd->antenna);
    }

    if (errorStatisticsXML != NULL)
    {
        builder.addErrorStatistics();
        xmlToErrorStatistics(errorStatisticsXML, sicd->errorStatistics);
    }

    if (matchInfoXML != NULL)
    {
        builder.addMatchInformation();
        xmlToMatchInfo(matchInfoXML, sicd->matchInformation);
    }

    if (pfaXML != NULL)
    {
        sicd->pfa = new PFA();
        xmlToPFA(pfaXML, sicd->pfa);
    }
    return sicd;
}

xml::lite::Document* ComplexXMLControl::toXML(Data *data)
{
    if (data->getDataClass() != DATA_COMPLEX)
    {
        throw except::Exception("Data must be SICD");
    }

    xml::lite::Document* doc = new xml::lite::Document();

    xml::lite::Element* root = newElement(doc, "SICD");
    doc->setRootElement(root);

    ComplexData *sicd = (ComplexData*) data;

    doc->insert(collectionInfoToXML(doc, sicd->collectionInformation), root);
    if (sicd->imageCreation)
    {
        doc->insert(imageCreationToXML(doc, sicd->imageCreation), root);
    }
    doc->insert(imageDataToXML(doc, sicd->imageData), root);
    doc->insert(geoDataToXML(doc, sicd->geoData), root);
    doc->insert(gridToXML(doc, sicd->grid), root);
    doc->insert(timelineToXML(doc, sicd->timeline), root);
    doc->insert(positionToXML(doc, sicd->position), root);
    doc->insert(radarCollectionToXML(doc, sicd->radarCollection), root);
    doc->insert(imageFormationToXML(doc, sicd->imageFormation), root);
    doc->insert(scpcoaToXML(doc, sicd->scpcoa), root);
    if (sicd->radiometric)
        doc->insert(radiometricToXML(doc, sicd->radiometric), root);
    if (sicd->antenna)
        doc->insert(antennaToXML(doc, sicd->antenna), root);
    if (sicd->errorStatistics)
        doc->insert(errorStatisticsToXML(doc, sicd->errorStatistics), root);
    if (sicd->matchInformation && !sicd->matchInformation->collects.empty())
        doc->insert(matchInfoToXML(doc, sicd->matchInformation), root);
    // Hack for now
    if (sicd->pfa)
        doc->insert(pfaToXML(doc, sicd->pfa), root);

    return doc;
}

xml::lite::Element* ComplexXMLControl::createFFTSign(xml::lite::Document* doc,
                                                     std::string name, six::FFTSign sign)
{
    std::string charData = (sign == FFT_SIGN_NEG) ? ("-1") : ("+1");
    xml::lite::Element* e = doc->createElement(name, mURI, charData);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:int");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* ComplexXMLControl::collectionInfoToXML(
        xml::lite::Document* doc, CollectionInformation *collInfo)
{
    xml::lite::Element* collInfoXML = newElement(doc, "CollectionInfo");

    collInfoXML->addChild(createString(doc, "CollectorName",
            collInfo->collectorName));
    if (!collInfo->illuminatorName.empty())
        collInfoXML->addChild(createString(doc, "IlluminatorName",
                collInfo->illuminatorName));
    collInfoXML->addChild(createString(doc, "CoreName", collInfo->coreName));
    if (!Init::isUndefined<CollectType>(collInfo->collectType))
        collInfoXML->addChild(createString(doc, "CollectType", str::toString<
                six::CollectType>(collInfo->collectType)));

    xml::lite::Element* radarModeXML = newElement(doc, "RadarMode");
    collInfoXML->addChild(radarModeXML);
    radarModeXML->addChild(createString(doc, "ModeType", str::toString(
            collInfo->radarMode)));
    if (!collInfo->radarModeID.empty())
        radarModeXML->addChild(createString(doc, "ModeID",
                collInfo->radarModeID));

    //TODO maybe add more class. info in the future
    collInfoXML->addChild(createString(doc, "Classification",
            collInfo->classification.level));

    for (std::vector<std::string>::iterator it = collInfo->countryCodes.begin(); it
            != collInfo->countryCodes.end(); ++it)
    {
        collInfoXML->addChild(createString(doc, "CountryCode", *it));
    }
    addParameters(doc, collInfoXML, "Parameter", collInfo->parameters);
    return collInfoXML;
}

xml::lite::Element* ComplexXMLControl::imageCreationToXML(
        xml::lite::Document* doc, ImageCreation *imageCreation)
{
    xml::lite::Element* imageCreationXML = newElement(doc, "ImageCreation");

    if (!imageCreation->application.empty())
        imageCreationXML->addChild(createString(doc, "Application",
                imageCreation->application));
    if (!Init::isUndefined<DateTime>(imageCreation->dateTime))
        imageCreationXML->addChild(createDateTime(doc, "DateTime",
                imageCreation->dateTime));
    if (!imageCreation->site.empty())
        imageCreationXML->addChild(createString(doc, "Site",
                imageCreation->site));
    if (!imageCreation->profile.empty())
        imageCreationXML->addChild(createString(doc, "Profile",
                imageCreation->profile));
    return imageCreationXML;
}

xml::lite::Element * ComplexXMLControl::imageDataToXML(
        xml::lite::Document* doc, ImageData *imageData)
{
    xml::lite::Element* imageDataXML = newElement(doc, "ImageData");

    imageDataXML->addChild(createString(doc, "PixelType", str::toString(
            imageData->pixelType)));
    if (imageData->amplitudeTable)
    {
        //TODO AmpTable
    }
    imageDataXML->addChild(createInt(doc, "NumRows", imageData->numRows));
    imageDataXML->addChild(createInt(doc, "NumCols", imageData->numCols));
    imageDataXML->addChild(createInt(doc, "FirstRow", imageData->firstRow));
    imageDataXML->addChild(createInt(doc, "FirstCol", imageData->firstCol));

    imageDataXML->addChild(createRowCol(doc, "FullImage", "NumRows", "NumCols",
            imageData->fullImage));
    imageDataXML->addChild(createRowCol(doc, "SCPPixel", imageData->scpPixel));

    //only if 3+ vertices
    unsigned int numVertices = imageData->validData.size();
    if (numVertices >= 3)
    {
        xml::lite::Element *vXML = newElement(doc, "ValidData");
        imageDataXML->addChild(vXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            xml::lite::Element *vertexXML = createRowCol(doc, "Vertex",
                    imageData->validData[i]);
            vXML->addChild(vertexXML);
            setAttribute(vertexXML, "index", str::toString(i));
        }
    }
    return imageDataXML;
}

xml::lite::Element* ComplexXMLControl::geoDataToXML(xml::lite::Document* doc,
        GeoData *geoData)
{
    xml::lite::Element* geoDataXML = newElement(doc, "GeoData");

    geoDataXML->addChild(createEarthModelType(doc, "EarthModel",
            geoData->earthModel));

    xml::lite::Element* scpXML = newElement(doc, "SCP");
    geoDataXML->addChild(scpXML);
    scpXML->addChild(createVector3D(doc, "ECF", geoData->scp.ecf));

    scpXML->addChild(createLatLonAlt(doc, "LLH", geoData->scp.llh));

    //createFootprint
    geoDataXML->addChild(createFootprint(doc, "ImageCorners", "ICP",
            geoData->imageCorners));

    //only if 3+ vertices
    unsigned int numVertices = geoData->validData.size();
    if (numVertices >= 3)
    {
        xml::lite::Element *vXML = newElement(doc, "ValidData");
        geoDataXML->addChild(vXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            xml::lite::Element *vertexXML = createLatLon(doc, "Vertex",
                    geoData->validData[i]);
            vXML->addChild(vertexXML);
            setAttribute(vertexXML, "index", str::toString(i));
        }
    }

    for (std::vector<GeoInfo*>::iterator it = geoData->geoInfos.begin(); it
            != geoData->geoInfos.end(); ++it)
    {
        geoDataXML->addChild(geoInfoToXML(doc, *it));
    }

    return geoDataXML;
}

xml::lite::Element* ComplexXMLControl::geoInfoToXML(xml::lite::Document* doc,
        GeoInfo *geoInfo)
{
    xml::lite::Element* geoInfoXML = newElement(doc, "GeoInfo");
    if (!geoInfo->name.empty())
        setAttribute(geoInfoXML, "name", geoInfo->name);

    for (std::vector<GeoInfo*>::iterator it = geoInfo->geoInfos.begin(); it
            != geoInfo->geoInfos.end(); ++it)
    {
        geoInfoXML->addChild(geoInfoToXML(doc, *it));
    }

    addParameters(doc, geoInfoXML, "Desc", geoInfo->desc);

    size_t numLatLons = geoInfo->geometryLatLon.size();
    if (numLatLons == 1)
    {
        geoInfoXML->addChild(createLatLon(doc, "Point",
                geoInfo->geometryLatLon[0]));
    }
    else if (numLatLons >= 2)
    {
        xml::lite::Element* linePolyXML = newElement(doc,
                numLatLons == 2 ? "Line" : "Polygon");
        geoInfoXML->addChild(linePolyXML);

        for (int i = 0; i < numLatLons; ++i)
        {
            linePolyXML->addChild(createLatLon(doc, numLatLons == 2
                    ? "Endpoint" : "Vertex", geoInfo->geometryLatLon[i]));
        }
    }
    return geoInfoXML;
}

xml::lite::Element* ComplexXMLControl::gridToXML(xml::lite::Document* doc,
        Grid *grid)
{
    xml::lite::Element* gridXML = newElement(doc, "Grid");

    gridXML->addChild(createString(doc, "ImagePlane", str::toString(
            grid->imagePlane)));
    gridXML->addChild(createString(doc, "Type", str::toString(grid->type)));
    gridXML->addChild(createPoly2D(doc, "TimeCOAPoly", grid->timeCOAPoly));

    xml::lite::Element* rowDirXML = newElement(doc, "Row");
    gridXML->addChild(rowDirXML);

    rowDirXML->addChild(createVector3D(doc, "UVectECF", grid->row->unitVector));
    rowDirXML->addChild(createDouble(doc, "SS", grid->row->sampleSpacing));
    rowDirXML->addChild(createDouble(doc, "ImpRespWid",
            grid->row->impulseResponseWidth));
    rowDirXML->addChild(createFFTSign(doc, "Sgn", grid->row->sign));
    rowDirXML->addChild(createDouble(doc, "ImpRespBW",
            grid->row->impulseResponseBandwidth));
    rowDirXML->addChild(createDouble(doc, "KCtr", grid->row->kCenter));
    rowDirXML->addChild(createDouble(doc, "DeltaK1", grid->row->deltaK1));
    rowDirXML->addChild(createDouble(doc, "DeltaK2", grid->row->deltaK2));

    if (grid->row->deltaKCOAPoly.orderX() >= 0
            && grid->row->deltaKCOAPoly.orderY() >= 0)
    {
        rowDirXML->addChild(createPoly2D(doc, "DeltaKCOAPoly",
                grid->row->deltaKCOAPoly));
    }

    if (!Init::isUndefined<std::string>(grid->row->weightType))
    {
        rowDirXML->addChild(createString(doc, "WgtType", grid->row->weightType));
    }

    if (grid->row->weights.size() > 0)
    {
        xml::lite::Element* wgtFuncXML = newElement(doc, "WgtFunc");

        for (std::vector<double>::iterator it = grid->row->weights.begin(); it
                != grid->row->weights.end(); ++it)
        {
            wgtFuncXML->addChild(createDouble(doc, "Wgt", *it));
        }
    }

    xml::lite::Element* colDirXML = newElement(doc, "Col");
    gridXML->addChild(colDirXML);

    colDirXML->addChild(createVector3D(doc, "UVectECF", grid->col->unitVector));
    colDirXML->addChild(createDouble(doc, "SS", grid->col->sampleSpacing));
    colDirXML->addChild(createDouble(doc, "ImpRespWid",
            grid->col->impulseResponseWidth));
    colDirXML->addChild(createFFTSign(doc, "Sgn", grid->col->sign));
    colDirXML->addChild(createDouble(doc, "ImpRespBW",
            grid->col->impulseResponseBandwidth));
    colDirXML->addChild(createDouble(doc, "KCtr", grid->col->kCenter));
    colDirXML->addChild(createDouble(doc, "DeltaK1", grid->col->deltaK1));
    colDirXML->addChild(createDouble(doc, "DeltaK2", grid->col->deltaK2));

    if (grid->col->deltaKCOAPoly.orderX() >= 0
            && grid->col->deltaKCOAPoly.orderY() >= 0)
    {
        colDirXML->addChild(createPoly2D(doc, "DeltaKCOAPoly",
                grid->col->deltaKCOAPoly));
    }

    if (!Init::isUndefined<std::string>(grid->col->weightType))
    {
        colDirXML->addChild(createString(doc, "WgtType", grid->col->weightType));
    }

    if (grid->col->weights.size() > 0)
    {
        xml::lite::Element* wgtFuncXML = newElement(doc, "WgtFunc");

        for (std::vector<double>::iterator it = grid->col->weights.begin(); it
                != grid->col->weights.end(); ++it)
        {
            wgtFuncXML->addChild(createDouble(doc, "Wgt", *it));
        }
    }

    return gridXML;
}

xml::lite::Element* ComplexXMLControl::timelineToXML(xml::lite::Document* doc,
        Timeline *timeline)
{
    xml::lite::Element* timelineXML = newElement(doc, "Timeline");

    timelineXML->addChild(createDateTime(doc, "CollectStart",
            timeline->collectStart));
    timelineXML->addChild(createDouble(doc, "CollectDuration",
            timeline->collectDuration));

    if (timeline->interPulsePeriod)
    {
        xml::lite::Element* ippXML = newElement(doc, "IPP");
        unsigned int setSize = timeline->interPulsePeriod->sets.size();
        ippXML->attribute("size") = str::toString<int>(setSize);
        timelineXML->addChild(ippXML);

        for (unsigned int i = 0; i < setSize; ++i)
        {

            TimelineSet* timelineSet = timeline->interPulsePeriod->sets[i];
            xml::lite::Element* setXML = newElement(doc, "Set");
            setXML->attribute("index") = str::toString<int>(i + 1);

            ippXML->addChild(setXML);

            setXML->addChild(createDouble(doc, "TStart", timelineSet->tStart));
            setXML->addChild(createDouble(doc, "TEnd", timelineSet->tEnd));
            setXML->addChild(createInt(doc, "IPPStart",
                    timelineSet->interPulsePeriodStart));
            setXML->addChild(createInt(doc, "IPPEnd",
                    timelineSet->interPulsePeriodEnd));
            setXML->addChild(createPoly1D(doc, "IPPPoly",
                    timelineSet->interPulsePeriodPoly));
        }
    }

    return timelineXML;
}

xml::lite::Element* ComplexXMLControl::positionToXML(xml::lite::Document* doc,
        Position *position)
{
    xml::lite::Element* positionXML = newElement(doc, "Position");

    positionXML->addChild(createPolyXYZ(doc, "ARPPoly", position->arpPoly));
    if (position->grpPoly.order() >= 0)
        positionXML->addChild(createPolyXYZ(doc, "GRPPoly", position->grpPoly));
    if (position->txAPCPoly.order() >= 0)
        positionXML->addChild(createPolyXYZ(doc, "TxAPCPoly",
                position->txAPCPoly));
    if (position->rcvAPC && !position->rcvAPC->rcvAPCPolys.empty())
    {
        unsigned int numPolys = position->rcvAPC->rcvAPCPolys.size();
        xml::lite::Element* rcvXML = newElement(doc, "RcvAPC");
        positionXML->addChild(rcvXML);
        setAttribute(rcvXML, "size", str::toString(numPolys));

        for (unsigned int i = 0; i < numPolys; ++i)
        {
            PolyXYZ xyz = position->rcvAPC->rcvAPCPolys[0];
            xml::lite::Element *xyzXML = createPolyXYZ(doc, "RcvAPCPoly", xyz);
            rcvXML->addChild(xyzXML);
            setAttribute(xyzXML, "index", str::toString(i));
        }
    }
    return positionXML;
}

xml::lite::Element* ComplexXMLControl::radarCollectionToXML(
        xml::lite::Document* doc, RadarCollection *radar)
{
    xml::lite::Element* radarXML = newElement(doc, "RadarCollection");

    if (radar->resolution)
    {
        xml::lite::Element* resXML = newElement(doc, "Res");
        radarXML->addChild(resXML);
        resXML->addChild(createDouble(doc, "RgImpResWid",
                radar->resolution->rangeImpulseResponseWidth));
        resXML->addChild(createDouble(doc, "AzImpResWid",
                radar->resolution->azimuthImpulseResponseWidth));

        if (!Init::isUndefined<double>(radar->resolution->referencePoint[0])
                && !Init::isUndefined<double>(radar->resolution->referencePoint[1])
                && !Init::isUndefined<double>(radar->resolution->referencePoint[2]))
        {
            xml::lite::Element *refXML = createVector3D(doc, "ReferencePoint",
                    radar->resolution->referencePoint);
            resXML->addChild(refXML);
            if (!radar->resolution->referenceName.empty())
            {
                setAttribute(refXML, "name", radar->resolution->referenceName);
            }
        }
    }

    if (!Init::isUndefined<int>(radar->refFrequencyIndex))
    {
        radarXML->addChild(createInt(doc, "RefFreqIndex",
                radar->refFrequencyIndex));
    }

    xml::lite::Element* txFreqXML = newElement(doc, "TxFrequency");
    radarXML->addChild(txFreqXML);
    txFreqXML->addChild(createDouble(doc, "Min", radar->txFrequencyMin));
    txFreqXML->addChild(createDouble(doc, "Max", radar->txFrequencyMax));

    if (radar->txPolarization != six::POL_NOT_SET)
    {
        radarXML->addChild(createString(doc, "TxPolarization", str::toString(
                radar->txPolarization)));
    }

    if (radar->polarizationHVAnglePoly.order() >= 0)
    {
        radarXML->addChild(createPoly1D(doc, "PolarizationHVAnglePoly",
                radar->polarizationHVAnglePoly));
    }

    if (!radar->txSequence.empty())
    {
        xml::lite::Element* txSeqXML = newElement(doc, "TxSequence");
        radarXML->addChild(txSeqXML);
        for (std::vector<TxStep*>::iterator it = radar->txSequence.begin(); it
                != radar->txSequence.end(); ++it)
        {
            TxStep *tx = *it;

            xml::lite::Element* txStepXML = newElement(doc, "TxStep");
            txSeqXML->addChild(txStepXML);

            if (!Init::isUndefined<int>(tx->waveformIndex))
            {
                txStepXML->addChild(
                        createInt(doc, "WFIndex", tx->waveformIndex));
            }
            if (tx->txPolarization != six::POL_NOT_SET)
            {
                txStepXML->addChild(createString(doc, "TxPolarization",
                        str::toString(tx->txPolarization)));
            }
        }
    }

    if (!radar->waveform.empty())
    {
        unsigned int numWaveforms = radar->waveform.size();
        xml::lite::Element* wfXML = newElement(doc, "Waveform");
        radarXML->addChild(wfXML);
        setAttribute(wfXML, "size", str::toString(numWaveforms));

        for (unsigned int i = 0; i < numWaveforms; ++i)
        {
            WaveformParameters *wf = radar->waveform[i];

            xml::lite::Element* wfpXML = newElement(doc, "WFParameters");
            wfXML->addChild(wfpXML);
            setAttribute(wfpXML, "index", str::toString(i));

            if (!Init::isUndefined<double>(wf->txPulseLength))
                wfpXML->addChild(createDouble(doc, "TxPulseLength",
                        wf->txPulseLength));
            if (!Init::isUndefined<double>(wf->txRFBandwidth))
                wfpXML->addChild(createDouble(doc, "TxRFBandwidth",
                        wf->txRFBandwidth));
            if (!Init::isUndefined<double>(wf->txFrequencyStart))
                wfpXML->addChild(createDouble(doc, "TxFreqStart",
                        wf->txFrequencyStart));
            if (!Init::isUndefined<double>(wf->txFMRate))
                wfpXML->addChild(createDouble(doc, "TxFMRate", wf->txFMRate));
            if (wf->rcvDemodType != six::DEMOD_NOT_SET)
                wfpXML->addChild(createString(doc, "RcvDemodType",
                        str::toString(wf->rcvDemodType)));
            if (!Init::isUndefined<double>(wf->rcvWindowLength))
                wfpXML->addChild(createDouble(doc, "RcvWindowLength",
                        wf->rcvWindowLength));
            if (!Init::isUndefined<double>(wf->adcSampleRate))
                wfpXML->addChild(createDouble(doc, "ADCSampleRate",
                        wf->adcSampleRate));
            if (!Init::isUndefined<double>(wf->rcvIFBandwidth))
                wfpXML->addChild(createDouble(doc, "RcvIFBandwidth",
                        wf->rcvIFBandwidth));
            if (!Init::isUndefined<double>(wf->rcvFrequencyStart))
                wfpXML->addChild(createDouble(doc, "RcvFreqStart",
                        wf->rcvFrequencyStart));
            if (!Init::isUndefined<double>(wf->rcvFMRate))
                wfpXML->addChild(createDouble(doc, "RcvFMRate", wf->rcvFMRate));
        }
    }

    unsigned int numChannels = radar->rcvChannels.size();
    xml::lite::Element* rcvChanXML = newElement(doc, "RcvChannels");
    radarXML->addChild(rcvChanXML);
    setAttribute(rcvChanXML, "size", str::toString(numChannels));
    for (unsigned int i = 0; i < numChannels; ++i)
    {
        ChannelParameters *cp = radar->rcvChannels[i];
        xml::lite::Element* cpXML = newElement(doc, "ChanParameters");
        rcvChanXML->addChild(cpXML);
        setAttribute(cpXML, "index", str::toString(i));

        if (!Init::isUndefined<int>(cp->rcvAPCIndex))
            cpXML->addChild(createInt(doc, "RcvAPCIndex", cp->rcvAPCIndex));

        if (cp->txRcvPolarization != six::DUAL_POL_NOT_SET)
        {
            cpXML->addChild(createString(doc, "TxRcvPolarization",
                    str::toString<DualPolarizationType>(cp->txRcvPolarization)));
        }
    }

    if (radar->area)
    {
        xml::lite::Element* areaXML = newElement(doc, "Area");
        radarXML->addChild(areaXML);

        Area *area = radar->area;

        bool haveApcCorners = true;

        for (unsigned int i = 0; i < area->apcCorners.size(); ++i)
        {
            if (Init::isUndefined<LatLon>(area->apcCorners[i]))
            {
                haveApcCorners = false;
                break;
            }
        }

        if (haveApcCorners)
        {
            areaXML->addChild(createFootprint(doc, "Corner", "APC",
                    area->apcCorners, true));
        }

        AreaPlane *plane = area->plane;
        if (plane)
        {
            xml::lite::Element* planeXML = newElement(doc, "Plane");
            areaXML->addChild(planeXML);
            xml::lite::Element* refPtXML = newElement(doc, "RefPt");
            planeXML->addChild(refPtXML);

            ReferencePoint refPt = plane->referencePoint;
            if (!refPt.name.empty())
                setAttribute(refPtXML, "name", refPt.name);

            xml::lite::Element* ecfXML = newElement(doc, "ECF");
            refPtXML->addChild(ecfXML);
            ecfXML->addChild(createDouble(doc, "X", refPt.ecef[0]));
            ecfXML->addChild(createDouble(doc, "Y", refPt.ecef[1]));
            ecfXML->addChild(createDouble(doc, "Z", refPt.ecef[2]));

            refPtXML->addChild(createDouble(doc, "Line", refPt.rowCol.row));
            refPtXML->addChild(createDouble(doc, "Sample", refPt.rowCol.col));

            planeXML->addChild(areaLineDirectionParametersToXML(doc, "XDir",
                    plane->xDirection));
            planeXML->addChild(areaSampleDirectionParametersToXML(doc, "YDir",
                    plane->yDirection));

            if (!plane->segmentList.empty())
            {
                xml::lite::Element* segListXML = newElement(doc, "SegmentList");
                planeXML->addChild(segListXML);
                setAttribute(segListXML, "size", str::toString(
                        plane->segmentList.size()));
                for (int i = 0, size = plane->segmentList.size(); i < size; ++i)
                {
                    Segment *segment = plane->segmentList[i];
                    xml::lite::Element* segXML = newElement(doc, "Segment");
                    segListXML->addChild(segXML);
                    setAttribute(segXML, "index", str::toString(i+1));

                    segXML->addChild(createInt(doc, "StartLine",
                            segment->startLine));
                    segXML->addChild(createInt(doc, "StartSample",
                            segment->startSample));
                    segXML->addChild(
                            createInt(doc, "EndLine", segment->endLine));
                    segXML->addChild(createInt(doc, "EndSample",
                            segment->endSample));
                    segXML->addChild(createString(doc, "Identifier",
                            segment->identifier));
                }
            }

            if (plane->timeCOAPoly.orderX() >= 0 && plane->timeCOAPoly.orderY()
                    >= 0)
            {
                planeXML->addChild(createPoly2D(doc, "TimeCOAPoly",
                        plane->timeCOAPoly));
            }
        }
    }

    addParameters(doc, radarXML, "Parameter", radar->parameters);
    return radarXML;
}

xml::lite::Element* ComplexXMLControl::areaLineDirectionParametersToXML(
        xml::lite::Document* doc, std::string name,
        AreaDirectionParameters *adp)
{
    xml::lite::Element* adpXML = newElement(doc, name);
    adpXML->addChild(createVector3D(doc, "UVectECF", adp->unitVector));
    adpXML->addChild(createDouble(doc, "LineSpacing", adp->spacing));
    adpXML->addChild(createInt(doc, "NumLines", adp->elements));
    adpXML->addChild(createInt(doc, "FirstLine", adp->first));
    return adpXML;
}

xml::lite::Element* ComplexXMLControl::areaSampleDirectionParametersToXML(
        xml::lite::Document* doc, std::string name,
        AreaDirectionParameters *adp)
{
    xml::lite::Element* adpXML = newElement(doc, name);
    adpXML->addChild(createVector3D(doc, "UVectECF", adp->unitVector));
    adpXML->addChild(createDouble(doc, "SampleSpacing", adp->spacing));
    adpXML->addChild(createInt(doc, "NumSamples", adp->elements));
    adpXML->addChild(createInt(doc, "FirstSample", adp->first));
    return adpXML;
}

xml::lite::Element* ComplexXMLControl::imageFormationToXML(
        xml::lite::Document* doc, ImageFormation *imageFormation)
{
    xml::lite::Element* imageFormationXML = newElement(doc, "ImageFormation");

    if (!imageFormation->segmentIdentifier.empty())
        imageFormationXML->addChild(createString(doc, "SegmentIdentifier",
                imageFormation->segmentIdentifier));

    //TODO this is actually required by the schema, but I don't want to seg fault
    if (imageFormation->rcvChannelProcessed)
    {
        xml::lite::Element* rcvChanXML = newElement(doc, "RcvChanProc");
        imageFormationXML->addChild(rcvChanXML);
        rcvChanXML->addChild(createInt(doc, "NumChanProc",
                imageFormation->rcvChannelProcessed->numChannelsProcessed));
        if (!Init::isUndefined<double>(
                imageFormation->rcvChannelProcessed->prfScaleFactor))
            rcvChanXML->addChild(createDouble(doc, "PRFScaleFactor",
                    imageFormation->rcvChannelProcessed->prfScaleFactor));

        for (std::vector<int>::iterator it =
                imageFormation->rcvChannelProcessed->channelIndex.begin(); it
                != imageFormation->rcvChannelProcessed->channelIndex.end(); ++it)
        {
            rcvChanXML->addChild(createInt(doc, "ChanIndex", *it));
        }
    }
    if (imageFormation->txRcvPolarizationProc != six::DUAL_POL_NOT_SET)
    {
        imageFormationXML->addChild(createString(doc, "TxRcvPolarizationProc",
                str::toString(imageFormation->txRcvPolarizationProc)));
    }

    imageFormationXML->addChild(createString(doc, "ImageFormAlgo",
            str::toString(imageFormation->imageFormationAlgorithm)));

    imageFormationXML->addChild(createDouble(doc, "TStartProc",
            imageFormation->tStartProc));
    imageFormationXML->addChild(createDouble(doc, "TEndProc",
            imageFormation->tEndProc));

    xml::lite::Element* txFreqXML = newElement(doc, "TxFrequencyProc");
    imageFormationXML->addChild(txFreqXML);
    txFreqXML->addChild(createDouble(doc, "MinProc",
            imageFormation->txFrequencyProcMin));
    txFreqXML->addChild(createDouble(doc, "MaxProc",
            imageFormation->txFrequencyProcMax));

    imageFormationXML->addChild(createString(doc, "STBeamComp", str::toString(
            imageFormation->slowTimeBeamCompensation)));
    imageFormationXML->addChild(createString(doc, "ImageBeamComp",
            str::toString(imageFormation->imageBeamCompensation)));
    imageFormationXML->addChild(createString(doc, "AzAutofocus", str::toString(
            imageFormation->azimuthAutofocus)));
    imageFormationXML->addChild(createString(doc, "RgAutofocus", str::toString(
            imageFormation->rangeAutofocus)));

    if (imageFormation->polarizationCalibration)
    {
        xml::lite::Element* pcXML = newElement(doc, "PolarizationCalibration");
        imageFormationXML->addChild(pcXML);

        addRequired(imageFormationXML, createBooleanType(doc,
                "HvAngleCompApplied", imageFormation->polarizationCalibration
                ->hvAngleCompensationApplied), "HvAngleCompApplied");

        addRequired(imageFormationXML, createBooleanType(doc,
                "DistortionCorrectionApplied", imageFormation
                ->polarizationCalibration->distortionCorrectionApplied),
                "DistortionCorrectionApplied");

        //TODO this is required, but doing this for safety - once we decide on a policy, maybe throw an exception
        Distortion *distortion =
                imageFormation->polarizationCalibration->distortion;
        if (distortion)
        {
            xml::lite::Element* distortionXML = newElement(doc, "Distortion");
            pcXML->addChild(distortionXML);

            //This should be optionally added...
            distortionXML->addChild(createDateTime(doc, "CalibrationDate",
                    distortion->calibrationDate));
            distortionXML->addChild(createDouble(doc, "A", distortion->a));
            distortionXML->addChild(createComplex(doc, "F1", distortion->f1));
            distortionXML->addChild(createComplex(doc, "Q1", distortion->q1));
            distortionXML->addChild(createComplex(doc, "Q2", distortion->q2));
            distortionXML->addChild(createComplex(doc, "F2", distortion->f2));
            distortionXML->addChild(createComplex(doc, "Q3", distortion->q3));
            distortionXML->addChild(createComplex(doc, "Q4", distortion->q4));

            if (!Init::isUndefined<double>(distortion->gainErrorA))
                distortionXML->addChild(createDouble(doc, "GainErrorA",
                        distortion->gainErrorA));
            if (!Init::isUndefined<double>(distortion->gainErrorF1))
                distortionXML->addChild(createDouble(doc, "GainErrorF1",
                        distortion->gainErrorF1));
            if (!Init::isUndefined<double>(distortion->gainErrorF2))
                distortionXML->addChild(createDouble(doc, "GainErrorF2",
                        distortion->gainErrorF2));
            if (!Init::isUndefined<double>(distortion->phaseErrorF1))
                distortionXML->addChild(createDouble(doc, "PhaseErrorF1",
                        distortion->phaseErrorF1));
            if (!Init::isUndefined<double>(distortion->phaseErrorF2))
                distortionXML->addChild(createDouble(doc, "PhaseErrorF2",
                        distortion->phaseErrorF2));
        }
    }
    return imageFormationXML;
}

xml::lite::Element* ComplexXMLControl::scpcoaToXML(xml::lite::Document* doc,
        SCPCOA *scpcoa)
{
    xml::lite::Element* scpcoaXML = newElement(doc, "SCPCOA");

    scpcoaXML->addChild(createDouble(doc, "SCPTime", scpcoa->scpTime));
    scpcoaXML->addChild(createVector3D(doc, "ARPPos", scpcoa->arpPos));
    scpcoaXML->addChild(createVector3D(doc, "ARPVel", scpcoa->arpVel));
    scpcoaXML->addChild(createVector3D(doc, "ARPAcc", scpcoa->arpAcc));
    scpcoaXML->addChild(createString(doc, "SideOfTrack", str::toString(
            scpcoa->sideOfTrack)));
    scpcoaXML->addChild(createDouble(doc, "SlantRange", scpcoa->slantRange));
    scpcoaXML->addChild(createDouble(doc, "GroundRange", scpcoa->groundRange));
    scpcoaXML->addChild(createDouble(doc, "DopplerConeAng",
            scpcoa->dopplerConeAngle));
    scpcoaXML->addChild(createDouble(doc, "GrazeAng", scpcoa->grazeAngle));
    scpcoaXML->addChild(createDouble(doc, "IncidenceAng",
            scpcoa->incidenceAngle));
    scpcoaXML->addChild(createDouble(doc, "TwistAng", scpcoa->twistAngle));
    scpcoaXML->addChild(createDouble(doc, "SlopeAng", scpcoa->slopeAngle));
    return scpcoaXML;
}

xml::lite::Element* ComplexXMLControl::antennaToXML(xml::lite::Document* doc,
        Antenna *antenna)
{
    xml::lite::Element* antennaXML = newElement(doc, "Antenna");

    if (antenna->tx)
        antennaXML->addChild(antennaParametersToXML(doc, "Tx", antenna->tx));
    if (antenna->rcv)
        antennaXML->addChild(antennaParametersToXML(doc, "Rcv", antenna->rcv));
    if (antenna->twoWay)
        antennaXML->addChild(antennaParametersToXML(doc, "TwoWay",
                antenna->twoWay));

    return antennaXML;
}

xml::lite::Element* ComplexXMLControl::antennaParametersToXML(
        xml::lite::Document* doc, std::string name, AntennaParameters *params)
{
    xml::lite::Element* apXML = newElement(doc, name);

    apXML->addChild(createPolyXYZ(doc, "XAxisPoly", params->xAxisPoly));
    apXML->addChild(createPolyXYZ(doc, "YAxisPoly", params->yAxisPoly));
    apXML->addChild(createDouble(doc, "FreqZero", params->frequencyZero));

    if (params->electricalBoresight)
    {
        xml::lite::Element* ebXML = newElement(doc, "EB");
        apXML->addChild(ebXML);
        ebXML->addChild(createPoly1D(doc, "DCXPoly",
                params->electricalBoresight->dcxPoly));
        ebXML->addChild(createPoly1D(doc, "DCYPoly",
                params->electricalBoresight->dcyPoly));
    }
    if (params->halfPowerBeamwidths)
    {
        xml::lite::Element* hpXML = newElement(doc, "HPBW");
        apXML->addChild(hpXML);
        hpXML->addChild(createDouble(doc, "DCX",
                params->halfPowerBeamwidths->dcx));
        hpXML->addChild(createDouble(doc, "DCY",
                params->halfPowerBeamwidths->dcy));
    }
    if (params->array)
    {
        xml::lite::Element* arrXML = newElement(doc, "Array");
        apXML->addChild(arrXML);
        arrXML->addChild(createPoly2D(doc, "GainPoly", params->array->gainPoly));
        arrXML->addChild(createPoly2D(doc, "PhasePoly",
                params->array->phasePoly));
    }
    if (params->element)
    {
        xml::lite::Element* elemXML = newElement(doc, "Elem");
        apXML->addChild(elemXML);
        elemXML->addChild(createPoly2D(doc, "GainPoly",
                params->element->gainPoly));
        elemXML->addChild(createPoly2D(doc, "PhasePoly",
                params->element->phasePoly));
    }
    if (params->gainBSPoly.order() >= 0)
    {
        apXML->addChild(createPoly1D(doc, "GainBSPoly", params->gainBSPoly));
    }

    addOptional(apXML, createBooleanType(doc, "EBFreqShift",
            params->electricalBoresightFrequencyShift));
    addOptional(apXML, createBooleanType(doc, "MLFreqDilation",
            params->mainlobeFrequencyDilation));

    return apXML;
}

xml::lite::Element * ComplexXMLControl::matchInfoToXML(
        xml::lite::Document* doc, MatchInformation *matchInfo)
{
    xml::lite::Element* matchInfoXML = newElement(doc, "MatchInfo");

    for (unsigned int i = 0; i < matchInfo->collects.size(); ++i)
    {
    
        MatchCollection *mc = matchInfo->collects[i];
        xml::lite::Element* mcXML = newElement(doc, "Collect");
	setAttribute(mcXML, "index", str::toString(i+1));
        matchInfoXML->addChild(mcXML);

        mcXML->addChild(createString(doc, "CollectorName", mc->collectorName));
        if (!mc->illuminatorName.empty())
            mcXML->addChild(createString(doc, "IlluminatorName",
                    mc->illuminatorName));
        mcXML->addChild(createString(doc, "CoreName", mc->coreName));

        for (std::vector<std::string>::iterator it = mc->matchType.begin(); it
                != mc->matchType.end(); ++it)
        {
            mcXML->addChild(createString(doc, "MatchType", *it));
        }
        addParameters(doc, mcXML, "Parameter", mc->parameters);
    }

    return matchInfoXML;
}

xml::lite::Element* ComplexXMLControl::pfaToXML(xml::lite::Document* doc,
        PFA *pfa)
{
    xml::lite::Element* pfaXML = newElement(doc, "PFA");

    pfaXML->addChild(createVector3D(doc, "FPN", pfa->focusPlaneNormal));
    pfaXML->addChild(createVector3D(doc, "IPN", pfa->imagePlaneNormal));
    pfaXML->addChild(createDouble(doc, "PolarAngRefTime",
            pfa->polarAngleRefTime));
    pfaXML->addChild(createPoly1D(doc, "PolarAngPoly", pfa->polarAnglePoly));
    pfaXML->addChild(createPoly1D(doc, "SpatialFreqSFPoly",
            pfa->spatialFrequencyScaleFactorPoly));
    pfaXML->addChild(createDouble(doc, "Krg1", pfa->krg1));
    pfaXML->addChild(createDouble(doc, "Krg2", pfa->krg2));
    pfaXML->addChild(createDouble(doc, "Kaz1", pfa->kaz1));
    pfaXML->addChild(createDouble(doc, "Kaz2", pfa->kaz2));
    if (pfa->slowTimeDeskew)
    {
        xml::lite::Element* stdXML = newElement(doc, "STDeskew");
        pfaXML->addChild(stdXML);

        addRequired(stdXML, createBooleanType(doc, "Applied",
                pfa->slowTimeDeskew->applied), "Applied");

        stdXML->addChild(createPoly2D(doc, "STDPhasePoly",
                pfa->slowTimeDeskew->slowTimeDeskewPhasePoly));
    }
    for (std::vector<Compensation*>::iterator it = pfa->compensations.begin(); it
            != pfa->compensations.end(); ++it)
    {
        Compensation *comp = *it;
        xml::lite::Element* compXML = newElement(doc, "Comp");
        pfaXML->addChild(compXML);
        compXML->addChild(createString(doc, "Type", comp->type));
        addRequired(compXML, createBooleanType(doc, "Applied", comp->applied),
                "Applied");
        addParameters(doc, compXML, "Parameter", comp->parameters);
    }
    return pfaXML;
}

void ComplexXMLControl::xmlToCollectionInfo(
        xml::lite::Element* collectionInfoXML, CollectionInformation *collInfo)
{
    parseString(getFirstAndOnly(collectionInfoXML, "CollectorName"),
            collInfo->collectorName);

    xml::lite::Element* element = getOptional(collectionInfoXML,
            "IlluminatorName");
    if (element)
        parseString(element, collInfo->illuminatorName);

    element = getOptional(collectionInfoXML, "CoreName");
    if (element)
        parseString(element, collInfo->coreName);

    element = getOptional(collectionInfoXML, "CollectType");
    if (element)
        collInfo->collectType = str::toType<six::CollectType>(
                element->getCharacterData());

    xml::lite::Element* radarModeXML = getFirstAndOnly(collectionInfoXML,
            "RadarMode");

    collInfo->radarMode = str::toType<RadarModeType>(getFirstAndOnly(
            radarModeXML, "ModeType")->getCharacterData());

    element = getOptional(radarModeXML, "ModeID");
    if (element)
        parseString(element, collInfo->radarModeID);

    parseString(getFirstAndOnly(collectionInfoXML, "Classification"),
            collInfo->classification.level);

    std::vector<xml::lite::Element*> countryCodeXML;
    collectionInfoXML->getElementsByTagName("CountryCode", countryCodeXML);

    //optional
    for (std::vector<xml::lite::Element*>::iterator it = countryCodeXML.begin();
            it != countryCodeXML.end(); ++it)
    {
        std::string cc;

        parseString(*it, cc);
        collInfo->countryCodes.push_back(cc);
    }

    //optional
    parseParameters(collectionInfoXML, "Parameter", collInfo->parameters);
}

void ComplexXMLControl::xmlToImageCreation(
        xml::lite::Element* imageCreationXML, ImageCreation *imageCreation)
{
    // Optional
    xml::lite::Element* element = getOptional(imageCreationXML, "Application");
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

void ComplexXMLControl::xmlToImageData(xml::lite::Element* imageDataXML,
        ImageData *imageData)
{
    imageData->pixelType = str::toType<PixelType>(getFirstAndOnly(imageDataXML,
            "PixelType")->getCharacterData());

    xml::lite::Element* ampTableXML = getOptional(imageDataXML, "AmpTable");

    if (ampTableXML != NULL)
    {
        std::vector<xml::lite::Element*> ampsXML;
        ampTableXML->getElementsByTagName("Amplitude", ampsXML);

        int i = 0;
        //TODO make sure there is at least 1 and not more than 256
        imageData->amplitudeTable = new AmplitudeTable();

	AmplitudeTable& ampTable = *(imageData->amplitudeTable);
		
        for (std::vector<xml::lite::Element*>::iterator it = ampsXML.begin(); 
	     it != ampsXML.end(); ++it)
        {
            parseDouble(*it, *(double*)ampTable[i++]);
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

    xml::lite::Element* validDataXML = getOptional(imageDataXML, "ValidData");
    if (validDataXML)
    {
        std::vector<xml::lite::Element*> verticesXML;
        validDataXML->getElementsByTagName("Vertex", verticesXML);

        //TODO make sure there are at least 3
        for (std::vector<xml::lite::Element*>::iterator it =
                verticesXML.begin(); it != verticesXML.end(); ++it)
        {
            RowColInt rowCol;

            parseRowColInt(*it, "Row", "Col", rowCol);
            imageData->validData.push_back(rowCol);
        }
    }

}

void ComplexXMLControl::xmlToGeoData(xml::lite::Element* geoDataXML,
        GeoData *geoData)
{
    parseEarthModelType(getFirstAndOnly(geoDataXML, "EarthModel"),
            geoData->earthModel);

    xml::lite::Element* tmpElem = getFirstAndOnly(geoDataXML, "SCP");
    parseVector3D(getFirstAndOnly(tmpElem, "ECF"), geoData->scp.ecf);
    parseLatLonAlt(getFirstAndOnly(tmpElem, "LLH"), geoData->scp.llh);

    parseFootprint(getFirstAndOnly(geoDataXML, "ImageCorners"), "ICP",
            geoData->imageCorners, false);

    tmpElem = getOptional(geoDataXML, "ValidData");
    if (tmpElem != NULL)
    {
        parseLatLons(tmpElem, "Vertex", geoData->validData);
    }

    std::vector<xml::lite::Element*> geoInfosXML;
    geoDataXML->getElementsByTagName("GeoInfo", geoInfosXML);

    //optional
    for (std::vector<xml::lite::Element*>::iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it)
    {
        GeoInfo *gi = new GeoInfo();
        xmlToGeoInfo(*it, gi);
        geoData->geoInfos.push_back(gi);
    }

}

void ComplexXMLControl::xmlToGeoInfo(xml::lite::Element* geoInfoXML,
        GeoInfo* geoInfo)
{
    std::vector<xml::lite::Element*> geoInfosXML;
    geoInfoXML->getElementsByTagName("GeoInfo", geoInfosXML);
    geoInfo->name = geoInfoXML->getAttributes().getValue("name");

    int i = 0;
    //optional
    for (std::vector<xml::lite::Element*>::iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it)
    {
        GeoInfo *gi = new GeoInfo();
        xmlToGeoInfo(*it, gi);
        geoInfo->geoInfos.push_back(gi);
    }

    //optional
    parseParameters(geoInfoXML, "Desc", geoInfo->desc);

    xml::lite::Element* tmpElem = getOptional(geoInfoXML, "Point");
    if (tmpElem)
    {
        LatLon ll;
        parseLatLon(tmpElem, ll);
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
            parseLatLons(tmpElem, pointName, geoInfo->geometryLatLon);
        }
    }
}

void ComplexXMLControl::xmlToGrid(xml::lite::Element* gridXML, Grid *grid)
{
    grid->imagePlane = str::toType<ComplexImagePlaneType>(getFirstAndOnly(
            gridXML, "ImagePlane")->getCharacterData());
    grid->type = str::toType<ComplexImageGridType>(getFirstAndOnly(gridXML,
            "Type")->getCharacterData());

    xml::lite::Element* tmpElem = getFirstAndOnly(gridXML, "TimeCOAPoly");
    parsePoly2D(tmpElem, grid->timeCOAPoly);

    tmpElem = getFirstAndOnly(gridXML, "Row");
    parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->row->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->row->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
            grid->row->impulseResponseWidth);
    grid->row->sign = str::toType<six::FFTSign>(getFirstAndOnly(tmpElem,
            "Sgn")->getCharacterData());
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespBW"),
            grid->row->impulseResponseBandwidth);
    parseDouble(getFirstAndOnly(tmpElem, "KCtr"), grid->row->kCenter);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK1"), grid->row->deltaK1);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK2"), grid->row->deltaK2);

    xml::lite::Element* optElem = getOptional(tmpElem, "DeltaKCOAPoly");
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

    xml::lite::Element* weightFuncXML = getOptional(tmpElem, "WgtFunct");
    if (weightFuncXML)
    {
        //optional
        //TODO make sure there is at least one and not more than 512 wgts
        std::vector<xml::lite::Element*> weightsXML;
        weightFuncXML->getElementsByTagName("Wgt", weightsXML);
        for (std::vector<xml::lite::Element*>::iterator it = weightsXML.begin(); it
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
    grid->col->sign = str::toType<six::FFTSign>(
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
        std::vector<xml::lite::Element*> weightsXML;
        weightFuncXML->getElementsByTagName("Wgt", weightsXML);
        for (std::vector<xml::lite::Element*>::iterator it = weightsXML.begin(); it
                != weightsXML.end(); ++it)
        {
            double value;
            parseDouble(*it, value);
            grid->col->weights.push_back(value);
        }
    }
}

void ComplexXMLControl::xmlToTimeline(xml::lite::Element* timelineXML,
        Timeline *timeline)
{
    parseDateTime(getFirstAndOnly(timelineXML, "CollectStart"),
            timeline->collectStart);
    parseDouble(getFirstAndOnly(timelineXML, "CollectDuration"),
            timeline->collectDuration);

    xml::lite::Element* ippXML = getOptional(timelineXML, "IPP");
    if (ippXML)
    {
        timeline->interPulsePeriod = new InterPulsePeriod();
        //TODO make sure there is at least one
        std::vector<xml::lite::Element*> setsXML;
        ippXML->getElementsByTagName("Set", setsXML);
        for (std::vector<xml::lite::Element*>::iterator it = setsXML.begin(); it
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

void ComplexXMLControl::xmlToPosition(xml::lite::Element* positionXML,
        Position *position)
{
    xml::lite::Element* tmpElem = getFirstAndOnly(positionXML, "ARPPoly");
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
        std::vector<xml::lite::Element*> polysXML;
        tmpElem->getElementsByTagName("RcvAPCPoly", polysXML);
        for (std::vector<xml::lite::Element*>::iterator it = polysXML.begin(); it
                != polysXML.end(); ++it)
        {
            PolyXYZ p;
            parsePolyXYZ(*it, p);
            position->rcvAPC->rcvAPCPolys.push_back(p);
        }
    }
}

void ComplexXMLControl::xmlToRadarCollection(
        xml::lite::Element* radarCollectionXML,
        RadarCollection *radarCollection)
{
    xml::lite::Element* tmpElem = NULL;
    xml::lite::Element* optElem = NULL;

    tmpElem = getOptional(radarCollectionXML, "Res");
    if (tmpElem)
    {
        radarCollection->resolution = new ResolutionParameters();

        parseDouble(getFirstAndOnly(tmpElem, "RgImpResWid"),
                radarCollection->resolution->rangeImpulseResponseWidth);
        parseDouble(getFirstAndOnly(tmpElem, "AzImpResWid"),
                radarCollection->resolution->azimuthImpulseResponseWidth);

        xml::lite::Element *refXML = getOptional(tmpElem, "ReferencePoint");
        if (refXML)
        {
            //optional
            parseVector3D(refXML, radarCollection->resolution->referencePoint);
            //this will throw if it the name doesn't exist, but that't ok - it's optional
            try
            {
                radarCollection->resolution->referenceName
                        = refXML->getAttributes().getValue("name");
            }
            catch (except::Exception& e)
            {
            }
        }
    }

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
                = str::toType<PolarizationType>(tmpElem->getCharacterData());
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
        std::vector<xml::lite::Element*> txStepsXML;
        tmpElem->getElementsByTagName("TxStep", txStepsXML);
        for (std::vector<xml::lite::Element*>::iterator it = txStepsXML.begin(); it
                != txStepsXML.end(); ++it)
        {
            TxStep* step = new TxStep();

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
                        = str::toType<PolarizationType>(
                                optElem->getCharacterData());
            }

            radarCollection->txSequence.push_back(step);
        }
    }

    tmpElem = getOptional(radarCollectionXML, "Waveform");
    if (tmpElem)
    {
        //optional
        //TODO make sure there is at least one
        std::vector<xml::lite::Element*> wfParamsXML;
        tmpElem->getElementsByTagName("WFParameters", wfParamsXML);
        for (std::vector<xml::lite::Element*>::iterator it =
                wfParamsXML.begin(); it != wfParamsXML.end(); ++it)
        {
            WaveformParameters* wfParams = new WaveformParameters();

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
                wfParams->rcvDemodType = str::toType<DemodType>(
                        optElem->getCharacterData());
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

            radarCollection->waveform.push_back(wfParams);
        }
    }

    tmpElem = getFirstAndOnly(radarCollectionXML, "RcvChannels");

    //optional
    std::vector<xml::lite::Element*> channelsXML;
    tmpElem->getElementsByTagName("ChanParameters", channelsXML);
    for (std::vector<xml::lite::Element*>::iterator it = channelsXML.begin(); it
            != channelsXML.end(); ++it)
    {
        ChannelParameters* chanParams = new ChannelParameters();

        xml::lite::Element* childXML = getOptional(*it, "RcvAPCIndex");
        if (childXML)
        {
            parseInt(childXML, chanParams->rcvAPCIndex);
        }

        childXML = getOptional(*it, "TxRcvPolarization");
        if (childXML)
        {
            //optional
            chanParams->txRcvPolarization = str::toType<DualPolarizationType>(
                    childXML->getCharacterData());
        }

        radarCollection->rcvChannels.push_back(chanParams);
    }

    xml::lite::Element* areaXML = getOptional(radarCollectionXML, "Area");
    if (areaXML)
    {
        //optional
        radarCollection->area = new Area();

        optElem = getOptional(areaXML, "Corner");
        if (optElem)
        {
            //optional
            parseFootprint(optElem, "APC", radarCollection->area->apcCorners,
                    true);
        }

        xml::lite::Element* planeXML = getOptional(areaXML, "Plane");
        if (planeXML)
        {
            //optional
            radarCollection->area->plane = new AreaPlane();

            xml::lite::Element* refPtXML = getFirstAndOnly(planeXML, "RefPt");
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

            xml::lite::Element* dirXML = getFirstAndOnly(planeXML, "XDir");
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

            xml::lite::Element* segmentListXML = getOptional(planeXML,
                    "SegmentList");
            if (segmentListXML != NULL)
            {
                //TODO make sure there is at least one
                std::vector<xml::lite::Element*> segmentsXML;
                segmentListXML->getElementsByTagName("Segment", segmentsXML);

                for (std::vector<xml::lite::Element*>::iterator it =
                        segmentsXML.begin(); it != segmentsXML.end(); ++it)
                {
                    Segment* seg = new Segment();

                    parseInt(getFirstAndOnly(*it, "StartLine"),seg->startLine);
                    parseInt(getFirstAndOnly(*it, "StartSample"),
                            seg->startSample);
                    parseInt(getFirstAndOnly(*it, "EndLine"), seg->endLine);
                    parseInt(getFirstAndOnly(*it, "EndSample"), seg->endSample);
                    parseString(getFirstAndOnly(*it, "Identifier"),
                            seg->identifier);

                    radarCollection->area->plane->segmentList.push_back(seg);
                }
            }

            //optional
            optElem = getOptional(planeXML, "TimeCOAPoly");
            if (optElem)
            {
                parsePoly2D(optElem,
                        radarCollection->area->plane->timeCOAPoly);
            }
        }
    }

    parseParameters(radarCollectionXML, "Parameter",
            radarCollection->parameters);
}

void ComplexXMLControl::xmlToImageFormation(
        xml::lite::Element* imageFormationXML, ImageFormation *imageFormation)
{
    xml::lite::Element* tmpElem = getOptional(imageFormationXML,
            "SegmentIdentifier");
    if (tmpElem)
    {
        //optional
        parseString(tmpElem, imageFormation->segmentIdentifier);
    }

    tmpElem = getFirstAndOnly(imageFormationXML, "RcvChanProc");

    parseUInt(getFirstAndOnly(tmpElem, "NumChanProc"),
            imageFormation->rcvChannelProcessed->numChannelsProcessed);

    xml::lite::Element* prfXML = getOptional(tmpElem, "PRFScaleFactor");
    if (prfXML)
    {
        //optional
        parseDouble(prfXML,
                imageFormation->rcvChannelProcessed->prfScaleFactor);
    }

    //TODO make sure there is at least one
    std::vector<xml::lite::Element*> chansXML;
    tmpElem->getElementsByTagName("ChanIndex", chansXML);
    for (std::vector<xml::lite::Element*>::iterator it = chansXML.begin(); it
            != chansXML.end(); ++it)
    {
        int value;
        parseInt(*it, value);
        imageFormation->rcvChannelProcessed->channelIndex.push_back(value);
    }

    tmpElem = getOptional(imageFormationXML, "TxRcvPolarizationProc");

    if (tmpElem)
    {
        imageFormation->txRcvPolarizationProc = str::toType<
                DualPolarizationType>(tmpElem->getCharacterData());
    }

    imageFormation->imageFormationAlgorithm
            = str::toType<ImageFormationType>(getFirstAndOnly(
                    imageFormationXML, "ImageFormAlgo")->getCharacterData());

    parseDouble(getFirstAndOnly(imageFormationXML, "TStartProc"),
            imageFormation->tStartProc);

    parseDouble(getFirstAndOnly(imageFormationXML, "TEndProc"),
            imageFormation->tEndProc);

    tmpElem = getFirstAndOnly(imageFormationXML, "TxFrequencyProc");

    parseDouble(getFirstAndOnly(tmpElem, "MinProc"),
            imageFormation->txFrequencyProcMin);

    parseDouble(getFirstAndOnly(tmpElem, "MaxProc"),
            imageFormation->txFrequencyProcMax);

    imageFormation->slowTimeBeamCompensation = str::toType<
            SlowTimeBeamCompensationType>(getFirstAndOnly(imageFormationXML,
            "STBeamComp")->getCharacterData());

    imageFormation->imageBeamCompensation = str::toType<
            ImageBeamCompensationType>(getFirstAndOnly(imageFormationXML,
            "ImageBeamComp")->getCharacterData());

    imageFormation->azimuthAutofocus
            = str::toType<AutofocusType>(getFirstAndOnly(imageFormationXML,
                    "AzAutofocus")->getCharacterData());

    imageFormation->rangeAutofocus
            = str::toType<AutofocusType>(getFirstAndOnly(imageFormationXML,
                    "RgAutofocus")->getCharacterData());

    xml::lite::Element* polCalXML = getOptional(imageFormationXML,
            "PolarizationCalibration");
    if (polCalXML)
    {
        //optional
        imageFormation->polarizationCalibration = new PolarizationCalibration();
        imageFormation->polarizationCalibration->distortion = new Distortion();

        parseBooleanType(getFirstAndOnly(polCalXML, "HVAngleCompApplied"),
                imageFormation->polarizationCalibration
                ->hvAngleCompensationApplied);

        parseBooleanType(getFirstAndOnly(polCalXML,
                "DistortionCorrectionApplied"),
                imageFormation->polarizationCalibration
                ->distortionCorrectionApplied);

        xml::lite::Element* distortionXML = getFirstAndOnly(polCalXML,
                "Distortion");

        xml::lite::Element* calibDateXML = getOptional(distortionXML,
                "CalibrationDate");
        if (calibDateXML)
        {
            parseDateTime(calibDateXML, imageFormation->polarizationCalibration
                    ->distortion->calibrationDate);
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

        xml::lite::Element* gainErrorXML = getOptional(distortionXML,
                "GainErrorA");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->polarizationCalibration
                    ->distortion->gainErrorA);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF1");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->polarizationCalibration
                    ->distortion->gainErrorF1);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF2");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->polarizationCalibration
                    ->distortion->gainErrorF2);
        }

        xml::lite::Element* phaseErrorXML = getOptional(distortionXML,
                "PhaseErrorF1");
        if (phaseErrorXML)
        {
            parseDouble(phaseErrorXML, imageFormation->polarizationCalibration
                    ->distortion->phaseErrorF1);
        }

        phaseErrorXML = getOptional(distortionXML, "PhaseErrorF2");
        if (phaseErrorXML)
        {
            parseDouble(phaseErrorXML, imageFormation->polarizationCalibration
                    ->distortion->phaseErrorF2);
        }

    }
}

void ComplexXMLControl::xmlToSCPCOA(xml::lite::Element* scpcoaXML,
        SCPCOA *scpcoa)
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

void ComplexXMLControl::xmlToAntennaParams(
        xml::lite::Element* antennaParamsXML, AntennaParameters* params)
{
    parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "XAxisPoly"),
            params->xAxisPoly);
    parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "YAxisPoly"),
            params->yAxisPoly);
    parseDouble(getFirstAndOnly(antennaParamsXML, "FreqZero"),
            params->frequencyZero);

    xml::lite::Element* tmpElem = getOptional(antennaParamsXML, "EB");
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

void ComplexXMLControl::xmlToAntenna(xml::lite::Element* antennaXML,
        Antenna *antenna)
{
    xml::lite::Element* antennaParamsXML = getOptional(antennaXML, "Tx");
    if (antennaParamsXML)
    {
        antenna->tx = new AntennaParameters();
        xmlToAntennaParams(antennaParamsXML, antenna->tx);
    }

    antennaParamsXML = getOptional(antennaXML, "Rcv");
    if (antennaParamsXML)
    {
        antenna->rcv = new AntennaParameters();
        xmlToAntennaParams(antennaParamsXML, antenna->rcv);
    }

    antennaParamsXML = getOptional(antennaXML, "TwoWay");
    if (antennaParamsXML)
    {
        antenna->twoWay = new AntennaParameters();
        xmlToAntennaParams(antennaParamsXML, antenna->twoWay);
    }
}

void ComplexXMLControl::xmlToMatchInfo(xml::lite::Element* matchInfoXML,
        MatchInformation *matchInfo)
{
    xml::lite::Element* optElem = NULL;

    //TODO make sure there is at least one
    std::vector<xml::lite::Element*> collectsXML;
    matchInfoXML->getElementsByTagName("Collect", collectsXML);
    for (std::vector<xml::lite::Element*>::iterator it = collectsXML.begin(); it
            != collectsXML.end(); ++it)
    {
        MatchCollection* coll = new MatchCollection();

        parseString(getFirstAndOnly(*it, "CollectorName"), coll->collectorName);

        optElem = getOptional(*it, "IlluminatorName");
        if (optElem)
        {
            //optional
            parseString(optElem, coll->illuminatorName);
        }

        parseString(getFirstAndOnly(*it, "CoreName"), coll->coreName);

        //optional
        std::vector<xml::lite::Element*> matchTypesXML;
        (*it)->getElementsByTagName("MatchType", matchTypesXML);
        for (std::vector<xml::lite::Element*>::iterator it2 =
                collectsXML.begin(); it2 != collectsXML.end(); ++it2)
        {
            std::string value;

            parseString(*it2, value);
            coll->matchType.push_back(value);
        }

        //optional
        parseParameters(*it, "Parameter", coll->parameters);

        matchInfo->collects.push_back(coll);
    }
}

void ComplexXMLControl::xmlToPFA(xml::lite::Element* pfaXML, PFA *pfa)
{
    parseVector3D(getFirstAndOnly(pfaXML, "FPN"), pfa->focusPlaneNormal);
    parseVector3D(getFirstAndOnly(pfaXML, "IPN"), pfa->imagePlaneNormal);
    parseDouble(getFirstAndOnly(pfaXML,
            "PolarAngRefTime"), pfa->polarAngleRefTime);
    parsePoly1D(getFirstAndOnly(pfaXML, "PolarAngPoly"), pfa->polarAnglePoly);
    parsePoly1D(getFirstAndOnly(pfaXML, "SpatialFreqSFPoly"),
            pfa->spatialFrequencyScaleFactorPoly);
    parseDouble(getFirstAndOnly(pfaXML, "Krg1"), pfa->krg1);
    parseDouble(getFirstAndOnly(pfaXML, "Krg2"), pfa->krg2);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz1"), pfa->kaz1);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz2"), pfa->kaz2);

    xml::lite::Element* deskewXML = getOptional(pfaXML, "STDeskew");
    if (deskewXML)
    {
        pfa->slowTimeDeskew = new SlowTimeDeskew();
        parseBooleanType(getFirstAndOnly(deskewXML, "Applied"),
                pfa->slowTimeDeskew->applied);

        parsePoly2D(getFirstAndOnly(deskewXML, "STDSPhasePoly"),
                pfa->slowTimeDeskew->slowTimeDeskewPhasePoly);
    }

    //optional
    std::vector<xml::lite::Element*> compsXML;
    pfaXML->getElementsByTagName("Comp", compsXML);
    for (std::vector<xml::lite::Element*>::iterator it = compsXML.begin(); it
            != compsXML.end(); ++it)
    {
        Compensation* comp = new Compensation();

        parseString(getFirstAndOnly(*it, "Type"), comp->type);
        parseBooleanType(getFirstAndOnly(*it, "Applied"), comp->applied);

        //optional
        parseParameters(*it, "Parameter", comp->parameters);

        pfa->compensations.push_back(comp);
    }
}

void ComplexXMLControl::parseFootprint(xml::lite::Element* footprint,
        std::string cornerName, std::vector<LatLon>& value, bool alt)
{
    std::vector<xml::lite::Element*> vertices;
    footprint->getElementsByTagName(cornerName, vertices);

    value.clear();
    value.resize(4);

    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        //check the index attr to know which corner it is
        int idx = str::toType<int>(vertices[i]->getAttributes().getValue(
                "index")) - 1;

        parseLatLon(vertices[i], value[idx]);

        if (alt)
        {
            double hae;

            parseDouble(getFirstAndOnly(vertices[i], "HAE"), hae);
            value[idx].setAlt(hae);
        }
    }
}

xml::lite::Element* ComplexXMLControl::createFootprint(
        xml::lite::Document* doc, std::string name, std::string cornerName,
        const std::vector<LatLon>& corners, bool alt)
{
    xml::lite::Element* footprint = newElement(doc, name);

    xml::lite::Element* vertex = createLatLon(doc, cornerName, corners[0]);
    setAttribute(vertex, "index", "1:FRFC");
    footprint->addChild(vertex);

    vertex = createLatLon(doc, cornerName, corners[1]);
    setAttribute(vertex, "index", "2:FRLC");
    footprint->addChild(vertex);

    vertex = createLatLon(doc, cornerName, corners[2]);
    setAttribute(vertex, "index", "3:LRLC");
    footprint->addChild(vertex);

    vertex = createLatLon(doc, cornerName, corners[3]);
    setAttribute(vertex, "index", "4:LRFC");
    footprint->addChild(vertex);

    return footprint;
}

