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

BandInfo::BandInfo(const Representation& representation) : BandInfo()
{
    this->representation = representation;
}
nitf::Field BandInfo::getRepresentation() const
{
    return nitf::Field(getNativeOrThrow()->representation);
}

BandInfo::BandInfo(const Subcategory& subcategory) : BandInfo()
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

// operator[] doesn't work with a "const" map as it can also insert
template<typename TKey, typename TValue>
static TValue index(const std::map<TKey, TValue>& map, TKey k)
{
    const auto it = map.find(k);
    if (it == map.end())
    {
        throw std::invalid_argument("key not found in map.");
    }
    return it->second;
}

template<typename TKey, typename TValue>
static std::map<TValue, TKey> swap_key_value(const std::map<TKey, TValue>& map)
{
    std::map<TValue, TKey> retval;
    for (const auto& p : map)
    {
        retval[p.second] = p.first;
    }
    return retval;
}

// Don't bother with checking lower-case; nobody should be passing
// an "r" directly to this routine, should always be the result of R.to_string()
#define NITF_string_to_Represenation_map_entry(name) { #name, Representation::name }
static const std::map<std::string, Representation> string_to_Represenation
{
   NITF_string_to_Represenation_map_entry(R),
   NITF_string_to_Represenation_map_entry(G),
   NITF_string_to_Represenation_map_entry(B),
   NITF_string_to_Represenation_map_entry(M),
   NITF_string_to_Represenation_map_entry(LU),
};
std::string nitf::to_string(Representation v)
{
    static const auto to_string_map = swap_key_value(string_to_Represenation);
    return index(to_string_map, v);
}
template<> nitf::Representation nitf::from_string(const std::string& v)
{
    return index(string_to_Represenation, v);
}

#define NITF_string_to_Subcategory_map_entry(name) { #name, Subcategory::name }
static const std::map<std::string, Subcategory> string_to_Subcategory
{
   NITF_string_to_Subcategory_map_entry(I),
   NITF_string_to_Subcategory_map_entry(Q),
};
std::string nitf::to_string(Subcategory v)
{
    static const auto to_string_map = swap_key_value(string_to_Subcategory);
    return index(to_string_map, v);
}
template<> nitf::Subcategory nitf::from_string(const std::string& v)
{
    return index(string_to_Subcategory, v);
}
