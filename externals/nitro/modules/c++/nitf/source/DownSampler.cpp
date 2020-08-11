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

#include "nitf/DownSampler.hpp"

uint32_t nitf::DownSampler::getRowSkip()
{
    return getNativeOrThrow()->rowSkip;
}

uint32_t nitf::DownSampler::getColSkip()
{
    return getNativeOrThrow()->colSkip;
}

void nitf::DownSampler::apply(NITF_DATA ** inputWindow,
        NITF_DATA ** outputWindow, uint32_t numBands,
        uint32_t numWindowRows, uint32_t numWindowCols,
        uint32_t numInputCols, uint32_t numSubWindowCols,
        uint32_t pixelType, uint32_t pixelSize,
        uint32_t rowsInLastWindow, uint32_t colsInLastWindow)
{
    nitf_DownSampler *ds = getNativeOrThrow();
    if (ds && ds->iface)
    {
        if (!ds->iface->apply(ds, inputWindow, outputWindow, numBands,
                              numWindowRows, numWindowCols, numInputCols,
                              numSubWindowCols, pixelType, pixelSize,
                              rowsInLastWindow, colsInLastWindow, &error))
            throw nitf::NITFException(&error);
    }
    else
    {
        throw except::NullPointerReference(Ctxt("DownSampler"));
    }
}

nitf::PixelSkip::PixelSkip(uint32_t rowSkip, uint32_t colSkip)
{
    setNative(nitf_PixelSkip_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::PixelSkip::~PixelSkip()
{
}

nitf::MaxDownSample::MaxDownSample(uint32_t rowSkip, uint32_t colSkip)
{
    setNative(nitf_MaxDownSample_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::MaxDownSample::~MaxDownSample()
{
}

nitf::SumSq2DownSample::SumSq2DownSample(uint32_t rowSkip,
        uint32_t colSkip)
{
    setNative(nitf_SumSq2DownSample_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::SumSq2DownSample::~SumSq2DownSample()
{
}

nitf::Select2DownSample::Select2DownSample(uint32_t rowSkip,
        uint32_t colSkip)
{
    setNative(nitf_Select2DownSample_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::Select2DownSample::~Select2DownSample()
{
}
