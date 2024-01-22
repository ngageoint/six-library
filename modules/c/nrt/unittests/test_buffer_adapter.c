/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#define TEST_BUF_SIZE 10

TEST_CASE(testReadInBounds)
{
    char buffer[TEST_BUF_SIZE];
    char output[5];
    nrt_Error error;
    size_t ii;

    memset(buffer, 0, 3);
    memset(buffer + 3, 1, 5);
    memset(buffer + 8, 2, 2);

    nrt_IOInterface* reader = nrt_BufferAdapter_construct(
        buffer, TEST_BUF_SIZE, 0, &error);

    nrt_IOInterface_seek(reader, 3, NRT_SEEK_SET, &error);
    nrt_IOInterface_read(reader, output, sizeof(output), &error);
    for (ii = 0; ii < sizeof(output); ++ii)
    {
        TEST_ASSERT(output[ii] == (char)1);
    }

    nrt_IOInterface_destruct(&reader);
}

TEST_CASE(testReadPastEnd)
{
    char buffer[TEST_BUF_SIZE];
    char output[5];
    nrt_Error error;
    NRT_BOOL success;

    memset(buffer, 0, 3);
    memset(buffer + 3, 1, 5);
    memset(buffer + 8, 2, 2);

    nrt_IOInterface* reader = nrt_BufferAdapter_construct(
        buffer, TEST_BUF_SIZE, 0, &error);

    nrt_IOInterface_seek(reader, 8, NRT_SEEK_SET, &error);
    success = nrt_IOInterface_read(reader, output, sizeof(output), &error);
    TEST_ASSERT(!success);

    nrt_IOInterface_destruct(&reader);
}

TEST_CASE(testReadOutOfBounds)
{
    char buffer[TEST_BUF_SIZE];
    char output[5];
    nrt_Error error;

    memset(buffer, 0, 3);
    memset(buffer + 3, 1, 5);
    memset(buffer + 8, 2, 2);

    nrt_IOInterface* reader = nrt_BufferAdapter_construct(
        buffer, TEST_BUF_SIZE, 0, &error);

    nrt_Off success = nrt_IOInterface_seek(reader, TEST_BUF_SIZE, NRT_SEEK_SET, &error);
    TEST_ASSERT(success != 0);
    success = nrt_IOInterface_read(reader, output, sizeof(output), &error);
    TEST_ASSERT(!success);

    nrt_IOInterface_destruct(&reader);
}

TEST_CASE(testWriteOutOfBounds)
{
    char buffer[TEST_BUF_SIZE];
    char input[5];
    nrt_Error error;

    memset(input, 0, sizeof(input));

    nrt_IOInterface* writer = nrt_BufferAdapter_construct(
        buffer, TEST_BUF_SIZE, 0, &error);

    nrt_Off success = nrt_IOInterface_seek(writer, TEST_BUF_SIZE, NRT_SEEK_SET, &error);
    TEST_ASSERT(success != 0);
    success = nrt_IOInterface_write(writer, input, 4, &error);
    TEST_ASSERT(!success);

    nrt_IOInterface_destruct(&writer);
}

TEST_MAIN(
    CHECK(testReadInBounds);
    CHECK(testReadPastEnd);
    CHECK(testReadOutOfBounds);
    CHECK(testWriteOutOfBounds);
    )
