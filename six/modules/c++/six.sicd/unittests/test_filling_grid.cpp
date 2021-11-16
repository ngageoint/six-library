/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; If not,
* see <http://www.gnu.org/licenses/>.
*
*/

#include <import/six/sicd.h>
#include "TestCase.h"

TEST_CASE(DerivedDeltaKsNoImageData)
{
    six::sicd::ImageData imageData;
    six::sicd::DirectionParameters row;
    row.impulseResponseBandwidth = 13;
    row.sampleSpacing = 5;
    row.deltaKCOAPoly = six::Poly2D(3, 3);
    for (size_t ii = 0; ii <= row.deltaKCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= row.deltaKCOAPoly.orderY(); ++jj)
        {
            row.deltaKCOAPoly[ii][jj] = static_cast<double>(ii + jj);
        }
    }
    row.fillDerivedFields(imageData);
    TEST_ASSERT_ALMOST_EQ(row.deltaK1, -0.1);
    TEST_ASSERT_ALMOST_EQ(row.deltaK2, 0.1);
}

TEST_CASE(DerivedDeltaKsWithImageData)
{
    six::sicd::ImageData imageData;
    imageData.validData.push_back(six::RowColInt(10, 20));
    imageData.validData.push_back(six::RowColInt(30, 40));
    imageData.validData.push_back(six::RowColInt(50, 60));
    six::sicd::DirectionParameters row;
    row.impulseResponseBandwidth = 13;
    row.sampleSpacing = 5;
    row.deltaKCOAPoly = six::Poly2D(3, 3);
    for (size_t ii = 0; ii <= row.deltaKCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= row.deltaKCOAPoly.orderY(); ++jj)
        {
            row.deltaKCOAPoly[ii][jj] = static_cast<double>(ii + jj);
        }
    }
    row.fillDerivedFields(imageData);
    //TODO: find test data where these numbers aren't identical to above test
    TEST_ASSERT_ALMOST_EQ(row.deltaK1, -0.1);
    TEST_ASSERT_ALMOST_EQ(row.deltaK2, 0.1);
}

TEST_CASE(IdentityWeightFunction)
{
    six::sicd::DirectionParameters row;
    row.weightType.reset(new six::sicd::WeightType());
    row.weightType->windowName = "UNIFORM";

    // This shouldn't affect anything, but it's a required argument
    six::sicd::ImageData imageData;
    row.fillDerivedFields(imageData);
    TEST_ASSERT(row.weights.empty());
}

TEST_CASE(HammingWindow)
{
    six::sicd::DirectionParameters row;
    row.weightType.reset(new six::sicd::WeightType());
    row.weightType->windowName = "HAMMING";

    six::sicd::ImageData imageData;
    row.fillDerivedFields(imageData);

    TEST_ASSERT_EQ(row.weights.size(), static_cast<size_t>(512));
    TEST_ASSERT_ALMOST_EQ(row.weights[163], .733193239);
    TEST_ASSERT_ALMOST_EQ(row.weights[300], .9328411378);
    TEST_ASSERT_ALMOST_EQ(row.weights[0], row.weights[511]);
}

TEST_CASE(KaiserWindow)
{
    six::sicd::DirectionParameters row;
    row.weightType.reset(new six::sicd::WeightType());
    row.weightType->windowName = "KAISER";
    six::Parameter param;
    param.setName("Name");
    param.setValue("10");
    row.weightType->parameters.push_back(param);

    six::sicd::ImageData imageData;
    row.fillDerivedFields(imageData);
    TEST_ASSERT_ALMOST_EQ(row.weights[10], .001442113);
    TEST_ASSERT_ALMOST_EQ(row.weights[300], .865089423);
    TEST_ASSERT_ALMOST_EQ(row.weights[460], .023764757);
}

TEST_CASE(FillUnitVectorsFromRMAT)
{
    six::sicd::Grid grid;
    six::PolyXYZ poly;
    six::sicd::RMA rma;
    rma.rmat.reset(new six::sicd::RMAT());
    six::Vector3 scp;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        rma.rmat->refPos[ii] = static_cast<double>(ii);
        rma.rmat->refVel[ii] = ii + 1.0;
        scp[ii] = (ii + 1) * 100.0;
    }

    grid.fillDerivedFields(rma, scp, poly);

    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[0], .872871560);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[1], .218217890);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[2], -0.43643578);

    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[0], -0.26726124);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[1], -0.53452248);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[2], -0.80178372);
}

