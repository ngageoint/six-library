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
#include <six/sicd/CropUtils.h>

#include <memory>
#include <algorithm>
#include <string>
#include <std/span>
#include <std/cstddef>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <str/Convert.h>
#include <mem/ScopedArray.h>
#include <sys/Path.h>

#include <six/NITFWriteControl.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/SlantPlanePixelTransformer.h>

#undef min
#undef max

namespace
{
void updateMinMax(double val, double& curMin, double& curMax)
{
    if (val < curMin)
    {
        curMin = val;
    }

    if (val > curMax)
    {
        curMax = val;
    }
}

six::sicd::ComplexData* updateMetadata(
        const six::sicd::ComplexData& data,
        const scene::SceneGeometry& geom,
        const scene::ProjectionModel& projection,
        const types::RowCol<size_t>& aoiOffset,
        const types::RowCol<size_t>& aoiDims)
{
    six::sicd::ComplexData* const aoiData(
               dynamic_cast<six::sicd::ComplexData*>(data.clone()));

    aoiData->imageData->firstRow += aoiOffset.row;
    aoiData->imageData->firstCol += aoiOffset.col;
    aoiData->imageData->numRows = aoiDims.row;
    aoiData->imageData->numCols = aoiDims.col;

    const size_t firstRow(aoiData->imageData->firstRow);
    const size_t lastRow(firstRow + aoiDims.row - 1);
    const size_t firstCol(aoiData->imageData->firstCol);
    const size_t lastCol(firstCol + aoiDims.col - 1);

    const six::sicd::SlantPlanePixelTransformer trans(data, geom, projection);
    six::LatLonCorners& corners(aoiData->geoData->imageCorners);

    corners.upperLeft = trans.toLatLon(
            types::RowCol<size_t>(firstRow, firstCol));
    corners.upperRight = trans.toLatLon(
            types::RowCol<size_t>(firstRow, lastCol));
    corners.lowerRight = trans.toLatLon(
            types::RowCol<size_t>(lastRow, lastCol));
    corners.lowerLeft = trans.toLatLon(
            types::RowCol<size_t>(lastRow, firstCol));

    return aoiData;
}

void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths_,
              const six::sicd::ComplexData& data,
              const scene::SceneGeometry& geom,
              const scene::ProjectionModel& projection,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname)
{
    // Make sure the AOI is in bounds
    const auto origDims = getExtent(data);

    if (aoiOffset.row + aoiDims.row > origDims.row ||
        aoiOffset.col + aoiDims.col > origDims.col)
    {
        throw except::Exception(Ctxt("AOI dimensions are out of bounds"));
    }

    if (aoiDims.row < 1 || aoiDims.col < 1)
    {
        throw except::Exception(Ctxt("AOI must be non-empty"));
    }

    // Read in the AOI
    //const size_t numBytesPerPixel(data.getNumBytesPerPixel());
    six::Region region;
    setOffset(region, aoiOffset);
    setDims(region, aoiDims);
    const auto buffer = region.setComplexBuffer(origDims.area());
    reader.interleaved(region, 0);

    std::unique_ptr<six::Data> aoiData(updateMetadata(
            data, geom,  projection,
            aoiOffset, aoiDims));

    // Write the AOI SICD out
    six::NITFWriteControl writer(std::move(aoiData));
    const std::span<const six::zfloat> image(buffer.get(), origDims.area());
    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    writer.save_image(image, outPathname, schemaPaths);
}

}

namespace six
{
namespace sicd
{

std::unique_ptr<six::sicd::ComplexData> cropMetaData(
        const six::sicd::ComplexData& complexData,
        const types::RowCol<size_t>& aoiOffset,
        const types::RowCol<size_t>& aoiDims)
{
    // Build up the geometry info
    std::unique_ptr<const scene::SceneGeometry> geom(
            six::sicd::Utilities::getSceneGeometry(&complexData));

    std::unique_ptr<const scene::ProjectionModel> projection(
            six::sicd::Utilities::getProjectionModel(&complexData, geom.get()));

    six::sicd::ComplexData* const aoiData = updateMetadata(
            complexData,
            *geom,
            *projection,
            aoiOffset,
            aoiDims);

    return std::unique_ptr<six::sicd::ComplexData>(aoiData);
}

void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname)
{
    six::NITFReadControl reader;
    reader.load(inPathname, schemaPaths);
    cropSICD(reader, schemaPaths, aoiOffset, aoiDims, outPathname);
}

void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname)
{
    // Make sure it's a SICD
    const auto container = reader.getContainer();

    const six::Data* const dataPtr = container->getData(0);
    if (container->getDataType() != six::DataType::COMPLEX ||
        dataPtr->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt("Input is not a SICD"));
    }

    const ComplexData* const data =
        dynamic_cast<const ComplexData*>(dataPtr);

    // Build up the geometry info
    std::unique_ptr<const scene::SceneGeometry> geom(
            six::sicd::Utilities::getSceneGeometry(data));

    std::unique_ptr<const scene::ProjectionModel> projection(
            six::sicd::Utilities::getProjectionModel(data, geom.get()));

    // Actually do the cropping
    ::cropSICD(reader, schemaPaths, *data, *geom, *projection,
               aoiOffset, aoiDims, outPathname);
}

