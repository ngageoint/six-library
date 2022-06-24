/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "tiff/TiffFileReader.h"

#include <string>
#include <vector>
#include <sstream>
#include <import/io.h>
#include <import/except.h>

#include "tiff/ImageReader.h"
#include "tiff/IFD.h"


void tiff::FileReader::openFile(const std::string& fileName)
{
    if (mInput.isOpen())
        throw except::Exception(Ctxt("Last file not closed; call close() first."));

    if (fileName == "-")
        throw except::Exception(Ctxt("Cannot read TIFF from std::cin"));

    if (fileName == "")
        throw except::Exception(Ctxt("No file name provided"));

    mInput.create(fileName.c_str());
    if (!mInput.isOpen())
        throw except::Exception(Ctxt("File was not opened"));

    // Read TIFF header from input
    mHeader.deserialize(mInput);
    
    mReverseBytes = mHeader.isDifferentByteOrdering();
    sys::Uint32_T offset = mHeader.getIFDOffset();
    while (offset != 0)
    {
        tiff::ImageReader *imageReader = new tiff::ImageReader(&mInput);

        mInput.seek(offset, io::Seekable::START);
        imageReader->process(mReverseBytes);
        mImages.push_back(imageReader);

        offset = imageReader->getNextOffset();
    }
}

void tiff::FileReader::close()
{
    mHeader = tiff::Header{};

    mInput.close();

    std::vector<tiff::ImageReader *>::iterator readIter;
    for (readIter = mImages.begin(); readIter != mImages.end(); ++readIter)
        delete *readIter;

    mImages.clear();
}

tiff::ImageReader *tiff::FileReader::operator[](const sys::Uint32_T index) const
{
    if (index >= mImages.size())
    throw except::Exception(Ctxt(FmtX("Index out of range: %d", index)));

    return mImages[index];
}

void tiff::FileReader::print(io::OutputStream &output) const
{
    // Print the header information
    mHeader.print(output);
    output.writeln("");

    // Print each subfile's information (each subfile has it's own IFD).
    for (sys::Uint32_T i = 0; i < mImages.size(); ++i)
    {
        std::ostringstream message;
        message << "Sub-File:      " << i + 1 << std::endl;
        output.write(message.str());

        mImages[i]->print(output);
        output.writeln("");
    }
}

void tiff::FileReader::getData(unsigned char *buffer,
        const sys::Uint32_T numElementsToRead, const sys::Uint32_T imageIndex)
{
    if (imageIndex >= mImages.size())
        throw except::Exception(Ctxt(FmtX("Index out of range", imageIndex)));

    mImages[imageIndex]->getData(buffer, numElementsToRead);
}


