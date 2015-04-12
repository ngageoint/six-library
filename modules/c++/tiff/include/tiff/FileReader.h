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

#ifndef __TIFF_FILE_READER_H__
#define __TIFF_FILE_READER_H__

#include <string>
#include <vector>
#include <import/io.h>

#include "tiff/Header.h"
#include "tiff/ImageReader.h"

namespace tiff
{

/**
 *********************************************************************
 * @class FileReader
 * @brief Reads a TIFF file.
 *
 * Reads a TIFF file and all images within that file.  Has functions
 * to access a specific image within the file, and to read data from
 * a specific image in the file.
 *********************************************************************/
class FileReader
{
public:

    //! Constructor
    FileReader()
    {
    }

    /**
     *****************************************************************
     * Constructor.  Opens and parses the specified file as a TIFF
     * file.
     *
     * @param fileName
     *   the file to open and parse as a TIFF file
     * @todo
     *   ios::nocreate missing in newer G++
     *****************************************************************/
    FileReader(const std::string& fileName)
    {
        openFile(fileName);
    }

    //! Destructor
    ~FileReader()
    {
        close();
    }

    /**
     *****************************************************************
     * Processes the TIFF file.  Reads the TIFF header, and every
     * IFD the file has, creating a new ImageReader for each.
     *****************************************************************/
    void openFile(const std::string& fileName);

    //! Closes the TIFF file and clears out member data.
    void close();

    /**
     *****************************************************************
     * Retrieves an ImageReader pointer to the specified image.
     *
     * @param index
     *   the numeric index of the image to retrieve
     * @return
     *   an ImageReader pointer to the specified image
     *****************************************************************/
    tiff::ImageReader *operator[](const sys::Uint32_T index) const;

    /**
     *****************************************************************
     * Prints the the file's header, and every image's IFD to the 
     * specified output stream.
     *
     * @param output
     *   the stream to print the file information to
     *****************************************************************/
    void print(io::OutputStream &output) const;

    /**
     *****************************************************************
     * Gets the specified number of elements from the TIFF image and
     * stores them into the specified buffer.  The buffer must be
     * allocated outside because it is not allocated in this function.
     * 
     * @param buffer
     *   the buffer to populate with image data
     * @param numElementsToRead
     *   the number of elements (not bytes) to read from the image
     * @param subSourceIndex
     *   the index of the image within the file to read from
     *****************************************************************/
    void getData(unsigned char *buffer, const sys::Uint32_T numElementsToRead,
            const sys::Uint32_T subSourceIndex = 0);

    /**
     *****************************************************************
     * Returns the number of images in the TIFF file. 
     *
     * @return
     *   the number of images in the TIFF file
     *****************************************************************/
    sys::Uint32_T getImageCount() const
    {
        return mImages.size();
    }

    
private:

    //! The input stream to use to read the TIFF file
    io::FileInputStream mInput;

    //! The TIFF file header
    tiff::Header mHeader;

    //! The images within the TIFF file
    std::vector<tiff::ImageReader *> mImages;

    //! Whether to reverse bytes while reading.
    bool mReverseBytes;
    
};

} // End namespace.

#endif // __TIFF_FILE_READER_H__
