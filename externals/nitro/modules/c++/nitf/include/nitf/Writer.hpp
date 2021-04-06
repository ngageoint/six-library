/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#pragma once

#include <memory>

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
    void operator()(nitf_Writer *writer) override;
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
    Writer();

    ~Writer() = default;
    Writer(Writer&&) = default;
    Writer& operator=(Writer&&) = default;

    //! Write the record to disk
    void write();

    /*!
     *  Prepare the writer
     *  \param io  The IO handle to use
     *  \param record  The record to write out
     */
    void prepare(nitf::IOHandle & io, nitf::Record & record);

    /*!
     *  Prepare the writer
     *  \param io  The IO handle to use
     *  \param record  The record to write out
     */
    void prepareIO(nitf::IOInterface & io, nitf::Record & record);

    /*!
     * Set write handlers for images, graphics, texts, and DES
     * \param io The input IO handle containing the data
     * \param record The record to write out
     */
    void setWriteHandlers(nitf::IOHandle& io, const nitf::Record& record);

    /*!
     * Set write handlers for images
     * \param io The input IO handle containing the data
     * \param record The record to write out
     */
    void setImageWriteHandlers(nitf::IOHandle& io, const nitf::Record& record);

    /*!
     * Set write handlers for graphics
     * \param io The input IO handle containing the data
     * \param record The record to write out
     */
    void setGraphicWriteHandlers(nitf::IOHandle& io, const nitf::Record& record);

    /*!
     * Set write handlers for texts
     * \param io The input IO handle containing the data
     * \param record The record to write out
     */
    void setTextWriteHandlers(nitf::IOHandle& io, const nitf::Record& record);

    /*!
     * Set write handlers for DEs
     * \param io The input IO handle containing the data
     * \param record The record to write out
     */
    void setDEWriteHandlers(nitf::IOHandle& io, const nitf::Record& record);

    /*!
     * Sets the WriteHandler for the Image at the given index.
     */
    void setImageWriteHandler(int index,
                              mem::SharedPtr<WriteHandler> writeHandler);

    /*!
     * Sets the WriteHandler for the Graphic at the given index.
     */
    void setGraphicWriteHandler(int index,
                                mem::SharedPtr<WriteHandler> writeHandler);

    /*!
     * Sets the WriteHandler for the Text at the given index.
     */
    void setTextWriteHandler(int index,
                             mem::SharedPtr<WriteHandler> writeHandler);

    /*!
     * Sets the WriteHandler for the DE Segment at the given index.
     */
    void setDEWriteHandler(int index,
                           mem::SharedPtr<WriteHandler> writeHandler);

    /**
     * Returns a NEW ImageWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::ImageWriter newImageWriter(int imageNumber);

    /**
     * Returns a NEW ImageWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::ImageWriter newImageWriter(int imageNumber,
                                     const std::map<std::string, void*>& options);

    /**
     * Returns a NEW SegmentWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::SegmentWriter newGraphicWriter(int graphicNumber);

    /**
     * Returns a NEW SegmentWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::SegmentWriter newTextWriter(int textNumber);

    /**
     * Returns a NEW SegmentWriter for the given index
     *
     * The pointer is deleted by the library, so don't delete it yourself.
     */
    nitf::SegmentWriter newDEWriter(int deNumber);

    //! Get the warningList
    nitf::List getWarningList();

    // NOTE: In general the following methods are not needed.  Only use these
    //       if you know what you're doing and are trying to write out a NITF
    //       piecemeal rather than through the normal Writer object interface.

    /*!
     * Writes out the file header (skips over the FL field as the total file
     * length is not known yet).  No seeking is performed so the underlying IO
     * object should be at the beginning of the file.
     *
     * \param fileLenOff Output parameter providing the offset in bytes in the
     *     file to the file length (FL) field in the header so that you can
     *     write it out later once the file length is known
     * \param hdrLen Output parameter providing the total number of bytes the
     *     file header is on disk
     */
    void writeHeader(nitf::Off& fileLenOff, uint32_t& hdrLen);

    /*!
     * Writes out an image subheader.  No seeking is performed so the underlying
     * IO object should be at the appropriate spot in the file prior to this
     * call.
     *
     * \param subhdr Image subheader to write out
     * \param version NITF file version to write (you probably want NITF_VER_21)
     * \param comratOff Output parameter containing the offset in bytes in the
     *     file where the COMRAT field would be populated for compressed files
     */
    void writeImageSubheader(nitf::ImageSubheader subheader,
                             nitf::Version version,
                             nitf::Off& comratOff);

    /*!
     * Writes out a data extension subheader.  No seeking is performed so the
     * underlying IO object should be at the appropriate spot in the file prior
     * to this call.
     *
     * \param subhdr Data extension subheader to write out
     * \param userSublen Output parameter containing the length in bytes of the
     *     user subheader
     * \param version NITF file version to write (you probably want NITF_VER_21)
     */
    void writeDESubheader(nitf::DESubheader subheader,
                          uint32_t& userSublen,
                          nitf::Version version);

    /*!
     * Writes an int64 field to the writer in its current position
     *
     * \param field Value of the field to write out
     * \param length Length of the field to write out
     * \param fill Fill character to use for any remaining bytes
     * \param fillDir Fill direction (NITF_WRITER_FILL_LEFT or
     *     NITF_WRITER_FILL_RIGHT)
     */
    void writeInt64Field(uint64_t field,
                         uint32_t length,
                         char fill,
                         uint32_t fillDir);

private:
    nitf_Error  error{};

    //! c++ write handlers need to be kept in scope
    std::vector<mem::SharedPtr<nitf::WriteHandler> > mWriteHandlers;
};

}
#endif
