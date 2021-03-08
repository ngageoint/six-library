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

#include <stdlib.h>

#include <math/linear/Matrix2D.h>
#include <serialize/Serialize.h>
#include "TestCase.h"

namespace
{

TEST_CASE(testMatrix2DDoubleSerializeSquare)
{
    //! create the input object
    math::linear::Matrix2D<double> in(6, 6);
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            in[ii][jj] = static_cast<double>(rand());
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::linear::Matrix2D<double> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            TEST_ASSERT_ALMOST_EQ_EPS(in[ii][jj], out[ii][jj],
                                      std::numeric_limits<double>::epsilon());
        }
    }
}

TEST_CASE(testMatrix2DDoubleSerializeRect)
{
    //! create the input object
    math::linear::Matrix2D<double> in(2, 12);
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            in[ii][jj] = static_cast<double>(rand());
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::linear::Matrix2D<double> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            TEST_ASSERT_ALMOST_EQ_EPS(in[ii][jj], out[ii][jj],
                                      std::numeric_limits<double>::epsilon());
        }
    }
}

TEST_CASE(testMatrix2DIntSerializeSquare)
{
    //! create the input object
    math::linear::Matrix2D<int> in(5, 5);
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            in[ii][jj] = rand();
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::linear::Matrix2D<int> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            TEST_ASSERT_EQ(in[ii][jj], out[ii][jj]);
        }
    }
}


TEST_CASE(testMatrix2DIntSerializeRect)
{
    //! create the input object
    math::linear::Matrix2D<int> in(21, 3);
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            in[ii][jj] = rand();
        }
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::linear::Matrix2D<int> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.rows(); ++ii)
    {
        for (size_t jj = 0; jj < in.cols(); ++jj)
        {
            TEST_ASSERT_EQ(in[ii][jj], out[ii][jj]);
        }
    }
}

}

int main(int argc, char** argv)
{
    srand(176);
    TEST_CHECK(testMatrix2DDoubleSerializeSquare);
    TEST_CHECK(testMatrix2DDoubleSerializeRect);
    TEST_CHECK(testMatrix2DIntSerializeSquare);
    TEST_CHECK(testMatrix2DIntSerializeRect);
}
