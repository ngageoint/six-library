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
#include "six/sicd/ComplexData.h"

#include <assert.h>

namespace six
{
namespace sicd
{
const char ComplexData::VENDOR_ID[] = "MDA-IS";

Data* ComplexData::clone() const
{
    return new ComplexData(*this);
}

ComplexData::ComplexData() :
    collectionInformation(new CollectionInformation()),
    imageData(new ImageData()),
    geoData(new GeoData()),
    grid(new Grid()),
    timeline(new Timeline()),
    position(new Position()),
    radarCollection(new RadarCollection()),
    imageFormation(new ImageFormation()),
    scpcoa(new SCPCOA()),
    mVersion("1.3.0")
{
    //only initialize the mandatory elements above
}

DateTime ComplexData::getCollectionStartDateTime() const
{
    return timeline->collectStart;
}

void ComplexData::getOutputPlaneOffsetAndExtent(
        types::RowCol<size_t>& offset,
        types::RowCol<size_t>& extent) const
{
    const six::sicd::AreaPlane& areaPlane =
        *radarCollection->area->plane;
    getOutputPlaneOffsetAndExtent(areaPlane, offset, extent);
}

void ComplexData::getOutputPlaneOffsetAndExtent(
        const AreaPlane& areaPlane,
        types::RowCol<size_t>& offset,
        types::RowCol<size_t>& extent) const
{
    offset.row = offset.col = 0;
    extent.row = areaPlane.xDirection->elements;
    extent.col = areaPlane.yDirection->elements;

    if (imageFormation.get())
    {
        const std::string& segmentID =
                imageFormation->segmentIdentifier;
        if (!segmentID.empty())
        {
            const six::sicd::Segment& segment = areaPlane.getSegment(segmentID);

            // TODO: These should be offset by areaPlane.xDirection->first
            //       and areaPlane.yDirection->first, but the problem is
            //       that SICD producers are currently doing this
            //       incorrectly - they're setting the
            //       FirstLine/FirstSample values to 1 but then setting
            //       startLine and startSample to 0.
            offset = segment.getOffset();
            extent = segment.getExtent();
        }
    }
}

types::RowCol<double>
ComplexData::pixelToImagePoint(const types::RowCol<double>& pixelLoc) const
{
    const types::RowCol<double> scpPixel(imageData->scpPixel);
    const types::RowCol<double> aoiOffset(static_cast<double>(imageData->firstRow),
                                          static_cast<double>(imageData->firstCol));

    const types::RowCol<double> offset(scpPixel - aoiOffset);

    const types::RowCol<double> sampleSpacing(grid->row->sampleSpacing,
                                              grid->col->sampleSpacing);

    const types::RowCol<double> imagePt(
            (pixelLoc.row - offset.row) * sampleSpacing.row,
            (pixelLoc.col - offset.col) * sampleSpacing.col);

    return imagePt;
}

bool ComplexData::operator_eq(const ComplexData& rhs) const
{
    return (collectionInformation == rhs.collectionInformation &&
        imageCreation == rhs.imageCreation &&
        imageData == rhs.imageData &&
        geoData == rhs.geoData &&
        grid == rhs.grid &&
        timeline == rhs.timeline &&
        position == rhs.position &&
        radarCollection == rhs.radarCollection &&
        imageFormation == rhs.imageFormation &&
        scpcoa == rhs.scpcoa &&
        radiometric == rhs.radiometric &&
        antenna == rhs.antenna &&
        errorStatistics == rhs.errorStatistics &&
        matchInformation == rhs.matchInformation &&
        pfa == rhs.pfa &&
        rma == rhs.rma &&
        rgAzComp == rhs.rgAzComp);
}

bool ComplexData::equalTo(const Data& rhs) const
{
    auto data = dynamic_cast<const ComplexData*>(&rhs);
    if (data != nullptr)
    {
        return this->operator_eq(*data);
    }
    return false;
}

double ComplexData::computeFc() const
{
    double fc(Init::undefined<double>());
    if (radarCollection->refFrequencyIndex == 0)
    {
        fc = (imageFormation->txFrequencyProcMin +
            imageFormation->txFrequencyProcMax) / 2;
    }
    return fc;
}

bool ComplexData::validate(logging::Logger& log) const
{
    // This function is a transcription of MATLAB file validate_sicd.m by Wade Schwartzkopf
    // Reference numbers (e.g. 2.3) reference the corresponding sections of the MATLAB file
    bool valid = grid->validate(*collectionInformation, *imageData, log);
    valid = position->validate(log) && valid;
    valid = scpcoa->validate(*geoData, *grid, *position, log) && valid;
    valid = imageData->validate(*geoData, log) && valid;
    valid = geoData->validate(log) && valid;
    valid = radarCollection->validate(log) && valid;

    const auto fc = computeFc();

    std::ostringstream messageBuilder;
    switch (imageFormation->imageFormationAlgorithm)
    {
    case ImageFormationType::RGAZCOMP: // 2.12.1
        if (rgAzComp.get())
        {
            valid = rgAzComp->validate(
                    *geoData, *grid, *scpcoa, *timeline, log) && valid;
            valid = grid->validate(
                    *rgAzComp, *geoData, *scpcoa, fc, log) && valid;
        }
        else
        {
            messageBuilder.str("");
            messageBuilder <<
                "RgAzComp specified in imageFormation.imageFormationAlgorithm,"
                << " but member pointer is nullptr.";
            log.error(messageBuilder.str());
            valid = false;
        }
        break;
    case ImageFormationType::PFA:      // 2.12.2

        if (pfa.get())
        {
            valid = pfa->validate(*scpcoa, log) && valid;
            valid = grid->validate(*pfa, *radarCollection, fc, log) && valid;
        }
        else
        {
            messageBuilder.str("");
            messageBuilder <<
                "PFA specified in imageFormation.imageFormationAlgorithm,"
                << " but member pointer is nullptr.";
            log.error(messageBuilder.str());
            valid = false;
        }
        break;
    case ImageFormationType::RMA:      // 2.12.3.*
        if (rma.get())
        {
            valid = rma->validate(*collectionInformation, geoData->scp.ecf,
                    position->arpPoly, fc, log) && valid;
            valid = grid->validate(*rma, geoData->scp.ecf, position->arpPoly,
                    fc, log) && valid;
        }
        else
        {
            messageBuilder.str("");
            messageBuilder <<
                "RMA specified in imageFormation.imageFormationAlgorithm,"
                << " but member pointer is nullptr.";
            log.error(messageBuilder.str());
            valid = false;
        }
        break;
    default:
        //2.12.3 (This is not a typo)

        messageBuilder << "Image formation not fully defined." << std::endl
            << "SICD.ImageFormation.ImageFormAlgo = OTHER or is not set.";
        log.warn(messageBuilder.str());
        valid = false;
        break;
    }

    return valid;
}

void ComplexData::fillDerivedFields(bool includeDefault)
{
    grid->fillDerivedFields(*collectionInformation, *imageData, *scpcoa);
    position->fillDerivedFields(*scpcoa);
    radarCollection->fillDerivedFields();

    // SCPCOA only needs geoData for the SCP, which is not derivable from
    // SCPCOA, so it's safe to do it backwards like this
    scpcoa->fillDerivedFields(*geoData, *grid, *position);
    const scene::PlaneProjectionModel model(
            scpcoa->slantPlaneNormal(geoData->scp.ecf),
            grid->row->unitVector,
            grid->col->unitVector,
            geoData->scp.ecf,
            position->arpPoly,
            grid->timeCOAPoly,
            scpcoa->look(geoData->scp.ecf));

    geoData->fillDerivedFields(*imageData, model);

    const auto fc = computeFc();

    switch (imageFormation->imageFormationAlgorithm)
    {
    case ImageFormationType::RGAZCOMP:
        if (rgAzComp.get())
        {
            rgAzComp->fillDerivedFields(*geoData, *grid, *scpcoa, *timeline);
            grid->fillDerivedFields(*rgAzComp, *geoData, *scpcoa, fc);
        }
        break;
    case ImageFormationType::PFA:
        if (pfa.get())
        {
            pfa->fillDerivedFields(*position);
        }
        break;
    case ImageFormationType::RMA:
        if (rma.get())
        {
            rma->fillDerivedFields(*geoData, *position);
            grid->fillDerivedFields(*rma, geoData->scp.ecf, position->arpPoly);
        }
        break;
    case ImageFormationType::NOT_SET:
    case ImageFormationType::OTHER:
    default:
        break; // nothing to do
    }

    if (includeDefault)
    {
        fillDefaultFields();
    }
}

void ComplexData::fillDefaultFields()
{
    imageFormation->fillDefaultFields(*radarCollection);
    const auto fc = computeFc();

    switch (imageFormation->imageFormationAlgorithm)
    {
    case ImageFormationType::PFA:
        if (pfa.get())
        {
            pfa->fillDefaultFields(*geoData, *grid, *scpcoa);
            grid->fillDefaultFields(*pfa, fc);
        }
        break;
    case ImageFormationType::RMA:
        if (rma.get())
        {
            rma->fillDefaultFields(*scpcoa, fc);
            grid->fillDefaultFields(*rma, fc);
        }
        break;

    case ImageFormationType::RGAZCOMP:        
    case ImageFormationType::NOT_SET:
    case ImageFormationType::OTHER:
    default:
        break; // nothing to do
    }
}
}
}
// Okay, little bit of a hack for now
const mem::ScopedCopyablePtr<six::LUT>& six::sicd::ComplexData::getDisplayLUT() const
{
    if (getPixelType() != PixelType::AMP8I_PHS8I)
    {
        throw except::Exception(Ctxt("Display LUT operation not supported"));
    }
    // throw except::Exception(Ctxt("Display LUT operation not supported"));

    // imageData->amplitudeTable is a ScopedCloneablePtr which can't be returned by 
    // reference as a ScopedCopyablePtr.  Instead, return something that is NULL
    // calling code can then try getAmplitudeTable().
    static mem::ScopedCopyablePtr<six::LUT> retval;
    retval.reset(); // in case somebody changed it
    return retval;
}
void six::sicd::ComplexData::setDisplayLUT(std::unique_ptr<AmplitudeTable>&& pLUT)
{
    imageData->amplitudeTable.reset(pLUT.release());
}