TEST_CASE(FillUnitVectorsFromRMCR)
{
    six::sicd::Grid grid;
    six::PolyXYZ poly;
    six::sicd::RMA rma;
    rma.rmcr.reset(new six::sicd::RMCR());
    six::Vector3 scp;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        rma.rmcr->refPos[ii] = static_cast<double>(ii);
        rma.rmcr->refVel[ii] = ii + 1.0;
        scp[ii] = (ii + 1) * 100.0;
    }

    grid.fillDerivedFields(rma, scp, poly);

    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[0], .268796809);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[1], .534905651);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[2], .801014493);

    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[0], .872399913);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[1], .217276959);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[2], -.43784599);
}


TEST_CASE(FillUnitVectorsFromINCA)
{
    six::sicd::Grid grid;
    six::sicd::RgAzComp rgAzComp;
    six::sicd::RMA rma;
    rma.inca.reset(new six::sicd::INCA());
    rma.inca->freqZero = 10;
    six::Vector3 scp;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        scp[ii] = (ii + 1) * 100.0;
    }

    std::vector<six::Vector3> arpPolyData(4);
    for (size_t ii = 0; ii < arpPolyData.size(); ++ii)
    {
        std::vector<double> nextTerm(3);
        for (size_t jj = 0; jj < nextTerm.size(); ++jj)
        {
            nextTerm[jj] = static_cast<double>(ii + jj);
        }
        arpPolyData[ii] = six::Vector3(nextTerm);
    }

    six::PolyXYZ poly(arpPolyData);

    std::vector<double> timePolyData(4);
    timePolyData[0] = 0;
    timePolyData[1] = 1;
    timePolyData[2] = 2;
    timePolyData[3] = 0;
    rma.inca->timeCAPoly = six::Poly1D(timePolyData);

    grid.fillDerivedFields(rma, scp, poly);

    TEST_ASSERT_EQ(grid.col->kCenter, 0);
    TEST_ASSERT_ALMOST_EQ(grid.row->kCenter, 6.6713e-08);

    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[0], .268796809);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[1], .534905651);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[2], .801014493);

    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[0], .872399913);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[1], .217276959);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[2], -.43784599);
}

TEST_CASE(FillUnitVectorsFromRgAzComp)
{
    six::sicd::Grid grid;
    six::sicd::RgAzComp rgAzComp;
    six::sicd::SCPCOA scpcoa;
    six::sicd::GeoData geoData;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        scpcoa.arpPos[ii] = static_cast<double>(ii);
        scpcoa.arpVel[ii] = ii + 1.0;
        geoData.scp.ecf[ii] = (ii + 1) * 100.0;
    }

    grid.fillDerivedFields(rgAzComp, geoData, scpcoa, 10);

    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[0], 0.26879680);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[1], 0.53490565);
    TEST_ASSERT_ALMOST_EQ(grid.row->unitVector[2], 0.80101449);

    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[0], 0.87239991);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[1], 0.21727695);
    TEST_ASSERT_ALMOST_EQ(grid.col->unitVector[2], -0.43784599);
}


TEST_CASE(FillDirectionParamsKCenterFromKCOAPoly)
{
    six::sicd::DirectionParameters col;
    col.deltaKCOAPoly = six::Poly2D(3, 3);
    for (size_t ii = 0; ii <= col.deltaKCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= col.deltaKCOAPoly.orderY(); ++jj)
        {
            col.deltaKCOAPoly[ii][jj] = static_cast<double>(ii + jj);
        }
    }
    six::sicd::RgAzComp rgAzComp;
    six::sicd::GeoData geoData;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        geoData.scp.ecf[ii] = (ii + 1) * 100.0;
    }

    col.fillDerivedFields(rgAzComp, 0);
    TEST_ASSERT_EQ(col.kCenter, 0);
}

