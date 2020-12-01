/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nrt.h>
#include "Test.h"

TEST_CASE(testCoreValues)
{
    int expected = 1; 
    TEST_ASSERT_EQ_INT(sizeof(uint8_t), expected);
    TEST_ASSERT_EQ_INT(sizeof(int8_t), expected);

    expected = 2;
    TEST_ASSERT_EQ_INT(sizeof(uint16_t), expected);
    TEST_ASSERT_EQ_INT(sizeof(int16_t), expected);

    expected = 4;
    TEST_ASSERT_EQ_INT(sizeof(uint32_t), expected);
    TEST_ASSERT_EQ_INT(sizeof(int32_t), expected);

    expected = 8;
    TEST_ASSERT_EQ_INT(sizeof(uint64_t), expected);
    TEST_ASSERT_EQ_INT(sizeof(int64_t), expected);

    int sizeof_long = sizeof(long);
    if (sizeof_long == 4)
    {
        const char *ok = "2147483647";
        const char *bad = "2147483648";
        /* Test our assertions for atol, format */

        printf("A long is 4 bytes\n");
        printf("Ok: (str: %s) [%ld]\n", ok, atol(ok));
        printf("Bad: (str: %s) [%ld]\n", bad, atol(bad));

        printf("As long long\n");
        printf("Ok: (str: %s) [%lld]\n", ok, NRT_ATO64(ok));
        printf("Bad: (str: %s) [%lld]\n", bad, NRT_ATO64(bad));

    }
    else if (sizeof_long == 8)
    {
        const char *ok = "9223372036854775807";
        const char *bad = "9223372036854775808";
        printf("A long is 8 bytes\n");
        printf("Ok: (str: %s) [%ld]\n", ok, atol(ok));
        printf("Bad: (str: %s) [%ld]\n", bad, atol(bad));

        printf("As long long\n");
        printf("Ok: (str: %s) [%lld]\n", ok, NRT_ATO64(ok));
        printf("Bad: (str: %s) [%lld]\n", bad, NRT_ATO64(bad));
    }
}

TEST_MAIN(
    (void)argc;
    (void)argv;

    CHECK(testCoreValues);
)