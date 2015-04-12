/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
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
#include <string.h>

#include <mem/SharedPtr.h>
#include <import/nitf.hpp>

class TestDirectBlockSource: public nitf::DirectBlockSource
{
public:
    TestDirectBlockSource(nitf::ImageReader& imageReader,
                      nitf::Uint32 numBands)
        throw (nitf::NITFException) : nitf::DirectBlockSource(imageReader, numBands){}

protected:
    virtual void nextBlock(void* buf,
                           const void* block,
                           nitf::Uint32 blockNumber,
                           nitf::Uint64 blockSize) throw (nitf::NITFException)
    {
        std::cout << "BLOCK NUMBER: " << blockNumber << " " << blockSize << std::endl;
        if (buf)
        {
            memcpy(buf, block, blockSize);
        }
    }
};

/*
 * This test tests the round-trip process of taking an input NITF
 * file and writing it to a new file. This includes writing the image
 * segments (headers, extensions, and image data). This is an example
 * of how users can write the image data to their NITF file
 */

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

        std::vector<nitf::ImageReader> imageReaders;
        std::vector<nitf::ImageWriter> imageWriters;
        std::map<std::string, void*> writerOptions;
        std::vector<mem::SharedPtr<nitf::DirectBlockSource> > bandSources;

        //nitf::Uint32 numRes = 1;
        //writerOptions[C8_NUM_RESOLUTIONS_KEY] = &numRes;

        for (nitf::Uint32 i = 0; i < num; i++)
        {
            //for the images, we'll use a DirectBlockSource for streaming
            nitf::ImageSegment imseg = *iter;
            iter++;
            imageReaders.push_back(reader.newImageReader(i));
            imageWriters.push_back(writer.newImageWriter(i, writerOptions));
            nitf::ImageSource iSource;

            bandSources.push_back(mem::SharedPtr<nitf::DirectBlockSource>(
                                      new TestDirectBlockSource(imageReaders[i], 1)));
            iSource.addBand(*bandSources[bandSources.size()-1]);

            imageWriters[i].attachSource(iSource);
            imageWriters[i].setDirectBlockWrite(1);
        }

        num = record.getNumGraphics();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newGraphicReader(i));
            mem::SharedPtr< ::nitf::WriteHandler> segmentWriter(
                new nitf::SegmentWriter(readerSource));
            writer.setGraphicWriteHandler(i, segmentWriter);
        }

        num = record.getNumTexts();
        for (nitf::Uint32 i = 0; i < num; i++)
        {
            nitf::SegmentReaderSource readerSource(reader.newTextReader(i));
            mem::SharedPtr< ::nitf::WriteHandler> segmentWriter(
                new nitf::SegmentWriter(readerSource));
            writer.setTextWriteHandler(i, segmentWriter);
        }

        num = record.getNumDataExtensions();
        for (nitf::Uint32 i = 0; i < num; i++)
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
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Throwable & t)
    {
        std::cerr << "Caught throwable: " << t.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }
}
