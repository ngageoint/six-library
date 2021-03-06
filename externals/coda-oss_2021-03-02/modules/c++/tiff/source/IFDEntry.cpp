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

#include <string>
#include <string.h>
#include <sstream>
#include <import/io.h>
#include <import/except.h>
#include <import/mem.h>

#include "tiff/Common.h"
#include "tiff/GenericType.h"
#include "tiff/KnownTags.h"
#include "tiff/TypeFactory.h"
#include "tiff/IFDEntry.h"


void tiff::IFDEntry::serialize(io::OutputStream& output)
{
    io::Seekable *seekable =
            dynamic_cast<io::Seekable *>(&output);
    if (seekable == NULL)
        throw except::Exception(Ctxt("Can only serialize IFDEntry to seekable stream"));

    output.write((sys::byte *)&mTag, sizeof(mTag));
    output.write((sys::byte *)&mType, sizeof(mType));
    output.write((sys::byte *)&mCount, sizeof(mCount));

    sys::Uint32_T size = mCount * tiff::Const::sizeOf(mType);

    if (size > 4)
    {
        // Keep the current position and jump to the write position.
        const auto current = seekable->tell();
        seekable->seek(mOffset, io::Seekable::START);

        // Write the values out at the current cursor position
        for (sys::Uint32_T i = 0; i < mValues.size(); ++i)
            output.write((sys::byte *)mValues[i]->data(),
                    mValues[i]->size());

        // Reset the cursor
        seekable->seek(current, io::Seekable::START);

        // Write out the data offset.
        output.write((sys::byte *)&mOffset, sizeof(mOffset));
    }
    else
    {
        for (sys::Uint32_T i = 0; i < mCount; ++i)
        {
            // The value must take up four bytes.  If the number of bytes is
            // less than four, duplicate the value until you hit 4 bytes in length.
            const auto iterations = (4 / mCount) / mValues[i]->size();

            for (size_t j = 0; j < iterations; ++j)
                output.write((sys::byte *)mValues[i]->data(),
                        mValues[i]->size());
        }
    }
}

void tiff::IFDEntry::deserialize(io::InputStream& input)
{
    deserialize(input, false);
}

void tiff::IFDEntry::deserialize(io::InputStream& input, const bool reverseBytes)
{
    io::Seekable *seekable =
            dynamic_cast<io::Seekable*>(&input);
    if (seekable == NULL)
        throw except::Exception(Ctxt("Can only deserialize IFDEntry from seekable stream"));

    input.read((char *)&mTag, sizeof(mTag));
    input.read((char *)&mType, sizeof(mType));
    input.read((char *)&mCount, sizeof(mCount));
    input.read((char *)&mOffset, sizeof(mOffset));

    if (reverseBytes)
    {
        mTag = sys::byteSwap(mTag);
        mType =  sys::byteSwap(mType);
        mCount = sys::byteSwap(mCount);
        mOffset = sys::byteSwap(mOffset);
    }

    sys::Uint32_T size = mCount * tiff::Const::sizeOf(mType);

    if (size > 4)
    {
        // Keep the current position and jump to the read position.
        const auto current = seekable->tell();
        seekable->seek(mOffset, io::Seekable::START);

        // Read in the value(s);
        sys::byte *buffer = new sys::byte[size];

        input.read(buffer, size);
        if (reverseBytes)
        {
            sys::Uint32_T elementSize = tiff::Const::sizeOf(mType);
            sys::Uint32_T numElements = mCount;
            if (mType == tiff::Const::Type::RATIONAL && mType
                    == tiff::Const::Type::SRATIONAL)
            {
                elementSize = tiff::Const::sizeOf(mType) / 2;
                numElements = mCount * 2;
            }
            if (elementSize > 1)
                sys::byteSwap(buffer, static_cast<unsigned short>(elementSize), numElements);
        }

        parseValues((const unsigned char *)buffer);
        delete[] buffer;

        // Reset the cursor position.
        seekable->seek(current, io::Seekable::START);
    }
    else
    {
        if (reverseBytes)
        {
            // Re-reverse because a value may be less than 4 bytes.
            mOffset = sys::byteSwap(mOffset);
            unsigned short elementSize = tiff::Const::sizeOf(mType);
            sys::byteSwap((sys::byte*)&mOffset, elementSize, sizeof(mOffset)
              / elementSize);
        }
        parseValues((unsigned char *)&mOffset);
    }

    //try to retrieve the name as well
    tiff::IFDEntry *mapEntry = tiff::KnownTagsRegistry::getInstance()[mTag];
    mName = mapEntry ? mapEntry->getName() : "";
}

void tiff::IFDEntry::print(io::OutputStream& output) const
{
    std::ostringstream message;
    message << "Tag:                 " << mTag << " (\"" << mName << "\")"
            << std::endl;
    message << "Element Type:        " << mType << std::endl;
    message << "Number of Elements:  " << mCount << std::endl;

    // Print the offset if one exists
    if (mCount * tiff::Const::sizeOf(mType) > 4)
        message << "Offset:              " << mOffset << std::endl;

    message << "Value(s):            ";
    for (sys::Uint32_T i = 0; i < mCount; ++i)
    {
        message << mValues[i]->toString();
        if (mType != tiff::Const::Type::ASCII)
            message << " ";
    }
    message << std::endl;

    output.write(message.str());
}

void tiff::IFDEntry::parseValues(const unsigned char *buffer,
        const sys::Uint32_T count)
{
    mCount = count;
    parseValues(buffer);
}

void tiff::IFDEntry::addValue(double value)
{
    const unsigned char* const valuePtr =
        reinterpret_cast<unsigned char *>(&value);

    addValue(tiff::TypeFactory::create(valuePtr,
                                       tiff::Const::Type::DOUBLE));
}

void tiff::IFDEntry::addValues(const char* str, int tiffType)
{
    const unsigned char* const strPtr =
        reinterpret_cast<const unsigned char *>(str);

    for (size_t ii = 0, len = ::strlen(str) + 1; ii < len; ++ii)
    {
        mem::auto_ptr<tiff::TypeInterface>
            value(tiff::TypeFactory::create(strPtr + ii, static_cast<unsigned short>(tiffType)));
        addValue(value.release());
    }
}

void tiff::IFDEntry::parseValues(const unsigned char *buffer)
{
    unsigned char *marker = (unsigned char *)buffer;
    for (sys::Uint32_T i = 0; i < mCount; i++)
    {
        tiff::TypeInterface *nextValue = tiff::TypeFactory::create(marker,
                mType);
        mValues.push_back(nextValue);
        unsigned short size = nextValue->size();
        marker = marker + size;
    }
}

sys::Uint32_T tiff::IFDEntry::finalize(const sys::Uint32_T offset)
{
    mCount = static_cast<sys::Uint32_T>(mValues.size());

    sys::Uint32_T size = mCount * tiff::Const::sizeOf(mType);
    if (size > 4)
    {
        mOffset = offset;
        return offset + size;
    }

    return offset;
}