six::AmplitudeTable* six::sicd::ComplexData::getAmplitudeTable() const
{
    auto const retval = imageData->amplitudeTable.get();
    if (getPixelType() != PixelType::AMP8I_PHS8I)
    {
        assert(retval == nullptr);
        throw except::Exception(Ctxt("Display LUT operation not supported"));
    }
    return retval;
}

inline const void* cast_to_pvoid(std::span<const std::byte> bytes)
{
    return bytes.data();
}
inline void* cast_to_pvoid(std::span<std::byte> bytes)
{
    return bytes.data();
}
template<typename T, typename U>
inline std::span<T> make_span(std::span<U> bytes)
{
    const auto size = bytes.size() / sizeof(T);
    return std::span<T>(static_cast<T*>(cast_to_pvoid(bytes)), size);
}

bool six::sicd::ComplexData::convertPixels_(std::span<const std::byte> from_, std::span<std::byte> to_) const
{
    if (getPixelType() != PixelType::AMP8I_PHS8I)
    {
        return false; // no conversion done as there is nothing to convert
    }

    // Convert the pixels from a complex<float> to AMP8I_PHS8I.
    const auto from = make_span<const six::sicd::cx_float>(from_);
    const auto to = make_span<six::AMP8I_PHS8I_t>(to_);
    imageData->fromComplex(from, to);
    return true; // converted
}
