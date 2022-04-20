/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <std/span>
#include <std/cstddef>

#include <import/io.h>
#include <mem/BufferView.h>
#include <sys/Conf.h>
#include <TestCase.h>
#include <string.h>

TEST_CASE(testStringStream)
{
    io::StringStream stream;
    stream.writeln("test");
    stream.writeln("test");
    TEST_ASSERT_EQ(stream.available(), 10);
    stream.seek(1, io::Seekable::START);
    TEST_ASSERT_EQ(stream.available(), 9);

    stream.write("0123456789");

    TEST_ASSERT_EQ(stream.available(), 19);
    TEST_ASSERT_EQ(stream.tell(), 1);

    stream.seek(22, io::Seekable::CURRENT);
    stream.seek(0, io::Seekable::START);

    stream.reset();
    TEST_ASSERT_EQ(stream.available(), 0);
    stream.write("test");
    TEST_ASSERT_EQ(stream.available(), 4);
    sys::byte buf[255];
    stream.read(buf, 4);
    buf[4] = 0;
    TEST_ASSERT_EQ(std::string(buf), "test");
}

TEST_CASE(testByteStream)
{
    io::ByteStream stream;
    stream.writeln("test");
    stream.writeln("test");

    TEST_ASSERT_EQ(stream.available(), 0);
    stream.seek(0, io::Seekable::START);
    TEST_ASSERT_EQ(stream.available(), 10);

    stream.seek(10, io::Seekable::START);
    stream.write("0123456789");
    TEST_ASSERT_EQ(stream.tell(), 20);
    TEST_ASSERT_EQ(stream.available(), 0);

    stream.seek(22, io::Seekable::CURRENT);
    TEST_ASSERT_EQ(stream.tell(), -1);
    stream.reset();
    TEST_ASSERT_EQ(stream.tell(), 0);
    stream.seek(0, io::Seekable::START);
    TEST_ASSERT_EQ(stream.tell(), 0);

    stream.seek(2, io::Seekable::END);
    TEST_ASSERT_EQ(stream.tell(), 18);
    TEST_ASSERT_EQ(stream.getSize(), static_cast<size_t>(20));

    stream.write("abcdef");
    TEST_ASSERT_EQ(stream.getSize(), static_cast<size_t>(24));

     const std::string test("test");
    {
        stream.clear();
        TEST_ASSERT_EQ(stream.available(), 0);
        stream.write(test);
        stream.seek(0, io::Seekable::START);
        TEST_ASSERT_EQ(stream.available(), 4);
        sys::byte buf[255];
        stream.read(buf, 4);
        buf[4] = 0;
        TEST_ASSERT_EQ(std::string(buf), test);
    }
    {
        stream.clear();
        const std::span<const std::string::value_type> test_span(test.data(), test.size());
        stream.write(test_span);
        stream.seek(0, io::Seekable::START);
        TEST_ASSERT_EQ(stream.available(), 4);
        std::byte buf[255];
        stream.read(std::span<std::byte>(buf, 4));
        buf[4] = std::byte(0);
        const void* pBuf = buf;
        auto pStrBuf = static_cast<std::string::const_pointer>(pBuf);
        TEST_ASSERT_EQ(pStrBuf, test);
    }
}

TEST_CASE(testProxyOutputStream)
{
    io::StringStream stream;
    io::ProxyOutputStream proxy(&stream);
    proxy.write("test1");
    sys::byte buf[255];
    stream.read(buf, 5);
    buf[5] = 0;
    TEST_ASSERT_EQ(std::string(buf), "test1");
}

TEST_CASE(testCountingOutputStream)
{
    io::ByteStream stream;
    io::CountingOutputStream counter(&stream);
    counter.write("test1");
    TEST_ASSERT_EQ(counter.getCount(), 5);
}

TEST_CASE(testBufferViewStream)
{
    {
        mem::BufferView<sys::ubyte> bufferView(nullptr, 0);
        io::BufferViewStream<sys::ubyte> stream(bufferView);
        TEST_ASSERT_EQ(stream.tell(), 0);
        TEST_ASSERT_EQ(stream.available(), 0);
        TEST_ASSERT_NULL(stream.get());
    }
    {
        std::vector<sys::ubyte> data(4);
        data[0] = 2;
        data[1] = 4;
        data[2] = 5;
        data[3] = 9;
        mem::BufferView<sys::ubyte> bufferView(&data[0], data.size());
        io::BufferViewStream<sys::ubyte> stream(bufferView);
        TEST_ASSERT_EQ(stream.tell(), 0);
        TEST_ASSERT_EQ(stream.available(), 4);

        std::vector<sys::ubyte> output(3);
        TEST_ASSERT_EQ(stream.read(&output[0], 2), 2);
        TEST_ASSERT_EQ(stream.tell(), 2);
        TEST_ASSERT_EQ(stream.available(), 2);
        stream.seek(1, io::Seekable::CURRENT);
        TEST_ASSERT_EQ(stream.read(&output[2], 1), 1);
        TEST_ASSERT_EQ(output[0], 2);
        TEST_ASSERT_EQ(output[1], 4);
        TEST_ASSERT_EQ(output[2], 9);

        stream.seek(1, io::Seekable::START);
        stream.write(&output[0], output.size());

        TEST_ASSERT_EQ(data[0], 2);
        TEST_ASSERT_EQ(data[1], 2);
        TEST_ASSERT_EQ(data[2], 4);
        TEST_ASSERT_EQ(data[3], 9);

        TEST_ASSERT_EQ(stream.available(), 0);
        TEST_EXCEPTION(stream.write(&data[0], data.size()));

        TEST_EXCEPTION(stream.seek(-1, io::Seekable::START));
        TEST_EXCEPTION(stream.seek(-1, io::Seekable::END));
    }
}

