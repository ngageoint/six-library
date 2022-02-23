/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include <stdio.h>
#include <inttypes.h>

#include <vector>
#include <string>
#include <std/filesystem>
#include <std/span>

#include <gsl/gsl.h>
#include <str/Format.h>

#include <import/nitf.hpp>
#include <nitf/J2KReader.hpp>
#include <nitf/J2KWriter.hpp>

namespace fs = std::filesystem;

#include "TestCase.h"

static std::string testName;

static fs::path findRoot(const fs::path& p)
{
    if (fs::is_regular_file(p / "LICENSE") && fs::is_regular_file(p / "README.md") && fs::is_regular_file(p / "CMakeLists.txt"))
    {
        return p;
    }
    return findRoot(p.parent_path());
}
inline fs::path findRoot()
{
    return findRoot(fs::current_path());
}

static std::string argv0;
static fs::path findInputFile_(const fs::path& inputFile)
{
    fs::path root = findRoot();
    return root / inputFile;
}
static fs::path findInputFile(const fs::path& fn)
{
    const auto inputPath = fs::path("modules") / "c++" / "nitf" / "unittests" / fn;
    return findInputFile_(inputPath);
}

static void test_image_loading_(const std::string& input_file, bool /*optz*/)
{
    /*  This is the io handle we will give the reader to parse  */
    nitf::IOHandle io(input_file);

    /*  This is the reader  */
    nitf::Reader reader;

    /*  Read the file (first pass) */
    nitf::Record record = reader.read(io);

    /*  These iterators are for going through the image segments  */
    /*  And set this one to the end, so we'll know when we're done!  */
    auto iter = record.getImages().begin();
    auto end = record.getImages().end();
    /*  While we are not done...  */
    for (int count = 0; iter != end; ++iter, ++count)
    {
        nitf::ImageSegment imageSegment(iter.get());

        /* Skip factors */
        //uint32_t rowSkipFactor = 1;
        //uint32_t columnSkipFactor = 1;

        ///*  Write the thing out  */
        //writeImage(imageSegment, reader, count, input_file,
        //    rowSkipFactor, columnSkipFactor, optz);
    }
}

TEST_CASE(test_j2k_loading)
{
    ::testName = testName;

    auto input_file = findInputFile("j2k_compressed_file1_jp2.ntf").string();
    test_image_loading_(input_file, false /*optz*/);
    //test_image_loading_(input_file, true /*optz*/);

    // These are too big for GitHub
    //input_file = findInputFile("sample_pan.nitf").string();
    //test_image_loading_(input_file, false /*optz*/);
    //input_file = findInputFile("xxxx_MSI.nitf").string();
    //test_image_loading_(input_file, false /*optz*/);
}

static void test_j2k_nitf_(const std::string& fname)
{
    nitf::IOHandle io(fname);
    nitf::Reader reader;
    const auto version = reader.getNITFVersion(io);
    TEST_ASSERT(version != NITF_VER_UNKNOWN);

    const auto record = reader.readIO(io);
    const auto images = record.getImages();

    for (auto it = images.begin(); it != images.end(); ++it)
    {
        const nitf::ImageSegment segment(*it);
        const auto subheader = segment.getSubheader();

        TEST_ASSERT_EQ(nitf::ImageCompression::C8, subheader.imageCompression());
        io.seek(gsl::narrow<nrt_Off>(segment.getImageOffset()), NRT_SEEK_SET);
        j2k::Reader j2kReader(io);
        auto container = j2kReader.getContainer();

        const auto nComponents = container.getNumComponents();
        for (uint32_t cmpIt = 0; cmpIt < nComponents; ++cmpIt)
        {
            const auto c = container.getComponent(cmpIt);
        }

        const auto width = container.getWidth();
        const auto height = container.getHeight();

        auto buf = j2k::make_Buffer();
        const auto result = j2kReader.readRegion(0, 0, width, height, buf);
        TEST_ASSERT(result.size() != 0);
        TEST_ASSERT(buf.get() != nullptr);
        TEST_ASSERT(buf.get() == result.data());
    }
}
TEST_CASE(test_j2k_nitf)
{
    ::testName = testName;

    j2k_Reader* pNative = nullptr;
    try
    {
        j2k::Reader j2kReader(std::move(pNative));
        TEST_ASSERT(false);
    }
    catch (const nitf::NITFException&)
    {
        TEST_ASSERT(true);
    }

    // This is a JP2 file, not J2K; see OpenJPEG_setup_()
    auto input_file = findInputFile("j2k_compressed_file1_jp2.ntf").string();
    test_j2k_nitf_(input_file);
}

