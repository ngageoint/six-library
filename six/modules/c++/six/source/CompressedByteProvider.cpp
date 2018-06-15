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

namespace
{
size_t countCompresedBytes(
        const std::vector<std::vector<size_t> >& bytesPerBlock)
{
    size_t sum = 0;
    for (size_t ii = 0; ii < bytesPerBlock.size(); ++ii)
    {
        for (size_t jj = 0; jj < bytesPerBlock[ii].size(); ++jj)
        {
            sum += bytesPerBlock[ii][jj];
        }
    }
    return sum;
}
size_t countUncompressedPixels(const six::Data& data)
{
    return data.getNumRows() * data.getNumCols();
}
}

namespace six
{
void CompressedByteProvider::initialize(
        mem::SharedPtr<Container> container,
        const XMLControlRegistry& xmlRegistry,
        const std::vector<std::string>& schemaPaths,
        const std::vector<std::vector<size_t> >& bytesPerBlock,
        size_t compressionRatio,
        size_t maxProductSize,
        size_t numRowsPerBlock,
        size_t numColsPerBlock)
{
    NITFWriteControl writer;
    const double bitrate =
            (countCompresedBytes(bytesPerBlock) * 8 /
             countUncompressedPixels(*container->getData(0)));
    writer.getOptions().setParameter(
            six::NITFWriteControl::OPT_J2K_COMPRESSION_BITRATE, bitrate);
    writer.getOptions().setParameter(
            six::NITFWriteControl::OPT_J2K_COMPRESSION_LOSSLESS,
            compressionRatio == 1);
    six::ByteProvider::populateWriter(container, xmlRegistry,
            maxProductSize, numRowsPerBlock, numColsPerBlock, writer);

    initialize(writer, schemaPaths, bytesPerBlock);
}

void CompressedByteProvider::initialize(
        const NITFWriteControl& writer,
        const std::vector<std::string>& schemaPaths,
        const std::vector<std::vector<size_t> >& bytesPerBlock)

{
    std::vector<std::string> xmlStrings;
    std::vector<PtrAndLength> desData;
    size_t numRowsPerBlock;
    size_t numColsPerBlock;
    six::ByteProvider::populateInitArgs(
            writer,
            schemaPaths,
            xmlStrings,
            desData,
            numRowsPerBlock,
            numColsPerBlock);

    // Do the full initialization
    nitf::Record record = writer.getRecord();
    nitf::CompressedByteProvider::initialize(
            record,
            bytesPerBlock,
            desData,
            numRowsPerBlock,
            numColsPerBlock);
}
}
