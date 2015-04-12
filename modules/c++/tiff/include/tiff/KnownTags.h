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

#ifndef __TIFF_KNOWN_TAGS_H__
#define __TIFF_KNOWN_TAGS_H__

#include <map>
#include <string>
#include <import/mt.h>

#include "tiff/IFDEntry.h"

namespace tiff
{

/**
 * This class is a container for known TIFF tags.
 */
class KnownTags
{
public:

    KnownTags();

    //! Destructor
    ~KnownTags();

    /**
     *****************************************************************
     * Returns the IFDEntry associated with the specified
     * string.
     *
     * @param nameKey
     *   the key to look up in the map, based on the name of the
     *   IFDEntry (i.e. "ImageWidth")
     * @return
     *   the IFDEntry associated with the name
     *****************************************************************/
    tiff::IFDEntry *operator[](const std::string& nameKey);

    /**
     *****************************************************************
     * Returns the IFDEntry associated with the specified
     * short.
     *
     * @param nameKey
     *   the key to look up in the map, based on the TIFF tag 
     *   identifier of the IFDEntry (i.e. 256)
     * @return
     *   the IFDEntry associated with the identifier
     *****************************************************************/
    tiff::IFDEntry *operator[](const unsigned short tagKey);

    //! Some common tags
    static const char IMAGE_WIDTH[];
    static const char IMAGE_LENGTH[];
    static const char BITS_PER_SAMPLE[];
    static const char COMPRESSION[];
    static const char SAMPLES_PER_PIXEL[];
    static const char PHOTOMETRIC_INTERPRETATION[];
    static const char SAMPLE_FORMAT[];


    /**
     *****************************************************************
     * Adds a IFDEntry with the specified values to the 
     * KnownTagsRegsitry.
     *
     * @param tag
     *   the TIFF tag identifier to add as (i.e. 256)
     * @param type
     *   the TIFF type of the IFDEntry
     * @param name
     *   the name identifier to add as (i.e. "ImageWidth")
     *****************************************************************/
    void addEntry(const unsigned short tag, const unsigned short type,
            const std::string& name);

private:

    //! The name to tag identifier map
    std::map<std::string, unsigned short> mNameMap;

    //! The tag identifier to IFDEntry map
    std::map<unsigned short, tiff::IFDEntry*> mKnownTags;
};

/**
 *********************************************************************
 * @class KnownTagsRegistry
 * @brief Contains a globally accessable listing of all possible IFD
 * entries supported in the TIFF 6.0 specification.
 *
 * This class contains all TIFF IFD entries that are in the TIFF 6.0 
 * specification.  This includes the TIFF type of each tag as well
 * as a string that names the tag (i.e. "ImageWidth").  The entries
 * are globally allocated the first time the KnownTagsRegistry is 
 * instantiated.  There are only two functions defined, both are
 * the [] operator, both return the TIFFIFDEntry associated with the
 * specified input.
 *********************************************************************/
typedef mt::Singleton<KnownTags, true> KnownTagsRegistry;

} // End namespace.

#endif // __TIFF_TAG_MAP_REGISTRY_H__
