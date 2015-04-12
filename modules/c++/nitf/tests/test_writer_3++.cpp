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

/*
 * This test tests the round-trip process of taking an input NITF
 * file and writing it to a new file. This includes writing the image
 * segments (headers, extensions, and image data). This is an example
 * of how users can write the image data to their NITF file
 */

nitf::Record doRead(const std::string& inFile);

std::string makeBandName(const std::string& rootFile, int imageNum, int bandNum)
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

nitf::ImageSource setupBands(int nbands, int imageNum, const std::string& inRootFile)
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

void doWrite(nitf::Record record, const std::string& inRootFile, const std::string& outFile)
{
    nitf::Writer writer;
    nitf::IOHandle output(outFile, NITF_ACCESS_WRITEONLY, NITF_CREATE);
    writer.prepare(output, record);

    int numImages = record.getHeader().getNumImages();
    nitf::ListIterator end = record.getImages().end();
    nitf::ListIterator iter = record.getImages().begin();

    for (int i = 0; i < numImages && iter != end; ++i, ++iter)
    {
        nitf::ImageSegment imseg;
        imseg = *iter;
        int nbands = imseg.getSubheader().getNumImageBands();
        nitf::ImageWriter iWriter = writer.newImageWriter(i);
        nitf::ImageSource iSource = setupBands(nbands, i, inRootFile);
        iWriter.attachSource(iSource);
    }
    writer.write();
    output.close();
}

int main(int argc, char **argv)
{
    try
    {
        //  Check argv and make sure we are happy
        if (argc != 3)
        {
            std::cout << "Usage: " << argv[0]
                      << " <input-file> <output-file>\n\n";
            exit(EXIT_FAILURE);
        }

        // Check that wew have a valid NITF
        if (nitf::Reader::getNITFVersion(argv[1]) == NITF_VER_UNKNOWN )
        {
            std::cout << "Invalid NITF: " << argv[1] << std::endl;
            exit(EXIT_FAILURE);
        }

        nitf::Record record = doRead(argv[1]);
        doWrite(record, argv[1], argv[2]);
        return 0;
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getMessage() << std::endl;
    }
}


void manuallyWriteImageBands(nitf::ImageSegment & segment,
                             const std::string& imageName,
                             nitf::ImageReader& deserializer,
                             int imageNumber)
{
    int padded;

    nitf::ImageSubheader subheader = segment.getSubheader();


    nitf::Uint32 nBits = subheader.getNumBitsPerPixel();
    nitf::Uint32 nBands = subheader.getNumImageBands();
    nitf::Uint32 xBands = subheader.getNumMultispectralImageBands();
    nBands += xBands;

    nitf::Uint32 nRows = subheader.getNumRows();
    nitf::Uint32 nColumns = subheader.getNumCols();

    //one row at a time
    size_t subWindowSize = nColumns * NITF_NBPP_TO_BYTES(nBits);

    std::cout << "NBANDS -> " << nBands << std::endl
        << "XBANDS -> " << xBands << std::endl
        << "NROWS -> " << nRows << std::endl
        << "NCOLS -> " << nColumns << std::endl
        << "PVTYPE -> " << subheader.getPixelValueType().toString() << std::endl
        << "NBPP -> " << subheader.getNumBitsPerPixel().toString() << std::endl
        << "ABPP -> " << subheader.getActualBitsPerPixel().toString() << std::endl
        << "PJUST -> " << subheader.getPixelJustification().toString() << std::endl
        << "IMODE -> " << subheader.getImageMode().toString() << std::endl
        << "NBPR -> " << subheader.getNumBlocksPerRow().toString() << std::endl
        << "NBPC -> " << subheader.getNumBlocksPerCol().toString() << std::endl
        << "NPPBH -> " << (int)subheader.getNumPixelsPerHorizBlock() << std::endl
        << "NPPBV -> " << (int)subheader.getNumPixelsPerVertBlock() << std::endl
        << "IC -> " << subheader.getImageCompression().toString() << std::endl
        << "COMRAT -> " << subheader.getCompressionRate().toString() << std::endl;

    nitf::Uint8** buffer = new nitf::Uint8*[nBands];
    nitf::Uint32* bandList = new nitf::Uint32[nBands];

    for (nitf::Uint32 band = 0; band < nBands; band++)
        bandList[band] = band;

    nitf::SubWindow subWindow;
    subWindow.setStartCol(0);
    subWindow.setNumRows(1);
    subWindow.setNumCols(nColumns);

    // necessary ?
    nitf::DownSampler* pixelSkip = new nitf::PixelSkip(1, 1);
    subWindow.setDownSampler(pixelSkip);
    subWindow.setBandList(bandList);
    subWindow.setNumBands(nBands);

    assert(buffer);
    for (nitf::Uint32 i = 0; i < nBands; i++)
    {
        buffer[i] = new nitf::Uint8[subWindowSize];
        assert(buffer[i]);
    }

    std::vector<nitf::IOHandle> handles;
    //make the files
    for (nitf::Uint32 i = 0; i < nBands; i++)
    {
        std::string name = makeBandName(imageName, imageNumber, i);
        nitf::IOHandle toFile(name, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        handles.push_back(toFile);
    }

    //read all row blocks and write to disk
    for (nitf::Uint32 i = 0; i < nRows; ++i)
    {
        subWindow.setStartRow(i);
        deserializer.read(subWindow, buffer, &padded);
        for (nitf::Uint32 j = 0; j < nBands; j++)
        {
            handles[j].write((const char*)buffer[j], subWindowSize);
        }
    }

    //close output handles
    for (nitf::Uint32 i = 0; i < nBands; i++)
        handles[i].close();

    /* free buffers */
    for (nitf::Uint32 i = 0; i < nBands; i++)
        delete [] buffer[i];
    delete [] buffer;
    delete [] bandList;
    delete pixelSkip;
}


nitf::Record doRead(const std::string& inFile)
{
    nitf::Reader reader;
    nitf::IOHandle io(inFile);
    nitf::Record record = reader.read(io);

    /*  Set this to the end, so we'll know when we're done!  */
    nitf::ListIterator iter = record.getImages().begin();


    nitf::Uint32 num = record.getNumImages();

    for (nitf::Uint32 i = 0; i < num; i++)
    {
        nitf::ImageSegment imageSegment = *iter;
        iter++;
        nitf::ImageReader deserializer = reader.newImageReader(i);
        std::cout << "Writing image " << i << "..." << std::endl;

        /*  Write the thing out  */
        manuallyWriteImageBands(imageSegment, inFile, deserializer, i);
        std::cout << "done.\n" << std::endl;
    }


    return record;
}
