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
struct FileWriter
{
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
    ~FileWriter();

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
    void putData(const unsigned char *buffer,
                 sys::Uint32_T numElementsToWrite,
                 sys::Uint32_T subImageIndex = 0);

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

    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;

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

/** Enumeration for automatic detection of image type */
enum { AUTO = -1 };

/*!
 *  This function is designed to mimic (roughly) the API for sio::lite::writeSIO().
 *  It attempts to automatically guess the correct TIFF type for an input image,
 *  and write as a TIFF file.
 *
 *  Unlike the writeSIO function, this function does not support complex pixels,
 *  and will end up writing them as IEEE_FLOAT, which is probably not the desired
 *  behavior, so care is required.  The supported types are double, float, RGB
 *  3-byte unsigned, and single unsigned byte (mono).
 *
 *  Most TIFF viewers will not support float files, and therefore, it may be more
 *  desirable to remap float data to bytes prior to calling this routine.
 *
 *  \param image The data to write to TIFF
 *  \param rows The number of rows in image
 *  \param cols The number of cols in image
 *  \param imageFile The name of the file
 *  \param et The element type, which can be any tiff::Const::SampleFormatType, or
 *         defaults to automatically guessing based on the input data size
 *  \param es The element size, which can be any size, but defaults to automatically
 *         guessing based on the input data size
 *
 */
template<typename T> void writeTIFF(const T* image, size_t rows, size_t cols,
                                    std::string imageFile, unsigned short et = AUTO, int es = AUTO)
{

    if (es == AUTO)
        es = sizeof(T);

    unsigned int photoInterp(1);
    unsigned short numBands(1);

    if (et == static_cast<unsigned short>(AUTO))
    {
        switch (es)
        {
        case 8:
            et = ::tiff::Const::SampleFormatType::IEEE_FLOAT;
            numBands = 2;
            break;

        case 4:
            et = ::tiff::Const::SampleFormatType::IEEE_FLOAT;
            break;

        case 3:
            et = ::tiff::Const::SampleFormatType::UNSIGNED_INT;
            photoInterp = (unsigned short) ::tiff::Const::PhotoInterpType::RGB;
            numBands = 3;
            break;

        case 1:
            et = ::tiff::Const::SampleFormatType::UNSIGNED_INT;
            break;
        default:
            throw except::Exception(Ctxt(FmtX("Unexpected es: %d", es)));
        }
    }
    unsigned short alpha(0);
    if (es == 4 && et == ::tiff::Const::SampleFormatType::UNSIGNED_INT)
    {
        photoInterp = (unsigned short) ::tiff::Const::PhotoInterpType::RGB;
        numBands = 4;
        // This is "unassociated alpha value"
        alpha = 2;

    }
    

    ::tiff::FileWriter fileWriter(imageFile);

    //write the header first
    fileWriter.writeHeader();

    
    ::tiff::ImageWriter* imageWriter = fileWriter.addImage();
    ::tiff::IFD* ifd = imageWriter->getIFD();

    ifd->addEntry(::tiff::KnownTags::IMAGE_WIDTH, cols);
    ifd->addEntry(::tiff::KnownTags::IMAGE_LENGTH, rows);
       
    ifd->addEntry(::tiff::KnownTags::COMPRESSION,
                  (unsigned short) ::tiff::Const::CompressionType::NO_COMPRESSION);

    
    ifd->addEntry(::tiff::KnownTags::PHOTOMETRIC_INTERPRETATION, photoInterp);
    // Added this for RGBA, because otherwise the ImageWriter::validate() changes all of
    // these fields back assuming 3 bytes per pixel
    ifd->addEntry(::tiff::KnownTags::SAMPLES_PER_PIXEL, numBands);
    ifd->addEntry(::tiff::KnownTags::BITS_PER_SAMPLE);
    ifd->addEntry(::tiff::KnownTags::SAMPLE_FORMAT);
    ::tiff::IFDEntry* bps = (*ifd)[::tiff::KnownTags::BITS_PER_SAMPLE];
    ::tiff::IFDEntry* sf = (*ifd)[::tiff::KnownTags::SAMPLE_FORMAT];
    
    unsigned short bitsPerBand = (es << 3) / numBands;

    //set some fields that have 'numSamples' values
    for (int band = 0; band < numBands; ++band)
    {
        bps->addValue(::tiff::TypeFactory::create((unsigned char *) &bitsPerBand,
                                                  ::tiff::Const::Type::SHORT));
        sf->addValue(::tiff::TypeFactory::create((unsigned char *) &et,
                                                 ::tiff::Const::Type::SHORT));
    }

    // If the alpha channel is on (note 0 is a valid value for ExtraSamples,
    // but Im using it to be boolean (0 = off) and the unassociated value here
    if (alpha)
        ifd->addEntry(std::string("ExtraSamples"), alpha);

    imageWriter->putData((unsigned char*) image, rows * cols);

    //write the IFD 
    imageWriter->writeIFD();
    fileWriter.close();

}

} // End namespace.

#endif // __TIFF_FILE_WRITER_H__
