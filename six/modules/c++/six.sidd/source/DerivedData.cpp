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

DerivedData::DerivedData(Version siddVersion, six::sidd300::ISMVersion ismVersion) :
    productCreation(new ProductCreation),
    mVersion(siddVersion), mISMVersion(ismVersion)
{
}
DerivedData::DerivedData(Version siddVersion) : DerivedData(siddVersion, six::sidd300::ISMVersion::current)
{
    if (siddVersion == Version::v300)
    {
        throw std::invalid_argument("Must use ISMVersion overload.");
    }
}
DerivedData::DerivedData() : DerivedData(Version::v100) // existing code
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

const mem::ScopedCopyablePtr<LUT>& DerivedData::getDisplayLUT() const
{
    if (mVersion == Version::v100)
    {
        if (display->remapInformation.get() == nullptr)
        {
            throw except::Exception(Ctxt("Display.RemapInformation is nullptr"));
        }
        return display->remapInformation->remapLUT;
    }
    else if ((mVersion == Version::v200) || (mVersion == Version::v300))
    {
        return nitfLUT;
    }
    
    throw except::Exception(Ctxt("Unknown version. Expected 3.0.0, 2.0.0, or 1.0.0"));
}
void DerivedData::setDisplayLUT(std::unique_ptr<AmplitudeTable>&& pLUT)
{
    nitfLUT.reset(pLUT.release());
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

bool DerivedData::operator_eq(const DerivedData& rhs) const
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
    auto data = dynamic_cast<const DerivedData*>(&rhs);
    if (data != nullptr)
    {
        return this->operator_eq(*data);
    }
    return false;
}

Version DerivedData::getSIDDVersion() const
{
    return mVersion;
}
std::string DerivedData::getVersion() const
{
    return to_string(getSIDDVersion());
}

void DerivedData::setSIDDVersion(Version siddVersion, six::sidd300::ISMVersion ismVersion)
{
    mVersion = siddVersion;
    mISMVersion = ismVersion;
}
void DerivedData::setSIDDVersion(Version siddVersion)
{
    if (siddVersion == Version::v300)
    {
        throw std::invalid_argument("Must use ISMVersion overload."); // TODO
    }
    mVersion = siddVersion;
}

void DerivedData::setVersion(const std::string& strVersion)
{
    // This is an `override` of `six::Data` so there's no way to pass
    // as six::sidd300::ISMVersion for SIDD 3.0.0.
    setVersion(strVersion, six::sidd300::ISMVersion::current);
}
void DerivedData::setVersion(const std::string& strVersion, six::sidd300::ISMVersion ismVersion)
{
    setSIDDVersion(normalizeVersion(strVersion), ismVersion);
}

six::sidd300::ISMVersion DerivedData::getISMVersion() const
{
    return mISMVersion;
}

}
}
