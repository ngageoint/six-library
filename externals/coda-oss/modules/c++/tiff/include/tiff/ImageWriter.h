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

#ifndef __TIFF_IMAGE_WRITER_H__
#define __TIFF_IMAGE_WRITER_H__

#include <import/io.h>

#include "tiff/Common.h"
#include "tiff/IFDEntry.h"
#include "tiff/IFD.h"

namespace tiff
{

/**
 *********************************************************************
 * @class ImageWriter
 * @brief Writes a TIFF image
 *
 * Writes a TIFF image to a stream.  Contains functions for writing
 * the image's IFD, and for putting data to a stream.
 *********************************************************************/
class ImageWriter
{
public:
    //! The ideal tile size if a tiled file.
    static const unsigned short CHUNK_SIZE;

    //! The format of the image, either STRIPPED or TILED.
    enum ImageFormat  { STRIPPED, TILED };

    /**
     *****************************************************************
     * Constructor.  Initializes the object with a pointer to the
     * output stream, and a file offset that indicates where the 
     * beginning of the image is in the file (since TIFF can have
     * more than one image in a file).
     *
     * @param output
     *   the output stream to write the image to
     * @param ifdOffset
     *   the offset to the beginning of the IFD for this image
     *****************************************************************/
    ImageWriter(io::FileOutputStream *output, const sys::Uint32_T ifdOffset) :
        mStripByteCounts(nullptr),
                mTileOffsets(nullptr),
                mOutput(output), mIFDOffset(ifdOffset),
                mIdealChunkSize(CHUNK_SIZE), mBytePosition(0), mElementSize(0),
                mValidated(false), mFormat(STRIPPED)
    {
    }

    //! Destructor
    ~ImageWriter()
    {
    }

    /**
     *****************************************************************
     * Writes data from the specified buffer into the stream.  The
     * data in the buffer must be in raster format.
     *
     * @param buffer
     *   the data to write to the output stream
     * @param numElementsToWrite
     *   the number of elements (not bytes) to write to the stream
     *****************************************************************/
    void putData(const unsigned char *buffer,
                 sys::Uint32_T numElementsToWrite);

    /**
     *****************************************************************
     * Returns a pointer to this image's IFD.  Allows the user to set
     * information in this image's IFD externally.
     *
     * @return
     *   a pointer to this image's IFD
     *****************************************************************/
    tiff::IFD *getIFD()
    {
        return &mIFD;
    }

    //! Writes this image's IFD to the output stream.
    void writeIFD();

    /**
     *****************************************************************
     * Returns the position to write the next IFD offset to.  When 
     * the IFD for this image is written out, the next IFD offset is
     * default to 0 (indicating there is no next image in the file).
     * If there is to be more than one image in the file, you have to
     * have a way of knowing where to write the offset to the next
     * image's IFD.  This returns the position where the next IFD
     * offset must be written.
     *
     * @return
     *   the position to write the next IFD offset to
     *****************************************************************/
    sys::Uint32_T getNextIFDOffset() const
    {
        return mIFDOffset;
    }

    /**
     *****************************************************************
     * Sets the ideal tile size.  Used if the image is a tiled image.
     * Indicates an ideal size to allocate for one tile.  The actual
     * tile size may not be exactly this number because there are
     * certain rules that must be followed for tile sizes, but it will
     * get as close to this number as it possibly can.
     *
     * @param size
     *   the ideal tile size to allocate if this is a tiled image
     *****************************************************************/
    void setIdealChunkSize(const sys::Uint32_T size)
    {
        mIdealChunkSize = size;
    }

    /**
     *****************************************************************
     * Sets the image format to either TILED or STRIPPED.  The 
     * default is STRIPPED.
     *
     * @param format
     *   the format to use for this image, either TILED or STRIPPED
     *****************************************************************/
    void setImageFormat(const ImageFormat format)
    {
        mFormat = format;
    }

    /**
     *****************************************************************
     * Retrieves the current image format for the image.
     *
     * @return
     *   the image format of this image, either TILED or STRIPPED
     *****************************************************************/
    ImageFormat getImageFormat()
    {
        return mFormat;
    }


    /**
     *****************************************************************
     * Makes sure that all necessary TIFF tags (IFD entries) are
     * defined in the IFD before allowing the image to be written.
     * For some tags, there are reasonable defaults that this 
     * function will set, others must be set by the user and this 
     * function will throw an exception indicating the missing tag.
     *****************************************************************/
    void validate();

private:
    /**
     *****************************************************************
     * Adds IFD entries to the IFD that indicate that the image 
     * should be written in a stripped format.
     *****************************************************************/
    void initStrips();

    /**
     *****************************************************************
     * Adds IFD entries to the IFD that indicate that the image 
     * should be written in a tiled format.
     *****************************************************************/
    void initTiles();

    /**
     *****************************************************************
     * Writes data to a file in stripped format.
     *
     * @param buffer
     *   the buffer to write to the file
     * @param numElementsToWrite
     *   the number of elements (not bytes) to write to the file
     *****************************************************************/
    void putStripData(const unsigned char *buffer,
                      sys::Uint32_T numElementsToWrite);

    /**
     *****************************************************************
     * Writes data to a file in tiled format.
     *
     * @param buffer
     *   the buffer to write to the file
     * @param numElementsToWrite
     *   the number of elements (not bytes) to write to the file
     *****************************************************************/
    void putTileData(const unsigned char *buffer,
                     sys::Uint32_T numElementsToWrite);

    //! The TIFF IFD for this image
    tiff::IFD mIFD;

    //! A pointer to the StripByteCounts entry, prevents frequent IFD access
    tiff::IFDEntry *mStripByteCounts;

    //! A pointer to the TileOffsets entry, prevents frequent IFD access
    tiff::IFDEntry *mTileOffsets;

    //! A pointer to the TileWidth entry, prevents frequent IFD access
    tiff::IFDEntry *mTileWidth;

    //! A pointer to the TileLength entry, prevents frequent IFD access
    tiff::IFDEntry *mTileLength;

    //! A pointer to the TileByteCounts entry, prevents frequent IFD access
    tiff::IFDEntry *mTileByteCounts;

    //! A pointer to the output stream
    io::FileOutputStream *mOutput;

    //! The position to write the next IFD to
    sys::Uint32_T mIFDOffset;

    //! The ideal size of a tile
    sys::Uint32_T mIdealChunkSize;

    //! Used to determine the position in the image
    sys::Uint32_T mBytePosition;

    //! The image's element size.  Stored here to prevent frequent IFD access
    unsigned short mElementSize;

    //! Indicates whether or not the IFD has been validated already
    bool mValidated;

    //! The format of the file, either TILED or STRIPPED
    ImageFormat mFormat;
};

} // End namespace.

#endif // __TIFF_IMAGE_WRITER_H__
