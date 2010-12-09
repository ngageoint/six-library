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

#include "nitf/Writer.hpp"

using namespace nitf;

void WriterDestructor::operator()(nitf_Writer *writer)
{
    if (writer && writer->record)
    {
        // this tells the handle manager that the Record is no longer managed
        nitf::Record rec(writer->record);
        rec.setManaged(false);
    }
    if (writer && writer->output)
    {
        // this tells the handle manager that the IOInterface is no longer managed
        nitf::IOInterface io(writer->output);
        io.setManaged(false);
    }
    nitf_Writer_destruct(&writer);
}

Writer::Writer(const Writer & x)
{
    setNative(x.getNative());
}

Writer & Writer::operator=(const Writer & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

Writer::Writer(nitf_Writer * x)
{
    setNative(x);
    getNativeOrThrow();
}

Writer::Writer() throw (nitf::NITFException)
{
    setNative(nitf_Writer_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

Writer::~Writer()
{
//    for (std::vector<nitf::WriteHandler*>::iterator it = mWriteHandlers.begin(); it
//            != mWriteHandlers.end(); ++it)
//    {
//        delete *it;
//    }
}

void Writer::write()
{
    NITF_BOOL x = nitf_Writer_write(getNativeOrThrow(), &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void Writer::prepare(nitf::IOHandle & io, nitf::Record & record)
        throw (nitf::NITFException)
{
    prepareIO(io, record);
}

void Writer::prepareIO(nitf::IOInterface & io, nitf::Record & record)
        throw (nitf::NITFException)
{
    NITF_BOOL x = nitf_Writer_prepareIO(getNativeOrThrow(), record.getNative(),
                                        io.getNative(), &error);
    if (!x)
        throw nitf::NITFException(&error);
    io.setManaged(true);
    record.setManaged(true);
}

void Writer::setImageWriteHandler(int index, WriteHandler writeHandler)
        throw (nitf::NITFException)
{
    if (!nitf_Writer_setImageWriteHandler(getNativeOrThrow(), index,
                                          writeHandler.getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler.setManaged(true);
}

void Writer::setGraphicWriteHandler(int index, WriteHandler writeHandler)
        throw (nitf::NITFException)
{
    if (!nitf_Writer_setGraphicWriteHandler(getNativeOrThrow(), index,
                                            writeHandler.getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler.setManaged(true);
}

void Writer::setTextWriteHandler(int index, WriteHandler writeHandler)
        throw (nitf::NITFException)
{
    if (!nitf_Writer_setTextWriteHandler(getNativeOrThrow(), index,
                                         writeHandler.getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler.setManaged(true);
}

void Writer::setDEWriteHandler(int index, WriteHandler writeHandler)
        throw (nitf::NITFException)
{
    if (!nitf_Writer_setDEWriteHandler(getNativeOrThrow(), index,
                                       writeHandler.getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler.setManaged(true);
}

nitf::ImageWriter Writer::newImageWriter(int imageNumber)
        throw (nitf::NITFException)
{
    nitf_SegmentWriter * x = nitf_Writer_newImageWriter(getNativeOrThrow(),
                                                        imageNumber, &error);
    if (!x)
        throw nitf::NITFException(&error);

    //manage the writer
    nitf::ImageWriter writer(x);
    writer.setManaged(true);
    return writer;
}

nitf::SegmentWriter Writer::newGraphicWriter(int graphicNumber)
        throw (nitf::NITFException)
{
    nitf_SegmentWriter * x =
            nitf_Writer_newGraphicWriter(getNativeOrThrow(), graphicNumber,
                                         &error);
    if (!x)
        throw nitf::NITFException(&error);

    //manage the writer
    nitf::SegmentWriter writer(x);
    writer.setManaged(true);
    return writer;
}

nitf::SegmentWriter Writer::newTextWriter(int textNumber)
        throw (nitf::NITFException)
{
    nitf_SegmentWriter * x = nitf_Writer_newTextWriter(getNativeOrThrow(),
                                                       textNumber, &error);
    if (!x)
        throw nitf::NITFException(&error);

    //manage the writer
    nitf::SegmentWriter writer(x);
    writer.setManaged(true);
    return writer;
}

nitf::SegmentWriter Writer::newDEWriter(int deNumber)
        throw (nitf::NITFException)
{
    nitf_SegmentWriter * x = nitf_Writer_newDEWriter(getNativeOrThrow(),
                                                     deNumber, &error);
    if (!x)
        throw nitf::NITFException(&error);

    //manage the writer
    nitf::SegmentWriter writer(x);
    writer.setManaged(true);
    return writer;
}

//! Get the warningList
nitf::List Writer::getWarningList()
{
    return nitf::List(getNativeOrThrow()->warningList);
}
