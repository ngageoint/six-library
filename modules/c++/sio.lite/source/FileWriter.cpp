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
#include "sio/lite/FileWriter.h"

void sio::lite::FileWriter::write(sio::lite::FileHeader* header, std::vector<io::InputStream*> bandStreams)
{
    header->to(bandStreams.size(), *mStream); //write header
    //write band-sequential
    for (std::vector<io::InputStream*>::iterator iter = bandStreams.begin(), end = bandStreams.end();
            iter != end; ++iter)
        (*iter)->streamTo(*mStream);
}


void sio::lite::FileWriter::write(int numLines, int numElements, int elementSize,
                                  int elementType, std::vector<io::InputStream*> bandStreams)
{
    sio::lite::FileHeader hdr(numLines, numElements, elementSize, elementType, 1);
    write(&hdr, bandStreams);
}


void sio::lite::FileWriter::write(sio::lite::FileHeader* header,
                                  const void* data,
                                  int numBands)
{
    header->to(numBands, *mStream); //write header
    mStream->write(static_cast<const sys::byte*>(data),
                   static_cast<size_t>(header->getNumLines()) *
                            static_cast<size_t>(header->getNumElements()) *
                            static_cast<size_t>(header->getElementSize()) *
                            static_cast<size_t>(numBands));
}

void sio::lite::FileWriter::write(int numLines, int numElements, int elementSize,
                                  int elementType, const void* data, int numBands)
{
    sio::lite::FileHeader hdr(numLines, numElements, elementSize, elementType);
    write(&hdr, data, numBands);
}

