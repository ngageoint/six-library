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
#include <import/except.h>

void writeImage(nitf::ImageSegment &segment,
                nitf::Reader &reader,
                const int imageNumber,
                const char *imageName,
                nitf_Uint32 rowSkipFactor,
                nitf_Uint32 columnSkipFactor, bool optz)
{
    size_t subWindowSize;
    nitf::SubWindow subWindow;
    unsigned int i;
    int padded;
    nitf::Uint8** buffer = NULL;
    nitf::Uint32 band;
    nitf::Uint32 * bandList;

    nitf::ImageReader deserializer = reader.newImageReader(imageNumber);

    // missing skip factor
    nitf::ImageSubheader subheader = segment.getSubheader();

    nitf::Uint32 nBits   = subheader.getNumBitsPerPixel();

    nitf::Uint32 nBands  = subheader.getNumImageBands();
    nitf::Uint32 xBands  = subheader.getNumMultispectralImageBands();
    nBands += xBands;


    nitf::Uint32 nRows   = subheader.getNumRows();
    nitf::Uint32 nCols   = subheader.getNumCols();
    subWindowSize = (size_t)(nRows / rowSkipFactor) *
        (size_t)(nCols / columnSkipFactor) *
        (size_t)NITF_NBPP_TO_BYTES(nBits);

    if (optz)
    {
        std::string irep =
                segment.getSubheader().getImageRepresentation().toString();
        std::string ic =
                segment.getSubheader().getImageCompression().toString();

        if (nBands == 3 && segment.getSubheader().getImageMode().toString()
                == "P" && str::startsWith(irep, "RGB")
                && NITF_NBPP_TO_BYTES(nBits) == 1 && str::startsWith(ic, "N"))
        {
            subWindowSize *= nBands;
            nBands = 1;
            std::cout << "Using accelerated 3-band RGB mode pix-interleaved image" << std::endl;
        }
        if (nBands == 2 && segment.getSubheader().getImageMode().toString()
                == "P" && str::startsWith(ic, "N"))
        {

            subWindowSize *= nBands;
            nBands = 1;
            std::cout << "Using accelerated 2-band IQ mode pix-interleaved image" << std::endl;
        }
    }


    std::cout << "NBANDS -> " << nBands << std::endl;
    std::cout << "XBANDS -> " << xBands << std::endl;
    std::cout << "NROWS -> "  << nRows  << std::endl;
    std::cout << "NCOLS -> "  << nCols  << std::endl;
    std::cout << "PVTYPE -> " << subheader.getPixelValueType().toString() << std::endl;
    std::cout << "NBPP -> " << subheader.getNumBitsPerPixel() .toString() << std::endl;
    std::cout << "ABPP -> " << subheader.getActualBitsPerPixel().toString() << std::endl;
    std::cout << "PJUST -> " << subheader.getPixelJustification().toString() << std::endl;
    std::cout << "IMODE -> " << subheader.getImageMode().toString() << std::endl;
    std::cout << "NBPR -> " << subheader.getNumBlocksPerRow().toString() << std::endl;
    std::cout << "NBPC -> " << subheader.getNumBlocksPerCol().toString() << std::endl;
    std::cout << "NPPBH -> " << (int)subheader.getNumPixelsPerHorizBlock() << std::endl;
    std::cout << "NPPBV -> " << (int)subheader.getNumPixelsPerVertBlock() << std::endl;
    std::cout << "IC -> " << subheader.getImageCompression().toString() << std::endl;
    std::cout << "COMRAT -> " << subheader.getCompressionRate().toString() << std::endl;

    std::cout << "Allocating work buffer..." << std::endl;

    buffer = new nitf::Uint8*[nBands];
    assert(buffer);

    band = 0;
    bandList = new nitf::Uint32[nBands];

    subWindow.setStartCol(0);
    subWindow.setStartRow(0);

    subWindow.setNumRows(nRows / rowSkipFactor);
    subWindow.setNumCols(nCols / columnSkipFactor);

    nitf::PixelSkip pixelSkip(rowSkipFactor, columnSkipFactor);

    subWindow.setDownSampler(&pixelSkip);

    for (band = 0; band < nBands; band++)
    {
        bandList[band] = band;
        buffer[band] = new nitf::Uint8[subWindowSize];
        assert(buffer[band]);
    }
    subWindow.setBandList(bandList);
    subWindow.setNumBands(nBands);

    std::cout << "Reading image..." << std::endl;
    deserializer.read(subWindow, buffer, &padded);

    std::cout << "Call completed!" << std::endl;

    std::cout << "Writing bands..." << std::endl;
    for (i = 0; i < nBands; i++)
    {
        std::cout << "Writing band # " << i << std::endl;
        std::string base = sys::Path::basename(imageName);

        size_t where = 0;
        while ((where = base.find(".")) != (size_t)std::string::npos)
            base.replace(where, 1, "_");

        std::ostringstream file;
        file << base << "__" << imageNumber << "__"
             << nRows / rowSkipFactor << '_'
             << nCols / columnSkipFactor << '_'
             << nBits << "_band_" << i << ".out";

        nitf::IOHandle toFile(file.str(), NITF_ACCESS_WRITEONLY, NITF_CREATE);
        toFile.write((const char *) buffer[i], subWindowSize);
        toFile.close();
        std::cout << "Finished # " << i << std::endl;
    }

    for (band = 0; band < nBands; band++)
        delete [] buffer[band];
    delete [] buffer;
    delete [] bandList;
}


int main(int argc, char **argv)
{
    /* Skip factors */
    nitf_Uint32 rowSkipFactor = 1;
    nitf_Uint32 columnSkipFactor = 1;

    try
    {
        /*  If you didnt give us a nitf file, we're croaking  */
        if (argc < 2)
        {
            std::cout << "Usage: " << argv[0] << " <nitf-file> [-o]"
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        bool optz = (argc > 2 && std::string(argv[2]) == "-o");

        /*  This is the reader  */
        nitf::Reader reader;

        /*  This is the io handle we will give the reader to parse  */
        nitf::IOHandle io(argv[1]);

        /*  Read the file (first pass) */
        nitf::Record record = reader.read(io);
        /*  These iterators are for going through the image segments  */
        nitf::ListIterator iter;
        nitf::ListIterator end;

        /*  And set this one to the end, so we'll know when we're done!  */
        iter = record.getImages().begin();
        end = record.getImages().end();
        std::cout << "Iterating list of images..." << std::endl;
        /*  While we are not done...  */
        for (int count = 0; iter != end; ++iter, ++count)
        {
            std::cout << "Getting image segment..." << std::endl;
            nitf::ImageSegment imageSegment((nitf_ImageSegment *) iter.get());

            std::cout << "Retrieved." << std::endl;
            std::cout << "Writing image... " << std::endl;

            /*  Write the thing out  */
            writeImage(imageSegment, reader, count, argv[1],
                       rowSkipFactor, columnSkipFactor, optz);
            std::cout << "done." << std::endl;
        }
    }
    catch (except::Exception &thr)
    {
        std::cout << "Caught module exception: " << thr.toString() << std::endl;
    }
    catch (std::exception & ex)
    {
        std::cout << "Caught C++ exception: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Caught unknown exception" << std::endl;
    }

    return 0;
}
