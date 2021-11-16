/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.poly-c++ is free software; you can redistribute it and/or modify
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

#include <import/math/linear.h>
#include <import/math/poly.h>
#include "TestCase.h"

using namespace math::linear;
using namespace math::poly;

namespace
{
inline
double diffSq(double lhs, double rhs)
{
    const double diff = lhs - rhs;
    return (diff * diff);
}

TEST_CASE(test1DPolyfit)
{
    const double xObs[] = { 1, -1, 2, -2 };
    const double yObs[] = { 3, 13, 1, 33 };
    const double zPoly[] = { 5, -4, 3, -1 };

    const std::vector<double> truthSTLVec(zPoly, zPoly + 4);

    const OneD<double> truth(3, zPoly);
    // First test the raw pointer signature
    const OneD<double> polyFromRaw = fit(4, xObs, yObs, 3);

    // should fail with not enough points (order>=npoints)
    TEST_EXCEPTION(fit(4, xObs, yObs, 4));
    
    // Now call the other one
    const Vector<double> xv(4, xObs);
    const Vector<double> yv(4, yObs);

    const OneD<double> polyFromVec = fit(xv, yv, 3);

    const Fixed1D<3> fixed = fit(xv, yv, 3);

    const OneD<double> polyFromSTL(truthSTLVec);

    // Polys better match
    TEST_ASSERT_EQ(polyFromRaw, polyFromVec);
    TEST_ASSERT_EQ(polyFromRaw, truth);
    //TEST_ASSERT_EQ(polyFromRaw, fixed);
    TEST_ASSERT(polyFromRaw == fixed);
    assert(polyFromRaw == truthSTLVec);
    TEST_ASSERT_EQ(polyFromSTL, truth);
}

TEST_CASE(test1DPolyfitLarge)
{
    // Fit a polynomial
    static const size_t NUM_OBS = 9;
    double xObs[] = { 1, -1, 2, -2, 3, 15, 29, -4, -14 };
    const double yObs[] = { 3, 13, 1, 33, -7, -2755, -21977, 133, 3393 };

    static const size_t POLY_ORDER = 3;
    const OneD<double> polyUnshifted = fit(NUM_OBS, xObs, yObs, POLY_ORDER);

    // Now shift all the x values and fit another polynomial
    static const size_t OFFSET = 10000;
    double xObsShifted[NUM_OBS];
    for (size_t ii = 0; ii < NUM_OBS; ++ii)
    {
        xObsShifted[ii] = xObs[ii] + OFFSET;
    }

    const OneD<double> polyShifted =
            fit(NUM_OBS, xObsShifted, yObs, POLY_ORDER);

    // If we evaluate the polynomials at equivalent x positions, we better
    // have almost the same values
    // TODO: Seems like I need a bigger epsilon here than I'd expect
    for (size_t ii = 0; ii < NUM_OBS; ++ii)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(polyUnshifted(xObs[ii]),
                                  polyShifted(xObsShifted[ii]),
                                  0.0005);
    }

    // Calculate the mean residual error to determine goodness of fit.
    double errorSumUnshifted = 0.0;
    double errorSumShifted = 0.0;
    for (size_t ii = 0; ii < NUM_OBS; ++ii)
    {
        errorSumUnshifted += diffSq(polyUnshifted(xObs[ii]), yObs[ii]);
        errorSumShifted += diffSq(polyShifted(xObsShifted[ii]), yObs[ii]);
    }
    const double meanResidualErrorUnshifted = errorSumUnshifted / NUM_OBS;
    const double meanResidualErrorShifted = errorSumShifted / NUM_OBS;

    /*
    std::cout << "meanResidualErrorUnshifted: "
              << meanResidualErrorUnshifted
              << "\nmeanResidualErrorShifted: "
              << meanResidualErrorShifted
              << std::endl;
    */

    TEST_ASSERT_ALMOST_EQ(meanResidualErrorUnshifted, 0.0);

    // TODO: This one is around 1.3e-7 which isn't as good as the 1.0e-22
    //       for the unshifted case
    TEST_ASSERT_ALMOST_EQ_EPS(meanResidualErrorShifted, 0.0, 2e-7);
}

