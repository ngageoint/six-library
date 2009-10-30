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
#include "six/DerivedDataBuilder.h"

six::DerivedDataBuilder::DerivedDataBuilder() :
    mData(new six::DerivedData()), mAdopt(true)
{
}

six::DerivedDataBuilder::DerivedDataBuilder(six::DerivedData *data) :
    mData(data), mAdopt(false)
{
}

six::DerivedDataBuilder::~DerivedDataBuilder()
{
    if (mData && mAdopt)
        delete mData;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addDisplay(
        six::PixelType pixelType)
{
    if (mData->display)
        delete mData->display;



    six::DisplayType displayType = six::DISPLAY_COLOR;
    if (pixelType == six::MONO8LU || pixelType == six::MONO8I || pixelType
            == six::MONO16I)
        displayType = six::DISPLAY_MONO;
    mData->display = new six::Display(displayType);
    mData->display->pixelType = pixelType;
    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addGeographicAndTarget(
        six::RegionType regionType)
{
    if (mData->geographicAndTarget)
        delete mData->geographicAndTarget;
    mData->geographicAndTarget = new six::GeographicAndTarget(regionType);

    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addMeasurement(
        six::ProjectionType projectionType)
{
    if (mData->measurement)
        delete mData->measurement;
    mData->measurement = new six::Measurement(projectionType);

    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addExploitationFeatures(
        unsigned int num)
{
    if (mData->exploitationFeatures)
        delete mData->exploitationFeatures;
    mData->exploitationFeatures = new six::ExploitationFeatures(num);

    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addProductProcessing()
{
    if (mData->productProcessing)
        delete mData->productProcessing;
    mData->productProcessing = new six::ProductProcessing();

    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addDownstreamReprocessing()
{
    if (mData->downstreamReprocessing)
        delete mData->downstreamReprocessing;
    mData->downstreamReprocessing = new six::DownstreamReprocessing();

    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addErrorStatistics()
{
    if (mData->errorStatistics)
        delete mData->errorStatistics;
    mData->errorStatistics = new six::ErrorStatistics();

    return *this;
}

six::DerivedDataBuilder& six::DerivedDataBuilder::addRadiometric()
{
    if (mData->radiometric)
        delete mData->radiometric;
    mData->radiometric = new six::Radiometric();

    return *this;
}

six::DerivedData* six::DerivedDataBuilder::get()
{
    return mData;
}

six::DerivedData* six::DerivedDataBuilder::steal()
{
    mAdopt = false;
    return get();
}

