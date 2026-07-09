/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2025, Arka Group, L.P.
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <TestCase.h>
#include <io/FileInputStreamOS.h>

#include <fstream>
#include <iostream>
#include <string>

std::string thisExecutable;

TEST_CASE(testParallelReads)
{
    // using self is easier than carting around extra files in the repo
    std::cout << "thisExecutable: " << thisExecutable << std::endl;

    // standard library read as a reference
    std::ifstream from(thisExecutable, std::ios::binary);
    from.seekg(0, std::ios::end);
    size_t trueLen = from.tellg();
    from.seekg(0);

    std::vector<char> referenceRead(trueLen);
    from.read(&referenceRead[0], trueLen);
    from.close();

    // test FileInputStreamOS different ways
    io::FileInputStreamOS fis(thisExecutable);

    // ensure threading is disabled by default
    TEST_ASSERT_EQ(fis.getMaxReadThreads(), 1);

    // single thread read
    size_t avail = fis.available();
    TEST_ASSERT_EQ(avail, trueLen);

    std::vector<char> singleThreadRead(avail);
    size_t wasRead = fis.read(&singleThreadRead[0], avail);
    TEST_ASSERT_EQ(wasRead, avail);
    TEST_ASSERT_TRUE(referenceRead == singleThreadRead);

    fis.seek(0, io::Seekable::START);

    // setup multithread read
    fis.setMaxReadThreads(16);
    fis.setMinimumChunkCount(4);
    fis.setParallelChunkSize(1024);

    // check set/get methods
    TEST_ASSERT_EQ(fis.getMaxReadThreads(), 16);
    TEST_ASSERT_EQ(fis.getMinimumChunkCount(), 4);
    TEST_ASSERT_EQ(fis.getParallelChunkSize(), 1024);

    std::vector<char> multiThreadRead(avail);
    wasRead = fis.read(&multiThreadRead[0], avail);
    TEST_ASSERT_EQ(wasRead, avail);
    TEST_ASSERT_TRUE(referenceRead == multiThreadRead);
}

int main(int, char* argv[])
{
    thisExecutable = std::string(argv[0]);
    TEST_CHECK(testParallelReads);
    return 0;
}
