/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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

#include "zip/ZipFile.h"

#define Z_READ_SHORT_INC(BUF, OFF) readShort(&BUF[OFF]); OFF += 2
#define Z_READ_INT_INC(BUF, OFF) readInt(&BUF[OFF]); OFF += 4

namespace zip
{
ZipFile::~ZipFile()
{
    for (size_t i = 0; i < mEntries.size(); ++i)
    {
        // Delete ZipEntry
        delete mEntries[i];
    }

    if (mCompressed)
        delete[] mCompressed;
}

sys::Uint32_T ZipFile::readInt(sys::ubyte* buf)
{
    sys::ubyte* p;
    sys::Uint32_T le;// = *((sys::Uint32_T *)buf);
    p = (sys::ubyte*) &le;
    memcpy(p, buf, 4);

    // Kind of hackish, but we need it like yesterday    
    if (mSwapBytes)
    {
        le = sys::byteSwap(le);

    }
    return le;
}

sys::Uint16_T ZipFile::readShort(sys::ubyte* buf)
{
    sys::ubyte* p;
    sys::Uint16_T le;// = *((sys::Uint16_T *)buf);
    p = (sys::ubyte*) &le;
    memcpy(p, buf, 2);

    if (mSwapBytes)
        le = sys::byteSwap(le);
    return le;
}

ZipFile::Iterator ZipFile::lookup(std::string fileName) const
{
    ZipFile::Iterator p;
    for (p = mEntries.begin(); p != mEntries.end(); ++p)
    {

        if ((*p)->getFileName() == fileName)
            break;
    }

    return p;

}

void ZipFile::readCentralDir()
{
    if (mCompressedLength < EOCD_LEN)
        throw except::IOException(Ctxt(
                "stream source too small to be a zip stream"));
    sys::ubyte* start;

    if (mCompressedLength > MAX_EOCD_SEARCH)
    {
        start = mCompressed + mCompressedLength - MAX_EOCD_SEARCH;
    }
    else
    {
        start = mCompressed;
    }
    sys::ubyte* p = mCompressed + mCompressedLength - 4;

    sys::ubyte* eocd = nullptr;
    auto i = mCompressedLength - 4;
    while (p >= start)
    {
        if (*p == 0x50)
        {
            if (readInt(p) == CD_SIGNATURE)
            {
                eocd = p;
                break;
            }
        }
        p--;
        i--;
    }
    if (p < start)
    {
        throw except::IOException(Ctxt("EOCD not found"));
    }
    // else still rockin'
    readCentralDirValues(eocd, (mCompressed + mCompressedLength) - eocd);

    p = mCompressed + mCentralDirOffset;
    const sys::SSize_T len = (mCompressed + mCompressedLength) - p;
    for (auto& entry : mEntries)
    {
        entry = newCentralDirEntry(&p, len);
    }

}

ZipEntry* ZipFile::newCentralDirEntry(sys::ubyte** buf, sys::SSize_T len)
{
    if (len < ENTRY_LEN)
        throw except::IOException(Ctxt("CDE entry not large enough"));

    sys::SSize_T off = 0;

    sys::ubyte* p = *buf;

    sys::Uint32_T entrySig = Z_READ_INT_INC(p, off);

    if (entrySig != ENTRY_SIGNATURE)
        throw except::IOException(Ctxt("Did not find entry signature"));

    sys::Uint16_T versionMadeBy = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T versionToExtract = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T generalPurposeBitFlag = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T compressionMethod = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T lastModifiedTime = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T lastModifiedDate = Z_READ_SHORT_INC(p, off);
    sys::Uint32_T crc32 = Z_READ_INT_INC(p, off);
    sys::Uint32_T compressedSize = Z_READ_INT_INC(p, off);
    sys::Uint32_T uncompressedSize = Z_READ_INT_INC(p, off);
    sys::Uint16_T fileNameLength = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T extraFieldLength = Z_READ_SHORT_INC(p, off);
    sys::Uint16_T fileCommentLength = Z_READ_SHORT_INC(p, off);
    Z_READ_SHORT_INC(p, off); // skipping diskNumberStart
    sys::Uint16_T internalAttrs = Z_READ_SHORT_INC(p, off);
    auto externalAttrs = Z_READ_INT_INC(p, off);
    sys::Uint32_T localHeaderRelOffset = readInt(&p[off]);
    p += ENTRY_LEN;

    std::string fileName;
    if (fileNameLength != 0)
        fileName = std::string((const char*) p, fileNameLength);

    p += fileNameLength;

    // I dont know what extra field is, but skip it for now	    
    if (extraFieldLength)
        p += extraFieldLength;

    std::string fileComment;
    if (fileCommentLength)
        fileComment = std::string((const char*) p, fileCommentLength);

    p += fileCommentLength;

    *buf = p;

    p = mCompressed + localHeaderRelOffset;

    extraFieldLength = readShort(&p[0x1c]);

    sys::Size_T dataOffset = localHeaderRelOffset + LFH_SIZE + fileNameLength
            + extraFieldLength;

    return new ZipEntry(mCompressed + dataOffset, compressedSize,
            uncompressedSize, fileName, fileComment, versionMadeBy,
            versionToExtract, generalPurposeBitFlag, compressionMethod,
            lastModifiedTime, lastModifiedDate, crc32, internalAttrs,
            externalAttrs);

}

void ZipFile::readCentralDirValues(sys::ubyte* buf, sys::SSize_T len)
{

    if (len < EOCD_LEN)
        throw except::IOException(Ctxt("len < EOCD_LEN"));

    sys::Uint16_T off = 4;

    sys::Uint16_T diskNum = Z_READ_SHORT_INC(buf, off);
    if (diskNum != 0)
        throw except::IOException(Ctxt("disk number must be 0"));
    sys::Uint16_T diskWithCentralDir = Z_READ_SHORT_INC(buf, off);

    if (diskWithCentralDir != diskNum)
        throw except::IOException(Ctxt("central dir disk number must be same"));

    sys::Uint16_T entryCount = Z_READ_SHORT_INC(buf, off);
    sys::Uint16_T totalEntries = Z_READ_SHORT_INC(buf, off);

    if (totalEntries != entryCount)
        throw except::IOException(Ctxt("Total entries must match entries"));

    mEntries.resize(entryCount);

    mCentralDirSize = Z_READ_INT_INC(buf, off);
    mCentralDirOffset = Z_READ_INT_INC(buf, off);

    sys::Uint16_T commentLength = Z_READ_SHORT_INC(buf, off);

    if (EOCD_LEN + commentLength > len)
        throw except::IOException(Ctxt("Comment line too long"));

    mComment = std::string((const char*) (buf + EOCD_LEN), commentLength);
}

std::ostream& operator<<(std::ostream& os, const ZipFile& zf)
{
    os << "central directory length: " << zf.getCentralDirSize() << std::endl;
    os << "central directory offset: " << zf.getCentralDirOffset() << std::endl;
    os << "comment: " << zf.getComment() << std::endl;
    for (ZipFile::Iterator p = zf.begin(); p != zf.end(); ++p)
    {
        ZipEntry* entry = *p;
        os << *entry << std::endl;
    }
    return os;
}
}
