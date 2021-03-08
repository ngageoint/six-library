/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
#include "sio/lite/StreamReader.h"


union _IntBuffer
{
    int iVal;
    sys::byte bVal[4];
};

int sio::lite::StreamReader::getNextInteger()
{
    if (header == NULL)
        throw
        sio::lite::InvalidHeaderException(
            Ctxt("Header == null")
        );

    union _IntBuffer buf;
    inputStream->read(buf.bVal, 4);

    if (header->isDifferentByteOrdering() )
    {
        sys::byteSwap(buf.bVal, 4, 1);
    }
    return buf.iVal;//(int) ( b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3] );
}

void sio::lite::StreamReader::checkMagic(bool calledFromConstructor)
{
    // Determine whether our platform is big or
    // little endian
    bool bigEndian = sys::isBigEndianSystem();
    // Read the first four bytes
    unsigned char b[4];
    inputStream->read((sys::byte*)b, 4);

    // We are big endian
    if (b[0] == 0xFF && b[2] == 0x7F && (b[1] ^ b[3]) == 0xFF)
    {
        // If is bigEndian is:
        // true: we are NOT a different byte ordering
        //       so differentByteOrdering is 0
        header->setIsDifferentByteOrdering(!bigEndian);
        header->setVersion(b[1]);
        dbg_printf("Detected big-endian image\n");
    }
    // We are little endian
    else if (b[3] == 0xFF && b[1] == 0x7F && (b[0] ^ b[2]) == 0xFF)
    {
        // If system is big endian, sure enough, different ordering
        header->setIsDifferentByteOrdering(bigEndian);
        header->setVersion(b[2]);
        dbg_printf("Detected little-endian image\n");
    }
    else
    {
        // If called from the constructor, we don't want to leak these
        if (calledFromConstructor)
        {
            killHeader();
            killStream();
        }

        throw
        sio::lite::InvalidHeaderException(
            Ctxt("Invalid magic header byte")
        );
    }
    dbg_printf("Detected header version: %d\n", header->getVersion() );
}

void sio::lite::StreamReader::killHeader()
{
    if (header)
        delete header;
    header = NULL;
    // Reset length so we always have a correct value
    headerLength = 0;
}

void sio::lite::StreamReader::killStream()
{
    if (inputStream && own)
    {
        delete inputStream;
    }
}

void sio::lite::StreamReader::parseHeader(bool calledFromConstructor)
{
    killHeader();
    header = new FileHeader();
    checkMagic(calledFromConstructor);

    header->setNumLines( getNextInteger() );
    header->setNumElements( getNextInteger() );
    header->setElementType( getNextInteger() );
    header->setElementSize( getNextInteger() );

    if (header->getVersion() >= 2)
        readType2Header();

    if (header->getVersion() > 2)
        dbg_printf("Warning: header version is [%d]\n",
                   header->getVersion() );

    // Cache this for seek speed
    headerLength = header->getLength();
}

void sio::lite::StreamReader::setInputStream(io::InputStream* is, bool adopt)
{
    killStream();
    inputStream = is;
    own = adopt;
    parseHeader();
}

void sio::lite::StreamReader::readType2Header()
{
    int numUDEntries = getNextInteger();

    for (int i = 0; i < numUDEntries; i++)
    {
        // Read the id size
        int idSize = getNextInteger();

        sys::byte *tmpBytes = new sys::byte[idSize];
        inputStream->read(tmpBytes, idSize);
        std::string id((const char*)tmpBytes);
        header->setNullTerminationFlag(tmpBytes[idSize-1] == 0x00);
        delete [] tmpBytes;

        int udSize = getNextInteger();
        // This is what we are storing in the hash table
        std::vector<sys::byte> udEntry;
        udEntry.reserve(udSize);
        for (int j = 0; j < udSize; j++)
        {
            sys::byte x;
            inputStream->read(&x, 1);
            udEntry.push_back(x);
        }
        header->getUserDataSection().add(id, udEntry);
    }
}