void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::Vector3>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded)
{
    six::NITFReadControl reader;
    reader.load(inPathname, schemaPaths);
    cropSICD(reader, schemaPaths, corners, outPathname, trimCornersIfNeeded);
}

void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::Vector3>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded)
{
    if (corners.size() != 4)
    {
        throw except::Exception(Ctxt("Expected four corners but got " +
                std::to_string(corners.size())));
    }

    // Make sure it's a SICD
    const auto container = reader.getContainer();

    const six::Data* const dataPtr = container->getData(0);
    if (container->getDataType() != six::DataType::COMPLEX ||
        dataPtr->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt("Input is not a SICD"));
    }

    const six::sicd::ComplexData* const data =
        dynamic_cast<const six::sicd::ComplexData*>(dataPtr);

    // Convert ECEF corners to slant pixel pixels
    const ImageData& imageData(*data->imageData);
    const types::RowCol<double> aoiOffset(static_cast<double>(imageData.firstRow),
                                          static_cast<double>(imageData.firstCol));

    const types::RowCol<double> offset(
            static_cast<double>(imageData.scpPixel.row) - aoiOffset.row,
            static_cast<double>(imageData.scpPixel.col) - aoiOffset.col);

    std::unique_ptr<const scene::SceneGeometry> geom(
            six::sicd::Utilities::getSceneGeometry(data));

    std::unique_ptr<const scene::ProjectionModel> projection(
            six::sicd::Utilities::getProjectionModel(data, geom.get()));

    types::RowCol<double> minPixel(getExtent(*data));
    types::RowCol<double> maxPixel(0.0, 0.0);
    for (size_t ii = 0; ii < corners.size(); ++ii)
    {
        const types::RowCol<double> imagePt =
                projection->sceneToImage(corners[ii], nullptr);

        const types::RowCol<double> spPixel(
                (imagePt.row / data->grid->row->sampleSpacing) + offset.row,
                (imagePt.col / data->grid->col->sampleSpacing) + offset.col);

        updateMinMax(spPixel.row, minPixel.row, maxPixel.row);
        updateMinMax(spPixel.col, minPixel.col, maxPixel.col);
    }

    // Floor the min pixel and ceiling the max pixel
    minPixel.row = std::floor(minPixel.row);
    minPixel.col = std::floor(minPixel.col);
    maxPixel.row = std::ceil(maxPixel.row);
    maxPixel.col = std::ceil(maxPixel.col);

    const types::RowCol<double> lastDim(static_cast<double>(data->getNumRows()) - 1.0,
                                        static_cast<double>(data->getNumCols()) - 1.0);

    if (!trimCornersIfNeeded &&
        (minPixel.row < 0 ||
         minPixel.col < 0 ||
         maxPixel.row > lastDim.row ||
         maxPixel.col > lastDim.col))
    {
        throw except::Exception(Ctxt(
                "One or more corners are outside of the image bounds"));
    }

    const types::RowCol<size_t> upperLeft(RowColDouble(std::max(minPixel.row, 0.0), std::max(minPixel.col, 0.0)));
    const types::RowCol<size_t> lowerRight(RowColDouble(std::min(maxPixel.row, lastDim.row), std::min(maxPixel.col, lastDim.col)));

    // This would only happen if the "upper left" corner was actually below or
    // to the right of the footprint (the lower right corner would have been
    // as well before we min()'d it)
    if (upperLeft.row >= lowerRight.row || upperLeft.col >= lowerRight.col)
    {
        throw except::Exception(Ctxt("AOI is outside of the footprint"));
    }

    const types::RowCol<size_t> aoiDims(lowerRight.row - upperLeft.row + 1,
                                        lowerRight.col - upperLeft.col + 1);

    // Actually do the cropping
    ::cropSICD(reader, schemaPaths, *data, *geom, *projection,
               upperLeft, aoiDims, outPathname);
}

void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::LatLonAlt>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded)
{
    six::NITFReadControl reader;
    reader.load(inPathname, schemaPaths);
    cropSICD(reader, schemaPaths, corners, outPathname, trimCornersIfNeeded);
}

void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::LatLonAlt>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded)
{

    std::vector<scene::Vector3> ecefCorners(corners.size());
    scene::LLAToECEFTransform toECEF;
    for (size_t ii = 0; ii < corners.size(); ++ii)
    {
        ecefCorners[ii] = toECEF.transform(corners[ii]);
    }

    cropSICD(reader, schemaPaths, ecefCorners, outPathname,
             trimCornersIfNeeded);
}
}
}
