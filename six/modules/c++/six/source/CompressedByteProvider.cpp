/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <six/ByteProvider.h>
#include <six/CompressedByteProvider.h>

namespace six
{
void CompressedByteProvider::initialize(
        mem::SharedPtr<Container> container,
        const XMLControlRegistry& xmlRegistry,
        const std::vector<std::string>& schemaPaths,
        size_t maxProductSize,
        size_t numRowsPerBlock,
        size_t numColsPerBlock)
{
    NITFWriteControl writer;
    six::ByteProvider::populateWriter(container, xmlRegistry,
            maxProductSize, numRowsPerBlock, numColsPerBlock, writer);

    initialize(writer, schemaPaths);
}

void CompressedByteProvider::initialize(
        const NITFWriteControl& writer,
        const std::vector<std::string>& schemaPaths)
{
    std::vector<PtrAndLength> desData;
    size_t numRowsPerBlock;
    size_t numColsPerBlock;
    six::ByteProvider::populateInitArgs(
            writer,
            schemaPaths,
            desData,
            numRowsPerBlock,
            numColsPerBlock);

    // Do the full initialization
    nitf::Record record = writer.getRecord();
    nitf::ByteProvider::initialize(record,
                                   desData,
                                   numRowsPerBlock,
                                   numColsPerBlock);
}
}
