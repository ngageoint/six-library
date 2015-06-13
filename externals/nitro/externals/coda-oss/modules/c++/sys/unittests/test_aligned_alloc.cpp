/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>

#include <sys/Conf.h>
#include <sys/Path.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include "TestCase.h"


namespace
{

const size_t numBytes(16384);

bool
testAlignedAlloc(const size_t numBytes, const size_t alignment)
{
    // Allocate an aligned buffer
    void* const ptr = sys::alignedAlloc(numBytes, alignment);

    // Confirm it's a multiple of alignment
    bool const isAligned(reinterpret_cast<size_t>(ptr) % alignment == 0);

    sys::alignedFree(ptr);

    if (!isAligned)
    {
        std::cerr << "Error: buffer " << ptr
                  << " isn't aligned as expected!\n";
    }
    return isAligned;
}

TEST_CASE(testAlignedAlloc8)
{
    TEST_ASSERT(testAlignedAlloc(numBytes, 8));
}
TEST_CASE(testAlignedAlloc16)
{
    TEST_ASSERT(testAlignedAlloc(numBytes, 16));
}
TEST_CASE(testAlignedAlloc32)
{
    TEST_ASSERT(testAlignedAlloc(numBytes, 32));
}
TEST_CASE(testAlignedAlloc64)
{
    TEST_ASSERT(testAlignedAlloc(numBytes, 64));
}
TEST_CASE(testAlignedAlloc128)
{
    TEST_ASSERT(testAlignedAlloc(numBytes, 128));
}

}

int main(int argc, char** argv)
{
    TEST_CHECK(testAlignedAlloc8);
    TEST_CHECK(testAlignedAlloc16);
    TEST_CHECK(testAlignedAlloc32);
    TEST_CHECK(testAlignedAlloc64);
    TEST_CHECK(testAlignedAlloc128);

    return 0;
}
