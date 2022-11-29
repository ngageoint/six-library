/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_sio_lite_SioFileWriter_h_INCLUDED_
#define CODA_OSS_sio_lite_SioFileWriter_h_INCLUDED_

#include <memory>
#include <vector>

#include <import/sys.h>
#include <import/io.h>
#include <import/mem.h>
#include <types/RowCol.h>
#include <sys/filesystem.h>

#include "sio/lite/InvalidHeaderException.h"
#include "sio/lite/FileHeader.h"


namespace sio
{
namespace lite
{
/*!
 *  \class FileWriter
 *  \brief (Deprecated) class to write an SIO file
 *
 *  This (deprecated) class will help to write out an SIO file.  It is almost never necessary
 *  since the FileHeader.to() method can be used to start a header, and then the OutputStream.
 *  may be accessed directly.  Additionally, the writeSIO() routine provides
 *  simpler functionality
 */
struct FileWriter
{
    FileWriter(const std::string& outputFile) : mFileName(outputFile), mAdopt(true)
    {
        mStream.reset(new io::FileOutputStream(mFileName));
    }

    FileWriter(const char* outputFile) : mFileName(outputFile), mAdopt(true)
    {
        mStream.reset(new io::FileOutputStream(mFileName));
    }

    //! The input stream will get freed by the Writer if adopt is set to true
    FileWriter(io::OutputStream* stream, bool adopt = true) : mAdopt(adopt)
    {
        mStream.reset(stream);
    }

    // need copy for Python binding w/SWIG
    //FileWriter(const FileWriter&) = default;
    //FileWriter& operator=(const FileWriter&) = default;
    //FileWriter(FileWriter&&) = default;
    //FileWriter& operator=(FileWriter&&) = default;

    virtual ~FileWriter()
    {
        //if we aren't adopting it, release it
        if (!mAdopt) mStream.release();
    }

    /*!
     * Writes the SIO given the FileHeader and InputStreams.
     */
    void write(FileHeader* header, std::vector<io::InputStream*> bandStreams);
    
    /*!
     * Writes a version 1 SIO given the basic file header contents and a
     * vector of InputStreams
     */
    void write(int numLines, int numElements, int elementSize,
               int elementType, std::vector<io::InputStream*> bandStreams);

    /*!
     * Writes the SIO given the FileHeader and a buffer of raw data in 
     * band-sequential format.
     */
    void write(FileHeader* header, const void* data, int numBands = 1);
    
    /*!
     * Writes a version 1 SIO given the basic file header contents and a buffer
     * of raw data in band-sequential format.
     */
    void write(int numLines, int numElements, int elementSize,
               int elementType, const void* data, int numBands = 1);

protected:
    std::string mFileName;
    std::unique_ptr<io::OutputStream> mStream;
    bool mAdopt;
};

/** Automatic data, this is not explicitly valid, dont use this in an FileHeader */
enum { AUTO = -1 };

/*!
 *  Utility routine to write an image of type T into an SIO file format.  Supported
 *  types are complex<float>, float, double, byte unsigned and N-byte unsigned.
 *
 *  Sizes are deduced from the template type automatically (complex<float> = 8,
 *  double = 8, float = 4, and byte = 1, unless the es option is given, which
 *  should typically only be done for N-byte images (like RGB images).
 *
 *  Types are deduced from the template arguments as well, via the size operator
 *  (not explicitly).  This can be a good thing, but in the case of complex<float>
 *  vs. double for example, the element type is treated as COMPLEX_FLOAT.  This is
 *  clearly not the intent for a double, so an option element type can be given
 *  explicitly.
 *
 *  If this function fails to validate the input, it may throw an exception.
 *  If it does not, the SIO file is presumed to be correct.
 *
 *  \param image Data to write (this really doesnt have to be an image at all)
 *  \param rows The number of rows in the data (nl in sio-speak)
 *  \param cols The number of cols in the data (ne in sio-speak)
 *  \param et The element type, which defaults to AUTO, in which this is guessed
 *  \param es The element size (in bytes), which defaults to AUTO, in which case
 *            it is guessed
 *
 *
 */
template<typename T> void writeSIO(const T* image, size_t rows, size_t cols,
                                   const std::string& imageFile,
                                   int et = AUTO, int es = AUTO)
{

    if (es == AUTO)
    {
        es = sizeof(T);
    }

    if (et == -1)
    {
        switch (es)
        {
        case 8:
            et = FileHeader::COMPLEX_FLOAT;
            break;

        case 4:
            et = FileHeader::FLOAT;
            break;

        case 3:
            et = FileHeader::N_BYTE_UNSIGNED;
            break;

        case 1:
            et = FileHeader::UNSIGNED;
            break;
        default:
            throw except::Exception(Ctxt(FmtX("Unexpected es: %d", es)));
        }
    }

    io::FileOutputStream imageStream(imageFile);

    FileHeader fhdr(static_cast<int>(rows), static_cast<int>(cols), es, et);
    fhdr.to(1, imageStream);

    imageStream.write(reinterpret_cast<const sys::byte*>(image),
                      rows * cols * es);

    imageStream.close();
}
template<typename T>
void writeSIO(const T* image, const types::RowCol<size_t>& dims, const sys::filesystem::path& imageFile,
                                   int et = AUTO, int es = AUTO)
{
    writeSIO(image, dims.row, dims.col, imageFile.string(), et, es);
}


}
}

#endif  // CODA_OSS_sio_lite_SioFileWriter_h_INCLUDED_
