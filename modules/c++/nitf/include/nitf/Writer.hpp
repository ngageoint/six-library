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

#ifndef __NITF_WRITER_HPP__
#define __NITF_WRITER_HPP__

#include "nitf/Writer.h"
#include "nitf/List.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/IOHandle.hpp"
#include "nitf/IOInterface.hpp"
#include "nitf/Record.hpp"
#include "nitf/ImageWriter.hpp"
#include "nitf/SegmentWriter.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file Writer.hpp
 *  \brief  Contains wrapper implementation for Writer
 */

namespace nitf
{

struct WriterDestructor : public nitf::MemoryDestructor<nitf_Writer>
{
    ~WriterDestructor()
    {
    }
    void operator()(nitf_Writer *writer);
};

/*!
 *  \class Writer
 *  \brief  The C++ wrapper for the nitf_Writer
 */
class Writer : public nitf::Object<nitf_Writer, WriterDestructor>
{
public:
    //! Copy constructor
    Writer(const Writer & x);

    //! Assignment Operator
    Writer & operator=(const Writer & x);

    //! Set native object
    Writer(nitf_Writer * x);

    //! Constructor
    Writer() throw (nitf::NITFException);

    ~Writer();

    //! Write the record to disk
    void write();

    /*!
     *  Prepare the writer
     *  \param io  The IO handle to use
     *  \param record  The record to write out
     */
    void prepare(nitf::IOHandle & io, nitf::Record & record)
            throw (nitf::NITFException);

    /*!
     *  Prepare the writer
     *  \param io  The IO handle to use
     *  \param record  The record to write out
     */
    void prepareIO(nitf::IOInterface & io, nitf::Record & record)
            throw (nitf::NITFException);

    /*!
     * Sets the WriteHandler for the Image at the given index.
     */
    void setImageWriteHandler(int index, WriteHandler writeHandler)
            throw (nitf::NITFException);

    /*!
     * Sets the WriteHandler for the Graphic at the given index.
     */
    void setGraphicWriteHandler(int index, WriteHandler writeHandler)
            throw (nitf::NITFException);

    /*!
     * Sets the WriteHandler for the Text at the given index.
     */
    void setTextWriteHandler(int index, WriteHandler writeHandler)
            throw (nitf::NITFException);

    /*!
     * Sets the WriteHandler for the DE Segment at the given index.
     */
    void setDEWriteHandler(int index, WriteHandler writeHandler)
            throw (nitf::NITFException);

    /**
     * Returns a NEW ImageWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::ImageWriter newImageWriter(int imageNumber)
            throw (nitf::NITFException);

    /**
     * Returns a NEW SegmentWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::SegmentWriter newGraphicWriter(int graphicNumber)
            throw (nitf::NITFException);

    /**
     * Returns a NEW SegmentWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::SegmentWriter newTextWriter(int textNumber)
            throw (nitf::NITFException);

    /**
     * Returns a NEW SegmentWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::SegmentWriter newDEWriter(int deNumber) throw (nitf::NITFException);

    //! Get the warningList
    nitf::List getWarningList();

private:
    nitf_Error error;
    //keep track of so we can delete later
    //    std::vector<nitf::WriteHandler*> mWriteHandlers;
};

}
#endif
