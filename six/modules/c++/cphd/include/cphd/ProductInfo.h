/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD_PRODUCT_INFO_H__
#define __CPHD_PRODUCT_INFO_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
/*
 * (Optional) Parameters that provide general information about the
 * CPHD product and/or the derived products that may be created from it
 */
struct ProductInfo
{
    /*
     * (Optional) Parameters that provide general information about
     * the CPHD product generation
     */
    struct CreationInfo
    {
        CreationInfo();

        bool operator==(const CreationInfo& other) const
        {
            return application == other.application &&
                    dateTime == other.dateTime && site == other.site &&
                    parameter == other.parameter;
        }

        std::string application;
        DateTime dateTime;
        std::string site;
        six::ParameterCollection parameter;
    };

    ProductInfo();

    bool operator==(const ProductInfo& other) const
    {
        return profile == other.profile &&
                creationInfo == other.creationInfo &&
                parameter == other.parameter;

    }

    bool operator!=(const ProductInfo& other) const
    {
        return !((*this) == other);
    }

    std::string profile;
    std::vector<CreationInfo> creationInfo;
    six::ParameterCollection parameter;
};

std::ostream& operator<< (std::ostream& os, const ProductInfo& p);
}

#endif