void writeFile(uint32_t x0, uint32_t y0,
    uint32_t x1, uint32_t y1, std::span<const uint8_t> buf, const std::string& prefix)
{
    const auto filename = str::format("%s-raw-%d_%d__%d_%d.out", prefix.c_str(), x0, y0, x1, y1);
    nitf::IOHandle outHandle(filename, NRT_ACCESS_WRITEONLY, NRT_CREATE);
    outHandle.write(buf.data(), buf.size());
    //printf("Wrote file: %s\n", filename.c_str());
}
void writeJ2K(uint32_t x0, uint32_t y0,
    uint32_t x1, uint32_t y1, std::span<const uint8_t> buf,
    const j2k::Container& inContainer, const std::string& prefix)
{
    const auto outName = str::format("%s-raw-%d_%d__%d_%d.j2k", prefix.c_str(), x0, y0, x1, y1);

    const auto num_x_tiles = inContainer.getTilesX();
    const auto num_y_tiles = inContainer.getTilesY();

    j2k::WriterOptions options;
    /* TODO set some options here */
    j2k::Writer writer(inContainer, options);

    nitf::IOHandle outIO(outName, NRT_ACCESS_WRITEONLY, NRT_CREATE);

    const auto tileSize = inContainer.tileSize();
    // TODO: determine tile range from read region
    for (uint32_t y = 0; y < num_y_tiles; ++y)
    {
        for (uint32_t x = 0; x < num_x_tiles; ++x)
        {
            // TODO: May need to handle this differently for multiple components
            const auto offset = inContainer.bufferOffset(x, y, 0);
            const std::span<const uint8_t> buf_(buf.data() + offset, tileSize);
            writer.setTile(x, y, buf_);
        }
    }

    writer.write(outIO);
    //printf("Wrote file: %s\n", outName.c_str());
}
void test_j2k_nitf_read_region_(const fs::path& fname)
{
    nitf::IOHandle io(fname.string(), NRT_ACCESS_READONLY, NRT_OPEN_EXISTING);
    nitf::Reader reader;
    const auto version = reader.getNITFVersion(io);
    TEST_ASSERT(version != NITF_VER_UNKNOWN);

    const auto record = reader.readIO(io);
    const auto images = record.getImages();

    int i = 0;
    for (auto it = images.begin(); it != images.end(); ++it, ++i)
    {
        const nitf::ImageSegment segment(*it);
        const auto subheader = segment.getSubheader();

        TEST_ASSERT_EQ(nitf::ImageCompression::C8, subheader.imageCompression());
        //printf("Image %d contains J2K compressed data\n", (i + 1));
        //printf("Offset: %" PRIu64 "\n", segment.getImageOffset());
        io.seek(gsl::narrow<nrt_Off>(segment.getImageOffset()), NRT_SEEK_SET);

        j2k::Reader j2kReader(io);
        auto container = j2kReader.getContainer();

        const auto x1 = container.getWidth();
        const auto y1 = container.getHeight();

        //printf("grid width:\t%d\n", container.getGridWidth());
        //printf("grid height:\t%d\n", container.getGridHeight());
        //printf("tile width:\t%d\n", container.getTileWidth());
        //printf("tile height:\t%d\n", container.getTileHeight());
        //printf("x tiles:\t%d\n", container.getTilesX());
        //printf("y tiles:\t%d\n", container.getTilesY());
        //printf("image type:\t%d\n", container.getImageType());

        const auto nComponents = container.getNumComponents();
        //printf("components:\t%d\n", nComponents);

        for (uint32_t cmpIt = 0; cmpIt < nComponents; ++cmpIt)
        {
            const auto c = container.getComponent(cmpIt);
            //printf("===component %d===\n", (cmpIt + 1));
            //printf("width:\t\t%d\n", c.getWidth());
            //printf("height:\t\t%d\n", c.getHeight());
            //printf("precision:\t%d\n", c.getPrecision());
            //printf("x0:\t\t%d\n", c.getOffsetX());
            //printf("y0:\t\t%d\n", c.getOffsetY());
            //printf("x separation:\t%d\n", c.getSeparationX());
            //printf("y separation:\t%d\n", c.getSeparationY());
            //printf("signed:\t\t%d\n", c.isSigned());
        }

        constexpr uint32_t x0 = 0;
        constexpr uint32_t y0 = 0;

        const auto c = container.getComponent(0);
        auto buf_ = j2k::make_Buffer();
        auto buf = j2kReader.readRegion(x0, y0, x1, y1, buf_);

        const auto width = container.getWidth();
        const auto height = container.getHeight();

        const auto result_ = j2kReader.readRegion(0, 0, width, height, buf);
        const std::span<const uint8_t> result(result_.data(), result_.size());

        const auto namePrefix = str::format("image-%d", (i + 1));
        // TODO: Update write to only output tiles in read region
        writeFile(0, 0, width, height, result, namePrefix);
        writeJ2K(0, 0, width, height, result, container, namePrefix);
    }
}
TEST_CASE(test_j2k_nitf_read_region)
{
    ::testName = testName;
    // This is a JP2 file, not J2K; see OpenJPEG_setup_()
    auto input_file = findInputFile("j2k_compressed_file1_jp2.ntf");
    test_j2k_nitf_read_region_(input_file);
}

TEST_MAIN(
    (void)argc;
    argv0 = argv[0];
    TEST_CHECK(test_j2k_loading);
    TEST_CHECK(test_j2k_nitf);
    TEST_CHECK(test_j2k_nitf_read_region);
    )