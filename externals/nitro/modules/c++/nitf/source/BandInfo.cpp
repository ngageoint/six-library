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

#include "nitf/BandInfo.hpp"

#include <stdexcept>
#include <map>

using namespace nitf;

BandInfo::BandInfo(const BandInfo & x)
{
    *this = x;
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

BandInfo::BandInfo() noexcept(false) : BandInfo(nitf_BandInfo_construct(&error))
{
    setManaged(false);
}

BandInfo::BandInfo(Representation representation) : BandInfo()
{
    this->representation = representation;
}
nitf::Field BandInfo::getRepresentation() const
{
    return nitf::Field(getNativeOrThrow()->representation);
}

BandInfo::BandInfo(Subcategory subcategory) : BandInfo()
{
    getSubcategory().set(to_string(subcategory));
}
nitf::Field BandInfo::getSubcategory() const
{
    return nitf::Field(getNativeOrThrow()->subcategory);
}

nitf::Field BandInfo::getImageFilterCondition() const
{
    return nitf::Field(getNativeOrThrow()->imageFilterCondition);
}

nitf::Field BandInfo::getImageFilterCode() const
{
    return nitf::Field(getNativeOrThrow()->imageFilterCode);
}

nitf::Field BandInfo::getNumLUTs() const
{
    return nitf::Field(getNativeOrThrow()->numLUTs);
}

nitf::Field BandInfo::getBandEntriesPerLUT() const
{
    return nitf::Field(getNativeOrThrow()->bandEntriesPerLUT);
}

nitf::LookupTable BandInfo::getLookupTable() const
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

static NITF_BOOL BandInfo_init(nitf_BandInfo* bandInfo,
    const std::string& representation,
    const std::string& subcategory,
    const std::string& imageFilterCondition,
    const std::string& imageFilterCode,
    uint32_t numLUTs,
    uint32_t bandEntriesPerLUT,
    nitf_LookupTable* lut,
    nitf_Error& error) noexcept
{
    return nitf_BandInfo_init(bandInfo,
        representation.c_str(),
        subcategory.c_str(),
        imageFilterCondition.c_str(),
        imageFilterCode.c_str(),
        numLUTs,
        bandEntriesPerLUT,
        lut,
        &error);
}

void BandInfo::init(const std::string& representation_,
                    const std::string& subcategory_,
                    const std::string& imageFilterCondition,
                    const std::string& imageFilterCode,
                    uint32_t numLUTs,
                    uint32_t bandEntriesPerLUT,
                    nitf::LookupTable& lut)
{
    if (getNativeOrThrow()->lut)
    {
        //release the owned lut
        nitf::LookupTable oldLut = nitf::LookupTable(getNativeOrThrow()->lut);
        oldLut.setManaged(false);
    }

    if (!BandInfo_init(getNativeOrThrow(),
                            representation_,
                            subcategory_,
                            imageFilterCondition,
                            imageFilterCode,
                            numLUTs,
                            bandEntriesPerLUT,
                            lut.getNative() ? lut.getNative() : nullptr,
                            error))
        throw nitf::NITFException(&error);


    //have the library manage the new lut
    lut.setManaged(true);
}
void BandInfo::init(const Representation& representation_, // C4458: declaration of '...' hides class member
                    const std::string& subcategory_, // C4458: declaration of '...' hides class member
                    const std::string& imageFilterCondition,
                    const std::string& imageFilterCode,
                    uint32_t numLUTs,
                    uint32_t bandEntriesPerLUT,
                    nitf::LookupTable& lut)
{
    init(to_string(representation_), subcategory_, imageFilterCondition, imageFilterCode, numLUTs, bandEntriesPerLUT, lut);
}

void BandInfo::init(const std::string& representation_, // C4458: declaration of '...' hides class member
                    const std::string& subcategory_, // C4458: declaration of '...' hides class member
                    const std::string& imageFilterCondition,
                    const std::string& imageFilterCode)
{
    if (getNativeOrThrow()->lut)
    {
        //release the owned lut
        nitf::LookupTable oldLut = nitf::LookupTable(getNativeOrThrow()->lut);
        oldLut.setManaged(false);
    }

    if (!BandInfo_init(getNativeOrThrow(),
                            representation_,
                            subcategory_,
                            imageFilterCondition,
                            imageFilterCode,
                            0, 0, nullptr, error))
        throw nitf::NITFException(&error);
}
void BandInfo::init(const Representation& representation_, // C4458: declaration of '...' hides class member
                    const std::string& subcategory_, // C4458: declaration of '...' hides class member
                    const std::string& imageFilterCondition,
                    const std::string& imageFilterCode)
{
    init(to_string(representation_), subcategory_, imageFilterCondition, imageFilterCode);
}

