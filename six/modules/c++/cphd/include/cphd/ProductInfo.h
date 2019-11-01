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
 *  \struct ProductInfo
 *
 *  \brief Contains product information
 *
 *  (Optional) Parameters that provide general information about the
 *  CPHD product and/or the derived products that may be created from it
 */
struct ProductInfo
{
    /*
     *  \struct CreationInfo
     *
     *  \brief Contains creation information
     *
     *  (Optional) Parameters that provide general information about
     *  the CPHD product generation
     */
    struct CreationInfo
    {
        //! Constructor
        CreationInfo();

        //! Equality operators
        bool operator==(const CreationInfo& other) const
        {
            return application == other.application &&
                    dateTime == other.dateTime && site == other.site &&
                    parameter == other.parameter;
        }
        bool operator!=(const CreationInfo& other) const
        {
            return !((*this) == other);
        }

        //! (Optional) Name and version of the applications that created
        //! the CPHD product
        std::string application;

        //! Date and time the CPHD product was created (UTC).
        DateTime dateTime;

        //! (Optional) Name of the site where the product was created
        std::string site;

        //! (Optional) Text field that can be used for program specific
        //! name and value
        six::ParameterCollection parameter;
    };

    //! Constructor
    ProductInfo();

    //! Equality operators
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

    //! (Optional) Identifies what profile was used to create this
    //! CPHD product
    std::string profile;

    //! (Optional) Parameters that provide general information
    //! about the CPHD product generation.
    std::vector<CreationInfo> creationInfo;

    //! (Optional) Text field that can be used for program specific
    //! parameter name & value
    six::ParameterCollection parameter;
};

//! Ostream operator
std::ostream& operator<< (std::ostream& os, const ProductInfo& p);
}

#endif
