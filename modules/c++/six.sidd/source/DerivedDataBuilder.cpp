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
#include "six/sidd/DerivedDataBuilder.h"

using namespace six;
using namespace six::sidd;

DerivedDataBuilder::DerivedDataBuilder() :
    mData(new DerivedData()), mAdopt(true)
{
}

DerivedDataBuilder::DerivedDataBuilder(DerivedData *data) :
    mData(data), mAdopt(false)
{
}

DerivedDataBuilder::~DerivedDataBuilder()
{
    if (mData && mAdopt)
        delete mData;
}

DerivedDataBuilder& DerivedDataBuilder::addDisplay(PixelType pixelType)
{
    if (mData->display)
        delete mData->display;

    DisplayType displayType = DisplayType::DISPLAY_COLOR;
    if (pixelType == PixelType::MONO8LU || pixelType == PixelType::MONO8I
            || pixelType == PixelType::MONO16I)
    {
        displayType = DisplayType::DISPLAY_MONO;
    }
    mData->display = new Display(displayType);
    mData->display->pixelType = pixelType;
    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addGeographicAndTarget(
                                                               RegionType regionType)
{
    if (mData->geographicAndTarget)
        delete mData->geographicAndTarget;
    mData->geographicAndTarget = new GeographicAndTarget(regionType);

    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addMeasurement(
                                                       ProjectionType projectionType)
{
    if (mData->measurement)
        delete mData->measurement;
    mData->measurement = new Measurement(projectionType);

    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addExploitationFeatures(
                                                                unsigned int num)
{
    if (mData->exploitationFeatures)
        delete mData->exploitationFeatures;
    mData->exploitationFeatures = new ExploitationFeatures(num);

    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addProductProcessing()
{
    if (mData->productProcessing)
        delete mData->productProcessing;
    mData->productProcessing = new ProductProcessing();

    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addDownstreamReprocessing()
{
    if (mData->downstreamReprocessing)
        delete mData->downstreamReprocessing;
    mData->downstreamReprocessing = new DownstreamReprocessing();

    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addErrorStatistics()
{
    if (mData->errorStatistics)
        delete mData->errorStatistics;
    mData->errorStatistics = new ErrorStatistics();

    return *this;
}

DerivedDataBuilder& DerivedDataBuilder::addRadiometric()
{
    if (mData->radiometric)
        delete mData->radiometric;
    mData->radiometric = new Radiometric();

    return *this;
}

DerivedData* DerivedDataBuilder::get()
{
    return mData;
}

DerivedData* DerivedDataBuilder::steal()
{
    mAdopt = false;
    return get();
}

