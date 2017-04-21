#include <math/Utilities.h>
#include <six/sicd/AreaPlaneUtility.h>
#include <six/sicd/Utilities.h>
#include "TestCase.h"

namespace
{
TEST_CASE(testAreaPlane)
{
    std::auto_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::createFakeComplexData();
    data->grid->row.reset(new six::sicd::DirectionParameters());
    data->grid->row->sign = -1;
    data->grid->col.reset(new six::sicd::DirectionParameters());
    data->grid->col->sign = -1;

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
    std::auto_ptr<six::sicd::ComplexData> data =
            six::sicd::Utilities::createFakeComplexData();
    data->grid->row.reset(new six::sicd::DirectionParameters());
    data->grid->row->sign = -1;
    data->grid->col.reset(new six::sicd::DirectionParameters());
    data->grid->col->sign = -1;

    data->radarCollection->area.reset(new six::sicd::Area());
    data->radarCollection->area->plane.reset(new six::sicd::AreaPlane());
    six::sicd::AreaPlane& areaPlane = *data->radarCollection->area->plane;
    six::sicd::AreaPlaneUtility::deriveAreaPlane(*data, areaPlane);

    std::auto_ptr<six::sicd::ComplexData> secondData =
            six::sicd::Utilities::createFakeComplexData();
    secondData->grid->row.reset(new six::sicd::DirectionParameters());
    secondData->grid->row->sign = -1;
    secondData->grid->col.reset(new six::sicd::DirectionParameters());
    secondData->grid->col->sign = -1;

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
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        TEST_CHECK(testAreaPlane);
        TEST_CHECK(testBothMethodsGiveSamePlane);
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 0;
}

