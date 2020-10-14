#include <math/Utilities.h>
#include <six/sicd/AreaPlaneUtility.h>
#include <six/sicd/Utilities.h>
#include "TestCase.h"

namespace
{
void setupData(six::sicd::ComplexData& data)
{
    data.grid->row.reset(new six::sicd::DirectionParameters());
    data.grid->row->sign = -1;
    data.grid->col.reset(new six::sicd::DirectionParameters());
    data.grid->col->sign = -1;
    data.setNumRows(100);
    data.setNumCols(100);
}

TEST_CASE(testAreaPlane)
{
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::createFakeComplexData();
    setupData(*data);
    six::sicd::AreaPlaneUtility::setAreaPlane(*data);
    const six::sicd::AreaPlane& areaPlane = *data->radarCollection->area->plane;

    TEST_ASSERT_EQ(areaPlane.segmentList[0]->getNumLines(), 1);
    TEST_ASSERT_EQ(areaPlane.segmentList[0]->getNumSamples(), 1);

    TEST_ASSERT(math::isNaN(areaPlane.xDirection->spacing));
    TEST_ASSERT(math::isNaN(areaPlane.yDirection->spacing));

    TEST_ASSERT(math::isNaN(areaPlane.getAdjustedReferencePoint().row));
    TEST_ASSERT(math::isNaN(areaPlane.getAdjustedReferencePoint().col));

    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.referencePoint.ecef[0],
            4191860, 1);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.referencePoint.ecef[1],
            901641, 1);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.referencePoint.ecef[2],
            4706688, 1);

    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.xDirection->unitVector[0], -0.684546,
            1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.xDirection->unitVector[1], -0.302646,
            1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.xDirection->unitVector[2], 0.663176,
            1e-5);

    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.yDirection->unitVector[0], 0.317979,
            1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.yDirection->unitVector[1], -0.942602,
            1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.yDirection->unitVector[2], -0.10194,
            1e-5);
}

TEST_CASE(testBothMethodsGiveSamePlane)
{
    std::unique_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::createFakeComplexData();
    setupData(*data);

    data->radarCollection->area.reset(new six::sicd::Area());
    data->radarCollection->area->plane.reset(new six::sicd::AreaPlane());
    six::sicd::AreaPlane& areaPlane = *data->radarCollection->area->plane;
    six::sicd::AreaPlaneUtility::deriveAreaPlane(*data, areaPlane, false);

    std::unique_ptr<six::sicd::ComplexData> secondData =
            six::sicd::Utilities::createFakeComplexData();
    setupData(*secondData);

    six::sicd::AreaPlaneUtility::setAreaPlane(*secondData, false);
    const six::sicd::AreaPlane& secondAreaPlane =
            *secondData->radarCollection->area->plane;

    TEST_ASSERT(math::isNaN(areaPlane.referencePoint.rowCol.row));
    TEST_ASSERT(math::isNaN(secondAreaPlane.referencePoint.rowCol.row));
    TEST_ASSERT(math::isNaN(areaPlane.referencePoint.rowCol.col));
    TEST_ASSERT(math::isNaN(secondAreaPlane.referencePoint.rowCol.col));

    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.referencePoint.ecef[0],
            secondAreaPlane.referencePoint.ecef[0], 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.referencePoint.ecef[1],
            secondAreaPlane.referencePoint.ecef[1], 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.referencePoint.ecef[2],
            secondAreaPlane.referencePoint.ecef[2], 1e-5);
    TEST_ASSERT_EQ(areaPlane.referencePoint.name,
            secondAreaPlane.referencePoint.name);

    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.xDirection->unitVector[0],
            secondAreaPlane.xDirection->unitVector[0], 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.xDirection->unitVector[1],
            secondAreaPlane.xDirection->unitVector[1], 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.xDirection->unitVector[2],
            secondAreaPlane.xDirection->unitVector[2], 1e-5);

    TEST_ASSERT(math::isNaN(areaPlane.xDirection->spacing));
    TEST_ASSERT(math::isNaN(secondAreaPlane.xDirection->spacing));

    TEST_ASSERT_EQ(areaPlane.xDirection->elements,
            secondAreaPlane.xDirection->elements);
    TEST_ASSERT_EQ(areaPlane.xDirection->first,
            secondAreaPlane.xDirection->first);

    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.yDirection->unitVector[0],
            secondAreaPlane.yDirection->unitVector[0], 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.yDirection->unitVector[1],
            secondAreaPlane.yDirection->unitVector[1], 1e-5);
    TEST_ASSERT_ALMOST_EQ_EPS(areaPlane.yDirection->unitVector[2],
            secondAreaPlane.yDirection->unitVector[2], 1e-5);


    TEST_ASSERT(math::isNaN(areaPlane.yDirection->spacing));
    TEST_ASSERT(math::isNaN(secondAreaPlane.yDirection->spacing));

    TEST_ASSERT_EQ(areaPlane.yDirection->elements,
            secondAreaPlane.yDirection->elements);
    TEST_ASSERT_EQ(areaPlane.yDirection->first,
            secondAreaPlane.yDirection->first);

    TEST_ASSERT_EQ(areaPlane.segmentList.size(),
            secondAreaPlane.segmentList.size());
    TEST_ASSERT(areaPlane.segmentList.empty());
    TEST_ASSERT_EQ(areaPlane.orientation, secondAreaPlane.orientation);
}