TEST_CASE(test2DPolyfit)
{
    const double coeffs[] =
    {
        -1.02141e-16, 0.15,
         0.08,        0.4825,
    };

    TwoD<double> truth(1, 1, coeffs);

    Matrix2D<double> x(3, 3);
    x(0, 0) = 1;  x(1, 0) = 0; x(2, 0) = 1;
    x(0, 1) = 1;  x(1, 1) = 1; x(2, 1) = 0;
    x(0, 2) = 0;  x(1, 2) = 1; x(2, 2) = 1;
        
    Matrix2D<double> y(3, 3);
    y(0, 0) = 1;  y(1, 0) = 1; y(2, 0) = 1;
    y(0, 1) = 0;  y(1, 1) = 1; y(2, 1) = 1;
    y(0, 2) = 0;  y(1, 2) = 0; y(2, 2) = 1;


    Matrix2D<double> z(3, 3);
    z(0, 0) = 1;   z(1, 0) = .3; z(2, 0) = 0;
    z(0, 1) = .16; z(1, 1) = 1;  z(2, 1) = 0;
    z(0, 2) = 0;   z(1, 2) = 0;  z(2, 2) = .85;

    // should fail with not enough points: (orderX+1)*(orderY+1) > npoints
    TEST_EXCEPTION(fit(x, y, z, 3, 3));

    TwoD<double> poly = fit(x, y, z, 1, 1);
    TEST_ASSERT_EQ(poly, truth);
}

TEST_CASE(test2DPolyfitLarge)
{
    // Use a defined polynomial to generate mapped values.  This ensures
    // it is possible to fit the points using at least as many coefficients.
    const double coeffs[] =
    {
        -1.021e-12, 7.5,    2.2,   5.5,
         0.88,      4.825,  .52,   .69,
         5.5,       1.0,    .62,   1.01,
         .012,      6.32,   1.56,  .376
    };

    TwoD<double> truth(3, 3, coeffs);

    // Specifically sampling points far from (0,0) to verify an issue
    // identified when fitting non-centered input.

    size_t gridSize = 9; // 9x9
    size_t xOffset = 25000;
    size_t xSpacing = 2134;
    size_t yOffset = 42000;
    size_t ySpacing = 3214;

    Matrix2D<double> x(gridSize, gridSize);
    for (size_t i = 0; i < gridSize; i++)
    {
        auto xidx = static_cast<double>(xOffset + i * xSpacing);
        for (size_t j = 0; j < gridSize; j++)
        {
            x(i, j) = xidx;
        }
    }

    Matrix2D<double> y(gridSize, gridSize);
    for (size_t j = 0; j < gridSize; j++)
    {
        auto yidx = static_cast<double>(yOffset + j * ySpacing);
        for (size_t i = 0; i < gridSize; i++)
        {
            y(i, j) = yidx;
        }
    }

    Matrix2D<double> z(gridSize, gridSize);
    for (size_t i = 0; i < gridSize; i++)
    {
        for (size_t j = 0; j < gridSize; j++)
        {
            z(i, j) = truth(static_cast<double>(i), static_cast<double>(j));
        }
    }

    // Fit polynomial
    TwoD<double> poly = fit(x, y, z, 5, 5);

    // Calculate the mean residual error to determine goodness of fit.
    double errorSum(0.0);
    for (size_t i = 0; i < gridSize; i++)
    {
        for (size_t j = 0; j < gridSize; j++)
        {
            errorSum += diffSq(z(i, j), poly(x(i, j), y(i, j)));
        }
    }
    double meanResidualError = errorSum / (gridSize * gridSize);

    // std::cout << "meanResidualError: " << meanResidualError << std::endl;

    TEST_ASSERT_ALMOST_EQ(meanResidualError, 0.0);
}

