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

#ifndef __NITF_READER_HPP__
#define __NITF_READER_HPP__

#include "nitf/Reader.h"
#include "nitf/IOHandle.hpp"
#include "nitf/IOInterface.hpp"
#include "nitf/Record.hpp"
#include "nitf/ImageReader.hpp"
#include "nitf/SegmentReader.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file Reader.hpp
 *  \brief  Contains wrapper implementation for Reader
 */

namespace nitf
{

struct ReaderDestructor : public nitf::MemoryDestructor<nitf_Reader>
{
    ~ReaderDestructor()
    {
    }
    void operator()(nitf_Reader *reader);
};

/*!
 *  \class Reader
 *  \brief  The C++ wrapper for the nitf_Reader
 */
class Reader : public nitf::Object<nitf_Reader, ReaderDestructor>
{
public:
    //! Copy constructor
    Reader(const Reader & x);

    //! Assignment Operator
    Reader & operator=(const Reader & x);

    //! Set native object
    Reader(nitf_Reader * x);

    //! Default Constructor
    Reader() throw(nitf::NITFException);

    ~Reader();

    /*!
     * Return the Version of the file passed in by its file name.
     * Valid return values are:
     *      NITF_VER_20, NITF_VER_21, or NITF_VER_UNKNOWN
     */
    static nitf::Version getNITFVersion(const std::string& fileName);

    /*!
     *  This is the preferred method for reading a NITF 2.1 file.
     *  \param io  The IO handle
     *  \return  A Record containing the read information
     */
    nitf::Record read(nitf::IOHandle & io) throw (nitf::NITFException);

    /*!
     *  This is the preferred method for reading a NITF 2.1 file.
     *  \param io  The IO handle
     *  \return  A Record containing the read information
     */
    nitf::Record readIO(nitf::IOInterface & io) throw (nitf::NITFException);

    /*!
     *  Get a new image reader for the segment
     *  \param imageSegmentNumber  The image segment number
     *  \return  An ImageReader matching the imageSegmentNumber
     */
    nitf::ImageReader newImageReader(int imageSegmentNumber)
        throw (nitf::NITFException);

    /*!
     *  Get a new DE reader for the segment
     *  \param deSegmentNumber  The DE segment number
     *  \return  A SegmentReader matching the deSegmentNumber
     */
    nitf::SegmentReader newDEReader(int deSegmentNumber)
        throw (nitf::NITFException);

    /*!
     *  Get a new SegmentReader for the Graphic segment
     *  \param segmentNumber the segment Number
     *  \return  A SegmentReader matching the segmentNumber
     */
    nitf::SegmentReader newGraphicReader(int segmentNumber)
        throw (nitf::NITFException);

    /*!
     *  Get a new SegmentReader for the Text segment
     *  \param segmentNumber the segment Number
     *  \return  A SegmentReader matching the segmentNumber
     */
    nitf::SegmentReader newTextReader(int segmentNumber)
        throw (nitf::NITFException);

    //! Get the warningList
    nitf::List getWarningList() const;

    //! Return the record
    nitf::Record getRecord() const;

    //! Return the input
    nitf::IOInterface getInput() const;

private:
    nitf_Error error;
};

}
#endif
