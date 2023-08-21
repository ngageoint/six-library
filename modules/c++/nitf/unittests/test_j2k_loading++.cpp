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
#include <std/cstddef>
#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>

#include <gsl/gsl.h>
#include <str/Format.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <sio/lite/FileWriter.h>
#include <sio/lite/ReadUtils.h>
#include <io/ReadUtils.h>
#include <io/FileOutputStream.h>
#include <sys/OS.h>
#include <sys/Span.h>

#include <import/nitf.hpp>
#include <nitf/J2KReader.hpp>
#include <nitf/J2KWriter.hpp>
#include <nitf/ComponentInfo.hpp>
#include <nitf/FileHeader.hpp>
#include <nitf/IOHandle.hpp>
#include <nitf/List.hpp>
#include <nitf/Reader.hpp>
#include <nitf/Record.hpp>
#include <nitf/J2KCompressor.hpp>
#include <nitf/UnitTests.hpp>

#include <TestCase.h>

static auto findInputFile(const std::filesystem::path& fn)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "nitf" / "unittests";
    return nitf::Test::findInputFile(unittests, fn);
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
    const auto input_file = findInputFile("j2k_compressed_file1_jp2.ntf").string();
    test_image_loading_(input_file, false /*optz*/);
    //test_image_loading_(input_file, true /*optz*/);

    // These are too big for GitHub
    //input_file = findInputFile("sample_pan.nitf").string();
    //test_image_loading_(input_file, false /*optz*/);
    //input_file = findInputFile("xxxx_MSI.nitf").string();
    //test_image_loading_(input_file, false /*optz*/);

    TEST_ASSERT_TRUE(true); // be sure hidden "testName" parameter is used
}

static void test_j2k_nitf_(const std::string& testName,
    const std::string& fname)
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
    const auto input_file = findInputFile("j2k_compressed_file1_jp2.ntf").string();
    test_j2k_nitf_(testName, input_file);
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
    options.setCompressionRatio(0.0); // lossless
    //options.setCompressionRatio(12.0); // lossy

    /* TODO set some options here */
    j2k::Writer writer(inContainer, options);

    nitf::IOHandle outIO(outName, NRT_ACCESS_WRITEONLY, NRT_CREATE);

    j2k::WriteTiler tiler(writer, buf);
    // TODO: determine tile range from read region
    for (uint32_t y = 0; y < num_y_tiles; ++y)
    {
        for (uint32_t x = 0; x < num_x_tiles; ++x)
        {
            // TODO: May need to handle this differently for multiple components
            tiler.setTile(x, y, 0);
        }
    }

    writer.write(outIO);
    //printf("Wrote file: %s\n", outName.c_str());
}
void test_j2k_nitf_read_region_(const std::string& testName,
    const std::filesystem::path& fname)
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
        const auto result = sys::make_const_span(result_);

        const auto namePrefix = str::format("image-%d", (i + 1));
        // TODO: Update write to only output tiles in read region
        writeFile(0, 0, width, height, result, namePrefix);
        writeJ2K(0, 0, width, height, result, container, namePrefix);
    }
}
TEST_CASE(test_j2k_nitf_read_region)
{
    // This is a JP2 file, not J2K; see OpenJPEG_setup_()
    const auto input_file = findInputFile("j2k_compressed_file1_jp2.ntf");
    test_j2k_nitf_read_region_(testName, input_file);

    TEST_ASSERT_TRUE(true); // be sure hidden "testName" parameter is used
}

