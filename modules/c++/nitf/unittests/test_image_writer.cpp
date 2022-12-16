/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <std/filesystem>

#include <import/nitf.hpp>
#include <nitf/ImageSubheader.hpp>
#include <nitf/ImageWriter.hpp>
#include <nitf/Record.hpp>
#include <nitf/UnitTests.hpp>

#include "TestCase.h"

using path = std::filesystem::path;

static void doChangeFileHeader(const std::string& inputPathname, const std::string& outputPathname)
{
    if (nitf::Reader::getNITFVersion(inputPathname) == nitf::Version::NITF_VER_UNKNOWN)
    {
        throw std::invalid_argument("Invalid NITF: " + inputPathname);
    }

    nitf::Reader reader;
    nitf::IOHandle io(inputPathname);
    
    nitf::Record record = reader.read(io);
    nitf::FileHeader fileHeader = record.getHeader();

    auto fileTitle = fileHeader.getFileTitle();
    std::string strFileTitle = fileTitle;
    str::replaceAll(strFileTitle, " ", "*"); // field is fixed length
    fileTitle.set(strFileTitle);

    record.setHeader(fileHeader);

    nitf::Writer writer;
    nitf::IOHandle output(outputPathname, NITF_ACCESS_WRITEONLY, NITF_CREATE);
    writer.prepare(output, record);
    writer.setWriteHandlers(io, record);
    writer.write();
}

TEST_CASE(imageWriterThrowsOnFailedConstruction)
{
    nitf::ImageSubheader subheader;
    TEST_EXCEPTION(nitf::ImageWriter(subheader));
}

TEST_CASE(constructValidImageWriter)
{
    nitf::Record record;
    nitf::ImageSegment segment = record.newImageSegment();
    nitf::ImageSubheader subheader = segment.getSubheader();
    std::vector<nitf::BandInfo> bands = {nitf::BandInfo(), nitf::BandInfo()};
    subheader.setPixelInformation(nitf::PixelValueType::Integer, 8, 8, "R", nitf::ImageRepresentation::MONO, "VIS", bands);
    subheader.setBlocking(100, 200, 10, 10, nitf::BlockingMode::Pixel);
    nitf::ImageWriter writer(subheader);
}

TEST_CASE(changeFileHeader)
{
    static const auto tests = std::filesystem::path("modules") / "c++" / "nitf" / "tests";
    static const auto inputPathname_ = nitf::Test::findInputFile(tests, "test_blank.ntf");
    TEST_ASSERT_TRUE(is_regular_file(inputPathname_));
	static const auto inputPathname = inputPathname_.string();
    constexpr auto outputPathname = "outputPathname.ntf";

    doChangeFileHeader(inputPathname, outputPathname);

    nitf::Reader reader;
    nitf::IOHandle io(outputPathname);
    nitf::Record record = reader.read(io);
    nitf::FileHeader fileHeader = record.getHeader();

    const std::string fileTitle = fileHeader.getFileTitle();
    auto npos = fileTitle.find(" ");
    TEST_ASSERT_EQ(npos, std::string::npos);
    npos = fileTitle.find("*");
    TEST_ASSERT(npos != std::string::npos);
}

TEST_MAIN(
    (void)argc;

    TEST_CHECK(imageWriterThrowsOnFailedConstruction);
    TEST_CHECK(constructValidImageWriter);
    TEST_CHECK(changeFileHeader);
    )
