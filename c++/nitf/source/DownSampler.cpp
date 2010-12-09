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

#include "nitf/DownSampler.hpp"

nitf::Uint32 nitf::DownSampler::getRowSkip()
{
    return getNativeOrThrow()->rowSkip;
}

nitf::Uint32 nitf::DownSampler::getColSkip()
{
    return getNativeOrThrow()->colSkip;
}

void nitf::DownSampler::apply(NITF_DATA ** inputWindow,
        NITF_DATA ** outputWindow, nitf::Uint32 numBands,
        nitf::Uint32 numWindowRows, nitf::Uint32 numWindowCols,
        nitf::Uint32 numInputCols, nitf::Uint32 numSubWindowCols,
        nitf::Uint32 pixelType, nitf::Uint32 pixelSize,
        nitf::Uint32 rowsInLastWindow, nitf::Uint32 colsInLastWindow)
        throw (nitf::NITFException)
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

nitf::PixelSkip::PixelSkip(nitf::Uint32 rowSkip, nitf::Uint32 colSkip)
        throw (nitf::NITFException)
{
    setNative(nitf_PixelSkip_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::PixelSkip::~PixelSkip()
{
}

nitf::MaxDownSample::MaxDownSample(nitf::Uint32 rowSkip, nitf::Uint32 colSkip)
        throw (nitf::NITFException)
{
    setNative(nitf_MaxDownSample_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::MaxDownSample::~MaxDownSample()
{
}

nitf::SumSq2DownSample::SumSq2DownSample(nitf::Uint32 rowSkip,
        nitf::Uint32 colSkip) throw (nitf::NITFException)
{
    setNative(nitf_SumSq2DownSample_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::SumSq2DownSample::~SumSq2DownSample()
{
}

nitf::Select2DownSample::Select2DownSample(nitf::Uint32 rowSkip,
        nitf::Uint32 colSkip) throw (nitf::NITFException)
{
    setNative(nitf_Select2DownSample_construct(rowSkip, colSkip, &error));
    setManaged(false);
}

nitf::Select2DownSample::~Select2DownSample()
{
}