TEST_CASE(testBufferViewIntStream)
{
    // Test for datatype with size > 1 to make sure copies are done correctly
    std::vector<int> data(4);
    data[0] = 2;
    data[1] = 4;
    data[2] = 5;
    data[3] = 9;
    mem::BufferView<int> bufferView(&data[0], data.size());
    io::BufferViewStream<int> stream(bufferView);
    std::vector<int> output(3);

    TEST_ASSERT_EQ(stream.read(&output[0], 2), 2);
    TEST_ASSERT_EQ(stream.tell(), static_cast<sys::Off_T>(2 * sizeof(int)));
    TEST_ASSERT_EQ(stream.available(), static_cast<sys::Off_T>(2 * sizeof(int)));
    stream.seek(1 * sizeof(int), io::Seekable::CURRENT);
    TEST_ASSERT_EQ(stream.read(&output[2], 1), 1);
    TEST_ASSERT_EQ(output[0], 2);
    TEST_ASSERT_EQ(output[1], 4);
    TEST_ASSERT_EQ(output[2], 9);

    stream.seek(1 * sizeof(int), io::Seekable::START);
    stream.write(&output[0], output.size());

    TEST_ASSERT_EQ(data[0], 2);
    TEST_ASSERT_EQ(data[1], 2);
    TEST_ASSERT_EQ(data[2], 4);
    TEST_ASSERT_EQ(data[3], 9);

    // Truncate properly if we ask for more elements than there are
    ::memset(&output[0], 0, output.size() * sizeof(output[0]));
    stream.seek(3 * sizeof(int), io::Seekable::START);
    TEST_ASSERT_EQ(stream.read(&output[0], 2), 1);
    TEST_ASSERT_EQ(stream.tell(), static_cast<sys::Off_T>(4 * sizeof(int)));
    TEST_ASSERT_EQ(output[0], 9);
    TEST_ASSERT_EQ(output[1], 0);
}

void cleanupFiles(std::string base)
{
    // cleanup
    sys::OS os;
    for (size_t i = 0;; ++i)
    {
        std::ostringstream oss;
        oss << base << "." << (i + 1);
        std::string fname(oss.str());
        if (os.isFile(fname))
            os.remove(fname);
        else
            break;
    }
    if (os.isFile(base))
        os.remove(base);
}

TEST_CASE(testRotate)
{
    std::string outFile = "test_rotate.txt";
    size_t maxFiles = 5;

    cleanupFiles( outFile);

    sys::OS os;

    {
        io::RotatingFileOutputStream out(outFile, 10, maxFiles);
        out.write("0123456789");
        TEST_ASSERT(os.exists(outFile));
        TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));

        out.write("1");
        TEST_ASSERT(os.isFile(outFile + ".1"));
        TEST_ASSERT_EQ(out.getCount(), 1);

        for(size_t i = 0; i < maxFiles - 1; ++i)
        {
            std::string fname = outFile + "." + str::toString(i + 1);
            std::string next = outFile + "." + str::toString(i + 2);

            TEST_ASSERT(os.isFile(fname));
            TEST_ASSERT_FALSE(os.isFile(next));

            out.write("0123456789");
            TEST_ASSERT(os.isFile(next));
        }
    }

    cleanupFiles( outFile);
}

TEST_CASE(testNeverRotate)
{
    std::string outFile = "test_rotate.txt";
    cleanupFiles( outFile);

    sys::OS os;
    {
        io::RotatingFileOutputStream out(outFile);
        for(size_t i = 0; i < 1024; ++i)
        out.write("0");
        TEST_ASSERT(os.exists(outFile));
        TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));
        TEST_ASSERT_EQ(out.getCount(), 1024);
    }
    cleanupFiles( outFile);
}

TEST_CASE(testRotateReset)
{
    std::string outFile = "test_rotate.txt";
    cleanupFiles( outFile);

    sys::OS os;
    io::RotatingFileOutputStream out(outFile, 10);
    out.write("01234567890");
    TEST_ASSERT(os.exists(outFile));
    TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));
    TEST_ASSERT_EQ(out.getCount(), 11);

    out.write("0");
    TEST_ASSERT(os.exists(outFile));
    TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));
    TEST_ASSERT_EQ(out.getCount(), 1);

    out.close();
    try
    {
        out.write("0");
        TEST_FAIL("Stream is closed; should throw.");
    }
    catch(except::Exception&)
    {
    }

    cleanupFiles( outFile);
}

int main(int, char**)
{
    TEST_CHECK(testStringStream);
    TEST_CHECK(testByteStream);
    TEST_CHECK(testProxyOutputStream);
    TEST_CHECK(testCountingOutputStream);
    TEST_CHECK(testBufferViewStream);
    TEST_CHECK(testBufferViewIntStream);
    TEST_CHECK(testRotate);
    TEST_CHECK(testNeverRotate);
    TEST_CHECK(testRotateReset);
}
