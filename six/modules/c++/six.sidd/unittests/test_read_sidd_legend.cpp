/* =========================================================================
 * This file is part of six.sidd-c++
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

#include <stdexcept>
#include <iostream>
#include <memory>

#include <std/filesystem>

#include "TestCase.h"

#include <except/Exception.h>
#include <mem/ScopedArray.h>
#include <sys/Path.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedDataBuilder.h>
#include <six/NITFReadControl.h>
#include <six/NITFWriteControl.h>
#include <six/NITFHeaderCreator.h>

namespace fs = std::filesystem;

namespace
{
six::LatLonCorners makeUpCornersFromDMS()
{
    int latTopDMS[3] = { 42, 17, 50 };
    int latBottomDMS[3] = { 42, 15, 14 };
    int lonEastDMS[3] = { -83, 42, 12 };
    int lonWestDMS[3] = { -83, 45, 44 };

    const double latTopDecimal =
        nitf::Utils::geographicToDecimal(latTopDMS[0],
                                         latTopDMS[1],
                                         latTopDMS[2]);

    const double latBottomDecimal =
        nitf::Utils::geographicToDecimal(latBottomDMS[0],
                                         latBottomDMS[1],
                                         latBottomDMS[2]);

    const double lonEastDecimal =
        nitf::Utils::geographicToDecimal(lonEastDMS[0],
                                         lonEastDMS[1],
                                         lonEastDMS[2]);

    const double lonWestDecimal =
        nitf::Utils::geographicToDecimal(lonWestDMS[0],
                                         lonWestDMS[1],
                                         lonWestDMS[2]);

    six::LatLonCorners corners;
    corners.upperLeft = six::LatLon(latTopDecimal, lonWestDecimal);
    corners.upperRight = six::LatLon(latTopDecimal, lonEastDecimal);
    corners.lowerRight = six::LatLon(latBottomDecimal, lonEastDecimal);
    corners.lowerLeft = six::LatLon(latBottomDecimal, lonWestDecimal);
    return corners;
}

mem::auto_ptr<six::Data>
mockupDerivedData(const types::RowCol<size_t>& dims)
{
    six::PixelType pixelType = six::PixelType::MONO8I;

    six::sidd::DerivedDataBuilder siddBuilder;
    siddBuilder.addDisplay(pixelType);
    siddBuilder.addGeographicAndTarget(six::RegionType::GEOGRAPHIC_INFO);
    siddBuilder.addMeasurement(six::ProjectionType::PLANE).
            addExploitationFeatures(1);

    six::sidd::DerivedData* siddData = siddBuilder.steal();
    mem::auto_ptr<six::Data> siddDataScoped(siddData);

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

    // We know this is PGD so this is safe
    six::sidd::PlaneProjection* const planeProjection =
        static_cast<six::sidd::PlaneProjection*>(
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
    siddData->geographicAndTarget->geographicCoverage.reset(
            new six::sidd::GeographicCoverage(
            six::RegionType::GEOGRAPHIC_INFO));

    six::LatLonCorners& corners =
        siddData->geographicAndTarget->geographicCoverage->footprint;
    corners.getCorner(0).setLat(1);
    corners.getCorner(0).setLon(2);

    corners.getCorner(1).setLat(2);
    corners.getCorner(1).setLon(3);

    corners.getCorner(2).setLat(3);
    corners.getCorner(2).setLon(4);

    corners.getCorner(3).setLat(4);
    corners.getCorner(3).setLon(5);

    return siddDataScoped;
}

struct TestHelper
{
    TestHelper() :
        mPathname("test_read_sidd_legend.nitf")
    {
        mXmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

        mMonoLegend.setDims(types::RowCol<size_t>(12, 34));
        mMonoLegend.mType = six::PixelType::MONO8I;
        mMonoLegend.mLocation.row = 56;
        mMonoLegend.mLocation.col = 78;

        mRgbLegend.setDims(types::RowCol<size_t>(123, 456));
        mRgbLegend.mType = six::PixelType::RGB8LU;
        mRgbLegend.mLocation.row = 98765;
        mRgbLegend.mLocation.col = 43217;

        mRgbLegend.mLUT.reset(new six::LUT(256, 3));
        for (size_t ii = 0, idx = 0;
             ii < mRgbLegend.mLUT->numEntries;
             ++ii, idx += 3)
        {
            const unsigned char lutValue = static_cast<unsigned char>(ii);
            mRgbLegend.mLUT->getTable()[idx] = lutValue;
            mRgbLegend.mLUT->getTable()[idx + 1] = lutValue;
            mRgbLegend.mLUT->getTable()[idx + 2] = lutValue;
        }

        write();
    }

    ~TestHelper()
    {
        try
        {
           fs::remove(mPathname);
        }
        catch (...)
        {
        }
    }

    void write()
    {
        // In order to make it easier to test segmenting, let's artificially
        // set the segment size really small
        const size_t numCols = 200;

        const size_t maxSize = numCols * 50;

        mem::SharedPtr<six::Container> container(new six::Container(
                six::DataType::DERIVED));

        std::vector<six::UByte*> buffers;

        // First a single segment without a legend
        types::RowCol<size_t> dims1(40, numCols);
        mem::auto_ptr<six::Data> data1(mockupDerivedData(dims1));

        const mem::ScopedArray<sys::ubyte> buffer1(new sys::ubyte[dims1.area()]);
        std::fill_n(buffer1.get(), dims1.area(), static_cast<sys::ubyte>(20));

        container->addData(std::move(data1));
        buffers.push_back(buffer1.get());

        // Now a single segment with a mono legend
        types::RowCol<size_t> dims2(40, numCols);
        mem::auto_ptr<six::Data> data2(mockupDerivedData(dims2));

        const mem::ScopedArray<sys::ubyte> buffer2(new sys::ubyte[dims2.area()]);
        std::fill_n(buffer2.get(), dims2.area(), static_cast<sys::ubyte>(100));

        mem::auto_ptr<six::Legend> monoLegend(new six::Legend(mMonoLegend));
        container->addData(std::move(data2), std::move(monoLegend));
        buffers.push_back(buffer2.get());

        // Now a multi-segment without a legend
        types::RowCol<size_t> dims3(150, numCols);
        mem::auto_ptr<six::Data> data3(mockupDerivedData(dims3));

        const mem::ScopedArray<sys::ubyte> buffer3(new sys::ubyte[dims3.area()]);
        std::fill_n(buffer3.get(), dims3.area(), static_cast<sys::ubyte>(60));

        container->addData(std::move(data3));
        buffers.push_back(buffer3.get());

        // Now a multi-segment with an RGB legend
        types::RowCol<size_t> dims4(155, numCols);
        mem::auto_ptr<six::Data> data4(mockupDerivedData(dims4));

        mem::auto_ptr<six::Legend> rgbLegend(new six::Legend(mRgbLegend));

        const mem::ScopedArray<sys::ubyte> buffer4(new sys::ubyte[dims4.area()]);
        std::fill_n(buffer4.get(), dims4.area(), static_cast<sys::ubyte>(200));

        container->addData(std::move(data4), std::move(rgbLegend));
        buffers.push_back(buffer4.get());

        // Write it out
        six::Options options;
        options.setParameter(
                six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                maxSize);

        six::NITFWriteControl writer(options, container, &mXmlRegistry);
        writer.save(buffers, mPathname);
    }

    const std::string mPathname;
    six::XMLControlRegistry mXmlRegistry;

    six::Legend mMonoLegend;
    six::Legend mRgbLegend;
};


TEST_CASE(testRead)
{
    TestHelper testHelper;
    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&testHelper.mXmlRegistry);

    reader.load(testHelper.mPathname);
    const auto container = reader.getContainer();

    TEST_ASSERT_EQ(container->getNumData(), 4);
    for (size_t ii = 0; ii < container->getNumData(); ++ii)
    {
        TEST_ASSERT_NOT_EQ(container->getData(ii), nullptr);
    }

    // First image shouldn't have a legend
    TEST_ASSERT_NULL(container->getLegend(0));

    // Second image should have a legend equal to the mono legend
    const six::Legend* legend = container->getLegend(1);
    TEST_ASSERT_NOT_EQ(legend, nullptr);
    TEST_ASSERT_EQ(legend->mType, testHelper.mMonoLegend.mType);
    TEST_ASSERT_EQ(legend->mLocation.row,
                   testHelper.mMonoLegend.mLocation.row);
    TEST_ASSERT_EQ(legend->mLocation.col,
                   testHelper.mMonoLegend.mLocation.col);
    TEST_ASSERT_EQ(legend->mDims.row, testHelper.mMonoLegend.mDims.row);
    TEST_ASSERT_EQ(legend->mDims.col, testHelper.mMonoLegend.mDims.col);
    TEST_ASSERT(legend->mImage == testHelper.mMonoLegend.mImage);
    TEST_ASSERT_NULL(legend->mLUT.get());

    // Third image shouldn't have a legend
    TEST_ASSERT_NULL(container->getLegend(2));

    // Fourth image should have a legend equal to the RGB legend
    legend = container->getLegend(3);
    TEST_ASSERT_NOT_EQ(legend, nullptr);
    TEST_ASSERT_EQ(legend->mType, testHelper.mRgbLegend.mType);
    TEST_ASSERT_EQ(legend->mLocation.row,
                   testHelper.mRgbLegend.mLocation.row);
    TEST_ASSERT_EQ(legend->mLocation.col,
                   testHelper.mRgbLegend.mLocation.col);
    TEST_ASSERT_EQ(legend->mDims.row, testHelper.mRgbLegend.mDims.row);
    TEST_ASSERT_EQ(legend->mDims.col, testHelper.mRgbLegend.mDims.col);
    TEST_ASSERT(legend->mImage == testHelper.mRgbLegend.mImage);
    TEST_ASSERT_NOT_EQ(legend->mLUT.get(), nullptr);
    TEST_ASSERT(*legend->mLUT == *testHelper.mRgbLegend.mLUT);
}
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(testRead);
)
