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
    TEST_ASSERT(success);
    TEST_ASSERT(output[0] == (char)2);
    TEST_ASSERT(output[1] == (char)2);
    TEST_ASSERT(output[2] == (char)0);
    TEST_ASSERT(output[3] == (char)0);
    TEST_ASSERT(output[4] == (char)0);
}

TEST_CASE(testReadOutOfBounds)
{
    char buffer[TEST_BUF_SIZE];
    char output[5];
    nrt_Error error;
    size_t ii;
    NRT_BOOL success;

    memset(buffer, 0, 3);
    memset(buffer + 3, 1, 5);
    memset(buffer + 8, 2, 2);

    nrt_IOInterface* reader = nrt_BufferAdapter_construct(
        buffer, TEST_BUF_SIZE, 0, &error);

    success = nrt_IOInterface_seek(reader, TEST_BUF_SIZE, NRT_SEEK_SET, &error);
    TEST_ASSERT(success);
    success = nrt_IOInterface_read(reader, output, sizeof(output), &error);
    TEST_ASSERT(success);
    for (ii = 0; ii < sizeof(output); ++ii)
    {
        TEST_ASSERT(output[ii] == (char)0);
    }
}

TEST_CASE(testWriteOutOfBounds)
{
    char buffer[TEST_BUF_SIZE];
    char input[5];
    nrt_Error error;
    NRT_BOOL success;

    memset(input, 0, sizeof(input));

    nrt_IOInterface* writer = nrt_BufferAdapter_construct(
        buffer, TEST_BUF_SIZE, 0, &error);

    success = nrt_IOInterface_seek(writer, TEST_BUF_SIZE, NRT_SEEK_SET, &error);
    TEST_ASSERT(success);
    success = nrt_IOInterface_write(writer, input, 4, &error);
    TEST_ASSERT(!success);
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    CHECK(testReadInBounds);
    CHECK(testReadPastEnd);
    CHECK(testReadOutOfBounds);
    CHECK(testWriteOutOfBounds);
    return 0;
}


