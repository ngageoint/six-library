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
#include <cphd/Wideband.h>

#include <cphd/Metadata.h>
#include <io/ByteStream.h>
#include "TestCase.h"

#ifdef GTEST_API_
namespace testing
{
    namespace internal
    {
        template<>
        AssertionResult CmpHelperEQ(const char* lhs_expression,
            const char* rhs_expression,
            const std::byte& lhs,
            const char& rhs) {
            if (static_cast<char>(lhs) == rhs) {
                return AssertionSuccess();
            }

            return CmpHelperEQFailure(lhs_expression, rhs_expression, lhs, rhs);
        }
    }
}
#endif // GTEST_API_

namespace
{
    inline bool operator==(std::byte lhs, char rhs)
    {
        return static_cast<char>(lhs) == rhs;
    }
    inline bool operator!=(std::byte lhs, char rhs)
    {
        return !(lhs == rhs);
    }
}
namespace str
{
    inline std::ostream& operator<<(std::ostream& os, std::byte v)
    {
        os << static_cast<unsigned char>(v);
        return os;
    }
}

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

    std::unique_ptr<std::byte[]> readData;
    wideband.read(0, readData);

    TEST_ASSERT_EQ(wideband.getBytesRequiredForRead(0), static_cast<size_t>(4));
    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('1'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('2'));
    TEST_ASSERT_EQ(readData[2], static_cast<std::byte>('3'));
    TEST_ASSERT_EQ(readData[3], static_cast<std::byte>('4'));
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

    std::unique_ptr<std::byte[]> readData;
    wideband.read(0, readData);

    TEST_ASSERT_EQ(wideband.getBytesRequiredForRead(0), static_cast<size_t>(8));
    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('1'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('2'));
    TEST_ASSERT_EQ(readData[2], static_cast<std::byte>('3'));
    TEST_ASSERT_EQ(readData[3], static_cast<std::byte>('4'));
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

    // Single pixel reads
    auto readData = wideband.read(0, 0, 0, 0, 0, 1);
    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('0'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('A'));

    readData = wideband.read(0, 1, 1, 0, 0, 1);
    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('2'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('C'));

    readData = wideband.read(0, 3, 3, 1, 1, 1);
    TEST_ASSERT_EQ(wideband.getBytesRequiredForRead(0, 3, 3, 1, 1), static_cast<size_t>(2));

    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('7'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('H'));

    // Full vector reads
    readData = wideband.read(0, 0, 0, 0, cphd::Wideband::ALL, 1);
    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('0'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('A'));
    TEST_ASSERT_EQ(readData[2], static_cast<std::byte>('1'));
    TEST_ASSERT_EQ(readData[3], static_cast<std::byte>('B'));

    readData = wideband.read(0, 3, 3, 0, cphd::Wideband::ALL, 1);
    TEST_ASSERT_EQ(
            wideband.getBytesRequiredForRead(0, 3, 3, 0, cphd::Wideband::ALL),
        static_cast<size_t>(4));

    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('6'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('G'));
    TEST_ASSERT_EQ(readData[2], static_cast<std::byte>('7'));
    TEST_ASSERT_EQ(readData[3], static_cast<std::byte>('H'));

    // Full sample reads
    TEST_ASSERT_EQ(
            wideband.getBytesRequiredForRead(0, 0, cphd::Wideband::ALL, 0, 0),
        static_cast<size_t>(8));
    readData = wideband.read(0, 0, cphd::Wideband::ALL, 0, 0, 1);
    TEST_ASSERT_EQ(readData[0], static_cast<std::byte>('0'));
    TEST_ASSERT_EQ(readData[1], static_cast<std::byte>('A'));
    TEST_ASSERT_EQ(readData[2], static_cast<std::byte>('2'));
    TEST_ASSERT_EQ(readData[3], static_cast<std::byte>('C'));
    TEST_ASSERT_EQ(readData[4], static_cast<std::byte>('4'));
    TEST_ASSERT_EQ(readData[5], static_cast<std::byte>('E'));
    TEST_ASSERT_EQ(readData[6], static_cast<std::byte>('6'));
    TEST_ASSERT_EQ(readData[7], static_cast<std::byte>('G'));
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

    TEST_EXCEPTION(wideband.read(0, 0, 0, 1, 1, 1));
    TEST_EXCEPTION(wideband.getBytesRequiredForRead(0, 0, 0, 1, 1));
}
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(testReadCompressedChannel);
    TEST_CHECK(testReadUncompressedChannel);
    TEST_CHECK(testReadChannelSubset);
    TEST_CHECK(testCannotDoPartialReadOfCompressedChannel);
    )
