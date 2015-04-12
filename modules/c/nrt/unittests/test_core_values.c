/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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
    TEST_ASSERT(sizeof(nrt_Uint8) == 1);
    TEST_ASSERT(sizeof(nrt_Uint16) == 2);
    TEST_ASSERT(sizeof(nrt_Uint32) == 4);
    TEST_ASSERT(sizeof(nrt_Uint64) == 8);

    TEST_ASSERT(sizeof(nrt_Int8) == 1);
    TEST_ASSERT(sizeof(nrt_Int16) == 2);
    TEST_ASSERT(sizeof(nrt_Int32) == 4);
    TEST_ASSERT(sizeof(nrt_Int64) == 8);

    if (sizeof(long) == 4)
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
    else if (sizeof(long) == 8)
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

int main(int argc, char **argv)
{
    CHECK(testCoreValues);
    return 0;
}
