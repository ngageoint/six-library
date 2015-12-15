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

#include <math/poly/OneD.h>
#include <serialize/Serialize.h>
#include "TestCase.h"

namespace
{

TEST_CASE(testOneDDoubleSerialize)
{
    //! create the input object
    math::poly::OneD<double> in(6);
    for (size_t ii = 0; ii < in.order() + 1; ++ii)
    {
        in[ii] = static_cast<double>(rand());
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::OneD<double> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.order() + 1; ++ii)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(in[ii], out[ii],
                                  std::numeric_limits<double>::epsilon());
    }
}

TEST_CASE(testOneDFloatSerialize)
{
    //! create the input object
    math::poly::OneD<float> in(13);
    for (size_t ii = 0; ii < in.order() + 1; ++ii)
    {
        in[ii] = static_cast<float>(rand());
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::OneD<float> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.order() + 1; ++ii)
    {
        TEST_ASSERT_ALMOST_EQ_EPS(in[ii], out[ii],
                                  std::numeric_limits<float>::epsilon());
    }
}

TEST_CASE(testOneDIntSerialize)
{
    //! create the input object
    math::poly::OneD<int> in(21);
    for (size_t ii = 0; ii < in.order() + 1; ++ii)
    {
        in[ii] = rand();
    }

    //! serialize to string
    std::stringstream ss;
    boost::archive::binary_oarchive arO(ss);
    arO & in;

    //! serialize from string
    math::poly::OneD<int> out;
    boost::archive::binary_iarchive arI(ss);
    arI & out;

    //! verify the result matches
    for (size_t ii = 0; ii < in.order() + 1; ++ii)
    {
        TEST_ASSERT_EQ(in[ii], out[ii]);
    }
}

}

int main(int argc, char** argv)
{
    srand(176);
    TEST_CHECK(testOneDDoubleSerialize);
    TEST_CHECK(testOneDFloatSerialize);
    TEST_CHECK(testOneDIntSerialize);
}
