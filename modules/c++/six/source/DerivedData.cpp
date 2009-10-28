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
#include "six/DerivedData.h"

using namespace six;

DerivedData::~DerivedData()
{
    if (productCreation != NULL)
        delete productCreation;

    if (display != NULL)
        delete display;

    if (geographicAndTarget != NULL)
        delete geographicAndTarget;

    if (measurement != NULL)
        delete measurement;

    if (exploitationFeatures != NULL)
        delete exploitationFeatures;

    if (productProcessing != NULL)
        delete productProcessing;

    if (downstreamReprocessing != NULL)
        delete downstreamReprocessing;

    if (errorStatistics != NULL)
        delete errorStatistics;

    if (radiometric)
        delete radiometric;
}

DerivedData::DerivedData() :
    display(NULL), geographicAndTarget(NULL), measurement(NULL),
            exploitationFeatures(NULL), productProcessing(NULL),
            downstreamReprocessing(NULL), errorStatistics(NULL), radiometric(
                    NULL)
{
    productCreation = new ProductCreation();
}

Data* DerivedData::clone() const
{
    DerivedData *data = new DerivedData();

    if (productCreation != NULL)
        data->productCreation = productCreation->clone();

    if (display != NULL)
        data->display = display->clone();

    if (geographicAndTarget != NULL)
        data->geographicAndTarget = geographicAndTarget->clone();

    if (measurement != NULL)
        data->measurement = measurement->clone();

    if (exploitationFeatures != NULL)
        data->exploitationFeatures = exploitationFeatures->clone();

    if (productProcessing != NULL)
        data->productProcessing = productProcessing->clone();

    if (downstreamReprocessing != NULL)
        data->downstreamReprocessing = downstreamReprocessing->clone();

    if (errorStatistics != NULL)
        data->errorStatistics = errorStatistics->clone();

    if (radiometric)
        data->radiometric = radiometric->clone();

    return data;
}
