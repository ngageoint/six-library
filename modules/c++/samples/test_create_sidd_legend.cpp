/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

/**
 *  This test serves as an example to show how one can construct and write
 *  a SIDD NITF from scratch, using only a memory buffer.
 *
 *  The caller may optionally pass in a SICD XML file (XML only please),
 *  in which case that data will be read into ComplexData, and placed
 *  at the end of the Container.
 *
 *  NITF and GeoTIFF support are builtin.  If the suffix is .nitf or .ntf,
 *  the test will write a NITF.  Otherwise, if tiff support is enabled, it
 *  will attempt to write a GeoTIFF.  If tiff support is disabled in the
 *  library an exception will be thrown.  NITF is always written
 *  in big-endian.  Tiff output will always be in native endian.
 *
 *  The usage is extremely simple:
 *
 *  $ test_create_sidd_from_mem <output>.<nitf|tiff> (sicd-xml)
 *
 *
 *
 */

#include <stdexcept>
#include <iostream>
#include <memory>

#include <except/Exception.h>
#include <mem/ScopedArray.h>
#include <sys/Path.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedDataBuilder.h>
#include "utils.h"

namespace
{
std::auto_ptr<six::Data>
mockupDerivedData(const types::RowCol<size_t>& dims)
{
    six::PixelType pixelType = six::PixelType::MONO8I;

    six::sidd::DerivedDataBuilder siddBuilder;
    siddBuilder.addDisplay(pixelType);
    siddBuilder.addGeographicAndTarget(six::RegionType::GEOGRAPHIC_INFO);
    siddBuilder.addMeasurement(six::ProjectionType::PLANE).
            addExploitationFeatures(1);

    std::auto_ptr<six::sidd::DerivedData> siddData(siddBuilder.steal());

    siddData->setNumRows(dims.row);
    siddData->setNumCols(dims.col);
    siddData->setImageCorners(makeUpCornersFromDMS());

    siddData->productCreation->productName = "ProductName";
    siddData->productCreation->productClass = "Classy";
    siddData->productCreation->classification.classification = "U";

    siddData->productCreation->processorInformation->application = "ProcessorName";
    siddData->productCreation->processorInformation->profile = "Profile";
    siddData->productCreation->processorInformation->site = "Ypsilanti, MI";

    siddData->display->decimationMethod = six::DecimationMethod::BRIGHTEST_PIXEL;
    siddData->display->magnificationMethod =
            six::MagnificationMethod::NEAREST_NEIGHBOR;

    // We know this is PGD so this is safe
    six::sidd::PlaneProjection* const planeProjection =
        reinterpret_cast<six::sidd::PlaneProjection*>(
                siddData->measurement->projection.get());

    planeProjection->timeCOAPoly = six::Poly2D(0, 0);
    planeProjection->timeCOAPoly[0][0] = 1;
    siddData->measurement->arpPoly = six::PolyXYZ(0);
    siddData->measurement->arpPoly[0] = six::Vector3(0.0);
    planeProjection->productPlane.rowUnitVector = six::Vector3(0.0);
    planeProjection->productPlane.colUnitVector = six::Vector3(0.0);

    six::sidd::Collection* const parent =
            siddData->exploitationFeatures->collections[0].get();
    parent->information->resolution.rg = 0;
    parent->information->resolution.az = 0;
    parent->information->collectionDuration = 0;

    parent->information->collectionDateTime = six::DateTime();
    parent->information->radarMode = six::RadarModeType::SPOTLIGHT;
    parent->information->sensorName.clear();
    siddData->exploitationFeatures->product.resolution.row = 0;
    siddData->exploitationFeatures->product.resolution.col = 0;

    return siddData;
}
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 2)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " <output NITF pathname>\n\n";
            return 1;
        }
        const std::string outPathname(argv[1]);
        verifySchemaEnvVariableIsSet();

        // In order to make it easier to test segmenting, let's artifically set
        // the segment size really small
        const types::RowCol<size_t> dims(100, 200);
        const size_t numPixels(dims.row * dims.col);
        const size_t maxSize = dims.col * 50;

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(
                six::DataType::DERIVED,
                new six::XMLControlCreatorT<
                        six::sidd::DerivedXMLControl>());



        six::Container container(six::DataType::DERIVED);
        std::auto_ptr<six::Data> data(mockupDerivedData(dims));

        const types::RowCol<size_t> legendDims(50, 50);
        std::auto_ptr<six::Legend> legend(new six::Legend());
        legend->mType = six::PixelType::RGB8LU;
        legend->mLocation.row = 10;
        legend->mLocation.col = 10;
        legend->setDims(legendDims);
        legend->mLUT.reset(new six::LUT(256, 3));
        for (size_t ii = 0, idx = 0;
             ii < legend->mLUT->numEntries;
             ++ii, idx += 3)
        {
            legend->mLUT->table[idx] = ii;
            legend->mLUT->table[idx + 1] = ii;
            legend->mLUT->table[idx + 2] = ii;
        }

        container.addData(data, legend);

        six::NITFWriteControl writer;

        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE,
                str::toString(maxSize));

        writer.setXMLControlRegistry(&xmlRegistry);
        writer.initialize(&container);

        const mem::ScopedArray<sys::ubyte> buffer(new sys::ubyte[numPixels]);
        std::fill_n(buffer.get(), numPixels, 0);

        writer.save(buffer.get(), outPathname);

        return 0;
    }
    catch (const except::Exception& e)
    {
        std::cerr << "Caught exception: " << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}
