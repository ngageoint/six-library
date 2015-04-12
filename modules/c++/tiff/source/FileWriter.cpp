/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include <string>
#include <import/except.h>
#include "tiff/ImageWriter.h"
#include "tiff/FileWriter.h"


void tiff::FileWriter::openFile(const std::string& fileName)
{
    if (mOutput.isOpen())
        throw except::Exception(Ctxt("Last file not closed; call close() first."));

    if (fileName == "-")
        throw except::Exception(Ctxt("Cannot write TIFF to std::cout"));

    if (fileName == "")
        throw except::Exception(Ctxt("No filename provided"));

    mOutput.create(fileName.c_str());
}

void tiff::FileWriter::close()
{
    mIFDOffset = 0;
    memset(&mHeader, 0, sizeof(mHeader));

    mOutput.close();

    mImages.clear();
}

tiff::ImageWriter * tiff::FileWriter::operator[](const sys::Uint32_T index) const
{
    if (index >= mImages.size())
    throw except::Exception(Ctxt("Invalid sub-image index"));

    return mImages[index];
}

void tiff::FileWriter::putData(unsigned char *buffer,
        const sys::Uint32_T numElementsToWrite, const sys::Uint32_T index)
{
    if (index >= mImages.size())
        throw except::Exception(Ctxt("Invalid sub-image index"));

    mImages[index]->putData(buffer, numElementsToWrite);
}

tiff::ImageWriter *tiff::FileWriter::addImage()
{
    if (mImages.size() != 0)
        mIFDOffset = mImages[mImages.size() - 1]->getNextIFDOffset();

    tiff::ImageWriter *image = new tiff::ImageWriter(&mOutput, mIFDOffset);
    mImages.push_back(image);
    return image;
}

void tiff::FileWriter::writeHeader()
{
    mHeader.serialize(mOutput);

    // Have to rewind a few bytes to write out the actual IFD offset.
    mIFDOffset = mOutput.tell();
    mIFDOffset -= (int)sizeof(sys::Uint32_T);
}
