/* =========================================================================
 * This file is part of six.sidd-c++
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
#include <six/sidd/CropUtils.h>

#include <memory>
#include <vector>
#include <std/cstddef>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <mem/ScopedArray.h>
#include <six/NITFReadControl.h>
#include <six/NITFWriteControl.h>
#include <six/sidd/Utilities.h>
#include <six/sidd/DerivedData.h>

namespace
{
struct Buffers final
{
    std::byte* add(size_t numBytes)
    {        
        mBuffers.push_back(std::vector<std::byte>(numBytes));
        return mBuffers.back().data();
    }

    std::vector<std::span<std::byte>> get()
    {
        std::vector<std::span<std::byte>> retval;
        for (auto& buffer : mBuffers)
        {
            retval.push_back(buffer);
        }
        return retval;
    }

private:
    std::vector<std::vector<std::byte>> mBuffers;
};

struct ChipCoordinateToFullImageCoordinate final
{
    ChipCoordinateToFullImageCoordinate(const six::sidd::DerivedData& data)
    {
        if (data.downstreamReprocessing.get() &&
            data.downstreamReprocessing->geometricChip.get())
        {
            mChip.reset(new six::sidd::GeometricChip(
                    *data.downstreamReprocessing->geometricChip));
        }
    }

    ChipCoordinateToFullImageCoordinate(const ChipCoordinateToFullImageCoordinate&) = delete;
    ChipCoordinateToFullImageCoordinate& operator=(const ChipCoordinateToFullImageCoordinate&) = delete;

    types::RowCol<double> operator()(const types::RowCol<double>& pos) const
    {
        if (mChip.get())
        {
            return mChip->getFullImageCoordinateFromChip(pos);
        }
        else
        {
            return pos;
        }
    }

    types::RowCol<double> operator()(size_t row, size_t col) const
    {
        const types::RowCol <size_t> rc(row, col);
        return operator()(types::RowCol<double>(rc));
    }

private:
    std::unique_ptr<const six::sidd::GeometricChip> mChip;
};

struct PixelToLatLon final
{
    PixelToLatLon(const six::sidd::DerivedData& data) :
        mGridTransform(six::sidd::Utilities::getGridECEFTransform(&data)),
        mRefPoint(data.measurement->projection->referencePoint.rowCol),
        mChipToFull(data)
    {
    }

    PixelToLatLon(const PixelToLatLon&) = delete;
    PixelToLatLon& operator=(const PixelToLatLon&) = delete;


    scene::LatLon operator()(size_t row, size_t col) const
    {
        const scene::Vector3 ecef =
                mGridTransform->rowColToECEF(mChipToFull(row, col));

        const scene::LatLonAlt latLon(scene::Utilities::ecefToLatLon(ecef));
        return scene::LatLon(latLon.getLat(), latLon.getLon());
    }

private:
    const std::unique_ptr<scene::GridECEFTransform> mGridTransform;
    const types::RowCol<double> mRefPoint;
    const ChipCoordinateToFullImageCoordinate mChipToFull;
};
}

namespace six
{
namespace sidd
{
void cropSIDD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname)
{
    // Make sure it's a SIDD
    six::NITFReadControl reader;
    reader.load(inPathname, schemaPaths);
    auto container(reader.getContainer());

    if (container->getDataType() != six::DataType::DERIVED)
    {
        throw except::Exception(Ctxt(inPathname + " is not a SIDD"));
    }

    Buffers buffers;
    for (size_t ii = 0, imageNum = 0; ii < container->size(); ++ii)
    {
        six::Data* const dataPtr = container->getData(ii);
        if (dataPtr->getDataType() == six::DataType::DERIVED)
        {
            six::sidd::DerivedData* const data =
                dynamic_cast<six::sidd::DerivedData*>(dataPtr);

            // Make sure the AOI is in bounds
            const auto origDims = getExtent(*data);
            if (aoiOffset.row + aoiDims.row > origDims.row ||
                aoiOffset.col + aoiDims.col > origDims.col)
            {
                throw except::Exception(Ctxt(
                        "AOI dimensions are out of bounds"));
            }

            if (aoiDims.row < 1 || aoiDims.col < 1)
            {
                throw except::Exception(Ctxt("AOI must be non-empty"));
            }

            // Read in the AOI
            const size_t numBytesPerPixel(data->getNumBytesPerPixel());
            const size_t numBytes =
                    origDims.row * origDims.col * numBytesPerPixel;
            std::byte* const buffer = buffers.add(numBytes);

            six::Region region;
            setOffset(region, aoiOffset);
            setDims(region, aoiDims);
            region.setBuffer(buffer);
            reader.interleaved(region, imageNum++);

            // Update to reflect the AOI in the SIX metadata
            // Construct the pixel --> lat/lon functor first so updating this
            // metadata won't affect its calculations
            const PixelToLatLon pixelToLatLon(*data);

            // Reflect the new GeometricChip
            // Note that it's possible this SIDD is already a chip itself,
            // so we need to convert all coordinates in the current SIDD back
            // to full image coordinates via chipToFull()
            const ChipCoordinateToFullImageCoordinate chipToFull(*data);

            GeometricChip chip;
            chip.setChipSize(aoiDims);
            const size_t lastRow = aoiOffset.row + aoiDims.row - 1;
            const size_t lastCol = aoiOffset.col + aoiDims.col - 1;

            chip.originalUpperLeftCoordinate =
                    chipToFull(aoiOffset.row, aoiOffset.col);

            chip.originalUpperRightCoordinate =
                    chipToFull(aoiOffset.row, lastCol);

            chip.originalLowerRightCoordinate = chipToFull(lastRow, lastCol);

            chip.originalLowerLeftCoordinate =
                    chipToFull(lastRow, aoiOffset.col);

            if (data->downstreamReprocessing.get() == nullptr)
            {
                data->downstreamReprocessing.reset(
                        new six::sidd::DownstreamReprocessing());
            }
            if (data->downstreamReprocessing->geometricChip.get() == nullptr)
            {
                data->downstreamReprocessing->geometricChip.reset(
                        new six::sidd::GeometricChip());
            }
            *data->downstreamReprocessing->geometricChip = chip;

            data->measurement->setPixelFootprint(aoiDims);

            six::LatLonCorners corners;
            corners.upperLeft = pixelToLatLon(aoiOffset.row, aoiOffset.col);
            corners.upperRight = pixelToLatLon(aoiOffset.row, lastCol);
            corners.lowerRight = pixelToLatLon(lastRow, lastCol);
            corners.lowerLeft = pixelToLatLon(lastRow, aoiOffset.col);
            data->setImageCorners(corners);
        }
    }

    // Write the AOI SIDD out
    six::NITFWriteControl writer(container);
    writer.save(buffers.get(), outPathname, schemaPaths);
}
}
}
