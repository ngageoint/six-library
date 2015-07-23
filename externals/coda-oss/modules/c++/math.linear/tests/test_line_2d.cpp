/* =========================================================================
 * This file is part of math.linear-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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

#include <math/linear/Line2D.h>
#include <TestCase.h>

namespace
{
double expectedDistToPt(const math::linear::Line2D& L,
        math::linear::Line2D::Point P)
{
    return std::abs(L.getSlope() * P.row - P.col + L.getYIntercept()) /
            std::sqrt(L.getSlope() * L.getSlope() + 1);
}
math::linear::Line2D::Point expectedOffSet(const math::linear::Line2D& L,
        math::linear::Line2D::Point P, double distance)
{
    double theta = std::atan(L.getSlope());
    P.row += distance * std::cos(theta);
    P.col += distance * std::sin(theta);
    return P;
}
TEST_CASE(testVertical)
{
    math::linear::Line2D::Point P1(5,5);
    math::linear::Line2D::Point P2(5, 75);
    math::linear::Line2D vertLine(P1, P2);
    // 1.testing getSlope()--should throw exception for vertical line
    TEST_EXCEPTION(vertLine.getSlope());
    // 2. testing getYIntercept() --should throw exception for vertical line
    TEST_EXCEPTION(vertLine.getYIntercept());
    // 3. testing getXIntercept() -- should return 5
    TEST_ASSERT_EQ(vertLine.getXIntercept(), 5);
    // 4. testing y(double x)--should throw exception for vertical line
    double m = 0;
    for (size_t i = 0; i < 10; ++i, m += 0.1)
    {
        TEST_EXCEPTION(vertLine.y(m));
    }
    // 5. testing x(double y)--should give 5 for any input
    m = 0;
    for (size_t i = 0; i < 10; ++i, m += 0.1)
    {
        TEST_ASSERT_EQ(5, vertLine.x(m));
    }
    // 6. testing Intersection

    // Intersecting another vertical line should throw an exception
    P1.row += 1;
    P2.row += 1;
    math::linear::Line2D vertLine2(P1, P2);
    TEST_EXCEPTION(vertLine.intersection(vertLine2));

    // Intersecting a horizontal line should give (vertLine.xIntercept, \
    // horiLine.yIntercept)
    math::linear::Line2D::Point P3(5,5);
    math::linear::Line2D::Point P4(6,5);
    math::linear::Line2D horiLine(P3, P4);

    math::linear::Line2D::Point expected1(vertLine.getXIntercept(),
            horiLine.getYIntercept());
    math::linear::Line2D::Point actual1 = vertLine.intersection(horiLine);
    TEST_ASSERT_EQ(actual1.row, expected1.row);
    TEST_ASSERT_EQ(actual1.col, expected1.col);

    // Intersecting a normal line should give (5, normLine.y(5))
    math::linear::Line2D::Point P5(2,4);
    math::linear::Line2D::Point P6(7,8);
    math::linear::Line2D normLine(P5, P6);

    math::linear::Line2D::Point expected2(5, normLine.y(5));
    math::linear::Line2D::Point actual2 = vertLine.intersection(normLine);
    TEST_ASSERT_EQ(actual2.row, expected2.row);
    TEST_ASSERT_EQ(actual2.col, expected2.col);

    // 7. testing parallelToLine
    // create a parallel line, then asking for its intersection with my
    // original one, expect an exception thrown
    math::linear::Line2D L1= vertLine.parallelToLine(P1);
    TEST_EXCEPTION(vertLine.intersection(L1));
    math::linear::Line2D L2 = vertLine.parallelToLine(P2);
    TEST_EXCEPTION(vertLine.intersection(L2));
    math::linear::Line2D L3 = vertLine.parallelToLine(P3);
    TEST_EXCEPTION(vertLine.intersection(L3));
    math::linear::Line2D L4 = vertLine.parallelToLine(P4);
    TEST_EXCEPTION(vertLine.intersection(L4));
    math::linear::Line2D L5 = vertLine.parallelToLine(P5);
    TEST_EXCEPTION(vertLine.intersection(L5));
    math::linear::Line2D L6 = vertLine.parallelToLine(P6);
    TEST_EXCEPTION(vertLine.intersection(L6));

    // 8. testing perpendicularToLine
    // this perpendicular line should be able to intersect with the original
    // vertLine at (vertLine.getXIntercept(), P.col)
    math::linear::Line2D PL1 = vertLine.perpendicularToLine(P1);
    math::linear::Line2D::Point pExpected1(vertLine.getXIntercept(), P1.col);
    math::linear::Line2D::Point pActual1(vertLine.intersection(PL1));
    TEST_ASSERT_EQ(pActual1.row, pExpected1.row);
    TEST_ASSERT_EQ(pActual1.col, pExpected1.col);

    math::linear::Line2D PL2 = vertLine.perpendicularToLine(P2);
    math::linear::Line2D::Point pExpected2(vertLine.getXIntercept(), P2.col);
    math::linear::Line2D::Point pActual2(vertLine.intersection(PL2));
    TEST_ASSERT_EQ(pActual2.row, pExpected2.row);
    TEST_ASSERT_EQ(pActual2.col, pExpected2.col);


    math::linear::Line2D PL3 = vertLine.perpendicularToLine(P3);
    math::linear::Line2D::Point pExpected3(vertLine.getXIntercept(), P3.col);
    math::linear::Line2D::Point pActual3(vertLine.intersection(PL3));
    TEST_ASSERT_EQ(pActual3.row, pExpected3.row);
    TEST_ASSERT_EQ(pActual3.col, pExpected3.col);


    math::linear::Line2D PL4 = vertLine.perpendicularToLine(P4);
    math::linear::Line2D::Point pExpected4(vertLine.getXIntercept(), P4.col);
    math::linear::Line2D::Point pActual4(vertLine.intersection(PL4));
    TEST_ASSERT_EQ(pActual4.row, pExpected4.row);
    TEST_ASSERT_EQ(pActual4.col, pExpected4.col);


    math::linear::Line2D PL5 = vertLine.perpendicularToLine(P5);
    math::linear::Line2D::Point pExpected5(vertLine.getXIntercept(), P5.col);
    math::linear::Line2D::Point pActual5(vertLine.intersection(PL5));
    TEST_ASSERT_EQ(pActual5.row, pExpected5.row);
    TEST_ASSERT_EQ(pActual5.col, pExpected5.col);


    math::linear::Line2D PL6 = vertLine.perpendicularToLine(P6);
    math::linear::Line2D::Point pExpected6(vertLine.getXIntercept(), P6.col);
    math::linear::Line2D::Point pActual6(vertLine.intersection(PL6));
    TEST_ASSERT_EQ(pActual6.row, pExpected6.row);
    TEST_ASSERT_EQ(pActual6.col, pExpected6.col);

    // 9. Testing distanceToPoint, should return abs(P.row - xIntercept)
    double dExpected1 = std::abs(P1.row - vertLine.getXIntercept());
    double dActual1 = vertLine.distanceToPoint(P1);
    TEST_ASSERT_EQ(dActual1, dExpected1);

    double dExpected2 = std::abs(P2.row - vertLine.getXIntercept());
    double dActual2 = vertLine.distanceToPoint(P2);
    TEST_ASSERT_EQ(dActual2, dExpected2);

    double dExpected3 = std::abs(P3.row - vertLine.getXIntercept());
    double dActual3 = vertLine.distanceToPoint(P3);
    TEST_ASSERT_EQ(dActual3, dExpected3);

    double dExpected4 = std::abs(P4.row - vertLine.getXIntercept());
    double dActual4 = vertLine.distanceToPoint(P4);
    TEST_ASSERT_EQ(dActual4, dExpected4);

    double dExpected5 = std::abs(P5.row - vertLine.getXIntercept());
    double dActual5 = vertLine.distanceToPoint(P5);
    TEST_ASSERT_EQ(dActual5, dExpected5);

    double dExpected6 = std::abs(P6.row - vertLine.getXIntercept());
    double dActual6 = vertLine.distanceToPoint(P6);
    TEST_ASSERT_EQ(dActual6, dExpected6);

    // 10. Testing offsetFromPoint, should return (P.row, P.col + distance)
    double d = 4.5; // distance > 0
    math::linear::Line2D::Point oExpected1(P1.row, P1.col + d);
    math::linear::Line2D::Point oActual1 = vertLine.offsetFromPoint(P1, d);
    TEST_ASSERT_EQ(oActual1.row, oExpected1.row);
    TEST_ASSERT_EQ(oActual1.col, oExpected1.col);

    math::linear::Line2D::Point oExpected2(P2.row, P2.col + d);
    math::linear::Line2D::Point oActual2 = vertLine.offsetFromPoint(P2, d);
    TEST_ASSERT_EQ(oActual2.row, oExpected2.row);
    TEST_ASSERT_EQ(oActual2.col, oExpected2.col);

    math::linear::Line2D::Point oExpected3(P3.row, P3.col + d);
    math::linear::Line2D::Point oActual3 = vertLine.offsetFromPoint(P3, d);
    TEST_ASSERT_EQ(oActual3.row, oExpected3.row);
    TEST_ASSERT_EQ(oActual3.col, oExpected3.col);

    math::linear::Line2D::Point oExpected4(P4.row, P4.col + d);
    math::linear::Line2D::Point oActual4 = vertLine.offsetFromPoint(P4, d);
    TEST_ASSERT_EQ(oActual4.row, oExpected4.row);
    TEST_ASSERT_EQ(oActual4.col, oExpected4.col);

    math::linear::Line2D::Point oExpected5(P5.row, P5.col + d);
    math::linear::Line2D::Point oActual5 = vertLine.offsetFromPoint(P5, d);
    TEST_ASSERT_EQ(oActual5.row, oExpected5.row);
    TEST_ASSERT_EQ(oActual5.col, oExpected5.col);

    math::linear::Line2D::Point oExpected6(P6.row, P6.col + d);
    math::linear::Line2D::Point oActual6 = vertLine.offsetFromPoint(P6, d);
    TEST_ASSERT_EQ(oActual6.row, oExpected6.row);
    TEST_ASSERT_EQ(oActual6.col, oExpected6.col);
    // testing when d < 0
    d = -0.5; // distance > 0
    math::linear::Line2D::Point o2Expected1(P1.row, P1.col + d);
    math::linear::Line2D::Point o2Actual1 = vertLine.offsetFromPoint(P1, d);
    TEST_ASSERT_EQ(o2Actual1.row, o2Expected1.row);
    TEST_ASSERT_EQ(o2Actual1.col, o2Expected1.col);

    math::linear::Line2D::Point o2Expected2(P2.row, P2.col + d);
    math::linear::Line2D::Point o2Actual2 = vertLine.offsetFromPoint(P2, d);
    TEST_ASSERT_EQ(o2Actual2.row, o2Expected2.row);
    TEST_ASSERT_EQ(o2Actual2.col, o2Expected2.col);

    math::linear::Line2D::Point o2Expected3(P3.row, P3.col + d);
    math::linear::Line2D::Point o2Actual3 = vertLine.offsetFromPoint(P3, d);
    TEST_ASSERT_EQ(o2Actual3.row, o2Expected3.row);
    TEST_ASSERT_EQ(o2Actual3.col, o2Expected3.col);

    math::linear::Line2D::Point o2Expected4(P4.row, P4.col + d);
    math::linear::Line2D::Point o2Actual4 = vertLine.offsetFromPoint(P4, d);
    TEST_ASSERT_EQ(o2Actual4.row, o2Expected4.row);
    TEST_ASSERT_EQ(o2Actual4.col, o2Expected4.col);

    math::linear::Line2D::Point o2Expected5(P5.row, P5.col + d);
    math::linear::Line2D::Point o2Actual5 = vertLine.offsetFromPoint(P5, d);
    TEST_ASSERT_EQ(o2Actual5.row, o2Expected5.row);
    TEST_ASSERT_EQ(o2Actual5.col, o2Expected5.col);

    math::linear::Line2D::Point o2Expected6(P6.row, P6.col + d);
    math::linear::Line2D::Point o2Actual6 = vertLine.offsetFromPoint(P6, d);
    TEST_ASSERT_EQ(o2Actual6.row, o2Expected6.row);
    TEST_ASSERT_EQ(o2Actual6.col, o2Expected6.col);
}
TEST_CASE(testHorizontal)
{
    math::linear::Line2D::Point P1(6, 7.5);
    math::linear::Line2D::Point P2(2, 7.5);
    math::linear::Line2D horiLine(P1, P2);
    // 1. Testing getSlope()--should return 0
    TEST_ASSERT_EQ(horiLine.getSlope(), 0);

    // 2. Testing getYIntercept--should return 7.5
    TEST_ASSERT_EQ(horiLine.getYIntercept(), 7.5);
    // 3. Testing getXIntercept--should throw an exception
    TEST_EXCEPTION(horiLine.getXIntercept());

    // 4. Testing y(double x)-- should return 7.5 for every input
    double m = 0.0;
    for (size_t i = 0; i < 10; ++i, m += 0.1)
    {
        TEST_ASSERT_EQ(horiLine.y(m), 7.5);
    }
    // 5. Testing x(double y) -- should throw an exception for every input
    m = 0;
    for (size_t i = 0; i < 10; ++i, m += 0.1)
    {
        TEST_EXCEPTION(horiLine.x(m));
    }
    // 6. Testing intersection
    // Testing with another horizontal line--should throw an exception
    P1.col += 1;
    P2.col += 1;
    math::linear::Line2D horiLine2(P1, P2);
    TEST_EXCEPTION(horiLine.intersection(horiLine2));

    // Testing with a vertical line--should return (veriLine.getXIntercept(),
    // horiLine.getYIntercept())
    math::linear::Line2D::Point P3(2, 8);
    math::linear::Line2D::Point P4(2, 9);
    math::linear::Line2D veriLine(P3, P4);
    math::linear::Line2D::Point expected1(veriLine.getXIntercept(),
            horiLine.getYIntercept());
    math::linear::Line2D::Point actual1 = horiLine.intersection(veriLine);
    TEST_ASSERT_EQ(actual1.row, expected1.row);
    TEST_ASSERT_EQ(actual1.col, expected1.col);

    // Testing with a normal line--should return (normLine.x(yIntercept),
    // yIntercept)
    math::linear::Line2D::Point P5(2, 9);
    math::linear::Line2D::Point P6(1, 4);
    math::linear::Line2D normLine(P5, P6);
    math::linear::Line2D::Point expected2(normLine.x(horiLine.getYIntercept()),
            horiLine.getYIntercept());
    math::linear::Line2D::Point actual2 = horiLine.intersection(normLine);

    TEST_ASSERT_EQ(actual2.row, expected2.row);
    TEST_ASSERT_EQ(actual2.col, expected2.col);

    // 7. testing parallelToLine
    // should get a horizontal line back and its intersection with my original
    // would horiLine have an exception thrown

    TEST_EXCEPTION(horiLine.intersection(horiLine.parallelToLine(P1)));
    TEST_EXCEPTION(horiLine.intersection(horiLine.parallelToLine(P2)));
    TEST_EXCEPTION(horiLine.intersection(horiLine.parallelToLine(P3)));
    TEST_EXCEPTION(horiLine.intersection(horiLine.parallelToLine(P4)));
    TEST_EXCEPTION(horiLine.intersection(horiLine.parallelToLine(P5)));
    TEST_EXCEPTION(horiLine.intersection(horiLine.parallelToLine(P6)));

    // 8. testing perpendicularToLine -- should intersect at (P.row,
    // horiLine.getYIntercept())
    math::linear::Line2D::Point pExpected1(P1.row, horiLine.getYIntercept());
    math::linear::Line2D::Point pActual1 =
            horiLine.intersection(horiLine.perpendicularToLine(P1));
    TEST_ASSERT_EQ(pActual1.row, pExpected1.row);
    TEST_ASSERT_EQ(pActual1.col, pExpected1.col);

    math::linear::Line2D::Point pExpected2(P2.row, horiLine.getYIntercept());
    math::linear::Line2D::Point pActual2 =
            horiLine.intersection(horiLine.perpendicularToLine(P2));
    TEST_ASSERT_EQ(pActual2.row, pExpected2.row);
    TEST_ASSERT_EQ(pActual2.col, pExpected2.col);

    math::linear::Line2D::Point pExpected3(P3.row, horiLine.getYIntercept());
    math::linear::Line2D::Point pActual3 =
            horiLine.intersection(horiLine.perpendicularToLine(P3));
    TEST_ASSERT_EQ(pActual3.row, pExpected3.row);
    TEST_ASSERT_EQ(pActual3.col, pExpected3.col);

    math::linear::Line2D::Point pExpected4(P4.row, horiLine.getYIntercept());
    math::linear::Line2D::Point pActual4 =
            horiLine.intersection(horiLine.perpendicularToLine(P4));
    TEST_ASSERT_EQ(pActual4.row, pExpected4.row);
    TEST_ASSERT_EQ(pActual4.col, pExpected4.col);

    math::linear::Line2D::Point pExpected5(P5.row, horiLine.getYIntercept());
    math::linear::Line2D::Point pActual5 =
            horiLine.intersection(horiLine.perpendicularToLine(P5));
    TEST_ASSERT_EQ(pActual5.row, pExpected5.row);
    TEST_ASSERT_EQ(pActual5.col, pExpected5.col);

    math::linear::Line2D::Point pExpected6(P6.row, horiLine.getYIntercept());
    math::linear::Line2D::Point pActual6 =
            horiLine.intersection(horiLine.perpendicularToLine(P6));
    TEST_ASSERT_EQ(pActual6.row, pExpected6.row);
    TEST_ASSERT_EQ(pActual6.col, pExpected6.col);

    // 9. distanceToPoint test
    // should return abs(P.col - yIntercept)
    double dExpected1 = std::abs(P1.col - horiLine.getYIntercept());
    double dActual1 = horiLine.distanceToPoint(P1);
    TEST_ASSERT_EQ(dActual1, dExpected1);

    double dExpected2 = std::abs(P2.col - horiLine.getYIntercept());
    double dActual2 = horiLine.distanceToPoint(P2);
    TEST_ASSERT_EQ(dActual2, dExpected2);

    double dExpected3 = std::abs(P3.col - horiLine.getYIntercept());
    double dActual3 = horiLine.distanceToPoint(P3);
    TEST_ASSERT_EQ(dActual3, dExpected3);

    double dExpected4 = std::abs(P4.col - horiLine.getYIntercept());
    double dActual4 = horiLine.distanceToPoint(P4);
    TEST_ASSERT_EQ(dActual4, dExpected4);

    double dExpected5 = std::abs(P5.col - horiLine.getYIntercept());
    double dActual5 = horiLine.distanceToPoint(P5);
    TEST_ASSERT_EQ(dActual5, dExpected5);

    double dExpected6 = std::abs(P6.col - horiLine.getYIntercept());
    double dActual6 = horiLine.distanceToPoint(P6);
    TEST_ASSERT_EQ(dActual6, dExpected6);

    // 10. offsetFromPoint
    double d = 4.5;
    math::linear::Line2D::Point oExpected1(P1.row + d, P1.col);
    math::linear::Line2D::Point oActual1 = horiLine.offsetFromPoint(P1, d);
    TEST_ASSERT_EQ(oActual1.row, oExpected1.row);
    TEST_ASSERT_EQ(oActual1.col, oExpected1.col);

    math::linear::Line2D::Point oExpected2(P2.row + d, P2.col);
    math::linear::Line2D::Point oActual2 = horiLine.offsetFromPoint(P2, d);
    TEST_ASSERT_EQ(oActual2.row, oExpected2.row);
    TEST_ASSERT_EQ(oActual2.col, oExpected2.col);

    math::linear::Line2D::Point oExpected3(P3.row + d, P3.col);
    math::linear::Line2D::Point oActual3 = horiLine.offsetFromPoint(P3, d);
    TEST_ASSERT_EQ(oActual3.row, oExpected3.row);
    TEST_ASSERT_EQ(oActual3.col, oExpected3.col);

    math::linear::Line2D::Point oExpected4(P4.row + d, P4.col);
    math::linear::Line2D::Point oActual4 = horiLine.offsetFromPoint(P4, d);
    TEST_ASSERT_EQ(oActual4.row, oExpected4.row);
    TEST_ASSERT_EQ(oActual4.col, oExpected4.col);

    math::linear::Line2D::Point oExpected5(P5.row + d, P5.col);
    math::linear::Line2D::Point oActual5 = horiLine.offsetFromPoint(P5, d);
    TEST_ASSERT_EQ(oActual5.row, oExpected5.row);
    TEST_ASSERT_EQ(oActual5.col, oExpected5.col);

    math::linear::Line2D::Point oExpected6(P6.row + d, P6.col);
    math::linear::Line2D::Point oActual6 = horiLine.offsetFromPoint(P6, d);
    TEST_ASSERT_EQ(oActual6.row, oExpected6.row);
    TEST_ASSERT_EQ(oActual6.col, oExpected6.col);
}

TEST_CASE(testNormal)
{
    math::linear::Line2D::Point P1(2.7, 6);
    math::linear::Line2D::Point P2(4, 1);
    math::linear::Line2D normLine(P1,P2);

    // 1. test getSlope
    double expectedSlope = (1.0 - 6.0) / (4.0 - 2.7);
    double actualSlope = normLine.getSlope();
    TEST_ASSERT_EQ(actualSlope, expectedSlope);

    // 2. test getYIntercept()
    double expectedYIntercept = P1.col - P1.row * expectedSlope;
    double actualYIntercept = normLine.getYIntercept();
    TEST_ASSERT_EQ(actualYIntercept, expectedYIntercept);

    // 3. test getXIntercept()
    double expectedXIntercept = (-expectedYIntercept) / expectedSlope;
    double actualXIntercept = normLine.getXIntercept();
    TEST_ASSERT_EQ(actualXIntercept, expectedXIntercept);

    // 4. test y(double x)
    double m = 0;
    for (size_t i = 0; i < 500; ++i, m += 0.05)
    {
        double expectedY = expectedSlope * m + expectedYIntercept;
        double actualY = normLine.y(m);
        TEST_ASSERT_EQ(actualY, expectedY);
    }
    // 5. test x(double y)
    m = 0;
    for (size_t i = 0; i < 500; ++i, m += 0.05)
    {
        double expectedX = (m - expectedYIntercept) / expectedSlope;
        double actualX = normLine.x(m);
        TEST_ASSERT_EQ(actualX, expectedX);
    }
    // 6. testing intersection
    // with another normal line:
    math::linear::Line2D::Point P3(34, 9.3);
    math::linear::Line2D::Point P4(3, 90);
    math::linear::Line2D normLine2(P3, P4);
    TEST_EXCEPTION(normLine.intersection(normLine));
    double expectedRow = (normLine2.getYIntercept() - normLine.getYIntercept())
                    / (normLine.getSlope() - normLine2.getSlope());
    double expectedCol = normLine.y(expectedRow);
    math::linear::Line2D::Point actualIntersection =
            normLine.intersection(normLine2);
    TEST_ASSERT_EQ(actualIntersection.row, expectedRow);
    TEST_ASSERT_EQ(actualIntersection.col, expectedCol);

    // with a vertical line:
    math::linear::Line2D vertLine(math::linear::Line2D::Point(4, 8),
            math::linear::Line2D::Point(4, 5));
    double expectedRow2 = 4;
    double expectedCol2 = normLine.y(4);
    math::linear::Line2D::Point actualIntersection2 =
            normLine.intersection(vertLine);
    TEST_ASSERT_EQ(actualIntersection2.row, expectedRow2);
    TEST_ASSERT_EQ(actualIntersection2.col, expectedCol2);

    // with a horizontal line:
    math::linear::Line2D horiLine(math::linear::Line2D::Point(2, 9),
            math::linear::Line2D::Point(4, 9));
    double expectedRow3 = normLine.x(9);
    double expectedCol3 = 9;
    math::linear::Line2D::Point actualIntersection3 =
            normLine.intersection(horiLine);
    TEST_ASSERT_EQ(actualIntersection3.row, expectedRow3);
    TEST_ASSERT_EQ(actualIntersection3.col, expectedCol3);

    // 7. testing parallelToLine
    math::linear::Line2D L1 = normLine.parallelToLine(P1);
    math::linear::Line2D L2 = normLine.parallelToLine(P2);
    math::linear::Line2D L3 = normLine.parallelToLine(P3);
    math::linear::Line2D L4 = normLine.parallelToLine(P4);
    TEST_EXCEPTION(normLine.intersection(L1));
    TEST_EXCEPTION(normLine.intersection(L2));
    TEST_EXCEPTION(normLine.intersection(L3));
    TEST_EXCEPTION(normLine.intersection(L4));

    // 8. testing perpendicularToLine
    math::linear::Line2D pL1 = normLine.perpendicularToLine(P1);
    math::linear::Line2D pL2 = normLine.perpendicularToLine(P2);
    math::linear::Line2D pL3 = normLine.perpendicularToLine(P3);
    math::linear::Line2D pL4 = normLine.perpendicularToLine(P4);

    math::linear::Line2D rPL1 = L1.perpendicularToLine(P1);
    math::linear::Line2D rPL2 = L2.perpendicularToLine(P2);
    math::linear::Line2D rPL3 = L3.perpendicularToLine(P3);
    math::linear::Line2D rPL4 = L4.perpendicularToLine(P4);

    TEST_EXCEPTION(pL1.intersection(rPL1));
    TEST_EXCEPTION(pL2.intersection(rPL2));
    TEST_EXCEPTION(pL3.intersection(rPL3));
    TEST_EXCEPTION(pL4.intersection(rPL4));

    // 9. testing distanceToPoint
    TEST_ASSERT_EQ(normLine.distanceToPoint(P1), expectedDistToPt(normLine, P1));
    TEST_ASSERT_EQ(normLine.distanceToPoint(P2), expectedDistToPt(normLine, P2));
    TEST_ASSERT_EQ(normLine.distanceToPoint(P3), expectedDistToPt(normLine, P3));
    TEST_ASSERT_EQ(normLine.distanceToPoint(P4), expectedDistToPt(normLine, P4));

    // 10. testing offsetFromPoint
    // P1
    m = 0;
    for (size_t i = 0; i < 100; ++i, m += 0.1)
    {
        math::linear::Line2D::Point expectedO = expectedOffSet(normLine, P1, m);
        math::linear::Line2D::Point actualO = normLine.offsetFromPoint(P1, m);
        TEST_ASSERT_EQ(actualO.row, expectedO.row);
        TEST_ASSERT_EQ(actualO.col, expectedO.col);
    }
    // P2
    m = 0;
    for (size_t i = 0; i < 100; ++i, m += 0.1)
    {
        math::linear::Line2D::Point expectedO = expectedOffSet(normLine, P2, m);
        math::linear::Line2D::Point actualO = normLine.offsetFromPoint(P2, m);
        TEST_ASSERT_EQ(actualO.row, expectedO.row);
        TEST_ASSERT_EQ(actualO.col, expectedO.col);
    }
    // P3
    m = 0;
    for (size_t i = 0; i < 100; ++i, m += 0.1)
    {
        math::linear::Line2D::Point expectedO = expectedOffSet(normLine, P3, m);
        math::linear::Line2D::Point actualO = normLine.offsetFromPoint(P3, m);
        TEST_ASSERT_EQ(actualO.row, expectedO.row);
        TEST_ASSERT_EQ(actualO.col, expectedO.col);
    }
    // P4
    m = 0;
    for (size_t i = 0; i < 100; ++i, m += 0.1)
    {
        math::linear::Line2D::Point expectedO = expectedOffSet(normLine, P4, m);
        math::linear::Line2D::Point actualO = normLine.offsetFromPoint(P4, m);
        TEST_ASSERT_EQ(actualO.row, expectedO.row);
        TEST_ASSERT_EQ(actualO.col, expectedO.col);
    }
}
}

int main()
{
    TEST_CHECK(testVertical);
    TEST_CHECK(testHorizontal);
    TEST_CHECK(testNormal);
    return 0;
}
