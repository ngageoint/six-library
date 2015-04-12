/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include "tiff/Utils.h"

bool tiff::Utils::hasGeoTiffIFD(tiff::IFD* ifd)
{
    return ifd->exists("GeoKeyDirectoryTag");
}

tiff::IFD* tiff::Utils::createGeoTiffIFD(tiff::IFD* ifd)
{
    if (!tiff::Utils::hasGeoTiffIFD(ifd))
        return NULL;

    std::map<unsigned short, std::string> keyMap;
    keyMap[1024] = "GTModelTypeGeoKey";
    keyMap[1025] = "GTRasterTypeGeoKey";
    keyMap[1026] = "GTCitationGeoKey";
    keyMap[2048] = "GeographicTypeGeoKey";
    keyMap[2049] = "GeogCitationGeoKey";
    keyMap[2050] = "GeogGeodeticDatumGeoKey";
    keyMap[2051] = "GeogPrimeMeridianGeoKey";
    keyMap[2061] = "GeogPrimeMeridianLongGeoKey";
    keyMap[2052] = "GeogLinearUnitsGeoKey";
    keyMap[2053] = "GeogLinearUnitSizeGeoKey";
    keyMap[2054] = "GeogAngularUnitsGeoKey";
    keyMap[2055] = "GeogAngularUnitSizeGeoKey";
    keyMap[2056] = "GeogEllipsoidGeoKey";
    keyMap[2057] = "GeogSemiMajorAxisGeoKey";
    keyMap[2058] = "GeogSemiMinorAxisGeoKey";
    keyMap[2059] = "GeogInvFlatteningGeoKey";
    keyMap[2060] = "GeogAzimuthUnitsGeoKey";
    keyMap[3072] = "ProjectedCSTypeGeoKey";
    keyMap[3073] = "PCSCitationGeoKey";
    keyMap[3074] = "ProjectionGeoKey";
    keyMap[3075] = "ProjCoordTransGeoKey";
    keyMap[3076] = "ProjLinearUnitsGeoKey";
    keyMap[3077] = "ProjLinearUnitSizeGeoKey";
    keyMap[3078] = "ProjStdParallel1GeoKey";
    keyMap[3079] = "ProjStdParallel2GeoKey";
    keyMap[3080] = "ProjNatOriginLongGeoKey";
    keyMap[3081] = "ProjNatOriginLatGeoKey";
    keyMap[3082] = "ProjFalseEastingGeoKey";
    keyMap[3083] = "ProjFalseNorthingGeoKey";
    keyMap[3084] = "ProjFalseOriginLongGeoKey";
    keyMap[3085] = "ProjFalseOriginLatGeoKey";
    keyMap[3086] = "ProjFalseOriginEastingGeoKey";
    keyMap[3087] = "ProjFalseOriginNorthingGeoKey";
    keyMap[3088] = "ProjCenterLongGeoKey";
    keyMap[3089] = "ProjCenterLatGeoKey";
    keyMap[3090] = "ProjCenterEastingGeoKey";
    keyMap[3091] = "ProjFalseOriginNorthingGeoKey";
    keyMap[3092] = "ProjScaleAtNatOriginGeoKey";
    keyMap[3093] = "ProjScaleAtCenterGeoKey";
    keyMap[3094] = "ProjAzimuthAngleGeoKey";
    keyMap[3095] = "ProjStraightVertPoleLongGeoKey";
    keyMap[2060] = "GeogAzimuthUnitsGeoKey";
    keyMap[4096] = "VerticalCSTypeGeoKey";
    keyMap[4097] = "VerticalCitationGeoKey";
    keyMap[4098] = "VerticalDatumGeoKey";
    keyMap[4099] = "VerticalUnitsGeoKey";

    tiff::IFD* geoIFD = new tiff::IFD;

    tiff::IFDEntry *geoDir = (*ifd)["GeoKeyDirectoryTag"];
    tiff::IFDEntry *doubleParams =
            ifd->exists("GeoDoubleParamsTag") ? (*ifd)["GeoDoubleParamsTag"]
                                              : NULL;
    tiff::IFDEntry *asciiParams =
            ifd->exists("GeoAsciiParamsTag") ? (*ifd)["GeoAsciiParamsTag"]
                                             : NULL;

    std::vector<tiff::TypeInterface*> geoVals = geoDir->getValues();
    size_t idx = 0;

    // 34736 == "GeoDoubleParamsTag"
    // 34737 == "GeoAsciiParamsTag"

    unsigned short keyDirVersion =
            str::toType<unsigned short>(geoVals[idx++]->toString());
    unsigned short keyRevision =
            str::toType<unsigned short>(geoVals[idx++]->toString());
    unsigned short keyRevisionMinor =
            str::toType<unsigned short>(geoVals[idx++]->toString());
    unsigned short numKeys =
            str::toType<unsigned short>(geoVals[idx++]->toString());

    for (unsigned short i = 0; i < numKeys; ++i)
    {
        unsigned short keyId =
                str::toType<unsigned short>(geoVals[idx++]->toString());
        unsigned short tiffTagLoc =
                str::toType<unsigned short>(geoVals[idx++]->toString());
        unsigned short count =
                str::toType<unsigned short>(geoVals[idx++]->toString());

        unsigned short entryType = tiff::Const::Type::SHORT;
        if (tiffTagLoc == 34736)
            entryType = tiff::Const::Type::DOUBLE;
        else if (tiffTagLoc == 34737)
            entryType = tiff::Const::Type::ASCII;

        std::string name = keyMap.find(keyId) != keyMap.end() ? keyMap[keyId]
                                                              : "";

        tiff::IFDEntry *entry = new tiff::IFDEntry(keyId, entryType, name);

        if (tiffTagLoc == 0)
        {
            entry->addValue(
                            new tiff::GenericType<unsigned short>(
                                                                  geoVals[idx++]->toString()));
        }
        else if (tiffTagLoc == 34736 && doubleParams)
        {
            unsigned short valueOffset =
                    str::toType<unsigned short>(geoVals[idx++]->toString());
            for (unsigned short j = 0; j < count; ++j)
            {
                entry->addValue(
                                new tiff::GenericType<double>(
                                                              (*doubleParams)[valueOffset
                                                                      + j]->toString()));
            }
        }
        else if (tiffTagLoc == 34737 && asciiParams)
        {
            unsigned short valueOffset =
                    str::toType<unsigned short>(geoVals[idx++]->toString());
            for (unsigned short j = 0; j < count; ++j)
            {
                entry->addValue(
                                new tiff::GenericType<std::string>(
                                                                   (*asciiParams)[valueOffset
                                                                           + j]->toString()));
            }
        }
        geoIFD->addEntry(entry);
    }

    return geoIFD;
}
