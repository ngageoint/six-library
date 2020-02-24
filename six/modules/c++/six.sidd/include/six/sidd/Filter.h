/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD_FILTER_H__
#define __SIX_SIDD_FILTER_H__

#include <six/sidd/Enums.h>

namespace six
{
namespace sidd
{
struct Filter
{
    struct Predefined
    {
        Predefined();

        // Exactly one of databaseName or filterFamiy+filterMember must be set

        //! Database name of filter to use
        FilterDatabaseName databaseName;

        //! Index specifying the filter family
        size_t filterFamily;

        //! Index specifying the member for the filter family
        size_t filterMember;

        bool operator==(const Predefined& rhs) const;
        bool operator!=(const Predefined& rhs) const
        {
            return !(*this == rhs);
        }
    };

    // Used for spatially invariant filtering
    struct Kernel
    {
        struct Custom
        {
            Custom();

            //! Size of the kernel
            RowColInt size;

            //! Coefficients for the kernelSize kernel.
            std::vector<double> filterCoef;

            bool operator==(const Custom& rhs) const;
            bool operator!=(const Custom& rhs) const
            {
                return !(*this == rhs);
            }
        };

        // Exactly one of Predefined or Custom
        mem::ScopedCopyablePtr<Predefined> predefined;
        mem::ScopedCopyablePtr<Custom> custom;

        bool operator==(const Kernel& rhs) const;
        bool operator!=(const Kernel& rhs) const
        {
            return !(*this == rhs);
        }
    };

    // Used for spatially variant filtering
    struct Bank
    {
        struct Custom
        {
            Custom();

            //! Size of the filter bank
            size_t numPhasings;
            size_t numPoints;

            //! Coefficients for the numPhasings * numPoints filter bank.
            std::vector<double> filterCoef;

            bool operator==(const Custom& rhs) const;
            bool operator!=(const Custom& rhs) const
            {
                return !(*this == rhs);
            }
        };

        // Exactly one of Predefined or Custom
        mem::ScopedCopyablePtr<Predefined> predefined;
        mem::ScopedCopyablePtr<Custom> custom;

        bool operator==(const Bank& rhs) const;
        bool operator!=(const Bank& rhs) const
        {
            return !(*this == rhs);
        }
    };

    //! Name of the filter
    std::string filterName;

    // Exactly one of filterKernel or filterBank
    mem::ScopedCopyablePtr<Kernel> filterKernel;
    mem::ScopedCopyablePtr<Bank> filterBank;

    /*! Specifies if the kernel/bank is to be applied using convolution or
        correlation
     */
    FilterOperation operation;

    bool operator==(const Filter& rhs) const;
    bool operator!=(const Filter& rhs) const
    {
        return !(*this == rhs);
    }
};
}
}

#endif
