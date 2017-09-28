/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <mem/SharedPtr.h>
#include <import/nitf.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Round-trip a NITF, removing any image segments with an IREP of NODISPLY
namespace
{
void stripImages(nitf::Record& record)
{
    std::vector<size_t> invisibleImages;
    nitf::List images = record.getImages();
    nitf::ListIterator iter = images.begin();
    for (size_t ii = 0; iter != images.end(); ++ii, ++iter)
    {
        nitf::ImageSegment image = *iter;
        nitf::ImageSubheader subheader = image.getSubheader();
        std::string irep(subheader.getImageRepresentation().getRawData());
        str::trim(irep);
        if (irep == "NODISPLY")
        {
            invisibleImages.push_back(ii);
        }
    }
    if (invisibleImages.size() == record.getNumImages())
    {
        throw except::Exception(Ctxt("All image segments are marked "
                "NODISPLY"));
    }

    for (int ii = invisibleImages.size() - 1; ii >= 0; --ii)
    {
        record.removeImageSegment(invisibleImages[ii]);
    }
}
}

int main(int argc, char** argv)
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

        const std::string inputPathname(argv[1]);
        const std::string outputPathname(argv[2]);

        // Check that we have a valid NITF
        if (nitf::Reader::getNITFVersion(inputPathname) == NITF_VER_UNKNOWN)
        {
            std::cout << "Invalid NITF: " << inputPathname << std::endl;
            exit( EXIT_FAILURE);
        }

        nitf::Reader reader;
        nitf::IOHandle io(inputPathname);
        nitf::Record record = reader.read(io);
        stripImages(record);

        nitf::Writer writer;
        nitf::IOHandle output(outputPathname, NITF_ACCESS_WRITEONLY,
                NITF_CREATE);
        writer.prepare(output, record);

        nitf::List images = record.getImages();
        const size_t numImages = record.getNumImages();
        for (size_t ii = 0; ii < numImages; ++ii)
        {
            nitf::ImageSegment segment = images[ii];
            const size_t offset = segment.getImageOffset();
            mem::SharedPtr<nitf::WriteHandler> handler(
                    new nitf::StreamIOWriteHandler(
                        io, offset, segment.getImageEnd() - offset));
            writer.setImageWriteHandler(ii, handler);
        }

        nitf::List graphics = record.getGraphics();
        const size_t numGraphics = record.getNumGraphics();
        for (size_t ii = 0; ii < numGraphics; ++ii)
        {
           nitf::GraphicSegment segment = graphics[ii];
           long offset = segment.getOffset();
           mem::SharedPtr< ::nitf::WriteHandler> handler(
               new nitf::StreamIOWriteHandler (
                   io, offset, segment.getEnd() - offset));
           writer.setGraphicWriteHandler(ii, handler);
        }

        nitf::List texts = record.getTexts();
        const size_t numTexts = record.getNumTexts();
        for (size_t ii = 0; ii < numTexts; ++ii)
        {
           nitf::TextSegment segment = texts[ii];
           const size_t offset = segment.getOffset();
           mem::SharedPtr< ::nitf::WriteHandler> handler(
               new nitf::StreamIOWriteHandler (
                   io, offset, segment.getEnd() - offset));
           writer.setTextWriteHandler(ii, handler);
        }

        nitf::List dataExtensions = record.getDataExtensions();
        const size_t numDEs = record.getNumDataExtensions();
        for (size_t ii = 0; ii < numDEs; ++ii)
        {
           nitf::DESegment segment = dataExtensions[ii];
           const size_t offset = segment.getOffset();
           mem::SharedPtr< ::nitf::WriteHandler> handler(
               new nitf::StreamIOWriteHandler (
                   io, offset, segment.getEnd() - offset));
           writer.setDEWriteHandler(ii, handler);
        }
        writer.write();

        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "An unknown exception occured\n";
    }
    return 1;
}
