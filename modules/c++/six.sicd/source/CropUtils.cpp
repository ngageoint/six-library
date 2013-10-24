/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include <memory>
#include <algorithm>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <mem/ScopedArray.h>
#include <scene/LLAToECEFTransform.h>
#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <six/NITFReadControl.h>
#include <six/NITFWriteControl.h>
#include <six/sicd/CropUtils.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexData.h>

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

class PixelToLatLon
{
public:
    PixelToLatLon(const six::sicd::ComplexData& data,
                  const scene::SceneGeometry& geom,
                  const scene::ProjectionModel& projection) :
        mGeom(geom),
        mProjection(projection),
        mOffset(data.imageData->scpPixel.row -
                        static_cast<double>(data.imageData->firstRow),
                data.imageData->scpPixel.col -
                        static_cast<double>(data.imageData->firstCol)),
        mSampleSpacing(data.grid->row->sampleSpacing,
                       data.grid->col->sampleSpacing),
        mGroundPlaneNormal(mGeom.getReferencePosition())
    {
        mGroundPlaneNormal.normalize();
    }

    scene::LatLon operator()(size_t row, size_t col) const
    {
        const types::RowCol<double> imagePt(
                (row - mOffset.row) * mSampleSpacing.row,
                (col - mOffset.col) * mSampleSpacing.col);

        double timeCOA(0.0);
        const scene::Vector3 groundPt =
                mProjection.imageToScene(imagePt,
                                         mGeom.getReferencePosition(),
                                         mGroundPlaneNormal,
                                         &timeCOA);

        const scene::LatLonAlt latLon(scene::Utilities::ecefToLatLon(groundPt));
        return scene::LatLon(latLon.getLat(), latLon.getLon());
    }

private:
    const scene::SceneGeometry& mGeom;
    const scene::ProjectionModel& mProjection;
    const types::RowCol<double> mOffset;
    const types::RowCol<double> mSampleSpacing;
    scene::Vector3 mGroundPlaneNormal;
};

void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const six::sicd::ComplexData& data,
              const scene::SceneGeometry& geom,
              const scene::ProjectionModel& projection,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname)
{
    // Make sure the AOI is in bounds
    const types::RowCol<size_t> origDims(data.getNumRows(),
                                         data.getNumCols());
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
    const size_t numBytesPerPixel(data.getNumBytesPerPixel());
    const size_t numBytes(origDims.row * origDims.col * numBytesPerPixel);
    const mem::ScopedArray<sys::ubyte> buffer(new sys::ubyte[numBytes]);

    six::Region region;
    region.setStartRow(aoiOffset.row);
    region.setStartCol(aoiOffset.col);
    region.setNumRows(aoiDims.row);
    region.setNumCols(aoiDims.col);
    region.setBuffer(buffer.get());
    reader.interleaved(region, 0);

    // Update to reflect the AOI in the SIX metadata
    six::sicd::ComplexData* const aoiData(
            reinterpret_cast<six::sicd::ComplexData*>(data.clone()));
    std::auto_ptr<six::Data> scopedData(aoiData);

    aoiData->imageData->firstRow += aoiOffset.row;
    aoiData->imageData->firstCol += aoiOffset.col;
    aoiData->imageData->numRows = aoiDims.row;
    aoiData->imageData->numCols = aoiDims.col;

    const size_t firstRow(aoiData->imageData->firstRow);
    const size_t lastRow(firstRow + aoiDims.row - 1);
    const size_t firstCol(aoiData->imageData->firstCol);
    const size_t lastCol(firstCol + aoiDims.col - 1);

    const PixelToLatLon pixelToLatLon(data, geom, projection);
    six::LatLonCorners& corners(aoiData->geoData->imageCorners);
    corners.upperLeft = pixelToLatLon(firstRow, firstCol);
    corners.upperRight = pixelToLatLon(firstRow, lastCol);
    corners.lowerRight = pixelToLatLon(lastRow, lastCol);
    corners.lowerLeft = pixelToLatLon(lastRow, firstCol);

    // Write the AOI SICD out
    six::Container container(six::DataType::COMPLEX);
    container.addData(scopedData);
    six::NITFWriteControl writer;
    writer.initialize(&container);
    six::BufferList images(1, buffer.get());
    writer.save(images, outPathname, schemaPaths);
}
}

