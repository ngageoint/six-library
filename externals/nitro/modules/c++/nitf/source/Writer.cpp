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

#include "nitf/Writer.hpp"

#include "gsl/gsl.h"

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
    *this = x;
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

Writer::Writer() : Writer(nitf_Writer_construct(&error))
{
    setManaged(false);
}

//Writer::~Writer()
//{
//    for (std::vector<nitf::WriteHandler*>::iterator it = mWriteHandlers.begin(); it
//            != mWriteHandlers.end(); ++it)
//    {
//        delete *it;
//    }
//}

void Writer::write()
{
    const NITF_BOOL x = nitf_Writer_write(getNativeOrThrow(), &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void Writer::prepare(nitf::IOHandle & io, nitf::Record & record)
{
    prepareIO(io, record);
}

void Writer::prepareIO(nitf::IOInterface & io, nitf::Record & record)
{
    const NITF_BOOL x = nitf_Writer_prepareIO(getNativeOrThrow(), record.getNative(),
                                        io.getNative(), &error);

    // It's possible prepareIO() failed but actually took ownership of one
    // or both of these objects.  So we need to call setManaged() on them
    // properly regardless of if the function succeeded.
    if (getNativeOrThrow()->record == record.getNative())
    {
        record.setManaged(true);
    }

    if (getNativeOrThrow()->output == io.getNative())
    {
        io.setManaged(true);
    }

    if (!x)
    {
        throw nitf::NITFException(&error);
    }
}

void Writer::setWriteHandlers(nitf::IOHandle& io, const nitf::Record& record)
{
    setImageWriteHandlers(io, record);
    setGraphicWriteHandlers(io, record);
    setTextWriteHandlers(io, record);
    setDEWriteHandlers(io, record);
}

void Writer::setImageWriteHandlers(nitf::IOHandle& io, const nitf::Record& record)
{
    nitf::List images = record.getImages();
    const auto numImages = record.getNumImages();
    for (uint32_t ii = 0; ii < numImages; ++ii)
    {
        nitf::ImageSegment segment = images[ii];
        const auto offset = segment.getImageOffset();
        mem::SharedPtr<nitf::WriteHandler> handler(
                new nitf::StreamIOWriteHandler(
                    io, offset, segment.getImageEnd() - offset));
        setImageWriteHandler(gsl::narrow<int>(ii), handler);
    }
}

void Writer::setGraphicWriteHandlers(nitf::IOHandle& io, const nitf::Record& record)
{
    nitf::List graphics = record.getGraphics();
    const auto numGraphics = record.getNumGraphics();
    for (uint32_t ii = 0; ii < numGraphics; ++ii)
    {
       nitf::GraphicSegment segment = graphics[ii];
       const auto offset = segment.getOffset();
       mem::SharedPtr< ::nitf::WriteHandler> handler(
           new nitf::StreamIOWriteHandler (
               io, offset, segment.getEnd() - offset));
       setGraphicWriteHandler(gsl::narrow<int>(ii), handler);
    }
}

void Writer::setTextWriteHandlers(nitf::IOHandle& io, const nitf::Record& record)
{
    nitf::List texts = record.getTexts();
    const auto numTexts = record.getNumTexts();
    for (uint32_t ii = 0; ii < numTexts; ++ii)
    {
       nitf::TextSegment segment = texts[ii];
       const auto offset = segment.getOffset();
       mem::SharedPtr< ::nitf::WriteHandler> handler(
           new nitf::StreamIOWriteHandler (
               io, offset, segment.getEnd() - offset));
       setTextWriteHandler(gsl::narrow<int>(ii), handler);
    }
}

void Writer::setDEWriteHandlers(nitf::IOHandle& io, const nitf::Record& record)
{
    nitf::List dataExtensions = record.getDataExtensions();
    const auto numDEs = record.getNumDataExtensions();
    for (uint32_t ii = 0; ii < numDEs; ++ii)
    {
       nitf::DESegment segment = dataExtensions[ii];
       const auto offset = segment.getOffset();
       mem::SharedPtr< ::nitf::WriteHandler> handler(
           new nitf::StreamIOWriteHandler (
               io, offset, segment.getEnd() - offset));
       setDEWriteHandler(gsl::narrow<int>(ii), handler);
    }
}

void Writer::setImageWriteHandler(int index,
                                  mem::SharedPtr<WriteHandler> writeHandler)
{
    if (!nitf_Writer_setImageWriteHandler(getNativeOrThrow(), index,
                                          writeHandler->getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler->setManaged(true);
    mWriteHandlers.push_back(writeHandler);
}

void Writer::setGraphicWriteHandler(int index,
                                    mem::SharedPtr<WriteHandler> writeHandler)
{
    if (!nitf_Writer_setGraphicWriteHandler(getNativeOrThrow(), index,
                                            writeHandler->getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler->setManaged(true);
    mWriteHandlers.push_back(writeHandler);
}

void Writer::setTextWriteHandler(int index,
                                 mem::SharedPtr<WriteHandler> writeHandler)
{
    if (!nitf_Writer_setTextWriteHandler(getNativeOrThrow(), index,
                                         writeHandler->getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler->setManaged(true);
    mWriteHandlers.push_back(writeHandler);
}

void Writer::setDEWriteHandler(int index,
                               mem::SharedPtr<WriteHandler> writeHandler)
{
    if (!nitf_Writer_setDEWriteHandler(getNativeOrThrow(), index,
                                       writeHandler->getNative(), &error))
        throw nitf::NITFException(&error);
    writeHandler->setManaged(true);
    mWriteHandlers.push_back(writeHandler);
}

nitf::ImageWriter Writer::newImageWriter(int imageNumber)
{
    nitf_SegmentWriter * x = nitf_Writer_newImageWriter(getNativeOrThrow(),
                                                        imageNumber, nullptr, &error);
    if (!x)
        throw nitf::NITFException(&error);

    //manage the writer
    nitf::ImageWriter writer(x);
    writer.setManaged(true);
    return writer;
}

nitf::ImageWriter Writer::newImageWriter(int imageNumber,
                                         const std::map<std::string, void*>& options)
{
    nitf::HashTable userOptions;
    nrt_HashTable* userOptionsNative = nullptr;

    if (!options.empty())
    {
        userOptions.setPolicy(NRT_DATA_RETAIN_OWNER);
        for (std::map<std::string, void*>::const_iterator iter =
                     options.begin();
            iter != options.end();
            ++iter)
        {
            userOptions.insert(iter->first, iter->second);
        }
        userOptionsNative = userOptions.getNative();
    }

    nitf_SegmentWriter* x = nitf_Writer_newImageWriter(getNativeOrThrow(),
                                                       imageNumber,
                                                       userOptionsNative,
                                                       &error);

    if (!x)
    {
        throw nitf::NITFException(&error);
    }

    //manage the writer
    nitf::ImageWriter writer(x);
    writer.setManaged(true);
    return writer;
}

nitf::SegmentWriter Writer::newGraphicWriter(int graphicNumber)
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

void Writer::writeHeader(nitf::Off& fileLenOff, uint32_t& hdrLen)
{
    if (!nitf_Writer_writeHeader(getNativeOrThrow(),
                                 &fileLenOff,
                                 &hdrLen,
                                 &error))
    {
        throw nitf::NITFException(&error);
    }
}

void Writer::writeImageSubheader(nitf::ImageSubheader subheader,
                                 nitf::Version version,
                                 nitf::Off& comratOff)
{
    if (!nitf_Writer_writeImageSubheader(getNativeOrThrow(),
                                         subheader.getNativeOrThrow(),
                                         version,
                                         &comratOff,
                                         &error))
    {
        throw nitf::NITFException(&error);
    }
}

void Writer::writeDESubheader(nitf::DESubheader subheader,
                              uint32_t& userSublen,
                              nitf::Version version)
{
    if (!nitf_Writer_writeDESubheader(getNativeOrThrow(),
                                      subheader.getNativeOrThrow(),
                                      &userSublen,
                                      version,
                                      &error))
    {
        throw nitf::NITFException(&error);
    }
}

void Writer::writeInt64Field(uint64_t field,
                             uint32_t length,
                             char fill,
                             uint32_t fillDir)
{
    if (!nitf_Writer_writeInt64Field(getNativeOrThrow(),
                                     field,
                                     length,
                                     fill,
                                     fillDir,
                                     &error))
    {
        throw nitf::NITFException(&error);
    }
}