static std::vector<std::byte> readImage(const std::string& testName,
    nitf::ImageReader& imageReader, const nitf::ImageSubheader& imageSubheader)
{
    const auto numBlocks = imageSubheader.numBlocksPerRow() * imageSubheader.numBlocksPerCol();
    TEST_ASSERT_GREATER(static_cast<int64_t>(numBlocks), 0);

    const auto imageLength = imageSubheader.getNumBytesOfImageData();
    TEST_ASSERT_GREATER(static_cast<int64_t>(imageLength), 0);

    // This assumes vertical blocking.
    // Interleaving would be required for horizontal blocks
    std::vector<std::byte> retval(imageLength);
    uint64_t byteOffset = 0;
    for (uint32_t block = 0; block < numBlocks; ++block)
    {
        uint64_t bytesRead;
        const auto blockData = imageReader.readBlock(block, &bytesRead);
        TEST_ASSERT(blockData != nullptr);
        memcpy(retval.data() + byteOffset, blockData, bytesRead);
        byteOffset += bytesRead;
    }
    return retval;
}
static void test_decompress_nitf_to_sio_(const std::string& testName,
    const std::filesystem::path& inputPathname, const std::filesystem::path& outputPathname)
{
    // Take a J2K-compressed NITF, decompress the image and save to an SIO.
    nitf::Reader reader;
    nitf::IOHandle io(inputPathname.string());
    const auto record = reader.read(io);
    auto iter = record.getImages().begin();
    nitf::ImageSegment imageSegment = *iter;
    const auto imageSubheader = imageSegment.getSubheader();

    auto imageReader = reader.newImageReader(0 /*imageSegmentNumber*/);
    const auto imageData = readImage(testName, imageReader, imageSubheader);

    const sys::filesystem::path outputPathname_ = outputPathname.string();
    sio::lite::writeSIO(imageData.data(), imageSubheader.dims(), outputPathname_);
}
TEST_CASE(test_j2k_decompress_nitf_to_sio)
{
    nitf::Test::j2kSetNitfPluginPath();

    const auto inputPathname = findInputFile("j2k_compressed_file1_jp2.ntf"); // This is a JP2 file, not J2K; see OpenJPEG_setup_()
    test_decompress_nitf_to_sio_(testName, inputPathname, "test_decompress_nitf.sio");

    TEST_ASSERT_TRUE(true); // be sure hidden "testName" parameter is used
}

TEST_CASE(test_j2k_compress_raw_image)
{
    nitf::Test::j2kSetNitfPluginPath();

    const auto inputPathname = findInputFile("j2k_compressed_file1_jp2.ntf"); // This is a JP2 file, not J2K; see OpenJPEG_setup_()
    const std::filesystem::path outputPathname = "test_j2k_compress_raw_image.sio";
    test_decompress_nitf_to_sio_(testName, inputPathname, outputPathname);
    // ---------------------------------------------------------------------------------------

    // J2K compresses the raw image data of an SIO file
    const auto& inPathname = outputPathname; // "input pathname to raw file or sio to compress"
    const auto& testJ2KPathname = inputPathname; // "optional J2K file to compare compressed result to"

    // Read in the raw data from the input SIO
    types::RowCol<size_t> rawDims;
    std::vector<std::byte> rawImage_;
    sio::lite::readSIO(inPathname, rawDims, rawImage_);
    const auto rawImage = sys::make_span(rawImage_);

    const auto& tileDims = rawDims;
    const size_t numThreads = sys::OS().getNumCPUs() - 1;
    const j2k::CompressionParameters params(rawDims, tileDims);
    const j2k::Compressor compressor(params, numThreads);

    std::vector<std::byte> compressedImage_;
    std::vector<size_t> bytesPerBlock;
    compressor.compress(rawImage, compressedImage_, bytesPerBlock);
    const auto compressedImage = sys::make_span(compressedImage_);

    const auto sumCompressedBytes = std::accumulate(bytesPerBlock.begin(), bytesPerBlock.end(), gsl::narrow<size_t>(0));
    TEST_ASSERT_EQ(sumCompressedBytes, compressedImage.size()); // "Size of compressed image does not match sum of bytes per block"

    const auto compressedPathname = "compressed_" + std::to_string(tileDims.row) + "x" + std::to_string(tileDims.col) + ".j2k";
    ::io::FileOutputStream os(compressedPathname);
    os.write(compressedImage);

    std::vector<std::byte> j2kData;
    io::readFileContents(testJ2KPathname, j2kData);
    //TEST_ASSERT_EQ((compressedImage.size(), j2kData.size());
    //for (size_t ii = 0; ii < compressedImage.size(); ++ii)
    //{
    //  //TEST_ASSERT_EQ(j2kData[ii], compressedImage[ii]);
    //}
}

TEST_MAIN(
    TEST_CHECK(test_j2k_loading);
    TEST_CHECK(test_j2k_nitf);
    TEST_CHECK(test_j2k_nitf_read_region);

    TEST_CHECK(test_j2k_decompress_nitf_to_sio);
    TEST_CHECK(test_j2k_compress_raw_image);
    )
