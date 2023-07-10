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


#include "tiff/IFD.h"
#include "tiff/Common.h"
#include "tiff/GenericType.h"
#include "tiff/IFDEntry.h"
#include "tiff/KnownTags.h"

#include <string>
#include <sstream>
#include <import/io.h>
#include <import/except.h>

tiff::IFDEntry *tiff::IFD::operator[](const char *name)
{
    tiff::IFDEntry *mapEntry = tiff::KnownTagsRegistry::getInstance()[name];
    if (!mapEntry) return nullptr;
    return (*this)[mapEntry->getTagID()];
}
const tiff::IFDEntry* tiff::IFD::operator[](const char* name) const
{
    tiff::IFDEntry* mapEntry = tiff::KnownTagsRegistry::getInstance()[name];
    if (!mapEntry)
        return nullptr;
    return (*this)[mapEntry->getTagID()];
}

tiff::IFDEntry *tiff::IFD::operator[](unsigned short tag)
{
    // Ensures that the key exists.  Without this check, the defined
    // behavior for the [] operator is to create an entry if it does
    // not exist.  This is to make sure that the map doesn't get full
    // of invalid key/value pairs if you're just trying to verify whether
    // a key already exists of not.
    return exists(tag) ? mIFD[tag] : nullptr;
}
const tiff::IFDEntry* tiff::IFD::operator[](unsigned short tag) const
{
    // Ensures that the key exists.  Without this check, the defined
    // behavior for the [] operator is to create an entry if it does
    // not exist.  This is to make sure that the map doesn't get full
    // of invalid key/value pairs if you're just trying to verify whether
    // a key already exists of not.
    return exists(tag) ? ifd()[tag] : nullptr;
}

bool tiff::IFD::exists(unsigned short tag) const
{
    return mIFD.find(tag) != mIFD.end();
}

bool tiff::IFD::exists(const char *name) const
{
    tiff::IFDEntry *mapEntry = tiff::KnownTagsRegistry::getInstance()[name];
    if (!mapEntry)
        return false;
    return exists(mapEntry->getTagID());
}

void tiff::IFD::addEntry(const tiff::IFDEntry *entry)
{
    unsigned short id = entry->getTagID();
    mIFD[id] = new tiff::IFDEntry;
    *(mIFD[id]) = *entry;
}

void tiff::IFD::addEntry(const std::string& name)
{
    tiff::IFDEntry *mapEntry = tiff::KnownTagsRegistry::getInstance()[name];
    // we can't add it b/c we don't know about this tag
    if (!mapEntry)
        throw except::Exception(Ctxt(FmtX(
                                "Unable to add IFD Entry: unknown tag [%s]", name.c_str())));

    unsigned short id = mapEntry->getTagID();

    mIFD[id] = new tiff::IFDEntry;
    *(mIFD[id]) = *mapEntry;
}

void tiff::IFD::deserialize(io::InputStream& input)
{
    deserialize(input, false);
}

void tiff::IFD::deserialize(io::InputStream& input, const bool reverseBytes)
{
    unsigned short ifdEntryCount;
    input.read((sys::byte *)&ifdEntryCount, sizeof(ifdEntryCount));
    if (reverseBytes)
        ifdEntryCount = sys::byteSwap(ifdEntryCount);

    for (unsigned short i = 0; i < ifdEntryCount; i++)
    {
        tiff::IFDEntry *entry = new tiff::IFDEntry();
        entry->deserialize(input, reverseBytes);
        mIFD[entry->getTagID()] = entry;
    }
}

