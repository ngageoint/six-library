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

#ifndef __TIFF_FILE_WRITER_H__
#define __TIFF_FILE_WRITER_H__

#include <string>
#include <vector>
#include <import/io.h>
#include "tiff/Header.h"
#include "tiff/ImageWriter.h"

namespace tiff
{

/**
 *********************************************************************
 * @class FileWriter
 * @brief Writes a TIFF file.
 *
 * Writes a TIFF file to an output stream.  Can write multiple images
 * to the same file.  Contains function for manipulating each 
 * sub-image and for writing data.
 *********************************************************************/
class FileWriter
{
public:

    //! Constructor
    FileWriter() :
        mIFDOffset(0)
    {
    }

    /**
     *****************************************************************
     * Constructor.  Opens the specified file name for writing.
     *
     * @param fileName
     *   the file to open for writing
     *****************************************************************/
    FileWriter(const std::string& fileName) :
        mIFDOffset(0)
    {
        openFile(fileName);
    }

    //! Destructor
    ~FileWriter()
    {
        close();
    }

    void openFile(const std::string& fileName);
    void close();

    /**
     *****************************************************************
     * Retrieves a ImageWriter pointer to the specified image.
     *
     * @param index
     *   the numeric index of the image to retrieve
     * @return
     *   a TIFFImageWriter pointer to the specified image
     *****************************************************************/
    tiff::ImageWriter *operator[](const sys::Uint32_T index) const;

    /**
     *****************************************************************
     * Writes data to the output file stream.  The input buffer
     * must be in raster format.  Converts raster data to either
     * tiled or stripped format.
     *
     * @param buffer
     *   the buffer to write to the output stream
     * @param numElementsToWrite
     *   the number of elements (not bytes) to write
     * @param subImageIndex
     *   the index to a sub-image to write
     *****************************************************************/
    void putData(unsigned char *buffer, const sys::Uint32_T numElementsToWrite,
            const sys::Uint32_T subImageIndex = 0);

    /**
     *****************************************************************
     * Creates and adds an image to the TIFF file.  Sets necessary
     * defaults within the image and returns a pointer to that image
     * so that the user can manipulate its IFD.
     *
     * @return
     *   A pointer to a TIFF image.
     *****************************************************************/
    tiff::ImageWriter *addImage();

    /**
     *****************************************************************
     * Writes the TIFF header to the file.  There is only one header
     * in a TIFF file regardless of how many images are in it.
     *****************************************************************/
    void writeHeader();


private:

    //! The position to write the offset to the first IFD to
    sys::Uint32_T mIFDOffset;

    //! The output stream
    io::FileOutputStream mOutput;

    //! The TIFF header
    tiff::Header mHeader;

    //! The images to write
    std::vector<tiff::ImageWriter *> mImages;

};

} // End namespace.

#endif // __TIFF_FILE_WRITER_H__