TEST_CASE(testRotatePlane)
{
    six::sicd::AreaPlane plane;
    plane.orientation = six::OrientationType::LEFT;
    plane.yDirection->elements = 10;
    plane.xDirection->elements = 20;

    plane.yDirection->spacing = 5;
    plane.xDirection->spacing = 7;

    plane.yDirection->unitVector[0] = 0;
    plane.yDirection->unitVector[1] = 1;
    plane.yDirection->unitVector[2] = 0;

    plane.xDirection->unitVector[0] = 1;
    plane.xDirection->unitVector[1] = 0;
    plane.xDirection->unitVector[2] = 0;

    plane.referencePoint.rowCol = six::RowColDouble(1, 2);

    plane.segmentList.resize(1);
    plane.segmentList[0].reset(new six::sicd::Segment());
    plane.segmentList[0]->startLine = 1;
    plane.segmentList[0]->startSample = 0;
    plane.segmentList[0]->endLine = 4;
    plane.segmentList[0]->endSample = 8;

    const size_t originalNumLines = plane.segmentList[0]->getNumLines();
    const size_t originalNumSamples = plane.segmentList[0]->getNumSamples();

    plane.rotateCCW();
    TEST_ASSERT(plane.orientation ==
            six::OrientationType(six::OrientationType::DOWN));
    TEST_ASSERT_EQ(plane.referencePoint.rowCol.row, 8);
    TEST_ASSERT_EQ(plane.referencePoint.rowCol.col, 1);

    TEST_ASSERT_EQ(plane.yDirection->unitVector[0], 1);
    TEST_ASSERT_EQ(plane.yDirection->unitVector[1], 0);
    TEST_ASSERT_EQ(plane.yDirection->unitVector[2], 0);

    TEST_ASSERT_EQ(plane.xDirection->unitVector[0], 0);
    TEST_ASSERT_EQ(plane.xDirection->unitVector[1], -1);
    TEST_ASSERT_EQ(plane.xDirection->unitVector[2], 0);

    TEST_ASSERT_EQ(plane.xDirection->elements, 10);
    TEST_ASSERT_EQ(plane.yDirection->elements, 20);

    TEST_ASSERT_EQ(plane.xDirection->spacing, 5);
    TEST_ASSERT_EQ(plane.yDirection->spacing, 7);

    TEST_ASSERT_EQ(plane.segmentList[0]->startLine, 0);
    TEST_ASSERT_EQ(plane.segmentList[0]->startSample, 1);
    TEST_ASSERT_EQ(plane.segmentList[0]->endLine, -8);
    TEST_ASSERT_EQ(plane.segmentList[0]->endSample, 4);

    TEST_ASSERT_EQ(originalNumLines, plane.segmentList[0]->getNumSamples());
    TEST_ASSERT_EQ(originalNumSamples, plane.segmentList[0]->getNumLines());

}


TEST_CASE(testCanRotateFourTimes)
{
    six::sicd::AreaPlane plane;
    scene::ECEFToLLATransform transformer;
    plane.orientation = six::OrientationType::LEFT;
    plane.yDirection->elements = 10;
    plane.xDirection->elements = 20;
    plane.referencePoint.rowCol = six::RowColDouble(1, 2);

    std::unique_ptr<six::sicd::AreaPlane> originalPlane(plane.clone());
    plane.rotateCCW();
    plane.rotateCCW();
    plane.rotateCCW();
    plane.rotateCCW();
    TEST_ASSERT(plane == *originalPlane);
}
}

TEST_MAIN(
    TEST_CHECK(testAreaPlane);
    TEST_CHECK(testBothMethodsGiveSamePlane);
    TEST_CHECK(testRotatePlane);
    TEST_CHECK(testCanRotateFourTimes);
    )

