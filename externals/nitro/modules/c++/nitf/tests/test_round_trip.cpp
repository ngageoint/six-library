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

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <import/nitf.hpp>

/*
 * This test tests the round-trip process of taking an input NITF
 * file and writing it to a new file. This includes writing the image
 * segments (headers, extensions, and image data). This is an example
 * of how users can write the image data to their NITF file
 */

namespace
{
class RowStreamer : public nitf::RowSourceCallback
{
public:
    RowStreamer(uint32_t band,
                uint32_t numCols,
                nitf::ImageReader reader) :
        mReader(reader),
        mBand(band)
    {
        mWindow.setNumRows(1);
        mWindow.setNumCols(numCols);
        mWindow.setBandList(&mBand);
        mWindow.setNumBands(1);
    }

    virtual void nextRow(uint32_t /*band*/, void* buffer)
    {
        int padded;
        mReader.read(mWindow, reinterpret_cast<std::byte**>(&buffer), &padded);
        mWindow.setStartRow(mWindow.getStartRow() + 1);
    }

private:
    nitf::ImageReader mReader;
    nitf::SubWindow mWindow;
    uint32_t mBand;
};

// RAII for managing a list of RowStreamer's
struct RowStreamers /*final*/   // no "final", SWIG doesn't like it
{
    nitf::RowSourceCallback* add(uint32_t band,
                                 uint32_t numCols,
                                 nitf::ImageReader reader)
    {
        mStreamers.emplace_back(new RowStreamer(band, numCols, reader));
        return mStreamers.back().get();
    }

private:
    std::vector<std::unique_ptr<RowStreamer>> mStreamers;
};
}

int main(int argc, char **argv)
{
    try
    {
        //  Check argv and make sure we are happy
        if (argc != 3)
        {
            std::cout << "Usage: %s <input-file> <output-file> \n" << argv[0]
                    << std::endl;
            exit( EXIT_FAILURE);
        }

        // Check that wew have a valid NITF
        if (nitf::Reader::getNITFVersion(argv[1]) == nitf::Version::NITF_VER_UNKNOWN)
        {
            std::cout << "Invalid NITF: " << argv[1] << std::endl;
            exit( EXIT_FAILURE);
        }

        //read it in
        nitf::Reader reader;
        nitf::IOHandle io(argv[1]);
        nitf::Record record = reader.read(io);

        //now, let's create the writer
        nitf::Writer writer;
        nitf::IOHandle output(argv[2], NITF_ACCESS_WRITEONLY, NITF_CREATE);
        writer.prepare(output, record);

        nitf::ListIterator iter = record.getImages().begin();
        uint32_t num = record.getNumImages();
        RowStreamers rowStreamers;
        for (uint32_t i = 0; i < num; i++)
        {
            //for the images, we'll use a RowSource for streaming
            nitf::ImageSegment imseg = *iter;
            iter++;
            nitf::ImageReader iReader = reader.newImageReader(i);
            nitf::ImageWriter iWriter = writer.newImageWriter(i);
            nitf::ImageSource iSource;
            uint32_t nBands = imseg.getSubheader().getNumImageBands();
            uint32_t nRows = imseg.getSubheader().getNumRows();
            uint32_t nCols = imseg.getSubheader().getNumCols();
            const auto pixelSize = static_cast<uint32_t>(NITF_NBPP_TO_BYTES(
                    imseg.getSubheader().getNumBitsPerPixel()));

            for (uint32_t ii = 0; i < nBands; i++)
            {
                nitf::RowSource rowSource(ii, nRows, nCols, pixelSize,
                                          rowStreamers.add(ii, nCols, iReader));
                iSource.addBand(rowSource);
            }
            iWriter.attachSource(iSource);
        }

        num = record.getNumGraphics();
        for (uint32_t i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newGraphicReader(i));
            mem::SharedPtr< ::nitf::WriteHandler> segmentWriter(
                new nitf::SegmentWriter(readerSource));
            writer.setGraphicWriteHandler(i, segmentWriter);
        }

        num = record.getNumTexts();
        for (uint32_t i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newTextReader(i));
            mem::SharedPtr< ::nitf::WriteHandler> segmentWriter(
                new nitf::SegmentWriter(readerSource));
            writer.setTextWriteHandler(i, segmentWriter);
        }

        num = record.getNumDataExtensions();
        for (uint32_t i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newDEReader(i));
            mem::SharedPtr< ::nitf::WriteHandler> segmentWriter(
                new nitf::SegmentWriter(readerSource));
            writer.setDEWriteHandler(i, segmentWriter);
        }

        writer.write();
        output.close();
        io.close();
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR!: " << ex.what() << "\n";
        return 1;
    }
}

