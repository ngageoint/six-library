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

/* *********************************************************************
 ** This test tests the round-trip process of taking an input NITF
 ** file and writing it to a new file. This includes writing the image
 ** segments (headers, extensions, and image data). This is an example
 ** of how users can write the image data to their NITF file
 ** ********************************************************************/

class JunkWriteHandler : public nitf::WriteHandler
{
public:
    JunkWriteHandler(){}
    ~JunkWriteHandler(){}

    void write(nitf::IOHandle handle) throw (nitf::NITFException)
    {
        //yup, does nothing...

        handle.write("01234567890", 11);
    }

};


nitf::Record doRead(const std::string& inFile);

std::string makeBandName(const std::string& rootFile, int imageNum, int bandNum)
{
    int pos = rootFile.find_last_of("/\\");
    std::ostringstream os;
    os << rootFile.substr(pos + 1) << "__" << imageNum << "_band_" << bandNum;
    std::string newFile = os.str();

    while ((pos = newFile.find(".")) != std::string::npos)
        newFile.replace(pos, 1, "_");
    newFile += ".man";
    return newFile;
}

nitf::ImageSource* setupBands(int nbands, int imageNum, const std::string& inRootFile)
{
    nitf::ImageSource* iSource = new nitf::ImageSource();
    for (int i = 0; i < nbands; i++)
    {
        std::string inFile = makeBandName(inRootFile, imageNum, i);
        nitf::IOHandle sourceHandle(inFile);
        iSource->addBand(new nitf::FileSource(sourceHandle, 0, 1, 0));
    }
    return iSource;
}

void doWrite(nitf::Record record, const std::string& inRootFile, const std::string& outFile)
{
    nitf::Writer writer;
    nitf::IOHandle output_io(outFile, NITF_ACCESS_WRITEONLY, NITF_CREATE);
    writer.prepare(output_io, record);

    int numImages = record.getHeader().getNumImages();
    nitf::ListIterator end = record.getImages().end();
    nitf::ListIterator iter = record.getImages().begin();
    nitf_Error error;

    for (int i = 0; i < numImages && iter != end; ++i, ++iter)
    {
        nitf::ImageSegment imseg;
        imseg = *iter;
        int nbands = imseg.getSubheader().getNumImageBands();
        nitf::ImageWriter* iWriter = writer.newImageWriter(i);
        //nitf::WriteHandler *iWriter = new JunkWriteHandler();
        //writer.setImageWriteHandler(i, iWriter);

        nitf::ImageSource* iSource = setupBands(nbands, i, inRootFile);
        iWriter->attachSource(iSource, true);
    }
    writer.write();
    output_io.close();
}

int main(int argc, char **argv)
{
    try
    {
        /*  Check argv and make sure we are happy  */
        if (argc != 3)
        {
            std::cout << "Usage: %s <input-file> <output-file> \n" << argv[0] << std::endl;
            exit(EXIT_FAILURE);
        }
        nitf::Record record = doRead(argv[1]);
        doWrite(record, argv[1], argv[2]);
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

    nitf::Uint32 nBits = segment.getSubheader().getNumBitsPerPixel();
    nitf::Uint32 nBands = segment.getSubheader().getNumImageBands();
    nitf::Uint32 xBands = segment.getSubheader().getNumMultispectralImageBands();
    nBands += xBands;

    nitf::Uint32 nRows = segment.getSubheader().getNumRows();
    nitf::Uint32 nColumns = segment.getSubheader().getNumCols();
    //nitf::Uint32 nBlocksPerCol = segment.getSubheader().getNumBlocksPerCol();
    //nitf::Uint32 nPixPerVertBlock = segment.getSubheader().getNumPixelsPerVertBlock();


    //one row at a time
    size_t subWindowSize = nColumns * NITF_NBPP_TO_BYTES(nBits);

    std::cout << "NBANDS -> " << nBands << std::endl
        << "XBANDS -> " << xBands << std::endl
        << "NROWS -> " << nRows << std::endl
        << "NCOLS -> " << nColumns << std::endl
        << "PVTYPE -> " << segment.getSubheader().getPixelValueType().toString() << std::endl
        << "NBPP -> " << segment.getSubheader().getNumBitsPerPixel().toString() << std::endl
        << "ABPP -> " << segment.getSubheader().getActualBitsPerPixel().toString() << std::endl
        << "PJUST -> " << segment.getSubheader().getPixelJustification().toString() << std::endl
        << "IMODE -> " << segment.getSubheader().getImageMode().toString() << std::endl
        << "NBPR -> " << segment.getSubheader().getNumBlocksPerRow().toString() << std::endl
        << "NBPC -> " << segment.getSubheader().getNumBlocksPerCol().toString() << std::endl
        << "NPPBH -> " << (int)segment.getSubheader().getNumPixelsPerHorizBlock() << std::endl
        << "NPPBV -> " << (int)segment.getSubheader().getNumPixelsPerVertBlock() << std::endl
        << "IC -> " << segment.getSubheader().getImageCompression().toString() << std::endl
        << "COMRAT -> " << segment.getSubheader().getCompressionRate().toString() << std::endl;

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
    for (int i = 0; i < nBands; i++)
    {
        buffer[i] = new nitf::Uint8[subWindowSize];
        assert(buffer[i]);
    }

    std::vector<nitf::IOHandle> handles;
    //make the files
    for (int i = 0; i < nBands; i++)
    {
        std::string name = makeBandName(imageName, imageNumber, i);
        nitf::IOHandle toFile(name, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        handles.push_back(toFile);
    }

    //nitf::Uint32 lastBlockRows = nPixPerVertBlock - (nBlocksPerCol * nPixPerVertBlock - nRows);
    //size_t lastBlockSize = nColumns * NITF_NBPP_TO_BYTES(nBits) * lastBlockRows;

    //read all row blocks and write to disk
    for (int i = 0; i < nRows; ++i)
    {
        subWindow.setStartRow(i);

        //if (i == nBlocksPerCol - 1)
        //    subWindow.setNumRows(lastBlockRows);

        deserializer.read(subWindow, buffer, &padded);
        for (int j = 0; j < nBands; j++)
        {
            //if (i == nBlocksPerCol - 1)
            //    handles[j].write((const char*)buffer[j], lastBlockSize);
            //else
            handles[j].write((const char*)buffer[j], subWindowSize);
        }
    }

    //close output handles
    for (int i = 0; i < nBands; i++)
        handles[i].close();

    /* free buffers */
    for (int i = 0; i < nBands; i++)
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
    nitf::ListIterator end = record.getImages().end();
    nitf::ListIterator iter = record.getImages().begin();
    for (int count = 0, numImages = record.getHeader().getNumImages();
            count < numImages && iter != end; ++count, ++iter)
    {
        nitf::ImageSegment imageSegment = *iter;
        nitf::ImageReader deserializer = reader.newImageReader(count);
        std::cout << "Writing image " << count << "..." << std::endl;

        /*  Write the thing out  */
        manuallyWriteImageBands(imageSegment, inFile, deserializer, count);
        std::cout << "done.\n" << std::endl;
    }

    return record;
}
