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
#include "six/sicd/ComplexData.h"

using namespace six;
using namespace six::sicd;

const char ComplexData::VENDOR_ID[] = "GDAIS";

Data* ComplexData::clone() const
{
    return new ComplexData(this);
}

ComplexData::~ComplexData()
{
    if (collectionInformation)
        delete collectionInformation;
    if (imageCreation)
        delete imageCreation;
    if (imageData)
        delete imageData;
    if (geoData)
        delete geoData;
    if (grid)
        delete grid;
    if (timeline)
        delete timeline;
    if (position)
        delete position;
    if (radarCollection)
        delete radarCollection;
    if (imageFormation)
        delete imageFormation;
    if (scpcoa)
        delete scpcoa;
    if (radiometric)
        delete radiometric;
    if (antenna)
        delete antenna;
    if (errorStatistics)
        delete errorStatistics;
    if (matchInformation)
        delete matchInformation;
    if (pfa)
        delete pfa;
    if (rma)
        delete rma;
}

ComplexData::ComplexData() :
    collectionInformation(NULL), imageCreation(NULL), imageData(NULL),
            geoData(NULL), grid(NULL), timeline(NULL), position(NULL),
            radarCollection(NULL), imageFormation(NULL), scpcoa(NULL),
            radiometric(NULL), antenna(NULL), errorStatistics(NULL),
            matchInformation(NULL), pfa(NULL), rma(NULL)
{
    //only initialize the mandatory elements
    collectionInformation = new CollectionInformation();
    imageData = new ImageData();
    geoData = new GeoData();
    grid = new Grid();
    timeline = new Timeline();
    position = new Position();
    radarCollection = new RadarCollection();
    imageFormation = new ImageFormation();
    scpcoa = new SCPCOA();
}

ComplexData::ComplexData(const ComplexData* cloner) :
    collectionInformation(NULL), imageCreation(NULL), imageData(NULL),
            geoData(NULL), grid(NULL), timeline(NULL), position(NULL),
            radarCollection(NULL), imageFormation(NULL), scpcoa(NULL),
            radiometric(NULL), antenna(NULL), errorStatistics(NULL),
            matchInformation(NULL), pfa(NULL), rma(NULL)
{
    if (cloner->collectionInformation)
        collectionInformation = cloner->collectionInformation->clone();
    if (cloner->imageCreation)
        imageCreation = cloner->imageCreation->clone();
    if (cloner->imageData)
        imageData = cloner->imageData->clone();
    if (cloner->geoData)
        geoData = cloner->geoData->clone();
    if (cloner->grid)
        grid = cloner->grid->clone();
    if (cloner->timeline)
        timeline = cloner->timeline->clone();
    if (cloner->position)
        position = cloner->position->clone();
    if (cloner->radarCollection)
        radarCollection = cloner->radarCollection->clone();
    if (cloner->imageFormation)
        imageFormation = cloner->imageFormation->clone();
    if (cloner->scpcoa)
        scpcoa = cloner->scpcoa->clone();
    if (cloner->radiometric)
        radiometric = cloner->radiometric->clone();
    if (cloner->antenna)
        antenna = cloner->antenna->clone();
    if (cloner->errorStatistics)
        errorStatistics = cloner->errorStatistics->clone();
    if (cloner->matchInformation)
        matchInformation = cloner->matchInformation->clone();
    if (cloner->pfa)
        pfa = cloner->pfa->clone();
    if (cloner->rma)
        rma = cloner->rma->clone();
}
