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

#include "nitf/Reader.hpp"

using namespace nitf;

void ReaderDestructor::operator()(nitf_Reader *reader)
{
    if (reader)
    {
        if (reader->record)
        {
            // this tells the handle manager that the Record is no longer managed
            nitf::Record rec(reader->record);
            rec.setManaged(false);
        }
        if (reader->input && !reader->ownInput)
        {
            // this tells the handle manager that the IOInterface is no longer managed
            nitf::IOInterface io(reader->input);
            io.setManaged(false);
        }
        nitf_Reader_destruct(&reader);
    }
}

Reader::Reader(const Reader & x)
{
    setNative(x.getNative());
}

Reader & Reader::operator=(const Reader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

Reader::Reader(nitf_Reader * x)
{
    setNative(x);
    getNativeOrThrow();
}

Reader::Reader() : Reader(nitf_Reader_construct(&error))
{
    setManaged(false);
}

nitf::Version Reader::getNITFVersion(const std::string& fileName) noexcept
{
    return nitf_Reader_getNITFVersion(fileName.c_str());
}

nitf::Version Reader::getNITFVersion(const IOInterface& io)
{
    return nitf_Reader_getNITFVersionIO(io.getNativeOrThrow());
}

nitf::Record Reader::read(nitf::IOHandle & io)
{
    return readIO(io);
}

nitf::Record Reader::readIO(nitf::IOInterface & io)
{
    //free up the existing record, if we have one
    nitf_Reader *reader = getNativeOrThrow();
    if (reader->record)
    {
        nitf::Record rec(reader->record);
        rec.setManaged(false);
    }
    if (reader->input && !reader->ownInput)
    {
        nitf::IOInterface oldIO(reader->input);
        oldIO.setManaged(false);
    }

    nitf_Record * x = nitf_Reader_readIO(getNativeOrThrow(), io.getNative(),
                                         &error);

    // It's possible readIO() failed but actually took ownership of the
    // io object.  So we need to call setManaged() on it regardless of if the
    // function succeeded.
    if (getNativeOrThrow()->input == io.getNative())
    {
        io.setManaged(true);
    }

    if (!x)
        throw nitf::NITFException(&error);
    nitf::Record rec(x);

    return rec;
}

nitf::ImageReader Reader::newImageReader(int imageSegmentNumber)
{
    nitf_ImageReader * x = nitf_Reader_newImageReader(getNativeOrThrow(),
                                                      imageSegmentNumber,
                                                      nullptr, &error);
    if (!x)
        throw nitf::NITFException(&error);

    nitf::ImageReader reader(x);
    //set it so it is NOT managed by the underlying library
    //this means the reader is subject to deletion when refcount == 0
    reader.setManaged(false);
    return reader;
}

nitf::ImageReader Reader::newImageReader(int imageSegmentNumber,
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

    nitf_ImageReader* x = nitf_Reader_newImageReader(getNativeOrThrow(),
                                                     imageSegmentNumber,
                                                     userOptionsNative,
                                                     &error);
    if (!x)
    {
        throw nitf::NITFException(&error);
    }

    nitf::ImageReader reader(x);
    //set it so it is NOT managed by the underlying library
    //this means the reader is subject to deletion when refcount == 0
    reader.setManaged(false);
    return reader;
}

nitf::SegmentReader Reader::newDEReader(int deSegmentNumber)
{
    nitf_SegmentReader * x = nitf_Reader_newDEReader(getNativeOrThrow(),
                                                     deSegmentNumber, &error);
    if (!x)
        throw nitf::NITFException(&error);
    nitf::SegmentReader reader(x);
    //set it so it is NOT managed by the underlying library
    //this means the reader is subject to deletion when refcount == 0
    reader.setManaged(false);
    return reader;
}

nitf::SegmentReader Reader::newGraphicReader(int segmentNumber)
{
    nitf_SegmentReader * x =
            nitf_Reader_newGraphicReader(getNativeOrThrow(), segmentNumber,
                                         &error);
    if (!x)
        throw nitf::NITFException(&error);
    nitf::SegmentReader reader(x);
    //set it so it is NOT managed by the underlying library
    //this means the reader is subject to deletion when refcount == 0
    reader.setManaged(false);
    return reader;
}

nitf::SegmentReader Reader::newTextReader(int segmentNumber)
{
    nitf_SegmentReader * x = nitf_Reader_newTextReader(getNativeOrThrow(),
                                                       segmentNumber, &error);
    if (!x)
        throw nitf::NITFException(&error);
    nitf::SegmentReader reader(x);
    //set it so it is NOT managed by the underlying library
    //this means the reader is subject to deletion when refcount == 0
    reader.setManaged(false);
    return reader;
}

nitf::List Reader::getWarningList() const
{
    return nitf::List(getNativeOrThrow()->warningList);
}

nitf::Record Reader::getRecord() const
{
    return nitf::Record(getNativeOrThrow()->record);
}

nitf::IOInterface Reader::getInput() const
{
    return nitf::IOInterface(getNativeOrThrow()->input);
}
