/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

/*
 * This test tests the round-trip process of taking an input NITF
 * file and writing it to a new file. This includes writing the image
 * segments (headers, extensions, and image data). This is an example
 * of how users can write the image data to their NITF file
 */

class RowStreamer: public nitf::RowSource
{
public:
    RowStreamer(nitf::Uint32 band, nitf::Uint32 numRows, nitf::Uint32 numCols,
            nitf::Uint32 pixelSize, nitf::ImageReader reader)
            throw (nitf::NITFException) :
        RowSource(band, numRows, numCols, pixelSize), mReader(reader)
    {
        mWindow.setStartRow(0);
        mWindow.setNumRows(1);
        mWindow.setStartCol(0);
        mWindow.setNumCols(numCols);
        mWindow.setBandList(&mBand);
        mWindow.setNumBands(1);
    }

    ~RowStreamer()
    {
    }

    void nextRow(char* buffer) throw (nitf::NITFException)
    {
        int padded;
        mReader.read(mWindow, (nitf::Uint8**) &buffer, &padded);
        mWindow.setStartRow(mWindow.getStartRow() + 1);
    }
protected:
    nitf::ImageReader mReader;
    nitf::SubWindow mWindow;
};

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
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            //for the images, we'll use a RowSource for streaming
            nitf::ImageSegment imseg = *iter;
            iter++;
            nitf::ImageReader iReader = reader.newImageReader(i);
            nitf::ImageWriter* iWriter = writer.newImageWriter(i);
            nitf::ImageSource* iSource = new nitf::ImageSource();
            nitf::Uint32 nBands = imseg.getSubheader().getNumImageBands();
            nitf::Uint32 nRows = imseg.getSubheader().getNumRows();
            nitf::Uint32 nCols = imseg.getSubheader().getNumCols();
            nitf::Uint32 pixelSize = NITF_NBPP_TO_BYTES(
                    imseg.getSubheader().getNumBitsPerPixel());

            for (nitf::Uint32 i = 0; i < nBands; i++)
            {
                iSource->addBand(new RowStreamer(i, nRows, nCols, pixelSize,
                        iReader));
            }
            iWriter->attachSource(iSource, true);
        }

        num = record.getNumGraphics();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            writer.setGraphicWriteHandler(i, new nitf::SegmentWriteHandler(
                    reader.newGraphicReader(i)));
        }

        num = record.getNumTexts();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            writer.setTextWriteHandler(i, new nitf::SegmentWriteHandler(
                    reader.newTextReader(i)));
        }

        num = record.getNumDataExtensions();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            writer.setDEWriteHandler(i, new nitf::SegmentWriteHandler(
                    reader.newDEReader(i)));
        }

        writer.write();
        output.close();
        io.close();
        return 0;
    }
    catch (except::Throwable & t)
    {
        std::cout << "ERROR!: " << t.getMessage() << std::endl;
    }
}

