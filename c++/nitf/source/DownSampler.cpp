/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

extern "C" NITF_BOOL __nitf_DownSampler_apply(nitf_DownSampler* userData,
                                              NITF_DATA ** inputWindow,
                                              NITF_DATA ** outputWindow,
                                              nitf_Uint32 numBands,
                                              nitf_Uint32 numWindowRows,
                                              nitf_Uint32 numWindowCols,
                                              nitf_Uint32 numInputCols,
                                              nitf_Uint32 numSubWindowCols,
                                              nitf_Uint32 pixelType,
                                              nitf_Uint32 pixelSize,
                                              nitf_Uint32 rowsInLastWindow,
                                              nitf_Uint32 colsInLastWindow,
                                              nitf_Error* error)
{
    // Get our object from the data and call the read function
    if (!userData) throw except::NullPointerReference(Ctxt("DownSampler_read"));
    ((nitf::DownSampler*)(userData->data))->apply(inputWindow,
                                                  outputWindow,
                                                  numBands,
                                                  numWindowRows,
                                                  numWindowCols,
                                                  numInputCols,
                                                  numSubWindowCols,
                                                  pixelType,
                                                  pixelSize,
                                                  rowsInLastWindow,
                                                  colsInLastWindow);
    return true;
}

extern "C" void __nitf_DownSampler_destruct(NITF_DATA* data){}

nitf::DownSampler::DownSampler(nitf::Uint32 rowSkip, nitf::Uint32 colSkip) throw (nitf::NITFException)
{
    static nitf_IDownSampler iDownSampler = getIDownSampler();

    // Create the dummy handle
    nitf_DownSampler * downSampler = (nitf_DownSampler*)NITF_MALLOC(sizeof(nitf_DownSampler));
    setNative(downSampler);
    if (!isValid())
        throw nitf::NITFException(Ctxt("Could not create down sampler"));

    // Attach 'this' as the data, which will be the data
    // for the DownSampler_read function
    downSampler->data = this;
    downSampler->rowSkip = rowSkip;
    downSampler->colSkip = colSkip;
    downSampler->iface = &iDownSampler;

    setManaged(false);
}

nitf::Uint32 nitf::DownSampler::getRowSkip()
{
    return getNativeOrThrow()->rowSkip;
}

nitf::Uint32 nitf::DownSampler::getColSkip()
{
    return getNativeOrThrow()->colSkip;
}

nitf::KnownDownSampler::KnownDownSampler(nitf::Uint32 rowSkip,
        nitf::Uint32 colSkip)
        : mData(NULL), mIface(NULL){}

nitf::KnownDownSampler::~KnownDownSampler(){}

void nitf::KnownDownSampler::apply(NITF_DATA ** inputWindow,
                                   NITF_DATA ** outputWindow,
                                   nitf::Uint32 numBands,
                                   nitf::Uint32 numWindowRows,
                                   nitf::Uint32 numWindowCols,
                                   nitf::Uint32 numInputCols,
                                   nitf::Uint32 numSubWindowCols,
                                   nitf::Uint32 pixelType,
                                   nitf::Uint32 pixelSize,
                                   nitf::Uint32 rowsInLastWindow,
                                   nitf::Uint32 colsInLastWindow) throw (nitf::NITFException)
{
    if (mIface)
    {
        NITF_BOOL x = mIface->apply(getNativeOrThrow(),
                                    inputWindow,
                                    outputWindow,
                                    numBands,
                                    numWindowRows,
                                    numWindowCols,
                                    numInputCols,
                                    numSubWindowCols,
                                    pixelType,
                                    pixelSize,
                                    rowsInLastWindow,
                                    colsInLastWindow,
                                    &error);
        if (!x)
            throw nitf::NITFException(&error);
    }
    else
    {
        throw except::NullPointerReference(Ctxt("KnownDownSampler"));
    }
}


nitf::PixelSkip::PixelSkip(nitf::Uint32 rowSkip,
                           nitf::Uint32 colSkip) throw (nitf::NITFException)
        : nitf::KnownDownSampler(rowSkip, colSkip)
{
    setNative(nitf_PixelSkip_construct(rowSkip, colSkip, &error));
    getNativeOrThrow();

    static nitf_IDownSampler iDownSampler = getIDownSampler();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iDownSampler;
}

nitf::PixelSkip::~PixelSkip(){}

nitf::MaxDownSample::MaxDownSample(nitf::Uint32 rowSkip,
                                   nitf::Uint32 colSkip) throw (nitf::NITFException)
        : nitf::KnownDownSampler(rowSkip, colSkip)
{
    setNative(nitf_MaxDownSample_construct(rowSkip, colSkip, &error));
    getNativeOrThrow();

    static nitf_IDownSampler iDownSampler = getIDownSampler();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iDownSampler;
}

nitf::MaxDownSample::~MaxDownSample(){}

nitf::SumSq2DownSample::SumSq2DownSample(nitf::Uint32 rowSkip,
        nitf::Uint32 colSkip) throw (nitf::NITFException)
        : nitf::KnownDownSampler(rowSkip, colSkip)
{
    setNative(nitf_SumSq2DownSample_construct(rowSkip, colSkip, &error));
    getNativeOrThrow();

    static nitf_IDownSampler iDownSampler = getIDownSampler();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iDownSampler;
}

nitf::SumSq2DownSample::~SumSq2DownSample(){}

nitf::Select2DownSample::Select2DownSample(nitf::Uint32 rowSkip,
        nitf::Uint32 colSkip) throw (nitf::NITFException)
        : nitf::KnownDownSampler(rowSkip, colSkip)
{
    setNative(nitf_Select2DownSample_construct(rowSkip, colSkip, &error));
    getNativeOrThrow();

    static nitf_IDownSampler iDownSampler = getIDownSampler();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iDownSampler;
}

nitf::Select2DownSample::~Select2DownSample(){}
