/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "nitf/SubWindow.hpp"

using namespace nitf;

SubWindow::SubWindow(const SubWindow & x)
{
    setNative(x.getNative());
}

SubWindow & SubWindow::operator=(const SubWindow & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

SubWindow::SubWindow(nitf_SubWindow * x)
{
    setNative(x);
    getNativeOrThrow();
}

SubWindow::SubWindow() throw(nitf::NITFException) : mDownSampler(NULL)
{
    setNative(nitf_SubWindow_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

SubWindow::~SubWindow()
{
    if (isValid() && getNative()->downsampler)
    {
        nitf::DownSampler ds(getNativeOrThrow()->downsampler);
        //decrement the current DownSampler
        ds.decRef();
    }
}

nitf::Uint32 SubWindow::getStartRow() const
{
    return getNativeOrThrow()->startRow;
}

void SubWindow::setStartRow(nitf::Uint32 value)
{
    getNativeOrThrow()->startRow = value;
}

nitf::Uint32 SubWindow::getNumRows() const
{
    return getNativeOrThrow()->numRows;
}

void SubWindow::setNumRows(nitf::Uint32 value)
{
    getNativeOrThrow()->numRows = value;
}

nitf::Uint32 SubWindow::getStartCol() const
{
    return getNativeOrThrow()->startCol;
}

void SubWindow::setStartCol(nitf::Uint32 value)
{
    getNativeOrThrow()->startCol = value;
}

nitf::Uint32 SubWindow::getNumCols() const
{
    return getNativeOrThrow()->numCols;
}

void SubWindow::setNumCols(nitf::Uint32 value)
{
    getNativeOrThrow()->numCols = value;
}

nitf::Uint32 SubWindow::getBandList(int i)
{
    return getNativeOrThrow()->bandList[i];
}

void SubWindow::setBandList(nitf::Uint32 * value)
{
    getNativeOrThrow()->bandList = (nitf_Uint32*)value;
}

nitf::Uint32 SubWindow::getNumBands() const
{
    return getNativeOrThrow()->numBands;
}

void SubWindow::setNumBands(nitf::Uint32 value)
{
    getNativeOrThrow()->numBands = value;
}

void SubWindow::setDownSampler(nitf::DownSampler* downSampler)
    throw (nitf::NITFException)
{
    if (getNativeOrThrow()->downsampler)
    {
        nitf::DownSampler ds(getNativeOrThrow()->downsampler);
        //decrement the current DownSampler
        ds.decRef();
    }

    //increment the reference for this DownSampler
    getNativeOrThrow()->downsampler = downSampler->getNative();
    downSampler->incRef();
    mDownSampler = downSampler;
}


nitf::DownSampler* SubWindow::getDownSampler() throw (nitf::NITFException)
{
    return mDownSampler;
}
