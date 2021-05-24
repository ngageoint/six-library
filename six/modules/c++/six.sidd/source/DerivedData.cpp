/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include <six/sidd/DerivedData.h>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>

namespace six
{
namespace sidd
{
const char DerivedData::VENDOR_ID[] = "MDA-IS";

DerivedData::DerivedData() :
    productCreation(new ProductCreation),
    mVersion("1.0.0")
{
}

Data* DerivedData::clone() const
{
    return new DerivedData(*this);
}

LatLonCorners DerivedData::getImageCorners() const
{
    if (geoData.get()) // SIDD 2.0
    {
        return geoData->imageCorners;
    }
    if (geographicAndTarget.get() &&
        geographicAndTarget->geographicCoverage.get()) // SIDD 1.0
    {
        return geographicAndTarget->geographicCoverage->footprint;
    }

    throw except::Exception(Ctxt(
            "Geographic fields aren't set to provide image corners"));
}

void DerivedData::setImageCorners(const LatLonCorners& imageCorners)
{
    if (geoData.get()) // SIDD 2.0
    {
        geoData->imageCorners = imageCorners;
    }
    else if (geographicAndTarget.get() &&
             geographicAndTarget->geographicCoverage.get()) // SIDD 1.0
    {
        geographicAndTarget->geographicCoverage->footprint = imageCorners;
    }
    else
    {
        throw except::Exception(Ctxt(
                "Geographic fields aren't allocated to set image "
                "corners"));
    }
}

DateTime DerivedData::getCollectionStartDateTime() const
{
    if (!exploitationFeatures.get() ||
        exploitationFeatures->collections.empty())
    {
        throw except::Exception(Ctxt("Must add a collection first"));
    }

    return exploitationFeatures->collections[0]->information.collectionDateTime;
}

mem::ScopedCopyablePtr<LUT>& DerivedData::getDisplayLUT()
{
    if (mVersion == "1.0.0")
    {
        if (display->remapInformation.get() == nullptr)
        {
            throw except::Exception(Ctxt("Display.RemapInformation is nullptr"));
        }
        return display->remapInformation->remapLUT;
    }
    else if (mVersion == "2.0.0")
    {
        return nitfLUT;
    }
    else
    {
        throw except::Exception(Ctxt("Unknown version. Expected 2.0.0 or 1.0.0"));
    }
}

types::RowCol<double>
DerivedData::pixelToImagePoint(const types::RowCol<double>& pixelLoc) const
{
    const types::RowCol<double> posRC(pixelLoc);
    types::RowCol<double> fullScenePos;
    if (downstreamReprocessing.get() &&
            downstreamReprocessing->geometricChip.get())
    {
        fullScenePos = downstreamReprocessing->geometricChip->
                getFullImageCoordinateFromChip(posRC);
    }
    else
    {
        fullScenePos = posRC;
    }

    if (!measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt(
                "Currently require a measurable projection type"));
    }

    const six::sidd::MeasurableProjection* projection =
        dynamic_cast<six::sidd::MeasurableProjection*>(
                    measurement->projection.get());
    const types::RowCol<double> ctrPt = projection->referencePoint.rowCol;

    return types::RowCol<double>(
            (fullScenePos.row - ctrPt.row) * projection->sampleSpacing.row,
            (fullScenePos.col - ctrPt.col) * projection->sampleSpacing.col);
}

bool DerivedData::operator==(const DerivedData& rhs) const
{
    return (productCreation == rhs.productCreation &&
        display == rhs.display &&
        geographicAndTarget == rhs.geographicAndTarget &&
        measurement == rhs.measurement &&
        exploitationFeatures == rhs.exploitationFeatures &&
        productProcessing == rhs.productProcessing &&
        downstreamReprocessing == rhs.downstreamReprocessing &&
        errorStatistics == rhs.errorStatistics &&
        radiometric == rhs.radiometric &&
        annotations == rhs.annotations);
}

bool DerivedData::equalTo(const Data& rhs) const
{
    const DerivedData* data = dynamic_cast<const DerivedData*>(&rhs);
    if (data != nullptr)
    {
        return *this == *data;
    }
    return false;
}
}
}
