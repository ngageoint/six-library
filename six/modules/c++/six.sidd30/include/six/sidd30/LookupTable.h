/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd30-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD30_LOOKUP_TABLE_H__
#define __SIX_SIDD30_LOOKUP_TABLE_H__

#include <mem/ScopedCopyablePtr.h>
#include <six/Types.h>

namespace six
{
namespace sidd30
{
class LookupTable
{
public:
    struct Predefined
    {
        Predefined();

        // Exactly one of databaseName or remapFamiy+remapMember must be set

        //! Database name of LUT to use
        std::string databaseName;

        //! Index specifying the remap family
        size_t remapFamily;

        //! Index specifying the member for the remap family
        size_t remapMember;

        bool operator==(const Predefined& rhs) const;
        bool operator!=(const Predefined& rhs) const
        {
            return !(*this == rhs);
        }
    };

    struct Custom
    {
        Custom(size_t numEntries, size_t numLuts);

        std::vector<LUT> lutValues;

        bool operator==(const Custom& rhs) const
        {
            return lutValues == rhs.lutValues;
        }

        bool operator!=(const Custom& rhs) const
        {
            return !(*this == rhs);
        }
    };

    std::string lutName;

    // Exactly one of Predefined or Custom
    mem::ScopedCopyablePtr<Predefined> predefined;
    mem::ScopedCopyablePtr<Custom> custom;

    bool operator==(const LookupTable& rhs) const;
    bool operator!=(const LookupTable& rhs) const
    {
        return !(*this == rhs);
    }
};
}
}

#endif
