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
#include "six/ComplexDataBuilder.h"

six::ComplexDataBuilder::ComplexDataBuilder() :
    mData(new six::ComplexData()), mAdopt(true)
{
}

six::ComplexDataBuilder::ComplexDataBuilder(six::ComplexData *data) :
    mData(data), mAdopt(false)
{
}

six::ComplexDataBuilder::~ComplexDataBuilder()
{
    if (mData && mAdopt)
        delete mData;
}

six::ComplexDataBuilder& six::ComplexDataBuilder::addImageCreation()
{
    if (mData->imageCreation)
        delete mData->imageCreation;
    mData->imageCreation = new six::ImageCreation();
    return *this;
}

six::ComplexDataBuilder& six::ComplexDataBuilder::addRadiometric()
{
    if (mData->radiometric)
        delete mData->radiometric;
    mData->radiometric = new six::Radiometric();
    return *this;
}

six::ComplexDataBuilder& six::ComplexDataBuilder::addAntenna()
{
    if (mData->antenna)
        delete mData->antenna;
    mData->antenna = new six::Antenna();
    return *this;
}

six::ComplexDataBuilder& six::ComplexDataBuilder::addErrorStatistics()
{
    if (mData->errorStatistics)
        delete mData->errorStatistics;
    mData->errorStatistics = new six::ErrorStatistics();
    return *this;
}

six::ComplexDataBuilder& six::ComplexDataBuilder::addMatchInformation()
{
    if (mData->matchInformation)
        delete mData->matchInformation;
    mData->matchInformation = new six::MatchInformation();
    return *this;
}

six::ComplexData* six::ComplexDataBuilder::get()
{
    return mData;
}

six::ComplexData* six::ComplexDataBuilder::steal()
{
    mAdopt = false;
    return get();
}
