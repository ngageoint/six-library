/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_IMAGE_CREATION_H__
#define __SIX_IMAGE_CREATION_H__

#include "six/Types.h"

namespace six
{
namespace sicd
{
/*!
 *  \struct ImageCreation
 *  \brief Contains SICD ImageCreation parameters
 *
 *  The ImageCreation block is the first sub-block under the 'SICD'
 *  tag.  It is optional altogether, and contains, information about
 *  the image creation (and processing).
 */
struct ImageCreation
{
    /*!
     *  (Optional) The name and version of the application used to 
     *  create the image
     */
    std::string application;

    /*!
     *  (Optional) Date and time that the image creation application
     *  processed the image (UTC)
     */
     DateTime dateTime;

    /*!
     *  (Optional) The creation site of this SICD product
     */
    std::string site;

    /*!
     *  (Optional) Identifies what profile was used to create this 
     *  SICD product
     */
    std::string profile;

    ImageCreation* clone() const
    {
        return new ImageCreation(*this);
    }

    //! Equality operator
    bool operator==(const ImageCreation& rhs) const
    {
        return (application == rhs.application &&
            dateTime == rhs.dateTime && site == rhs.site &&
            profile == rhs.profile);
    }
    bool operator!=(const ImageCreation& rhs) const
    {
        return !(*this == rhs);
    }
};

}
}
#endif

