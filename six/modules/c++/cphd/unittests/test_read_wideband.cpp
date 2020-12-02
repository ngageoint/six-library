/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/Metadata.h>
#include <cphd/Wideband.h>
#include <io/ByteStream.h>
#include "TestCase.h"

namespace
{
TEST_CASE(testReadCompressedChannel)
{
    auto input = std::make_shared<io::ByteStream>();
    input->write("1234");
    input->seek(0, io::Seekable::START);

    cphd::Metadata metadata;
    metadata.data.channels.resize(1);
    metadata.data.channels[0].numSamples = 4;
    metadata.data.channels[0].numVectors = 8;
    metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CI2;
    metadata.data.signalCompressionID = "Test Compression";
    metadata.data.channels[0].compressedSignalSize = 4;

    cphd::Wideband wideband(input, metadata, 0, 4);

    std::unique_ptr<sys::ubyte[]> readData;
    wideband.read(0, readData);

    TEST_ASSERT_EQ(wideband.getBytesRequiredForRead(0), 4);
    TEST_ASSERT_EQ(readData[0], '1');
    TEST_ASSERT_EQ(readData[1], '2');
    TEST_ASSERT_EQ(readData[2], '3');
    TEST_ASSERT_EQ(readData[3], '4');
}

TEST_CASE(testReadUncompressedChannel)
{
    auto input = std::make_shared<io::ByteStream>();
    input->write("12345678");
    input->seek(0, io::Seekable::START);

    cphd::Metadata metadata;
    metadata.data.channels.resize(1);
    metadata.data.channels[0].numSamples = 1;
    metadata.data.channels[0].numVectors = 4;
    metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CI2;
    metadata.data.channels[0].compressedSignalSize =
            six::Init::undefined<size_t>();

    cphd::Wideband wideband(input, metadata, 0, 4);

    std::unique_ptr<sys::ubyte[]> readData;
    wideband.read(0, readData);

    TEST_ASSERT_EQ(wideband.getBytesRequiredForRead(0), 8);
    TEST_ASSERT_EQ(readData[0], '1');
    TEST_ASSERT_EQ(readData[1], '2');
    TEST_ASSERT_EQ(readData[2], '3');
    TEST_ASSERT_EQ(readData[3], '4');
}

TEST_CASE(testReadChannelSubset)
{
    cphd::Metadata metadata;
    metadata.data.channels.resize(1);
    metadata.data.channels[0].numSamples = 2;
    metadata.data.channels[0].numVectors = 4;
    metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CI2;

    auto input = std::make_shared<io::ByteStream>();
    input->write("0A1B");
    input->write("2C3D");
    input->write("4E5F");
    input->write("6G7H");
    input->seek(0, io::Seekable::START);

    cphd::Wideband wideband(input, metadata, 0, 32);
    std::unique_ptr<sys::ubyte[]> readData;

    // Single pixel reads
    wideband.read(0, 0, 0, 0, 0, 1, readData);
    TEST_ASSERT_EQ(readData[0], '0');
    TEST_ASSERT_EQ(readData[1], 'A');

    wideband.read(0, 1, 1, 0, 0, 1, readData);
    TEST_ASSERT_EQ(readData[0], '2');
    TEST_ASSERT_EQ(readData[1], 'C');

    wideband.read(0, 3, 3, 1, 1, 1, readData);
    TEST_ASSERT_EQ(wideband.getBytesRequiredForRead(0, 3, 3, 1, 1), 2);

    TEST_ASSERT_EQ(readData[0], '7');
    TEST_ASSERT_EQ(readData[1], 'H');

    // Full vector reads
    wideband.read(0, 0, 0, 0, cphd::Wideband::ALL, 1, readData);
    TEST_ASSERT_EQ(readData[0], '0');
    TEST_ASSERT_EQ(readData[1], 'A');
    TEST_ASSERT_EQ(readData[2], '1');
    TEST_ASSERT_EQ(readData[3], 'B');

    wideband.read(0, 3, 3, 0, cphd::Wideband::ALL, 1, readData);
    TEST_ASSERT_EQ(
            wideband.getBytesRequiredForRead(0, 3, 3, 0, cphd::Wideband::ALL),
            4);

    TEST_ASSERT_EQ(readData[0], '6');
    TEST_ASSERT_EQ(readData[1], 'G');
    TEST_ASSERT_EQ(readData[2], '7');
    TEST_ASSERT_EQ(readData[3], 'H');

    // Full sample reads
    TEST_ASSERT_EQ(
            wideband.getBytesRequiredForRead(0, 0, cphd::Wideband::ALL, 0, 0),
            8);
    wideband.read(0, 0, cphd::Wideband::ALL, 0, 0, 1, readData);
    TEST_ASSERT_EQ(readData[0], '0');
    TEST_ASSERT_EQ(readData[1], 'A');
    TEST_ASSERT_EQ(readData[2], '2');
    TEST_ASSERT_EQ(readData[3], 'C');
    TEST_ASSERT_EQ(readData[4], '4');
    TEST_ASSERT_EQ(readData[5], 'E');
    TEST_ASSERT_EQ(readData[6], '6');
    TEST_ASSERT_EQ(readData[7], 'G');
}

TEST_CASE(testCannotDoPartialReadOfCompressedChannel)
{
    auto input = std::make_shared<io::ByteStream>();
    input->write("1234");
    input->seek(0, io::Seekable::START);

    cphd::Metadata metadata;
    metadata.data.channels.resize(1);
    metadata.data.channels[0].numSamples = 4;
    metadata.data.channels[0].numVectors = 8;
    metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CI2;
    metadata.data.signalCompressionID = "Test Compression";
    metadata.data.channels[0].compressedSignalSize = 4;

    cphd::Wideband wideband(input, metadata, 0, 4);

    std::unique_ptr<sys::ubyte[]> readData;
    TEST_EXCEPTION(wideband.read(0, 0, 0, 1, 1, 1, readData));
    TEST_EXCEPTION(wideband.getBytesRequiredForRead(0, 0, 0, 1, 1));
}
}

TEST_MAIN(
    TEST_CHECK(testReadCompressedChannel);
    TEST_CHECK(testReadUncompressedChannel);
    TEST_CHECK(testReadChannelSubset);
    TEST_CHECK(testCannotDoPartialReadOfCompressedChannel);
    )