TEST_CASE(testVectorValuedOrderChange)
{
    // When fitting a vector-valued polynomial, math::poly::fit()
    // will perform 3 independent fits, and then assemble the coefficients
    // into vector-valued coefficients for the resulting polynomial.
    // During each independent fit, however, the order may end up being reduced
    // if the leading coefficient (and any sub-leading coefficients) are
    // identically 0. As a result, care must be taken when assembling
    // the scalar coefficients into a vector that zeros are added when
    // we exceed the order of one of the reduced polynomials.

    std::vector<double> indep(3);
    indep[0] = 0.0; indep[1] = 1.0; indep[2] = 2.0;
    std::vector<double> compZeroed(3);
    compZeroed[0] = 0.0; compZeroed[1] = 0.0; compZeroed[2] = 0.0;
    std::vector<double> comp1(3);
    comp1[0] = 1.0; comp1[1] = 2.0; comp1[2] = 4.0;
    std::vector<double> comp2(3);
    comp2[0] = 0.1; comp2[1] = 0.9; comp2[2] = 0.1;

    // First, test that we're actually reducing the order of some of the fits
    const OneD<double> polyZeroed = fit(indep, compZeroed, 2);
    const OneD<double> poly1 = fit(indep, comp1, 2);
    const OneD<double> poly2 = fit(indep, comp2, 2);

    auto order = polyZeroed.order();
    TEST_ASSERT_EQ(order, static_cast<size_t>(0));
    order = poly1.order();
    TEST_ASSERT_EQ(order, static_cast<size_t>(2));
    order = poly2.order();
    TEST_ASSERT_EQ(order, static_cast<size_t>(2));

    // Now, attempt to fit the vector-vector version.
    // We'll check reshuffle the zero component to make sure we caught all
    // component order reductions.

    // Zeroed values in the X component
    {
        const OneD<VectorN<3, double> > poly =
                fit(indep, compZeroed, comp1, comp2, 2);

        TEST_ASSERT_EQ(poly.order(), static_cast<size_t>(2));

        // X-component
        TEST_ASSERT_ALMOST_EQ(poly[0][0], 0.0);
        TEST_ASSERT_ALMOST_EQ(poly[1][0], 0.0);
        TEST_ASSERT_ALMOST_EQ(poly[2][0], 0.0);

        // Y-component
        TEST_ASSERT_ALMOST_EQ(poly[0][1], 1.0);
        TEST_ASSERT_ALMOST_EQ(poly[1][1], 0.5);
        TEST_ASSERT_ALMOST_EQ(poly[2][1], 0.5);

        // Z-component
        TEST_ASSERT_ALMOST_EQ(poly[0][2], 0.1);
        TEST_ASSERT_ALMOST_EQ(poly[1][2], 1.6);
        TEST_ASSERT_ALMOST_EQ(poly[2][2], -0.8);
    }

    // Zeroed values in the Y component
    {
        const OneD<VectorN<3, double> > poly =
                fit(indep, comp1, compZeroed, comp2, 2);

        TEST_ASSERT_EQ(poly.order(), static_cast<size_t>(2));

        // X-component
        TEST_ASSERT_ALMOST_EQ(poly[0][0], 1.0);
        TEST_ASSERT_ALMOST_EQ(poly[1][0], 0.5);
        TEST_ASSERT_ALMOST_EQ(poly[2][0], 0.5);

        // Y-component
        TEST_ASSERT_ALMOST_EQ(poly[0][1], 0.0);
        TEST_ASSERT_ALMOST_EQ(poly[1][1], 0.0);
        TEST_ASSERT_ALMOST_EQ(poly[2][1], 0.0);

        // Z-component
        TEST_ASSERT_ALMOST_EQ(poly[0][2], 0.1);
        TEST_ASSERT_ALMOST_EQ(poly[1][2], 1.6);
        TEST_ASSERT_ALMOST_EQ(poly[2][2], -0.8);
    }

    // Zeroed values in the Z component
    {
        const OneD<VectorN<3, double> > poly =
                fit(indep, comp1, comp2, compZeroed, 2);

        TEST_ASSERT_EQ(poly.order(), static_cast<size_t>(2));

        // X-component
        TEST_ASSERT_ALMOST_EQ(poly[0][0], 1.0);
        TEST_ASSERT_ALMOST_EQ(poly[1][0], 0.5);
        TEST_ASSERT_ALMOST_EQ(poly[2][0], 0.5);

        // Y-component
        TEST_ASSERT_ALMOST_EQ(poly[0][1], 0.1);
        TEST_ASSERT_ALMOST_EQ(poly[1][1], 1.6);
        TEST_ASSERT_ALMOST_EQ(poly[2][1], -0.8);

        // Z-component
        TEST_ASSERT_ALMOST_EQ(poly[0][2], 0.0);
        TEST_ASSERT_ALMOST_EQ(poly[1][2], 0.0);
        TEST_ASSERT_ALMOST_EQ(poly[2][2], 0.0);
    }
}
}

int main(int, char**)
{
    TEST_CHECK(test1DPolyfit);
    TEST_CHECK(test1DPolyfitLarge);
    TEST_CHECK(test2DPolyfit);
    TEST_CHECK(test2DPolyfitLarge);
    TEST_CHECK(testVectorValuedOrderChange);
    return 0;
}
