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

#include <sys/Conf.h>
#include <except/Exception.h>
#include <mem/ScopedArray.h>
#include <six/NITFReadControl.h>
#include <six/NITFWriteControl.h>
#include <six/sicd/CropUtils.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexData.h>

namespace
{
class PixelToLatLon
{
public:
    PixelToLatLon(const six::sicd::ComplexData& data) :
        mGeom(six::sicd::Utilities::getSceneGeometry(&data)),
        mProjection(six::sicd::Utilities::getProjectionModel(&data,
                                                             mGeom.get())),
        mOffset(data.imageData->scpPixel.row -
                        static_cast<double>(data.imageData->firstRow),
                data.imageData->scpPixel.col -
                        static_cast<double>(data.imageData->firstCol)),
        mSampleSpacing(data.grid->row->sampleSpacing,
                       data.grid->col->sampleSpacing),
        mGroundPlaneNormal(mGeom->getReferencePosition())
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
                mProjection->imageToScene(imagePt,
                                          mGeom->getReferencePosition(),
                                          mGroundPlaneNormal,
                                          &timeCOA);

        const scene::LatLonAlt latLon(scene::Utilities::ecefToLatLon(groundPt));
        return scene::LatLon(latLon.getLat(), latLon.getLon());
    }

private:
    std::auto_ptr<const scene::SceneGeometry> mGeom;
    std::auto_ptr<const scene::ProjectionModel> mProjection;
    const types::RowCol<double> mOffset;
    const types::RowCol<double> mSampleSpacing;
    scene::Vector3 mGroundPlaneNormal;
};
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

    six::sicd::ComplexData* const data =
            reinterpret_cast<six::sicd::ComplexData*>(dataPtr);

    // Make sure the AOI is in bounds
    const types::RowCol<size_t> origDims(data->getNumRows(),
                                         data->getNumCols());
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
    const size_t numBytesPerPixel(data->getNumBytesPerPixel());
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
    // Construct the pixel --> lat/lon functor first to make sure updating
    // this metadata won't affect its calculations
    const PixelToLatLon pixelToLatLon(*data);

    data->imageData->firstRow += aoiOffset.row;
    data->imageData->firstCol += aoiOffset.col;
    data->imageData->numRows = aoiDims.row;
    data->imageData->numCols = aoiDims.col;

    const size_t firstRow(data->imageData->firstRow);
    const size_t lastRow(firstRow + aoiDims.row - 1);
    const size_t firstCol(data->imageData->firstCol);
    const size_t lastCol(firstCol + aoiDims.col - 1);

    six::LatLonCorners& corners(data->geoData->imageCorners);
    corners.upperLeft = pixelToLatLon(firstRow, firstCol);
    corners.upperRight = pixelToLatLon(firstRow, lastCol);
    corners.lowerRight = pixelToLatLon(lastRow, lastCol);
    corners.lowerLeft = pixelToLatLon(lastRow, firstCol);

    // Write the AOI SICD out
    six::NITFWriteControl writer;
    writer.initialize(container);
    six::BufferList images(1, buffer.get());
    writer.save(images, outPathname, schemaPaths);
}
}
}
