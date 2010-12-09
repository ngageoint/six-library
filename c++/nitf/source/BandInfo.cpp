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

#include "nitf/BandInfo.hpp"

using namespace nitf;

BandInfo::BandInfo(const BandInfo & x)
{
    setNative(x.getNative());
}

BandInfo & BandInfo::operator=(const BandInfo & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

BandInfo::BandInfo(nitf_BandInfo * x)
{
    setNative(x);
    getNativeOrThrow();
}

BandInfo::BandInfo()
{
    setNative(nitf_BandInfo_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

BandInfo::~BandInfo() {}

nitf::Field BandInfo::getRepresentation()
{
    return nitf::Field(getNativeOrThrow()->representation);
}

nitf::Field BandInfo::getSubcategory()
{
    return nitf::Field(getNativeOrThrow()->subcategory);
}

nitf::Field BandInfo::getImageFilterCondition()
{
    return nitf::Field(getNativeOrThrow()->imageFilterCondition);
}

nitf::Field BandInfo::getImageFilterCode()
{
    return nitf::Field(getNativeOrThrow()->imageFilterCode);
}

nitf::Field BandInfo::getNumLUTs()
{
    return nitf::Field(getNativeOrThrow()->numLUTs);
}

nitf::Field BandInfo::getBandEntriesPerLUT()
{
    return nitf::Field(getNativeOrThrow()->bandEntriesPerLUT);
}

nitf::LookupTable BandInfo::getLookupTable()
{
    if (!getNativeOrThrow()->lut)
    {
        //we'll create one - there's no harm in doing it
        getNativeOrThrow()->lut = nitf_LookupTable_construct(0, 0, &error);
        if (!getNativeOrThrow()->lut)
            throw nitf::NITFException(&error);
    }
    return nitf::LookupTable(getNativeOrThrow()->lut);
}

void BandInfo::init(const std::string& representation,
                    const std::string& subcategory,
                    const std::string& imageFilterCondition,
                    const std::string& imageFilterCode,
                    nitf::Uint32 numLUTs,
                    nitf::Uint32 bandEntriesPerLUT,
                    nitf::LookupTable& lut) throw(nitf::NITFException)
{
    if (getNativeOrThrow()->lut)
    {
        //release the owned lut
        nitf::LookupTable oldLut = nitf::LookupTable(getNativeOrThrow()->lut);
        oldLut.setManaged(false);
    }

    if (!nitf_BandInfo_init(getNativeOrThrow(),
                            representation.c_str(),
                            subcategory.c_str(),
                            imageFilterCondition.c_str(),
                            imageFilterCode.c_str(),
                            numLUTs,
                            bandEntriesPerLUT,
                            lut.getNative() ? lut.getNative() : NULL,
                            &error))
        throw nitf::NITFException(&error);


    //have the library manage the new lut
    lut.setManaged(true);
}

void BandInfo::init(const std::string& representation,
                    const std::string& subcategory,
                    const std::string& imageFilterCondition,
                    const std::string& imageFilterCode) throw(nitf::NITFException)
{
    if (getNativeOrThrow()->lut)
    {
        //release the owned lut
        nitf::LookupTable oldLut = nitf::LookupTable(getNativeOrThrow()->lut);
        oldLut.setManaged(false);
    }

    if (!nitf_BandInfo_init(getNativeOrThrow(),
                            representation.c_str(),
                            subcategory.c_str(),
                            imageFilterCondition.c_str(),
                            imageFilterCode.c_str(),
                            0, 0, NULL, &error))
        throw nitf::NITFException(&error);
}

