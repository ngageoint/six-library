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

#ifndef __TIFF_IMAGE_READER_H__
#define __TIFF_IMAGE_READER_H__

#include <import/io.h>

#include "tiff/IFDEntry.h"
#include "tiff/IFD.h"

namespace tiff
{

/**
 *********************************************************************
 * @class ImageReader
 * @brief Reads a TIFF image
 *
 * Reads a TIFF image and parses out the IFD.  Contains functions for
 * getting data from the image and retrieving the TIFF IFD.
 *********************************************************************/
class ImageReader
{
public:
    /**
     *****************************************************************
     * Constructor.  Sets the image reader to read from the specified
     * file stream.
     *
     * @param input
     *   the stream to read the TIFF image from
     *****************************************************************/
    ImageReader(io::FileInputStream *input) :
        mIFD(), mStripByteCounts(nullptr), mStripOffsets(nullptr), mInput(input),
                mNextOffset(0), mBytePosition(0), mStripIndex(0),
                mElementSize(0), mReverseBytes(false)
    {
    }

    //! Destructor
    ~ImageReader()
    {
    }

    /**
     *****************************************************************
     * Processes the image from the file.  Reads the image's IFD
     * and stores it for later use.
     *****************************************************************/
    void process(const bool reverseBytes = false);

    /**
     *****************************************************************
     * Prints the image's IFD to the specified output stream.
     *
     * @param output
     *   the stream to print the IFD to
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
     *****************************************************************/
    void getData(unsigned char *buffer, const sys::Uint32_T numElementsToRead);

    /**
     *****************************************************************
     * Returns a pointer to the IFD for this image.
     *
     * @return
     *   a pointer to the IFD for this image
     *****************************************************************/
    tiff::IFD* getIFD()
    {
        return &mIFD;
    }

    /**
     *****************************************************************
     * Writes out any GeoTIFF information found in the IFD to the
     * specified file.
     *
     * @param fileName
     *   the file to write the GeoTIFF information to
     *****************************************************************/
    void writeGeoTIFFInfo(const std::string &fileName);

    /**
     *****************************************************************
     * Returns the offset to the next IFD.  Used to determine if 
     * there is another image in the file, and if so where is the
     * file position of the next image's IFD.
     *
     * @return
     *   the next IFD offset
     *****************************************************************/
    sys::Uint32_T getNextOffset() const
    {
        return mNextOffset;
    }

private:

    /**
     *****************************************************************
     * Reads the specified number of elements into the specified 
     * buffer, in TIFF stripped format.  Converts the TIFF image from
     * the TIFF stripped format into raster format.
     * @param buffer
     *   the buffer to populate with image data
     * @param numElementsToRead
     *   the number of elements (not bytes) to read from the image
     *****************************************************************/
    void getStripData(unsigned char *buffer, sys::Uint32_T numElementsToRead);

    /**
     *****************************************************************
     * Reads the specified number of elements into the specified 
     * buffer, in TIFF tiled format.  Converts the TIFF image from
     * the TIFF tiled format into raster format.
     * @param buffer
     *   the buffer to populate with image data
     * @param numElementsToRead
     *   the number of elements (not bytes) to read from the image
     *****************************************************************/
    void getTileData(unsigned char *buffer, sys::Uint32_T numElementsToRead);

    //! Contains the IFD for this image.
    tiff::IFD mIFD;

    //! A pointer to the "StripByteCounts" IFDEntry to make reading faster.
    tiff::IFDEntry *mStripByteCounts;

    //! A pointer to the "StripOffsets" IFDEntry to make reading faster.
    tiff::IFDEntry *mStripOffsets;

    //! Points to the input file stream.
    io::FileInputStream *mInput;

    //! The offset to the next IFD.
    sys::Uint32_T mNextOffset;

    //! Used to keep track of the current read position in the file.
    sys::Uint32_T mBytePosition;
    
    sys::Uint32_T mStripIndex;

    //! The element size of the image.
    unsigned short mElementSize;

    //! Whether to reverse bytes when reading.
    bool mReverseBytes;
};

} // End namespace.

#endif // __TIFF_IMAGE_READER_H__
