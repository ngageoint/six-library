/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include <import/nitf.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

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
    RowStreamer(nitf::Uint32 band,
                nitf::Uint32 numCols,
                nitf::ImageReader reader) throw (nitf::NITFException) :
        mReader(reader),
        mBand(band)
    {
        mWindow.setStartRow(0);
        mWindow.setNumRows(1);
        mWindow.setStartCol(0);
        mWindow.setNumCols(numCols);
        mWindow.setBandList(&mBand);
        mWindow.setNumBands(1);
    }

    void nextRow(nitf::Uint32 band, char* buffer) throw (nitf::NITFException)
    {
        int padded;
        mReader.read(mWindow, (nitf::Uint8**) &buffer, &padded);
        mWindow.setStartRow(mWindow.getStartRow() + 1);
    }

private:
    nitf::ImageReader mReader;
    nitf::SubWindow mWindow;
    nitf::Uint32 mBand;
};

// RAII for managing a list of RowStreamer's
class RowStreamers
{
public:
    ~RowStreamers()
    {
        for (size_t ii = 0; ii < mStreamers.size(); ++ii)
        {
            delete mStreamers[ii];
        }
    }

    nitf::RowSourceCallback* add(nitf::Uint32 band,
                                 nitf::Uint32 numCols,
                                 nitf::ImageReader reader)
    {
        std::auto_ptr<RowStreamer>
            streamer(new RowStreamer(band, numCols, reader));
        RowStreamer* const streamerPtr(streamer.get());

        mStreamers.push_back(streamerPtr);
        streamer.release();
        return streamerPtr;
    }

private:
    std::vector<RowStreamer*> mStreamers;
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
        if (nitf::Reader::getNITFVersion(argv[1]) == NITF_VER_UNKNOWN)
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
        nitf::Uint32 num = record.getNumImages();
        RowStreamers rowStreamers;
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            //for the images, we'll use a RowSource for streaming
            nitf::ImageSegment imseg = *iter;
            iter++;
            nitf::ImageReader iReader = reader.newImageReader(i);
            nitf::ImageWriter iWriter = writer.newImageWriter(i);
            nitf::ImageSource iSource;
            nitf::Uint32 nBands = imseg.getSubheader().getNumImageBands();
            nitf::Uint32 nRows = imseg.getSubheader().getNumRows();
            nitf::Uint32 nCols = imseg.getSubheader().getNumCols();
            nitf::Uint32
                    pixelSize =
                            NITF_NBPP_TO_BYTES(
                                               imseg.getSubheader().getNumBitsPerPixel());

            for (nitf::Uint32 i = 0; i < nBands; i++)
            {
                nitf::RowSource rowSource(i, nRows, nCols, pixelSize,
                                          rowStreamers.add(i, nCols, iReader));
                iSource.addBand(rowSource);
            }
            iWriter.attachSource(iSource);
        }

        num = record.getNumGraphics();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newGraphicReader(i));
            nitf::SegmentWriter segmentWriter(readerSource);
            writer.setGraphicWriteHandler(i, segmentWriter);
        }

        num = record.getNumTexts();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newTextReader(i));
            nitf::SegmentWriter segmentWriter(readerSource);
            writer.setTextWriteHandler(i, segmentWriter);
        }

        num = record.getNumDataExtensions();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newDEReader(i));
            nitf::SegmentWriter segmentWriter(readerSource);
            writer.setDEWriteHandler(i, segmentWriter);
        }

        writer.write();
        output.close();
        io.close();
        return 0;
    }
    catch (except::Throwable & t)
    {
        std::cout << "ERROR!: " << t.toString() << std::endl;
    }
}

