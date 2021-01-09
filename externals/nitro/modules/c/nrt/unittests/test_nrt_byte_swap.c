/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nrt.h>
#include "Test.h"


TEST_CASE(testFourBytes)
{
    uint32_t value;
    uint32_t swappedValue;

    value = 0x12345678;
    swappedValue = 0x78563412;
    nrt_Utils_byteSwap((uint8_t*)&value, 4);
    TEST_ASSERT(value == swappedValue);
}

TEST_CASE(testTwoBytes)
{
    uint16_t value;
    uint16_t swappedValue;

    value = 0x0304;
    swappedValue = 0x0403;
    nrt_Utils_byteSwap((uint8_t*)&value, 2);
    TEST_ASSERT(value == swappedValue);
}

TEST_CASE(testEightBytes)
{
    uint64_t value;
    uint64_t swappedValue;

    value = 0x0123456789ABCDEF;
    swappedValue = 0xEFCDAB8967452301;
    nrt_Utils_byteSwap((uint8_t*)&value, 8);
    TEST_ASSERT(value == swappedValue);
}

TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK(testFourBytes);
    CHECK(testTwoBytes);
    CHECK(testEightBytes);
    )

