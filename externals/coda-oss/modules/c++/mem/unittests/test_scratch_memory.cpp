/* =========================================================================
 * This file is part of CODA-OSS
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * CODA-OSS is free software; you can redistribute it and/or modify
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

#include <mem/ScratchMemory.h>

#include <mem/BufferView.h>
#include <sys/Conf.h>
#include <cstdlib>
#include <vector>
#include <set>
#include "TestCase.h"

namespace
{
TEST_CASE(testReleaseSingleEndBuffer)
{
    //A single release of the last element. Tests with different scratch templates.
    mem::ScratchMemory scratch;

    scratch.put<sys::ubyte>("buf0", 11, 1, 13);
    scratch.put<int>("buf1", 17, 1, 13);

    size_t numBytes0 = 11 + 13 - 1;
    size_t numBytes1 = 17 * sizeof(int) + 13 - 1;
    TEST_ASSERT_EQ(scratch.getNumBytes(), numBytes0 + numBytes1);

    scratch.release("buf1");
    TEST_ASSERT_EQ(scratch.getNumBytes(), numBytes0 + numBytes1);

    scratch.put<sys::ubyte>("buf2", 10, 1, 13);
    TEST_ASSERT_EQ(scratch.getNumBytes(), numBytes0 + numBytes1);

    scratch.put<sys::ubyte>("buf3", 60, 1, 13);
    size_t numBytes2 = 10 + 13 - 1;
    size_t numBytes3 = 60 + 13 - 1;
    TEST_ASSERT_EQ(scratch.getNumBytes(), numBytes0 + numBytes2 + numBytes3);

    scratch.put<char>("buf4", 4, 3, 13);
    size_t numBytes4 = 3 * (4 + 13 - 1);
    TEST_ASSERT_EQ(scratch.getNumBytes(), numBytes0 + numBytes2 + numBytes3 + numBytes4);

    scratch.setup();
    sys::ubyte* pBuf0 = scratch.get<sys::ubyte>("buf0");
    sys::ubyte* pBuf1 = scratch.get<sys::ubyte>("buf1");
    sys::ubyte* pBuf2 = scratch.get<sys::ubyte>("buf2");

    TEST_ASSERT_EQ(pBuf1, pBuf2);
    TEST_ASSERT_NOT_EQ(pBuf0, pBuf2);
}

TEST_CASE(testReleaseMultipleEndBuffers)
{
    //Tests multiple releases of the current last element
    //Also tests releases two concurrent segments
    mem::ScratchMemory scratch;

    scratch.put<sys::ubyte>("buf0", 3, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 4);

    scratch.put<sys::ubyte>("buf1", 3, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 8);

    scratch.release("buf1");
    TEST_ASSERT_EQ(scratch.getNumBytes(), 8);

    scratch.put<sys::ubyte>("buf2", 2, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 8);

    scratch.put<sys::ubyte>("buf3", 3, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 11);

    scratch.release("buf3");
    TEST_ASSERT_EQ(scratch.getNumBytes(), 11);

    scratch.put<sys::ubyte>("buf4", 4, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 12);

    scratch.release("buf4");
    TEST_ASSERT_EQ(scratch.getNumBytes(), 12);

    scratch.put<sys::ubyte>("buf5", 2, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 12);

    scratch.put<sys::ubyte>("buf6", 3, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 14);

    scratch.release("buf6");
    scratch.release("buf5");

    scratch.put<sys::ubyte>("buf7", 2, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 14);

    scratch.setup();
    sys::ubyte* pBuf1 = scratch.get<sys::ubyte>("buf1");
    sys::ubyte* pBuf2 = scratch.get<sys::ubyte>("buf2");
    sys::ubyte* pBuf3 = scratch.get<sys::ubyte>("buf3");
    sys::ubyte* pBuf4 = scratch.get<sys::ubyte>("buf4");
    sys::ubyte* pBuf6 = scratch.get<sys::ubyte>("buf6");
    sys::ubyte* pBuf7 = scratch.get<sys::ubyte>("buf7");

    TEST_ASSERT_EQ(pBuf1, pBuf2);
    TEST_ASSERT_EQ(pBuf3, pBuf4);
    TEST_ASSERT_EQ(pBuf4, pBuf6);
    TEST_ASSERT_EQ(pBuf3, pBuf6);
    TEST_ASSERT_EQ(pBuf7, pBuf6);
}

TEST_CASE(testReleaseNonEndBuffers)
{
    //Test putting then releasing then putting again and releasing again
    mem::ScratchMemory scratch;

    scratch.put<sys::ubyte>("buf0", 2, 1, 2);
    scratch.put<sys::ubyte>("buf1", 2, 1, 2);
    scratch.put<sys::ubyte>("buf2", 2, 1, 2);
    scratch.put<sys::ubyte>("buf3", 3, 1, 2);

    scratch.release("buf1");
    scratch.release("buf3");
    scratch.put<sys::ubyte>("buf4", 2, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 13);

    scratch.put<sys::ubyte>("buf5", 3, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 13);

    scratch.release("buf4");
    scratch.release("buf5");

    scratch.put<sys::ubyte>("buf6", 2, 1, 2);
    scratch.put<sys::ubyte>("buf7", 3, 1, 2);

    scratch.setup();
    sys::ubyte* pBuf1 = scratch.get<sys::ubyte>("buf1");
    sys::ubyte* pBuf4 = scratch.get<sys::ubyte>("buf4");
    sys::ubyte* pBuf3 = scratch.get<sys::ubyte>("buf3");
    sys::ubyte* pBuf5 = scratch.get<sys::ubyte>("buf5");
    sys::ubyte* pBuf6 = scratch.get<sys::ubyte>("buf6");
    sys::ubyte* pBuf7 = scratch.get<sys::ubyte>("buf7");
    TEST_ASSERT_EQ(pBuf1, pBuf4);
    TEST_ASSERT_EQ(pBuf4, pBuf6);
    TEST_ASSERT_EQ(pBuf3, pBuf5);
    TEST_ASSERT_EQ(pBuf5, pBuf7);
}

TEST_CASE(testReleaseInteriorBuffers)
{
    //Tests released with filled in buffers for analysis
    mem::ScratchMemory scratch;

    scratch.put<unsigned char>("a", 2, 1, 2);
    scratch.put<unsigned char>("b", 2, 1, 2);
    scratch.put<unsigned char>("c", 2, 1, 2);
    scratch.release("b");
    scratch.put<unsigned char>("d", 3, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 10);
    scratch.release("a");
    TEST_ASSERT_EQ(scratch.getNumBytes(), 10);
    scratch.put<unsigned char>("e", 1, 1, 2);
    TEST_ASSERT_EQ(scratch.getNumBytes(), 10);
    scratch.put<unsigned char>("f", 3, 1, 2);

    scratch.setup();

    mem::BufferView<unsigned char> bufViewA = scratch.getBufferView<sys::ubyte>("a");
    mem::BufferView<unsigned char> bufViewB = scratch.getBufferView<sys::ubyte>("b");
    mem::BufferView<unsigned char> bufViewC = scratch.getBufferView<sys::ubyte>("c");
    mem::BufferView<unsigned char> bufViewD = scratch.getBufferView<sys::ubyte>("d");
    mem::BufferView<unsigned char> bufViewE = scratch.getBufferView<sys::ubyte>("e");
    mem::BufferView<unsigned char> bufViewF = scratch.getBufferView<sys::ubyte>("f");

    for (size_t i = 0; i < bufViewA.size; ++i) 
    {
        bufViewA.data[i] = 'a';
    }
    for (size_t i = 0; i < bufViewB.size; ++i) 
    {
        bufViewB.data[i] = 'b';
    }
    for (size_t i = 0; i < bufViewC.size; ++i) 
    {
        bufViewC.data[i] = 'c';
    }
    for (size_t i = 0; i < bufViewB.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewB.data[i], 'b');
    }
    for (size_t i = 0; i < bufViewD.size; ++i) 
    {
        bufViewD.data[i] = 'd';
    }
    for (size_t i = 0; i < bufViewA.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewA.data[i], 'a');
    }
    for (size_t i = 0; i < bufViewC.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewC.data[i], 'c');
    }
    for (size_t i = 0; i < bufViewD.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewD.data[i], 'd');
    }
    for (size_t i = 0; i < bufViewE.size; ++i) 
    {
        bufViewE.data[i] = 'e';
    }
    for (size_t i = 0; i < bufViewF.size; ++i) 
    {
        bufViewF.data[i] = 'f';
    }
    for (size_t i = 0; i < bufViewC.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewC.data[i], 'c');
    }
    for (size_t i = 0; i < bufViewD.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewD.data[i], 'd');
    }
    for (size_t i = 0; i < bufViewE.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewE.data[i], 'e');
    }
    for (size_t i = 0; i < bufViewF.size; ++i) 
    {
        TEST_ASSERT_EQ(bufViewF.data[i], 'f');
    }
    TEST_ASSERT_EQ(scratch.getNumBytes(), 13);
}

struct Operation
{
    std::string op;
    size_t bytes;
    unsigned char name;
};

TEST_CASE(testReleaseConcurrentKeys)
{
    srand((unsigned)time(0));
    mem::ScratchMemory scratch;
    std::vector<Operation> operations;

    for (unsigned char ii = 'a'; ii <= 'x'; ++ii)
    {
        size_t numElements = (rand() % 5) + 5;
        size_t numBuffers = (rand() % 2) + 1;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        Operation putOp;
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);
        ++ii;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);
        ++ii;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);

        scratch.release(std::string(1, ii - 1));
        putOp.op = "release";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii - 1;
        operations.push_back(putOp);

        scratch.release(std::string(1, ii));
        putOp.op = "release";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);
        ++ii;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);

        scratch.release(std::string(1, ii - 3));
        putOp.op = "release";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii - 3;
        operations.push_back(putOp);

        scratch.setup();

        std::set<std::string> keysToCheck;
        for (size_t jj = 0; jj < operations.size(); ++jj)
        {
            Operation currentOp = operations.at(jj);
            std::string key = std::string(1, currentOp.name);

            if (currentOp.op == "put")
            {
                mem::BufferView<unsigned char> bufView = scratch.getBufferView<sys::ubyte>(key);
                for (size_t i = 0; i < bufView.size; ++i)
                {
                    bufView.data[i] = currentOp.name;
                }
                keysToCheck.insert(key);
            }
            if (currentOp.op == "release")
            {
                keysToCheck.erase(key);
                for (std::set<std::string>::iterator it = keysToCheck.begin(); it != keysToCheck.end(); ++it)
                {
                    mem::BufferView<unsigned char> bufView = scratch.getBufferView<sys::ubyte>(*it);
                    for (size_t i = 0; i < bufView.size; ++i)
                    {
                        TEST_ASSERT_EQ(bufView.data[i], (*it)[0]);
                    }
                }
            }
        }
    }
}

TEST_CASE(testReleaseConnectedKeys)
{
    srand((unsigned)time(0));
    mem::ScratchMemory scratch;
    std::vector<Operation> operations;

    for (unsigned char ii = 'a'; ii <= 'x'; ++ii)
    {
        size_t numElements = (rand() % 5) + 5;
        size_t numBuffers = (rand() % 2) + 1;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        Operation putOp;
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);
        ++ii;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);

        scratch.release(std::string(1, ii));
        putOp.op = "release";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);
        ++ii;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);

        scratch.release(std::string(1, ii - 2));
        putOp.op = "release";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii - 2;
        operations.push_back(putOp);
        ++ii;

        scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);
        putOp.op = "put";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii;
        operations.push_back(putOp);

        scratch.release(std::string(1, ii - 1));
        putOp.op = "release";
        putOp.bytes = numElements * numBuffers;
        putOp.name = ii - 1;
        operations.push_back(putOp);
    }
    scratch.setup();

    std::set<std::string> keysToCheck;

    for (size_t jj = 0; jj < operations.size(); ++jj)
    {
        Operation currentOp = operations.at(jj);
        std::string key = std::string(1, currentOp.name);

        if (currentOp.op == "put")
        {
            mem::BufferView<unsigned char> bufView = scratch.getBufferView<sys::ubyte>(key);
            for (size_t i = 0; i < bufView.size; ++i)
            {
                bufView.data[i] = currentOp.name;
            }
            keysToCheck.insert(key);
        }
        if (currentOp.op == "release")
        {
            keysToCheck.erase(key);
            for (std::set<std::string>::iterator it = keysToCheck.begin(); it != keysToCheck.end(); ++it)
            {
                mem::BufferView<unsigned char> bufView = scratch.getBufferView<sys::ubyte>(*it);
                for (size_t i = 0; i < bufView.size; ++i)
                {
                    TEST_ASSERT_EQ(bufView.data[i], (*it)[0]);
                }
            }
        }
    }
}

TEST_CASE(testGenerateBuffersForRelease)
{
    srand((unsigned)time(0));

    for (unsigned int run = 0; run < 50; ++run) {
        mem::ScratchMemory scratch;
        std::vector<Operation> operations;
        std::vector<unsigned char> notReleased;

        for (unsigned char ii = 'a'; ii <= 'z'; ++ii)
        {
            size_t numElements = (rand() % 20) + 5;
            size_t numBuffers = (rand() % 5) + 1;

            scratch.put<sys::ubyte>(std::string(1, ii), numElements, numBuffers);

            Operation putOp;
            putOp.op = "put";
            putOp.bytes = numElements * numBuffers;
            putOp.name = ii;
            operations.push_back(putOp);
            notReleased.push_back(ii);

            unsigned int releaseIfFour = (rand() % 4) + 1;
            if ((releaseIfFour == 4) && (operations.size() > 3))
            {
                unsigned int keyToReleaseIndex = (rand() % notReleased.size());
                scratch.release(std::string(1, notReleased.at(keyToReleaseIndex)));

                Operation releaseOp;
                releaseOp.op = "release";
                releaseOp.bytes = 0;
                releaseOp.name = notReleased.at(keyToReleaseIndex);
                operations.push_back(releaseOp);

                notReleased.erase(notReleased.begin() + keyToReleaseIndex);
            }
        }

        scratch.setup();

        std::set<std::string> keysToCheck;

        for (size_t jj = 0; jj < operations.size(); ++jj)
        {
            Operation currentOp = operations.at(jj);
            std::string key = std::string(1, currentOp.name);

            if (currentOp.op == "put")
            {
                mem::BufferView<unsigned char> bufView = scratch.getBufferView<sys::ubyte>(key);
                for (size_t i = 0; i < bufView.size; ++i)
                {
                    bufView.data[i] = currentOp.name;
                }
                keysToCheck.insert(key);
            }
            if (currentOp.op == "release")
            {
                keysToCheck.erase(key);
                for (std::set<std::string>::iterator it = keysToCheck.begin(); it != keysToCheck.end(); ++it)
                {
                    mem::BufferView<unsigned char> bufView = scratch.getBufferView<sys::ubyte>(*it);
                    for (size_t i = 0; i < bufView.size; ++i)
                    {
                        TEST_ASSERT_EQ(bufView.data[i], (*it)[0]);
                    }
                }
            }
        }
    }
}

TEST_CASE(testScratchMemory)
{
    mem::ScratchMemory scratch;

    TEST_ASSERT_EQ(scratch.getNumBytes(), 0);

    scratch.put<sys::ubyte>("buf0", 11, 1, 13);
    scratch.put<int>("buf1", 17, 1, 23);
    scratch.put<char>("buf2", 29, 3, 31);
    scratch.put<double>("buf3", 8);

    size_t numBytes0 = 11 + 13 - 1;
    size_t numBytes1 = 17 * sizeof(int) + 23 - 1;
    size_t numBytes2 = 3 * (29 + 31 - 1);
    size_t numBytes3 = 8 * sizeof(double) + sys::SSE_INSTRUCTION_ALIGNMENT - 1;
    TEST_ASSERT_EQ(scratch.getNumBytes(),
                   numBytes0 + numBytes1 + numBytes2 + numBytes3);

    // trying to get scratch before setting up should throw
    TEST_EXCEPTION(scratch.get<sys::ubyte>("buf0"));

    // set up external buffer
    std::vector<sys::ubyte> storage(scratch.getNumBytes());
    mem::BufferView<sys::ubyte> buffer(storage.data(), storage.size());

    // first pass with external buffer, second with internal allocation
    for (size_t ii = 0; ii < 2; ++ii)
    {
        if (ii == 0)
        {
            scratch.setup(buffer);
        }
        else
        {
            scratch.setup();
        }

        // trying to get nonexistent key should throw
        TEST_EXCEPTION(scratch.get<char>("buf999"));

        sys::ubyte* pBuf0 = scratch.get<sys::ubyte>("buf0");
        sys::ubyte* pBuf1 = scratch.get<sys::ubyte>("buf1");
        sys::ubyte* pBuf2_0 = scratch.get<sys::ubyte>("buf2", 0);
        sys::ubyte* pBuf2_1 = scratch.get<sys::ubyte>("buf2", 1);
        sys::ubyte* pBuf2_2 = scratch.get<sys::ubyte>("buf2", 2);
        sys::ubyte* pBuf3 = scratch.get<sys::ubyte>("buf3");

        // verify getBufferView matches get
        mem::BufferView<sys::ubyte> bufView0 =
                scratch.getBufferView<sys::ubyte>("buf0");
        mem::BufferView<sys::ubyte> bufView1 =
                scratch.getBufferView<sys::ubyte>("buf1");
        mem::BufferView<sys::ubyte> bufView2_0 =
                scratch.getBufferView<sys::ubyte>("buf2", 0);
        mem::BufferView<sys::ubyte> bufView2_1 =
                scratch.getBufferView<sys::ubyte>("buf2", 1);
        TEST_ASSERT_EQ(pBuf0, bufView0.data);
        TEST_ASSERT_EQ(pBuf1, bufView1.data);
        TEST_ASSERT_EQ(pBuf2_0, bufView2_0.data);
        TEST_ASSERT_EQ(pBuf2_1, bufView2_1.data);

        // verify getBufferView size in bytes
        TEST_ASSERT_EQ(bufView0.size, 11);
        TEST_ASSERT_EQ(bufView1.size, 17 * sizeof(int));
        TEST_ASSERT_EQ(bufView2_0.size, 29);
        TEST_ASSERT_EQ(bufView2_1.size, 29);

        // verify get works with const reference to ScratchMemory
        const mem::ScratchMemory& constScratch = scratch;
        const sys::ubyte* pConstBuf0 = constScratch.get<sys::ubyte>("buf0");
        TEST_ASSERT_EQ(pBuf0, pConstBuf0);

        // verify getBufferView works with const reference to ScratchMemory
        mem::BufferView<const sys::ubyte> constBufView0 =
                constScratch.getBufferView<sys::ubyte>("buf0");
        TEST_ASSERT_EQ(bufView0.data, constBufView0.data);

        // trying to get buffer index out of range should throw
        TEST_EXCEPTION(scratch.get<sys::ubyte>("buf0", 1));
        TEST_EXCEPTION(scratch.get<sys::ubyte>("buf0", -1));
        TEST_EXCEPTION(scratch.get<sys::ubyte>("buf2", 3));

        // verify alignment
        TEST_ASSERT_EQ(reinterpret_cast<size_t>(pBuf0) % 13, 0);
        TEST_ASSERT_EQ(reinterpret_cast<size_t>(pBuf1) % 23, 0);
        TEST_ASSERT_EQ(reinterpret_cast<size_t>(pBuf2_0) % 31, 0);
        TEST_ASSERT_EQ(reinterpret_cast<size_t>(pBuf2_1) % 31, 0);
        TEST_ASSERT_EQ(reinterpret_cast<size_t>(pBuf2_2) % 31, 0);
        TEST_ASSERT_EQ(reinterpret_cast<size_t>(pBuf3) % sys::SSE_INSTRUCTION_ALIGNMENT, 0);

        // verify no overlap between buffers
        TEST_ASSERT_TRUE(pBuf1 - pBuf0 >= static_cast<ptrdiff_t>(11));
        TEST_ASSERT_TRUE(pBuf2_0 - pBuf1 >=
                static_cast<ptrdiff_t>(17 * sizeof(int)));
        TEST_ASSERT_TRUE(pBuf2_1 - pBuf2_0 >= static_cast<ptrdiff_t>(29));
        TEST_ASSERT_TRUE(pBuf2_2 - pBuf2_1 >= static_cast<ptrdiff_t>(29));
        TEST_ASSERT_TRUE(pBuf3 - pBuf2_2 >= static_cast<ptrdiff_t>(29));
    }

    // put should invalidate the scratch memory until setup is called again
    scratch.put<sys::ubyte>("buf4", 8);
    TEST_EXCEPTION(scratch.get<sys::ubyte>("buf0"));
    scratch.setup();
    scratch.get<sys::ubyte>("buf0");

    // calling setup with buffer that is too small should throw
    mem::BufferView<sys::ubyte> smallBuffer(buffer.data, buffer.size - 1);
    TEST_EXCEPTION(scratch.setup(smallBuffer));

    // calling setup with invalid external buffer should throw
    mem::BufferView<sys::ubyte> invalidBuffer(NULL, buffer.size);
    TEST_EXCEPTION(scratch.setup(invalidBuffer));
}
}

int main(int, char**)
{
    TEST_CHECK(testScratchMemory);
    TEST_CHECK(testReleaseSingleEndBuffer);
    TEST_CHECK(testReleaseMultipleEndBuffers);
    TEST_CHECK(testReleaseNonEndBuffers);
    TEST_CHECK(testReleaseInteriorBuffers);
    TEST_CHECK(testReleaseConcurrentKeys);
    TEST_CHECK(testReleaseConnectedKeys);
    TEST_CHECK(testGenerateBuffersForRelease);

    return 0;
}
