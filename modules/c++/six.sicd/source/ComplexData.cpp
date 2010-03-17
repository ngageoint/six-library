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

Data* ComplexData::clone() const
{
    ComplexData *data = new ComplexData();
    if (collectionInformation)
        data->collectionInformation = collectionInformation->clone();
    if (imageData)
        data->imageData = imageData->clone();

    if (geoData)
        data->geoData = geoData->clone();
    if (grid)
        data->grid = grid->clone();
    if (timeline)
        data->timeline = timeline->clone();
    if (position)
        data->position = position->clone();
    if (radarCollection)
        data->radarCollection = radarCollection->clone();
    if (imageFormation)
        data->imageFormation = imageFormation->clone();
    if (scpcoa)
        data->scpcoa = scpcoa->clone();
    if (radiometric)
        data->radiometric = radiometric->clone();
    if (antenna)
        data->antenna = antenna->clone();
    if (errorStatistics)
        data->errorStatistics = errorStatistics->clone();
    if (matchInformation)
        data->matchInformation = matchInformation->clone();

    if (pfa)
        data->pfa = pfa->clone();

    return data;

}

ComplexData::~ComplexData()
{
    if (collectionInformation)
        delete collectionInformation;
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
}

ComplexData::ComplexData() :
    imageCreation(NULL), radiometric(NULL), antenna(NULL),
            errorStatistics(NULL), matchInformation(NULL)
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
    pfa = new PFA();
}

