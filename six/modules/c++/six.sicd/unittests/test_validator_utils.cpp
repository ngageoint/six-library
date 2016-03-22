/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <import/six/sicd.h>
#include "TestCase.h"

TEST_CASE(CoordsFromPolyXYZ)
{
    std::vector<six::Vector3> coefs;

    for (size_t ii = 0; ii < 3; ++ii)
    {
        std::vector<double> source;
        source.push_back(0.0 + ii);
        source.push_back(3.0 + ii);
        source.push_back(6.0 + ii);
        six::Vector3 coefficient(source);
        coefs.push_back(coefficient);
    }
    six::PolyXYZ poly(coefs);
    six::Poly1D xPoly = six::sicd::ComplexXMLValidator::polyAt(poly, 0);

    TEST_ASSERT_ALMOST_EQ(xPoly[0], 0.0);
    TEST_ASSERT_ALMOST_EQ(xPoly[1], 1.0);
    TEST_ASSERT_ALMOST_EQ(xPoly[2], 2.0);
    TEST_ASSERT_EQ(xPoly.size(), 3);

    six::Poly1D yPoly = six::sicd::ComplexXMLValidator::polyAt(poly, 1);
    TEST_ASSERT_ALMOST_EQ(yPoly[0], 3.0);
    TEST_ASSERT_ALMOST_EQ(yPoly[1], 4.0);
    TEST_ASSERT_ALMOST_EQ(yPoly[2], 5.0);
    TEST_ASSERT_EQ(yPoly.size(), 3);

    six::Poly1D zPoly = six::sicd::ComplexXMLValidator::polyAt(poly, 2);
    TEST_ASSERT_ALMOST_EQ(zPoly[0], 6.0);
    TEST_ASSERT_ALMOST_EQ(zPoly[1], 7.0);
    TEST_ASSERT_ALMOST_EQ(zPoly[2], 8.0);
    TEST_ASSERT_EQ(zPoly.size(), 3);
}

TEST_CASE(linSpace)
{
    std::vector<double> spaced = six::sicd::ComplexXMLValidator::linspace(-3.4, 5.7, 50);
    TEST_ASSERT_ALMOST_EQ(spaced[0], -3.4);
    TEST_ASSERT_ALMOST_EQ(spaced[spaced.size() - 1], 5.7);
    TEST_ASSERT_EQ(spaced.size(), 50);
    for (size_t ii = 1; ii < spaced.size() - 1; ++ii)
    {
        TEST_ASSERT_ALMOST_EQ(std::abs(spaced[ii] - spaced[ii - 1]), std::abs(spaced[ii + 1] - spaced[ii]));
    }
}

int main(int, char**)
{
    TEST_CHECK(CoordsFromPolyXYZ);
    TEST_CHECK(linSpace);
    return 0;
}