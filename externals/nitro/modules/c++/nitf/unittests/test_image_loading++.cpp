/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <vector>
#include <string>

#include <import/nitf.hpp>

namespace fs = std::filesystem;

#include "TestCase.h"

static std::string testName;

static std::string argv0;
static fs::path findInputFile(const fs::path& inputFile)
{
    fs::path root;
    if (argv0.empty())
    {
        // running in Visual Studio
        root = fs::current_path().parent_path().parent_path();
    }
    else
    {
        root = fs::absolute(argv0).parent_path().parent_path().parent_path().parent_path();
        root = root.parent_path().parent_path();
    }

    return root / inputFile;
}
static fs::path findInputFile()
{
    const auto inputPath = fs::path("modules") / "c++" / "nitf" / "unittests" / "sicd_50x50.nitf";
    return findInputFile(inputPath);
}
static fs::path findInputFile(bool withAmpTable)
{
    fs::path inputPath;
    if (withAmpTable)
    {
        inputPath = fs::path("modules") / "c++" / "nitf" / "unittests" / "8_bit_Amp_Phs_Examples" / 
            "With_amplitude_table" /
            "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    }
    else
    {
        inputPath = fs::path("modules") / "c++" / "nitf" / "unittests" / "8_bit_Amp_Phs_Examples" / 
            "No_amplitude_table" /
            "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";

    }
    return findInputFile(inputPath);
}

struct expected_values final
{
    uint32_t nRows = 50;
    uint32_t nCols = 50;
    nitf::PixelValueType pixelValueType = nitf::PixelValueType::Floating; // "R"
    uint32_t bitsPerPixel = 32;
    std::string actualBitsPerPixel = "32";
    uint32_t pixelsPerHorizBlock = 50;
    uint32_t pixelsPerVertBlock = 50;
};

static void writeImage(nitf::ImageSegment &segment,
                nitf::Reader &reader,
                const int imageNumber,
                const std::string& imageName,
                uint32_t rowSkipFactor,
                uint32_t columnSkipFactor, bool optz,
    const expected_values& expected)
{
    nitf::ImageReader deserializer = reader.newImageReader(imageNumber);

    // missing skip factor
    nitf::ImageSubheader subheader = segment.getSubheader();

    uint32_t nBits   = subheader.getNumBitsPerPixel();

    uint32_t nBands  = subheader.getNumImageBands();
    uint32_t xBands  = subheader.getNumMultispectralImageBands();
    nBands += xBands;


    uint32_t nRows   = subheader.getNumRows();
    uint32_t nCols   = subheader.getNumCols();
    size_t subWindowSize = (size_t)(nRows / rowSkipFactor) *
        (size_t)(nCols / columnSkipFactor) *
        (size_t)NITF_NBPP_TO_BYTES(nBits);

    if (optz)
    {
        const auto irep = segment.getSubheader().imageRepresentation();
        const auto irepStartsWithRGB = (irep == nitf::ImageRepresentation::RGB) || (irep == nitf::ImageRepresentation::RGB_LUT);
        const auto ic = segment.getSubheader().imageCompression();

        const auto imageMode = segment.getSubheader().imageBlockingMode();
        if ((nBands == 3) && (imageMode == nitf::BlockingMode::Pixel) && irepStartsWithRGB
                && (NITF_NBPP_TO_BYTES(nBits) == 1) && str::startsWith(ic, "N"))
        {
            subWindowSize *= nBands;
            nBands = 1;
            //std::cout << "Using accelerated 3-band RGB mode pix-interleaved image" << std::endl;
        }
        if ((nBands == 2) && (imageMode == nitf::BlockingMode::Pixel) && str::startsWith(ic, "N"))
        {

            subWindowSize *= nBands;
            nBands = 1;
            //std::cout << "Using accelerated 2-band IQ mode pix-interleaved image" << std::endl;
        }
    }

    TEST_ASSERT_EQ(optz ? 1 : 2, nBands);
    TEST_ASSERT_EQ(0, xBands);
    TEST_ASSERT_EQ(expected.nRows, nRows);
    TEST_ASSERT_EQ(expected.nCols, nCols);
    TEST_ASSERT_EQ(expected.pixelValueType, subheader.pixelValueType()); 
    TEST_ASSERT_EQ(expected.bitsPerPixel, subheader.numBitsPerPixel());
    TEST_ASSERT_EQ(expected.actualBitsPerPixel, subheader.getActualBitsPerPixel().toString());
    TEST_ASSERT_EQ("R", subheader.getPixelJustification().toString());
    TEST_ASSERT_EQ(nitf::BlockingMode::Pixel, subheader.imageBlockingMode()); // "P"
    TEST_ASSERT_EQ(1, subheader.numBlocksPerRow());
    TEST_ASSERT_EQ(1, subheader.numBlocksPerCol());
    TEST_ASSERT_EQ(expected.pixelsPerHorizBlock, subheader.numPixelsPerHorizBlock());
    TEST_ASSERT_EQ(expected.pixelsPerVertBlock, subheader.numPixelsPerVertBlock());
    TEST_ASSERT_EQ("NC", subheader.imageCompression());
    TEST_ASSERT_EQ("    ", subheader.getCompressionRate().toString());

    nitf::BufferList<std::byte> buffer(nBands);
    std::vector<uint32_t> bandList(nBands);

    nitf::SubWindow subWindow(nRows / rowSkipFactor, nCols / columnSkipFactor);

    nitf::PixelSkip pixelSkip(rowSkipFactor, columnSkipFactor);
    subWindow.setDownSampler(pixelSkip);

    buffer.initialize(subWindowSize);
    for (uint32_t band = 0; band < nBands; band++)
    {
        bandList[band] = band;
    }
    setBands(subWindow, bandList);

    int padded;
    deserializer.read(subWindow, buffer.data(), &padded);

    for (unsigned int i = 0; i < nBands; i++)
    {
        std::string base = fs::path(imageName).filename().string();

        size_t where = 0;
        while ((where = base.find(".")) != (size_t)std::string::npos)
            base.replace(where, 1, "_");

        std::ostringstream file;
        file << base << "__" << imageNumber << "__"
             << nRows / rowSkipFactor << '_'
             << nCols / columnSkipFactor << '_'
             << nBits << "_band_" << i << ".out";

        nitf::IOHandle toFile(file.str(), NITF_ACCESS_WRITEONLY, NITF_CREATE);
        toFile.write(buffer[i], subWindowSize);
        toFile.close();
    }
}

static void test_image_loading_(const std::string& input_file, bool optz, const expected_values& expected)
{
    /* Skip factors */
    uint32_t rowSkipFactor = 1;
    uint32_t columnSkipFactor = 1;

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

        /*  Write the thing out  */
        writeImage(imageSegment, reader, count, input_file,
            rowSkipFactor, columnSkipFactor, optz, expected);
    }
}

