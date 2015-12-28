#include <six/sidd/DownstreamReprocessing.h>
#include "TestCase.h"

namespace
{
TEST_CASE(testGeometricChip)
{
    six::sidd::GeometricChip chip;
    chip.chipSize.row = 1000;
    chip.chipSize.col = 1000;

    chip.originalUpperLeftCoordinate.row = 146.9986;
    chip.originalUpperLeftCoordinate.col = 462.7939;

    chip.originalUpperRightCoordinate.row = 346.9986;
    chip.originalUpperRightCoordinate.col = 365.9017;

    chip.originalLowerRightCoordinate.row = 256.5206;
    chip.originalLowerRightCoordinate.col = 179.1415;

    chip.originalLowerLeftCoordinate.row = 56.5206;
    chip.originalLowerLeftCoordinate.col = 276.0336;

    // Convert from full --> chip
    const six::RowColDouble fullCoord(180.2995, 415.5702);
    static const double TOL = 0.001;

    const six::RowColDouble computedChipCoord =
            chip.getChipCoordinateFromFullImage(fullCoord);
    TEST_ASSERT_ALMOST_EQ_EPS(computedChipCoord.row, 134.6943, TOL);
    TEST_ASSERT_ALMOST_EQ_EPS(computedChipCoord.col, 227.2724, TOL);

    // Convert back from chip --> full
    const six::RowColDouble computedFullCoord =
            chip.getFullImageCoordinateFromChip(computedChipCoord);
    TEST_ASSERT_ALMOST_EQ_EPS(computedFullCoord.row, fullCoord.row, TOL);
    TEST_ASSERT_ALMOST_EQ_EPS(computedFullCoord.col, fullCoord.col, TOL);
}
}

int main(int, char**)
{
    TEST_CHECK(testGeometricChip);
    return 0;
}
