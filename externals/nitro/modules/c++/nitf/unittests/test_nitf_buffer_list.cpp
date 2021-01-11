/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <string.h>

#include "TestCase.h"

#include <nitf/NITFBufferList.hpp>

namespace
{
TEST_CASE(testGetNumBlocks)
{
    // 5000 total bytes
    nitf::NITFBufferList bufferList;
    bufferList.pushBack(nullptr, 1000);
    bufferList.pushBack(nullptr, 2000);
    bufferList.pushBack(nullptr, 500);
    bufferList.pushBack(nullptr, 1500);

    // Evenly divides
    TEST_ASSERT_EQ(bufferList.getNumBlocks(1000), 5);

    // Doesn't evenly divide - we should just get one bigger block
    TEST_ASSERT_EQ(bufferList.getNumBlocks(999), 5);
}

TEST_CASE(testGetBlock)
{
    // 100 total bytes
    std::vector<nitf::byte> buffer(100);
    for (size_t ii = 0; ii < buffer.size(); ++ii)
    {
        buffer[ii] = static_cast<nitf::byte>(rand() % 256);
    }

    // Break this into a few pieces
    std::vector<nitf::byte> buffer1(buffer.begin(), buffer.begin() + 10);
    std::vector<nitf::byte> buffer2(buffer.begin() + 10, buffer.begin() + 20);
    std::vector<nitf::byte> buffer3(buffer.begin() + 20, buffer.begin() + 35);
    std::vector<nitf::byte> buffer4(buffer.begin() + 35, buffer.begin() + 57);
    std::vector<nitf::byte> buffer5(buffer.begin() + 57, buffer.end());

    // Add them all on
    nitf::NITFBufferList bufferList;
    bufferList.pushBack(buffer1);
    bufferList.pushBack(buffer2);
    bufferList.pushBack(buffer3);
    bufferList.pushBack(buffer4);
    bufferList.pushBack(buffer5);

    // No matter what the block size is, we should get back all the bytes
    for (size_t blockSize = 1; blockSize <= 100; ++blockSize)
    {
        // Get the total number of bytes across all blocks
        // This should match the total size
        size_t numTotalBytes(0);
        const size_t numBlocks = bufferList.getNumBlocks(blockSize);
        for (size_t block = 0; block < numBlocks; ++block)
        {
            numTotalBytes += bufferList.getNumBytesInBlock(blockSize, block);
        }
        TEST_ASSERT_EQ(numTotalBytes, buffer.size());

        // Extract all the bytes
        std::vector<nitf::byte> extracted(numTotalBytes);
        nitf::byte* ptr = extracted.data();
        std::vector<nitf::byte> scratch;

        size_t numBytesInBlock;
        for (size_t block = 0; block < numBlocks; ++block)
        {
            const void* const blockPtr = bufferList.getBlock(blockSize,
                                                             block,
                                                             scratch,
                                                             numBytesInBlock);

            memcpy(ptr, blockPtr, numBytesInBlock);
            ptr += numBytesInBlock;
        }

        // Bytes should all match
        for (size_t ii = 0; ii < buffer.size(); ++ii)
        {
            const auto extracted_ii = static_cast<uint8_t>(extracted[ii]);
            const auto buffer_ii = static_cast<uint8_t>(buffer[ii]);
            TEST_ASSERT_EQ(extracted_ii, buffer_ii);
        }

        TEST_EXCEPTION(bufferList.getBlock(blockSize, numBlocks, scratch,
                                           numBytesInBlock));
    }
}
}

TEST_MAIN(
    (void)argc;
    (void)argv;
    TEST_CHECK(testGetNumBlocks);
    TEST_CHECK(testGetBlock);
    )