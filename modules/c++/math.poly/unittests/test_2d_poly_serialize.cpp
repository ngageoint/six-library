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

#include <stdlib.h>

#include <math/poly/TwoD.h>
#include <serialize/Serialize.h>
#include "TestCase.h"

namespace
{

TEST_CASE(testTwoDDoubleSerializeSquare)
{
    //! create the input object
    math::poly::TwoD<double> in(6, 6);
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            in[ii][jj] = static_cast<double>(rand());
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::TwoD<double> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            TEST_ASSERT_ALMOST_EQ_EPS(in[ii][jj], out[ii][jj],
                                      std::numeric_limits<double>::epsilon());
        }
    }
}

TEST_CASE(testTwoDDoubleSerializeRect)
{
    //! create the input object
    math::poly::TwoD<double> in(2, 12);
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            in[ii][jj] = static_cast<double>(rand());
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::TwoD<double> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            TEST_ASSERT_ALMOST_EQ_EPS(in[ii][jj], out[ii][jj],
                                      std::numeric_limits<double>::epsilon());
        }
    }
}

TEST_CASE(testTwoDIntSerializeSquare)
{
    //! create the input object
    math::poly::TwoD<int> in(5, 5);
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            in[ii][jj] = rand();
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::TwoD<int> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            TEST_ASSERT_EQ(in[ii][jj], out[ii][jj]);
        }
    }
}


TEST_CASE(testTwoDIntSerializeRect)
{
    //! create the input object
    math::poly::TwoD<int> in(21, 3);
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            in[ii][jj] = rand();
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::TwoD<int> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.orderX() + 1; ++ii)
    {
        for (size_t jj = 0; jj < in.orderY() + 1; ++jj)
        {
            TEST_ASSERT_EQ(in[ii][jj], out[ii][jj]);
        }
    }
}

}

int main(int argc, char** argv)
{
    srand(176);
    TEST_CHECK(testTwoDDoubleSerializeSquare);
    TEST_CHECK(testTwoDDoubleSerializeRect);
    TEST_CHECK(testTwoDIntSerializeSquare);
    TEST_CHECK(testTwoDIntSerializeRect);
}
