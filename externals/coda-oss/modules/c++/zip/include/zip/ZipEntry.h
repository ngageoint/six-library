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

#ifndef __ZIP_ZIP_ENTRY_H__
#define __ZIP_ZIP_ENTRY_H__

#include "zip/Types.h"

namespace zip
{
/*!
 *  \class ZipEntry
 *  \brief Each entry in a ZipFile is a ZipEntry
 *
 *  Class stores the information about individual elements
 *  in a PKZIP zip file
 */
class ZipEntry
{
    enum CompressionMethod
    {
        COMP_STORED = 0, COMP_DEFLATED = 8
    };

    sys::ubyte* mCompressedData;
    sys::Size_T mCompressedSize;
    sys::Size_T mUncompressedSize;
    std::string mFileName;
    std::string mFileComment;

    sys::Uint16_T mVersionMadeBy;
    sys::Uint16_T mVersionToExtract;
    sys::Uint16_T mGeneralPurposeBitFlag;
    sys::Uint16_T mCompressionMethod;
    sys::Uint16_T mLastModifiedTime;
    sys::Uint16_T mLastModifiedDate;
    sys::Uint32_T mCRC32;
    sys::Uint16_T mInternalAttrs;
    sys::Uint32_T mExternalAttrs;

    static void inflate(sys::ubyte* out, sys::Size_T outLen, sys::ubyte* in,
            sys::Size_T inLen);

public:

    ZipEntry(sys::ubyte* compressedData, sys::Size_T compressedSize,
            sys::Size_T uncompressedSize, std::string fileName,
            std::string fileComment, sys::Uint16_T versionMadeBy,
            sys::Uint16_T versionToExtract,
            sys::Uint16_T generalPurposeBitFlag,
            sys::Uint16_T compressionMethod, sys::Uint16_T lastModifiedTime,
            sys::Uint16_T lastModifiedDate, sys::Uint32_T crc32,
            sys::Uint16_T internalAttrs, sys::Uint32_T externalAttrs) :
        mCompressedData(compressedData), mCompressedSize(compressedSize),
                mUncompressedSize(uncompressedSize), mFileName(fileName),
                mFileComment(fileComment), mVersionMadeBy(versionMadeBy),
                mVersionToExtract(versionToExtract), mGeneralPurposeBitFlag(
                        generalPurposeBitFlag), mCompressionMethod(
                        compressionMethod),
                mLastModifiedTime(lastModifiedTime), mLastModifiedDate(
                        lastModifiedDate), mCRC32(crc32), mInternalAttrs(
                        internalAttrs), mExternalAttrs(externalAttrs)
    {
    }

    ~ZipEntry()
    {
    }

    sys::ubyte* decompress();
    void decompress(sys::ubyte* out, sys::Size_T outLen);

    sys::Uint16_T getVersionMadeBy() const
    {
        return mVersionMadeBy;
    }
    const char* getVersionMadeByString() const;

    sys::Uint16_T getVersionToExtract() const
    {
        return mVersionToExtract;
    }
    sys::Uint16_T getGeneralPurposeBitFlag() const
    {
        return mGeneralPurposeBitFlag;
    }
    sys::Uint16_T getCompressionMethod() const
    {
        return mCompressionMethod;
    }
    sys::Uint16_T getLastModifiedTime() const
    {
        return mLastModifiedTime;
    }

    sys::Uint16_T getLastModifiedDate() const
    {
        return mLastModifiedDate;
    }
    sys::Uint32_T getCRC32() const
    {
        return mCRC32;
    }
    sys::Uint16_T getInternalAttrs() const
    {
        return mInternalAttrs;
    }
    sys::Uint32_T getExternalAttrs() const
    {
        return mExternalAttrs;
    }

    std::string getFileName() const
    {
        return mFileName;
    }
    std::string getFileComment() const
    {
        return mFileComment;
    }
    sys::Size_T getUncompressedSize() const
    {
        return mUncompressedSize;
    }
    sys::Size_T getCompressedSize() const
    {
        return mCompressedSize;
    }
};

}

/*!
 *  Output stream overload
 */
std::ostream& operator<<(std::ostream& os, const zip::ZipEntry& ze);

#endif