void tiff::IFD::serialize(io::OutputStream& output)
{
    io::Seekable *seekable =
            dynamic_cast<io::Seekable *>(&output);
    if (seekable == nullptr)
        throw except::Exception(Ctxt("Can only serialize IFD to seekable stream"));

    // Makes sure all data offsets are defined for each entry.
    // Keep the offset just past the end of the IFD.  This offset
    // is where the next potential image could be written.
    const auto endOffset = finalize(static_cast<sys::Uint32_T>(seekable->tell()));

    // Write out IFD entry count.
    const auto ifdEntryCount = static_cast<uint16_t>(mIFD.size());
    output.write((sys::byte *)&ifdEntryCount, sizeof(ifdEntryCount));

    // Write out each IFD entry.
    for (IFDType::const_iterator i = mIFD.begin(); i != mIFD.end(); ++i)
    {
        tiff::IFDEntry *entry = i->second;
        entry->serialize(output);
    }

    // Remember the current position in case there is another IFD after
    // this one.
    mNextIFDOffsetPosition = static_cast<sys::Uint32_T>(seekable->tell());

    // Write out the default next IFD location.
    sys::Uint32_T nextOffset = 0;
    output.write((sys::byte *)&nextOffset, sizeof(sys::Uint32_T));

    // Seek the end of the IFD, the next image can begin here.
    seekable->seek(endOffset, io::Seekable::START);
}

void tiff::IFD::print(io::OutputStream& output) const
{
    sys::Uint32_T x = 1;

    for (IFDType::const_iterator i = mIFD.begin(); i != mIFD.end(); ++i, ++x)
    {
        std::ostringstream message;
        message << "Entry Number:        " << x << std::endl;
        output.write(message.str());

        i->second->print(output);
        output.writeln("");
    }
}

sys::Uint32_T tiff::IFD::getImageWidth() const
{
    auto imageWidth = (*this)[tiff::KnownTags::IMAGE_WIDTH];
    if (!imageWidth)
        return 0;

    if (imageWidth->getType() == tiff::Const::Type::LONG)
        return *(tiff::GenericType<sys::Uint32_T> *)(*imageWidth)[0];

    return *(tiff::GenericType<unsigned short> *)(*imageWidth)[0];
}

sys::Uint32_T tiff::IFD::getImageLength() const
{
    auto imageLength = (*this)[tiff::KnownTags::IMAGE_LENGTH];
    if (!imageLength)
        return 0;

    if (imageLength->getType() == tiff::Const::Type::LONG)
        return *(tiff::GenericType<sys::Uint32_T> *)(*imageLength)[0];

    return *(tiff::GenericType<unsigned short> *)(*imageLength)[0];
}

sys::Uint32_T tiff::IFD::getImageSize() const
{
    sys::Uint32_T width = getImageWidth();
    sys::Uint32_T length = getImageLength();
    unsigned short elementSize = getElementSize();

    return width * length * elementSize;
}

unsigned short tiff::IFD::getNumBands() const
{
    unsigned short numBands = 1;
    
    auto samplesPerPixel = (*this)[tiff::KnownTags::SAMPLES_PER_PIXEL];
    auto bitsPerSample = (*this)[tiff::KnownTags::BITS_PER_SAMPLE];
    
    if (samplesPerPixel)
        numBands = *(::tiff::GenericType<unsigned short> *)(*samplesPerPixel)[0];
    else if (bitsPerSample)
        numBands = static_cast<unsigned short>(bitsPerSample->getCount());
    
    return numBands;
}

unsigned short tiff::IFD::getElementSize() const
{
    auto bitsPerSample = (*this)[tiff::KnownTags::BITS_PER_SAMPLE];
    const auto bytesPerSample = (!bitsPerSample) ? 1
            : *(tiff::GenericType<unsigned short> *)(*bitsPerSample)[0] >> 3;

    return static_cast<unsigned short>(bytesPerSample * getNumBands());
}

sys::Uint32_T tiff::IFD::finalize(const sys::Uint32_T offset)
{
    // Find the beginning offset to extra IFD data.  The IFD length is
    // the size of an IFD entry multiplied by the number of entries, plus
    // 4 bytes to hold the offset to the next IFD, and 2 bytes to hold the
    // IFD entry count.
    auto dataOffset = static_cast<sys::Uint32_T>(offset + sizeof(short) + (mIFD.size()
            * tiff::IFDEntry::sizeOf()) + sizeof(sys::Uint32_T));

    for (IFDType::iterator i = mIFD.begin(); i != mIFD.end(); ++i)
    {
        // Send in the current offset.  If the value size of the IFD entry
        // requires that data be placed outside the IFD entry, the offset that
        // is returned will be adjusted to compensate for that data.
        dataOffset = i->second->finalize(dataOffset);
    }

    return dataOffset;
}
