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

#include "zip/ZipEntry.h"

const static char* sZipFileMadeByStr[] = {
        "MS-DOS and OS/2 (FAT / VFAT / FAT32 file systems)", "Amiga",
        "OpenVMS", "UNIX", "VM/CMS", "Atari ST", "OS/2 H.P.F.S.", "Macintosh",
        "Z-System", "CP/M", "Windows NTFS", "MVS (OS/390 - Z/OS)", "VSE",
        "Acorn Risc", "VFAT", "alternative MVS", "BeOS", "Tandem", "OS/400",
        "OS/X (Darwin)", NULL };

namespace zip
{
void ZipEntry::inflate(sys::ubyte* out, sys::Size_T outLen, sys::ubyte* in,
        sys::Size_T inLen)
{
    z_stream zstream;
    memset(&zstream, 0, sizeof(zstream));
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.next_in = in;
    zstream.avail_in = inLen;
    zstream.next_out = (Bytef*) out;
    zstream.avail_out = outLen;
    zstream.data_type = Z_UNKNOWN;

    int zerr = inflateInit2(&zstream, -MAX_WBITS);
    if (zerr != Z_OK)
    {
        throw except::IOException(Ctxt(FmtX("inflateInit2 failed [%d]", zerr)));
    }

    // decompress
    zerr = ::inflate(&zstream, Z_FINISH);

    if (zerr != Z_STREAM_END)
    {
        throw except::IOException(Ctxt(FmtX(
                "inflate failed [%d]: wanted: %d, got: %lu", zerr,
                Z_STREAM_END, zstream.total_out)));
    }
    inflateEnd(&zstream);
}

const char* ZipEntry::getVersionMadeByString() const
{

    if (mVersionMadeBy >= 20)
        return NULL;

    return sZipFileMadeByStr[mVersionMadeBy];
}

sys::ubyte* ZipEntry::decompress()
{
    sys::ubyte* uncompressed = new sys::ubyte[mUncompressedSize];
    decompress(uncompressed, mUncompressedSize);
    return uncompressed;
}

void ZipEntry::decompress(sys::ubyte* out, sys::Size_T outLen)
{
    if (mCompressionMethod == COMP_STORED)
    {
        memcpy(out, mCompressedData, outLen);
    }
    else
    {
        inflate(out, outLen, mCompressedData, mCompressedSize);
    }
}

std::ostream& operator<<(std::ostream& os, const zip::ZipEntry& ze)
{
    const char* madeBy = ze.getVersionMadeByString();
    std::string asStr = "Unknown";
    if (madeBy != NULL)
    {
        asStr = madeBy;
    }

    os << "version made by: " << asStr << std::endl;
    os << "version to extract: " << ze.getVersionToExtract() << std::endl;
    os << "general purpose bits: " << ze.getGeneralPurposeBitFlag()
            << std::endl;
    os << "compression method: " << ze.getCompressionMethod() << std::endl;

    os << "last modified : " << ze.getLastModifiedTime() << std::endl;
    os << "last modified date: " << ze.getLastModifiedDate() << std::endl;
    os << "crc (hex): " << std::hex << ze.getCRC32() << std::endl;
    os << "internal attrs: " << ze.getInternalAttrs() << std::endl;
    os << "external attrs: " << ze.getExternalAttrs() << std::endl;
    os << "file name: " << ze.getFileName() << std::endl;
    os << "comment: " << ze.getFileComment() << std::endl;
    os << "compressed: " << ze.getCompressedSize() << std::endl;
    os << "uncompressed: " << ze.getUncompressedSize() << std::endl;
    return os;
}
}
