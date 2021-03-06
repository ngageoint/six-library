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

#include "nitf/BlockingInfo.hpp"

using namespace nitf;

BlockingInfo::BlockingInfo(const BlockingInfo & x)
{
    setNative(x.getNative());
}

BlockingInfo & BlockingInfo::operator=(const BlockingInfo & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

BlockingInfo::BlockingInfo(nitf_BlockingInfo * x)
{
    setNative(x);
    getNativeOrThrow();
}

BlockingInfo::BlockingInfo()
{
    setNative(nitf_BlockingInfo_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

uint32_t BlockingInfo::getNumBlocksPerRow() const
{
    return getNativeOrThrow()->numBlocksPerRow;
}

void BlockingInfo::setNumBlocksPerRow(uint32_t value)
{
    getNativeOrThrow()->numBlocksPerRow = value;
}

uint32_t BlockingInfo::getNumBlocksPerCol() const
{
    return getNativeOrThrow()->numBlocksPerCol;
}

void BlockingInfo::setNumBlocksPerCol(uint32_t value)
{
    getNativeOrThrow()->numBlocksPerCol = value;
}

uint32_t BlockingInfo::getNumRowsPerBlock() const
{
    return getNativeOrThrow()->numRowsPerBlock;
}

void BlockingInfo::setNumRowsPerBlock(uint32_t value)
{
    getNativeOrThrow()->numRowsPerBlock = value;
}

uint32_t BlockingInfo::getNumColsPerBlock() const
{
    return getNativeOrThrow()->numColsPerBlock;
}

void BlockingInfo::setNumColsPerBlock(uint32_t value)
{
    getNativeOrThrow()->numColsPerBlock = value;
}

size_t BlockingInfo::getLength() const
{
    return getNativeOrThrow()->length;
}

void BlockingInfo::setLength(size_t value)
{
    getNativeOrThrow()->length = value;
}

void BlockingInfo::print(FILE *file)
{
    nitf_BlockingInfo_print(getNativeOrThrow(), file);
}
