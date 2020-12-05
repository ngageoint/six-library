/* =========================================================================
 * This file is part of six.convert-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.convert-c++ is free software; you can redistribute it and/or modify
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
#include <six/convert/ConvertingReadControl.h>
#include <types/RowCol.h>

namespace six
{
namespace convert
{
ConvertingReadControl::ConvertingReadControl(
        const std::string& pluginPathname) :
    mPluginPathnames(std::vector<std::string>(1, pluginPathname))
{
}

ConvertingReadControl::ConvertingReadControl(
        const std::vector<std::string>& pluginPathnames) :
    mPluginPathnames(pluginPathnames)
{
}

DataType ConvertingReadControl::getDataType(const std::string& /*pathname*/) const
{
    // Only SICDs
    return DataType::COMPLEX;
}

void ConvertingReadControl::load(const std::string& pathname,
        const std::vector<std::string>& /*schemaPaths*/)
{
    mContainer.reset(new Container(getDataType(pathname)));
    mConverter = mRegistry.loadAndFind(mPluginPathnames, pathname);
    if (mConverter == nullptr)
    {
        throw except::Exception(Ctxt("Unable to load " + pathname));
    }
    mConverter->load(pathname);
    mContainer->addData(mConverter->convert().release());
}

std::string ConvertingReadControl::getFileType() const
{
    if (mConverter == nullptr)
    {
        throw except::Exception(Ctxt("Please load ConvertingReadControl "
                "before calling getFileType()"));
    }
    return mConverter->getFileType();
}

UByte* ConvertingReadControl::interleaved(size_t imageNumber)
{
    Region region;
    region.setStartRow(0);
    region.setStartCol(0);
    region.setNumRows(-1);
    region.setNumCols(-1);
    return interleaved(region, imageNumber);
}

UByte* ConvertingReadControl::interleaved(Region& region, size_t imageNumber)
{
    if (mConverter == nullptr)
    {
        throw except::Exception(Ctxt("Please load ConvertingReadControl "
                "before calling interleaved()"));
    }
    const Data* data = mContainer->getData(imageNumber);
    if (region.getNumRows() == -1)
    {
        region.setNumRows(data->getNumRows());
    }
    if (region.getNumCols() == -1)
    {
        region.setNumCols(data->getNumCols());
    }

    const types::RowCol<size_t> extent(
            region.getStartRow() + region.getNumRows(),
            region.getStartCol() + region.getNumCols());

    if (extent.row > data->getNumRows() ||
            static_cast<size_t>(region.getStartRow()) > data->getNumRows())
    {
        throw except::Exception(Ctxt("Too many rows requested [" +
                str::toString(region.getNumRows()) + "]"));
    }
    if (extent.col > data->getNumCols() ||
            static_cast<size_t>(region.getStartCol()) > data->getNumCols())
    {
        throw except::Exception(Ctxt("Too many cols requested [" +
                str::toString(region.getNumCols()) + "]"));
    }

    UByte* buffer = region.getBuffer();
    if (buffer == nullptr)
    {
        buffer = new UByte[region.getNumRows() * region.getNumCols() *
                data->getNumBytesPerPixel()];
        region.setBuffer(buffer);
    }

    const types::RowCol<size_t> startingLocation(region.getStartRow(),
            region.getStartCol());
    const types::RowCol<size_t> readLength(region.getNumRows(),
            region.getNumCols());
    mConverter->readData(startingLocation, readLength, buffer);
    return buffer;
}
}
}

