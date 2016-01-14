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
#ifndef __SIX_SIDD_Filter_H__
#define __SIX_SIDD_Filter_H__

#include <six/Init.h>
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

        // Exactly one of dbName or FilterFamiy+FilterMember must be set

        //! Database name of filter to use
        FilterDatabaseName dbName;

        //! Index specifying the Filter family
        size_t FilterFamily;

        //! Index specifying the member for the Filter family
        size_t FilterMember;
    };

    struct Custom
    {
        Custom();

        /*! General indicates the same Filter is used for the entire operation
            Filterbank indicates that the Filter is spatially variant
         */
        FilterCustomType type;

        //! Size of the Filter
        RowColInt FilterSize;

        /*! For FilterType=General, populate with the coefficients for the
            FilterSize Filter.  For FilterType=FilterBank, each row should
            contain the coefficients for that phasing.
         */
        std::vector<double> FilterCoef;
    };

    //! Name of the filter
    std::string FilterName;

    // Exactly one of Predefined or Custom
    mem::ScopedCopyablePtr<Predefined> predefined;
    mem::ScopedCopyablePtr<Custom> custom;

    /*! Specifies if the Filter is to be applied using convolution or
        correlation
     */
    FilterOperation operation;
};
}
}

#endif
