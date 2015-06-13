/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * str-c++ is free software; you can redistribute it and/or modify
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

#include <import/str.h>
#include "TestCase.h"

TEST_CASE(testConvert)
{
    TEST_ASSERT_EQ(str::toType<long long>("0x3BC7", 16), (long long) 0x3BC7);
    TEST_ASSERT_EQ(str::toType<long long>("1101", 2), (long long) 13);
    TEST_ASSERT_EQ(str::toType<long long>("231", 5), (long long) 66);
    TEST_ASSERT_EQ(str::toType<unsigned long long>("0xFFFFFFFFFFFFFFFF", 16),
                   (unsigned long long) 0xFFFFFFFFFFFFFFFF);
    TEST_ASSERT_EQ(str::toType<unsigned long long>("-10", 10),
                   (unsigned long long) -10);
    TEST_ASSERT_EQ(str::toType<short>("13", 4), (short) 7);
}

TEST_CASE(testBadConvert)
{
    TEST_EXCEPTION(str::toType<long long>("Not a number", 10));
    TEST_EXCEPTION(str::toType<long long>("0xFFFFFFFFFFFFFFFF", 16));
    TEST_EXCEPTION(str::toType<short>("0xFFFFF", 16));
}

int main(int argc, char* argv[])
{
    TEST_CHECK(testConvert);
    TEST_CHECK(testBadConvert);
}