TEST_CASE(test_image_loading)
{
    ::testName = testName;

    /*  If you didnt give us a nitf file, we're croaking  */
    const auto input_file = findInputFile().string();
    expected_values expected; // braced-initialization cause CodeQL to fail?
    expected.nRows = expected.nCols = 50;
    expected.pixelValueType = nitf::PixelValueType::Floating; // "R"
    expected.bitsPerPixel = 32;
    expected.actualBitsPerPixel = "32";
    expected.pixelsPerHorizBlock = expected.nCols;
    expected.pixelsPerVertBlock = expected.nRows;

    test_image_loading_(input_file, false /*optz*/, expected);
    test_image_loading_(input_file, true /*optz*/, expected);
}

TEST_CASE(test_8bit_image_loading)
{
    ::testName = testName;

    auto input_file = findInputFile(true /*withAmpTable*/).string();
    expected_values expected; // braced-initialization cause CodeQL to fail?
    expected.nRows = 3975;
    expected.nCols = 6724;
    expected.pixelValueType = nitf::PixelValueType::Integer; // "R"
    expected.bitsPerPixel = 8;
    expected.actualBitsPerPixel = "08";
    expected.pixelsPerHorizBlock = expected.nCols;
    expected.pixelsPerVertBlock = expected.nRows;

    test_image_loading_(input_file, false /*optz*/, expected);
    test_image_loading_(input_file, true /*optz*/, expected);

    input_file = findInputFile(false /*withAmpTable*/).string();
    test_image_loading_(input_file, false /*optz*/, expected);
    test_image_loading_(input_file, true /*optz*/, expected);
}

TEST_MAIN(
    (void)argc;
    argv0 = argv[0];
    TEST_CHECK(test_image_loading);
    TEST_CHECK(test_8bit_image_loading);
)