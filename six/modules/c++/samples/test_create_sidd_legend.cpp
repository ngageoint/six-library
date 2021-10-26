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
 *  a SIDD NITF with legends from scratch, using only a memory buffer.
 *
 *  It creates NITFs with and without blocking and adds derived products of
 *  varying numbers of image segments with and without legends.
 *
 *  The usage is extremely simple:
 *
 *  $ test_create_sidd_legend <output NITF prefix>
 *
 */

#include <stdexcept>
#include <iostream>
#include <memory>

#include <std/filesystem>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <mem/ScopedArray.h>
#include <sys/Path.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedDataBuilder.h>
#include <six/NITFHeaderCreator.h>
#include "utils.h"

namespace fs = std::filesystem;


namespace
{
std::unique_ptr<six::Data>
mockupDerivedData(const types::RowCol<size_t>& dims)
{
    six::PixelType pixelType = six::PixelType::MONO8I;

    six::sidd::DerivedDataBuilder siddBuilder;
    siddBuilder.addDisplay(pixelType);
    siddBuilder.addGeographicAndTarget(six::RegionType::GEOGRAPHIC_INFO);
    siddBuilder.addMeasurement(six::ProjectionType::PLANE).
            addExploitationFeatures(1);

    six::sidd::DerivedData* siddData = siddBuilder.steal();
    std::unique_ptr<six::Data> siddDataScoped(siddData);

    setExtent(*siddData, dims);
    siddData->setImageCorners(makeUpCornersFromDMS());

    siddData->productCreation->productName = "ProductName";
    siddData->productCreation->productClass = "Classy";
    siddData->productCreation->classification.classification = "U";

    siddData->productCreation->processorInformation.application = "ProcessorName";
    siddData->productCreation->processorInformation.profile = "Profile";
    siddData->productCreation->processorInformation.site = "Ypsilanti, MI";

    siddData->display->decimationMethod = six::DecimationMethod::BRIGHTEST_PIXEL;
    siddData->display->magnificationMethod =
            six::MagnificationMethod::NEAREST_NEIGHBOR;

    six::Parameter param;
    param.setName("GeoName");
    param.setValue("GeoValue");
    siddData->geographicAndTarget.reset(new six::sidd::GeographicAndTarget());
    siddData->geographicAndTarget->geographicCoverage.reset(
            new six::sidd::GeographicCoverage(six::RegionType::GEOGRAPHIC_INFO));

    six::sidd::GeographicCoverage* geoCoverage =
            siddData->geographicAndTarget->geographicCoverage.get();
    geoCoverage->georegionIdentifiers.push_back(param);
    geoCoverage->footprint.getCorner(0).setLat(10);
    geoCoverage->footprint.getCorner(0).setLon(30);

    geoCoverage->footprint.getCorner(1).setLat(11);
    geoCoverage->footprint.getCorner(1).setLon(30);

    geoCoverage->footprint.getCorner(2).setLat(11);
    geoCoverage->footprint.getCorner(2).setLon(31);

    geoCoverage->footprint.getCorner(3).setLat(10);
    geoCoverage->footprint.getCorner(3).setLon(31);

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
    parent->information.resolution.rg = 0;
    parent->information.resolution.az = 0;
    parent->information.collectionDuration = 0;

    parent->information.collectionDateTime = six::DateTime();
    parent->information.radarMode = six::RadarModeType::SPOTLIGHT;
    parent->information.sensorName.clear();

    siddData->exploitationFeatures->product.resize(1);
    siddData->exploitationFeatures->product[0].resolution.row = 0;
    siddData->exploitationFeatures->product[0].resolution.col = 0;

    return siddDataScoped;
}
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 2)
        {
            std::cerr << "Usage: " << fs::path(argv[0]).filename().string()
                      << " <output NITF pathname prefix>\n\n";
            return 1;
        }
        const std::string outPathnamePrefix(argv[1]);

        // In order to make it easier to test segmenting, let's artificially set
        // the segment size really small
        const size_t numCols = 200;

        const size_t maxSize = numCols * 50;

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

        mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::DERIVED));

        six::buffer_list buffers;

        // First a single segment without a legend
        types::RowCol<size_t> dims1(40, numCols);
        std::unique_ptr<six::Data> data1(mockupDerivedData(dims1));

        std::vector<std::byte> buffer1(dims1.area());
        std::fill_n(buffer1.data(), dims1.area(), static_cast<std::byte>(20));

        container->addData(std::move(data1));
        buffers.push_back(buffer1);

        // Now a single segment with a mono legend
        types::RowCol<size_t> dims2(40, numCols);
        std::unique_ptr<six::Data> data2(mockupDerivedData(dims2));

        const types::RowCol<size_t> legendDims(50, 50);
        std::unique_ptr<six::Legend> monoLegend(new six::Legend());
        monoLegend->mType = six::PixelType::MONO8I;
        monoLegend->mLocation.row = 10;
        monoLegend->mLocation.col = 10;
        monoLegend->setDims(legendDims);

        std::vector<std::byte> buffer2(dims2.area());
        std::fill_n(buffer2.data(), dims2.area(), static_cast<std::byte>(100));

        container->addData(std::move(data2), std::move(monoLegend));
        buffers.push_back(buffer2);

        // Now a multi-segment without a legend
        types::RowCol<size_t> dims3(150, numCols);
        std::unique_ptr<six::Data> data3(mockupDerivedData(dims3));

        std::vector<std::byte> buffer3(dims3.area());
        std::fill_n(buffer3.data(), dims3.area(), static_cast<std::byte>(60));

        container->addData(std::move(data3));
        buffers.push_back(buffer3);

        // Now a multi-segment with an RGB legend
        types::RowCol<size_t> dims4(155, numCols);
        std::unique_ptr<six::Data> data4(mockupDerivedData(dims4));

        std::unique_ptr<six::Legend> rgbLegend(new six::Legend());
        rgbLegend->mType = six::PixelType::RGB8LU;
        rgbLegend->mLocation.row = 10;
        rgbLegend->mLocation.col = 10;
        rgbLegend->setDims(legendDims);
        rgbLegend->mLUT.reset(new six::LUT(256, 3));
        for (size_t ii = 0, idx = 0;
             ii < rgbLegend->mLUT->numEntries;
             ++ii, idx += 3)
        {
            unsigned char lutValue = static_cast<unsigned char>(ii);
            rgbLegend->mLUT->getTable()[idx] = lutValue;
            rgbLegend->mLUT->getTable()[idx + 1] = lutValue;
            rgbLegend->mLUT->getTable()[idx + 2] = lutValue;
        }

        std::vector<std::byte> buffer4(dims4.area());
        std::fill_n(buffer4.data(), dims4.area(), static_cast<std::byte>(200));

        container->addData(std::move(data4), std::move(rgbLegend));
        buffers.push_back(buffer4);

        // Write it out
        {
            six::Options options;
            options.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                    maxSize);

            six::NITFWriteControl writer(options, container, &xmlRegistry);
            writer.save(buffers, outPathnamePrefix + "_unblocked.nitf");
        }

        // Write it out with blocking
        {
            six::Options options;
            options.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                    maxSize);

            const std::string blockSize("23");
            options.setParameter(
                    six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK,
                    blockSize);

            options.setParameter(
                    six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK,
                    blockSize);

            six::NITFWriteControl writer(options, container, &xmlRegistry);
            writer.save(buffers, outPathnamePrefix + "_blocked.nitf");
        }

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
