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

#include <import/nitf.hpp>
#include <nitf/ImageSubheader.hpp>
#include <nitf/ImageWriter.hpp>
#include <nitf/Record.hpp>

#include "TestCase.h"

namespace fs = std::filesystem;

static fs::path argv0;
static const fs::path file = __FILE__;

static bool is_linux()
{
	const auto cpp = file.filename().stem(); // i.e., "test_valid_six"
	const auto exe = argv0.filename(); // e.g., "test_valid_six.exe"
	return cpp == exe; // no ".exe", must be Linux
}

static bool is_vs_gtest()
{
	return argv0.empty(); // no argv[0] in VS w/GTest
}

static fs::path buildFileDir(const fs::path& relativePath)
{
	if (is_vs_gtest())
	{
		static const auto cwd = fs::current_path();

		// Running GTest unit-tests in Visual Studio on Windows
		return cwd.parent_path().parent_path() / relativePath;
	}

    auto root_dir = argv0.parent_path().parent_path().parent_path().parent_path();
    if (is_linux())
    {
        if (root_dir.stem() == "build") // CMake, in ./build directory
        {
            root_dir = root_dir.parent_path();
        }
        else if (root_dir.stem() == "modules") // WAF
        {
	  root_dir = root_dir.parent_path().parent_path();
        }
        return root_dir / relativePath;
    }

    // must be Windows w/o VS
    root_dir = root_dir.parent_path();
    if (root_dir.stem() == "build") // in ./build directory, CMake
    {
        root_dir = root_dir.parent_path();
    }
    else if (root_dir.stem() == "target") // WAF
    {
        root_dir = root_dir.parent_path();
    }

    return root_dir / relativePath;
}

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

namespace
{
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
    subheader.setPixelInformation(nitf::PixelType::Integer, 8, 8, "R", nitf::ImageRepresentation::MONO, "VIS", bands);
    subheader.setBlocking(100, 200, 10, 10, nitf::BlockingMode::Pixel);
    nitf::ImageWriter writer(subheader);
}

TEST_CASE(changeFileHeader)
{
	const auto inputPathname = buildFileDir(fs::path("modules") / "c++" / "nitf" / "tests" / "test_blank.ntf").string();
    TEST_ASSERT_NOT_EQ(inputPathname, "");
    //std::clog << "'" << inputPathname << "'\n";
    TEST_ASSERT_TRUE(fs::is_regular_file(inputPathname));
	const auto outputPathname = buildFileDir(fs::path("outputPathname.ntf")).string();

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
}

TEST_MAIN(
    (void)argc;
	argv0 = fs::absolute(argv[0]).string();

    TEST_CHECK(imageWriterThrowsOnFailedConstruction);
    TEST_CHECK(constructValidImageWriter);
    TEST_CHECK(changeFileHeader);
    )
