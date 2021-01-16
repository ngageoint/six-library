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
#include <iostream>
#include <string>

#include <import/nitf.hpp>

#include "TestCase.h"

static std::string testName;
const std::string output_file = "test_writer_3++.nitf";

namespace fs = std::filesystem;

static std::string argv0;
static fs::path findInputFile()
{
    const fs::path inputFile = fs::path("modules") / "c++" / "nitf" / "unittests" / "sicd_50x50.nitf";
 
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

static nitf::Record doRead(const std::string& inFile, nitf::Reader& reader);

static std::string makeBandName(const std::string& rootFile, int imageNum, int bandNum)
{
    std::string::size_type pos = rootFile.find_last_of("/\\");
    std::ostringstream os;
    os << rootFile.substr(pos + 1) << "__" << imageNum << "_band_" << bandNum;
    std::string newFile = os.str();

    while ((pos = newFile.find(".")) != std::string::npos)
        newFile.replace(pos, 1, "_");
    newFile += ".man";
    return newFile;
}

static nitf::ImageSource setupBands(int nbands, int imageNum, const std::string& inRootFile)
{
    nitf::ImageSource iSource;
    for (int i = 0; i < nbands; i++)
    {
        std::string inFile = makeBandName(inRootFile, imageNum, i);
        nitf::FileSource fs(inFile, 0, 1, 0);
        iSource.addBand(fs);
    }
    return iSource;
}

static void doWrite(const nitf::Record& record_, nitf::Reader& reader, const std::string& inRootFile,
    nitf::Writer& writer)
{
    auto& record = const_cast<nitf::Record&>(record_); // TODO: remove when API is const-correct

    int numImages = record.getHeader().getNumImages();
    nitf::ListIterator end = record.getImages().end();
    nitf::ListIterator iter = record.getImages().begin();

    for (int i = 0; i < numImages && iter != end; ++i, ++iter)
    {
        nitf::ImageSegment imseg;
        imseg = *iter;
        const auto nbands = static_cast<int>(imseg.getSubheader().numImageBands());
        nitf::ImageWriter iWriter = writer.newImageWriter(i);
        nitf::ImageSource iSource = setupBands(nbands, i, inRootFile);
        iWriter.attachSource(iSource);
    }

    const auto num = static_cast<int>(record.getNumDataExtensions());
    for (int i = 0; i < num; i++)
    {
        nitf::SegmentReaderSource readerSource(reader.newDEReader(i));
        mem::SharedPtr< ::nitf::WriteHandler> segmentWriter(
            new nitf::SegmentWriter(readerSource));
        writer.setDEWriteHandler(i, segmentWriter);
    }

    writer.write();
}

static void manuallyWriteImageBands(nitf::ImageSegment & segment,
                             const std::string& imageName,
                             nitf::ImageReader& deserializer,
                             int imageNumber)
{
    int padded;

    nitf::ImageSubheader subheader = segment.getSubheader();

    uint32_t nBits = subheader.getNumBitsPerPixel();
    uint32_t nBands = subheader.getNumImageBands();
    uint32_t xBands = subheader.getNumMultispectralImageBands();
    nBands += xBands;

    const auto nRows = subheader.numRows();
    const auto nColumns = subheader.numCols();

    //one row at a time
    const auto subWindowSize = static_cast<size_t>(nColumns * NITF_NBPP_TO_BYTES(nBits));

    TEST_ASSERT_EQ(2, nBands);
    TEST_ASSERT_EQ(0, xBands);
    TEST_ASSERT_EQ(50, nRows);
    TEST_ASSERT_EQ(50, nColumns);
    TEST_ASSERT_EQ("R  ", subheader.getPixelValueType().toString());
    TEST_ASSERT_EQ(32, subheader.numBitsPerPixel());
    TEST_ASSERT_EQ("32", subheader.getActualBitsPerPixel().toString());
    TEST_ASSERT_EQ("R", subheader.getPixelJustification().toString());
    TEST_ASSERT_EQ("P", subheader.getImageMode().toString());
    TEST_ASSERT_EQ(1, subheader.numBlocksPerRow());
    TEST_ASSERT_EQ(1, subheader.numBlocksPerCol());
    TEST_ASSERT_EQ(50, subheader.numPixelsPerHorizBlock());
    TEST_ASSERT_EQ(50, subheader.numPixelsPerVertBlock());
    TEST_ASSERT_EQ("NC", subheader.imageCompression());
    TEST_ASSERT_EQ("    ", subheader.getCompressionRate().toString());

    nitf::BufferList<std::byte> buffer(nBands);
    std::vector<uint32_t> bandList(nBands);

    for (uint32_t band = 0; band < nBands; band++)
        bandList[band] = band;

    nitf::SubWindow subWindow;
    subWindow.setNumRows(1);
    subWindow.setNumCols(static_cast<uint32_t>(nColumns));

    // necessary ?
    nitf::DownSampler* pixelSkip = new nitf::PixelSkip(1, 1);
    subWindow.setDownSampler(pixelSkip);
    setBands(subWindow, bandList);

    buffer.initialize(subWindowSize);

    std::vector<nitf::IOHandle> handles;
    //make the files
    for (int i = 0; i < static_cast<int>(nBands); i++)
    {
        std::string name = makeBandName(imageName, imageNumber, i);
        nitf::IOHandle toFile(name, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        handles.push_back(toFile);
    }

    //read all row blocks and write to disk
    for (uint32_t i = 0; i < nRows; ++i)
    {
        subWindow.setStartRow(i);
        deserializer.read(subWindow, buffer.data(), &padded);
        for (uint32_t j = 0; j < nBands; j++)
        {
            handles[j].write(buffer[j], subWindowSize);
        }
    }

    //close output handles
    for (uint32_t i = 0; i < nBands; i++)
        handles[i].close();
}

static nitf::Record doRead(const std::string& inFile, nitf::Reader& reader)
{
    // Check that wew have a valid NITF
    const auto version = nitf::Reader::getNITFVersion(inFile);
    TEST_ASSERT(version != NITF_VER_UNKNOWN);

    nitf::IOHandle io(inFile);
    nitf::Record record = reader.read(io);

    /*  Set this to the end, so we'll know when we're done!  */
    nitf::ListIterator end = record.getImages().end();
    nitf::ListIterator iter = record.getImages().begin();
    for (int count = 0, numImages = record.getHeader().getNumImages();
        count < numImages && iter != end; ++count, ++iter)
    {
        nitf::ImageSegment imageSegment = *iter;
        nitf::ImageReader deserializer = reader.newImageReader(count);

        /*  Write the thing out  */
        manuallyWriteImageBands(imageSegment, inFile, deserializer, count);
    }

    return record;
}

namespace test_writer_3
{
    /*
     * This test tests the round-trip process of taking an input NITF
     * file and writing it to a new file. This includes writing the image
     * segments (headers, extensions, and image data). This is an example
     * of how users can write the image data to their NITF file
     */
    static void doWrite(nitf::Record record, nitf::Reader& reader, const std::string& inRootFile, const std::string& outFile)
    {
        nitf::Writer writer;
        nitf::IOHandle output(outFile, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        writer.prepare(output, record);

        ::doWrite(record, reader, inRootFile, writer);

        output.close();
    }
}
TEST_CASE(test_writer_3_)
{
    ::testName = testName;

    const auto input_file = findInputFile().string();

    nitf::Reader reader;
    nitf::Record record = doRead(input_file, reader);
    test_writer_3::doWrite(record, reader, input_file, output_file);
}

namespace test_buffered_write
{
    /*
     * This test tests the round-trip process of taking an input NITF
     * file and writing it to a new file. This includes writing the image
     * segments (headers, extensions, and image data).
     *
     * This example differs from test_writer_3 in that it tests the
     * BufferedWriter classes, and writes the entire file as a set of
     * configurable sized blocks.  The last block may be smaller than the others
     * if the data does not fill the block.
     *
     */
    void doWrite(nitf::Record record, nitf::Reader& reader,
        const std::string& inRootFile,
        const std::string& outFile,
        size_t bufferSize)
    {
        nitf::BufferedWriter output(outFile, bufferSize);
        nitf::Writer writer;
        writer.prepareIO(output, record);

        ::doWrite(record, reader, inRootFile, writer);

        const auto blocksWritten = output.getNumBlocksWritten();
        const auto partialBlocksWritten = output.getNumPartialBlocksWritten();
        output.close();
        TEST_ASSERT_EQ(60, blocksWritten);
        TEST_ASSERT_EQ(53, partialBlocksWritten);
    }
}
TEST_CASE(test_buffered_write_)
{
    ::testName = testName;

    const auto input_file = findInputFile().string();

    size_t blockSize = 8192;

    nitf::Reader reader;
    nitf::Record record = doRead(input_file, reader);
    test_buffered_write::doWrite(record, reader, input_file, output_file, blockSize);

}

TEST_MAIN(
    (void)argc;
    argv0 = argv[0];
    TEST_CHECK(test_writer_3_);
    TEST_CHECK(test_buffered_write_);
    )