namespace six
{
namespace sicd
{
void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname)
{
    // Make sure it's a SICD
    six::NITFReadControl reader;
    reader.load(inPathname, schemaPaths);
    six::Container* container = reader.getContainer();

    six::Data* const dataPtr = container->getData(0);
    if (container->getDataType() != six::DataType::COMPLEX ||
        dataPtr->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt(inPathname + " is not a SICD"));
    }

    const ComplexData* const data = reinterpret_cast<ComplexData*>(dataPtr);

    // Build up the geometry info
    std::auto_ptr<const scene::SceneGeometry> geom(
            six::sicd::Utilities::getSceneGeometry(data));

    std::auto_ptr<const scene::ProjectionModel> projection(
            six::sicd::Utilities::getProjectionModel(data, geom.get()));

    // Actually do the cropping
    ::cropSICD(reader, schemaPaths, *data, *geom, *projection,
               aoiOffset, aoiDims, outPathname);
}

void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::Vector3>& corners,
              const std::string& outPathname)
{
    if (corners.size() != 4)
    {
        throw except::Exception(Ctxt("Expected four corners but got " +
                str::toString(corners.size())));
    }

    // Make sure it's a SICD
    six::NITFReadControl reader;
    reader.load(inPathname, schemaPaths);
    six::Container* container = reader.getContainer();

    six::Data* const dataPtr = container->getData(0);
    if (container->getDataType() != six::DataType::COMPLEX ||
        dataPtr->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt(inPathname + " is not a SICD"));
    }

    const six::sicd::ComplexData* const data =
            reinterpret_cast<six::sicd::ComplexData*>(dataPtr);

    // Convert ECEF corners to slant pixel pixels
    const ImageData& imageData(*data->imageData);
    const types::RowCol<double> aoiOffset(imageData.firstRow,
                                          imageData.firstCol);

    const types::RowCol<double> offset(
            imageData.scpPixel.row - aoiOffset.row,
            imageData.scpPixel.col - aoiOffset.col);

    std::auto_ptr<const scene::SceneGeometry> geom(
            six::sicd::Utilities::getSceneGeometry(data));

    std::auto_ptr<const scene::ProjectionModel> projection(
            six::sicd::Utilities::getProjectionModel(data, geom.get()));

    types::RowCol<double> minPixel(data->getNumRows(), data->getNumCols());
    types::RowCol<double> maxPixel(0.0, 0.0);
    for (size_t ii = 0; ii < corners.size(); ++ii)
    {
        const types::RowCol<double> imagePt =
                projection->sceneToImage(corners[ii], NULL);

        const types::RowCol<double> spPixel(
                (imagePt.row / data->grid->row->sampleSpacing) + offset.row,
                (imagePt.col / data->grid->col->sampleSpacing) + offset.col);

        updateMinMax(spPixel.row, minPixel.row, maxPixel.row);
        updateMinMax(spPixel.col, minPixel.col, maxPixel.col);

    }

    const types::RowCol<size_t> upperLeft(
            std::floor(std::max(minPixel.row, 0.0)),
            std::floor(std::max(minPixel.col, 0.0)));

    const types::RowCol<size_t> lowerRight(
            std::ceil(std::min(maxPixel.row, data->getNumRows() - 1.0)),
            std::ceil(std::min(maxPixel.col, data->getNumCols() - 1.0)));

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
              const std::string& outPathname)
{
    std::vector<scene::Vector3> ecefCorners(corners.size());
    scene::LLAToECEFTransform toECEF;
    for (size_t ii = 0; ii < corners.size(); ++ii)
    {
        ecefCorners[ii] = toECEF.transform(corners[ii]);
    }

    cropSICD(inPathname, schemaPaths, ecefCorners, outPathname);
}
}
}