TEST_CASE(DefaultFromRMAT)
{
    six::sicd::Grid grid;
    grid.type = six::ComplexImageGridType::NOT_SET; //defaults to RGAZIM
    six::sicd::RMA rma;
    rma.rmat.reset(new six::sicd::RMAT());
    rma.rmat->dopConeAngleRef = 5;

    grid.fillDefaultFields(rma, 950);

    TEST_ASSERT(grid.imagePlane == "SLANT");
    TEST_ASSERT_EQ(grid.imagePlane, six::ComplexImagePlaneType::SLANT);
    TEST_ASSERT(grid.type == "XCTYAT");
    TEST_ASSERT_EQ(grid.type, six::ComplexImageGridType::XCTYAT);
    TEST_ASSERT_ALMOST_EQ(grid.row->kCenter, 5.52368502e-7);
    TEST_ASSERT_ALMOST_EQ(grid.col->kCenter, 6.31360087e-6);
}

TEST_CASE(DefaultFromRMCR)
{
    six::sicd::Grid grid;
    grid.type = six::ComplexImageGridType::NOT_SET; //defaults to RGAZIM
    six::sicd::RMA rma;
    rma.rmcr.reset(new six::sicd::RMCR());

    grid.fillDefaultFields(rma, 950);

    TEST_ASSERT(grid.imagePlane == "SLANT");
    TEST_ASSERT_EQ(grid.imagePlane, six::ComplexImagePlaneType::SLANT);
    TEST_ASSERT(grid.type == "XRGYCR");
    TEST_ASSERT_EQ(grid.type, six::ComplexImageGridType::XRGYCR);
    TEST_ASSERT_ALMOST_EQ(grid.row->kCenter, 6.33771780e-6);
    TEST_ASSERT_ALMOST_EQ(grid.col->kCenter, 0);
}

TEST_CASE(DefaultFromPFA)
{
    six::sicd::Grid grid;
    grid.type = six::ComplexImageGridType::NOT_SET; //defaults to RGAZIM
    six::sicd::PFA pfa;
    pfa.krg1 = 1189;
    pfa.krg2 = 1131;
    double fc = 950;

    grid.fillDefaultFields(pfa, fc);

    TEST_ASSERT(grid.type == "RGAZIM");
    TEST_ASSERT_EQ(grid.type, six::ComplexImageGridType::RGAZIM);
    TEST_ASSERT_ALMOST_EQ(grid.row->kCenter, 1160);
    TEST_ASSERT_ALMOST_EQ(grid.col->kCenter, 0);

    // Reset for next case
    grid.row->kCenter = six::Init::undefined<double>();
    grid.col->kCenter = six::Init::undefined<double>();
    pfa.krg1 = six::Init::undefined<double>();
    pfa.krg2 = six::Init::undefined<double>();

    std::vector<double> sfPolyData(4);
    sfPolyData[0] = 0;
    sfPolyData[1] = 1;
    sfPolyData[2] = 4;
    sfPolyData[3] = 9;
    pfa.spatialFrequencyScaleFactorPoly = six::Poly1D(sfPolyData);
    fc = 950;

    grid.fillDefaultFields(pfa, fc);

    TEST_ASSERT_ALMOST_EQ(grid.row->kCenter, 0);
    TEST_ASSERT_ALMOST_EQ(grid.col->kCenter, 0);
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(DerivedDeltaKsNoImageData);
    TEST_CHECK(DerivedDeltaKsWithImageData);
    TEST_CHECK(IdentityWeightFunction);
    TEST_CHECK(HammingWindow);
    TEST_CHECK(KaiserWindow);
    TEST_CHECK(FillUnitVectorsFromRMAT);
    TEST_CHECK(FillUnitVectorsFromRMCR);
    TEST_CHECK(FillUnitVectorsFromINCA);
    TEST_CHECK(FillUnitVectorsFromRgAzComp);
    TEST_CHECK(FillDirectionParamsKCenterFromKCOAPoly);
    TEST_CHECK(DefaultFromRMAT);
    TEST_CHECK(DefaultFromRMCR);
    TEST_CHECK(DefaultFromPFA);
    )

