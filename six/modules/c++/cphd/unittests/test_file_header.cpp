/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#include <cphd/FileHeader.h>
#include <io/ByteStream.h>
#include "TestCase.h"

namespace
{
static const char FILE_TYPE_HEADER[] = "CPHD/1.0\n";
static const char FILE_HEADER_CONTENT[] = "CPHD/1.0\n"
        "XML_BLOCK_SIZE := 3\n"
        "XML_BLOCK_BYTE_OFFSET := 10\n"
        "SUPPORT_BLOCK_SIZE := 4\n"
        "SUPPORT_BLOCK_BYTE_OFFSET := 15\n"
        "PVP_BLOCK_SIZE := 5\n"
        "PVP_BLOCK_BYTE_OFFSET := 19\n"
        "SIGNAL_BLOCK_SIZE := 6\n"
        "SIGNAL_BLOCK_BYTE_OFFSET := 24\n"
        "CLASSIFICATION := UNCLASSIFIED\n"
        "RELEASE_INFO := UNRESTRICTED\n"
        "\f\n";

TEST_CASE(testReadVersion)
{
    io::ByteStream fileTypeHeader;
    fileTypeHeader.write(FILE_TYPE_HEADER, strlen(FILE_TYPE_HEADER));
    TEST_ASSERT_EQ(cphd::FileHeader::readVersion(fileTypeHeader), "1.0");
}

TEST_CASE(testCanReadHeaderWithoutBreaking)
{
    io::ByteStream fileHeaderContentWithSupport;
    fileHeaderContentWithSupport.write(FILE_HEADER_CONTENT, strlen(FILE_HEADER_CONTENT));
    cphd::FileHeader headerWithSupport;
    headerWithSupport.read(fileHeaderContentWithSupport);
    TEST_ASSERT_EQ(headerWithSupport.getXMLBlockSize(), 3);
    TEST_ASSERT_EQ(headerWithSupport.getXMLBlockByteOffset(), 10);
    TEST_ASSERT_EQ(headerWithSupport.getSupportBlockSize(), 4);
    TEST_ASSERT_EQ(headerWithSupport.getSupportBlockByteOffset(), 15);
    TEST_ASSERT_EQ(headerWithSupport.getPvpBlockSize(), 5);
    TEST_ASSERT_EQ(headerWithSupport.getPvpBlockByteOffset(), 19);
    TEST_ASSERT_EQ(headerWithSupport.getSignalBlockSize(), 6);
    TEST_ASSERT_EQ(headerWithSupport.getSignalBlockByteOffset(), 24);
    TEST_ASSERT_EQ(headerWithSupport.getClassification(), "UNCLASSIFIED");
    TEST_ASSERT_EQ(headerWithSupport.getReleaseInfo(), "UNRESTRICTED");

    io::ByteStream fileHeaderContentWithoutSupport;
    std::string fileHeaderTxtNoSupport = "CPHD/1.0\n"
            "XML_BLOCK_SIZE := 3\n"
            "XML_BLOCK_BYTE_OFFSET := 10\n"
            "PVP_BLOCK_SIZE := 5\n"
            "PVP_BLOCK_BYTE_OFFSET := 15\n"
            "SIGNAL_BLOCK_SIZE := 6\n"
            "SIGNAL_BLOCK_BYTE_OFFSET := 20\n"
            "CLASSIFICATION := UNCLASSIFIED\n"
            "RELEASE_INFO := UNRESTRICTED\n"
            "\f\n";
    fileHeaderContentWithoutSupport.write(fileHeaderTxtNoSupport.c_str(),
                                            fileHeaderTxtNoSupport.size());
    cphd::FileHeader headerWithoutSupport;
    headerWithoutSupport.read(fileHeaderContentWithoutSupport);
    TEST_ASSERT_EQ(headerWithoutSupport.getXMLBlockSize(), 3);
    TEST_ASSERT_EQ(headerWithoutSupport.getXMLBlockByteOffset(), 10);
    TEST_ASSERT_EQ(headerWithoutSupport.getSupportBlockSize(), 0);
    TEST_ASSERT_EQ(headerWithoutSupport.getSupportBlockByteOffset(), 0);
    TEST_ASSERT_EQ(headerWithoutSupport.getPvpBlockSize(), 5);
    TEST_ASSERT_EQ(headerWithoutSupport.getPvpBlockByteOffset(), 15);
    TEST_ASSERT_EQ(headerWithoutSupport.getSignalBlockSize(), 6);
    TEST_ASSERT_EQ(headerWithoutSupport.getSignalBlockByteOffset(), 20);
    TEST_ASSERT_EQ(headerWithoutSupport.getClassification(), "UNCLASSIFIED");
    TEST_ASSERT_EQ(headerWithoutSupport.getReleaseInfo(), "UNRESTRICTED");

    std::string fileHeaderTxtNoClass = "CPHD/1.0\n"
            "XML_BLOCK_SIZE := 3\n"
            "XML_BLOCK_BYTE_OFFSET := 10\n"
            "PVP_BLOCK_SIZE := 5\n"
            "PVP_BLOCK_BYTE_OFFSET := 15\n"
            "SIGNAL_BLOCK_SIZE := 6\n"
            "SIGNAL_BLOCK_BYTE_OFFSET := 20\n"
            "RELEASE_INFO := UNRESTRICTED\n"
            "\f\n";
    io::ByteStream fileHeaderContentWithoutClassification;
    fileHeaderContentWithoutClassification.write(fileHeaderTxtNoClass.c_str(),
                                                    fileHeaderTxtNoClass.size());
    TEST_THROWS(cphd::FileHeader().read(fileHeaderContentWithoutClassification));

    std::string fileHeaderTxtInvalid = "CPHD/1.0\n"
            "XML_BLOCK_SIZE := foo\n"
            "XML_BLOCK_BYTE_OFFSET := 10\n"
            "PVP_BLOCK_SIZE := 5\n"
            "PVP_BLOCK_BYTE_OFFSET := 15\n"
            "SIGNAL_BLOCK_SIZE := 6\n"
            "SIGNAL_BLOCK_BYTE_OFFSET := 20\n"
            "CLASSIFICATION := UNCLASSIFIED\n"
            "RELEASE_INFO := UNRESTRICTED\n"
            "\f\n";
    io::ByteStream fileHeaderContentWithInvalidValue;
    fileHeaderContentWithInvalidValue.write(fileHeaderTxtInvalid.c_str(),
                                                fileHeaderTxtInvalid.size());
    TEST_THROWS(cphd::FileHeader().read(fileHeaderContentWithInvalidValue));
}

TEST_CASE(testRoundTripHeader)
{
    io::ByteStream headerContent;
    headerContent.write(FILE_HEADER_CONTENT, strlen(FILE_HEADER_CONTENT));
    cphd::FileHeader header;
    header.read(headerContent);
    std::string outString = header.toString();

    io::ByteStream roundTrippedContent;
    roundTrippedContent.write(outString.c_str(), outString.size());
    cphd::FileHeader roundTrippedHeader;
    roundTrippedHeader.read(roundTrippedContent);

    TEST_ASSERT_EQ(header.getXMLBlockSize(),
            roundTrippedHeader.getXMLBlockSize());
    TEST_ASSERT_EQ(header.getXMLBlockByteOffset(),
            roundTrippedHeader.getXMLBlockByteOffset());
    TEST_ASSERT_EQ(header.getSupportBlockSize(),
            roundTrippedHeader.getSupportBlockSize());
    TEST_ASSERT_EQ(header.getSupportBlockByteOffset(),
            roundTrippedHeader.getSupportBlockByteOffset());
    TEST_ASSERT_EQ(header.getPvpBlockSize(),
            roundTrippedHeader.getPvpBlockSize());
    TEST_ASSERT_EQ(header.getPvpBlockByteOffset(),
            roundTrippedHeader.getPvpBlockByteOffset());
    TEST_ASSERT_EQ(header.getSignalBlockSize(),
            roundTrippedHeader.getSignalBlockSize());
    TEST_ASSERT_EQ(header.getSignalBlockByteOffset(),
            roundTrippedHeader.getSignalBlockByteOffset());
    TEST_ASSERT_EQ(header.getClassification(),
            roundTrippedHeader.getClassification());
    TEST_ASSERT_EQ(header.getReleaseInfo(),
            roundTrippedHeader.getReleaseInfo());
}
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        TEST_CHECK(testReadVersion);
        TEST_CHECK(testCanReadHeaderWithoutBreaking);
        TEST_CHECK(testRoundTripHeader);
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 1;
}
