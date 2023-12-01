/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#pragma once
#ifndef __SIX_GEO_DATA_H__
#define __SIX_GEO_DATA_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/GeoDataBase.h"
#include "six/GeoInfo.h"
#include "scene/ProjectionModel.h"
#include <logging/Logger.h>

#include "six/sicd/Exports.h"

namespace six
{
namespace sicd
{
struct ImageData;
/*!
 *  \struct GeoData
 *  \brief SICD GeoData block
 *
 *  This block describes the geographic coordinates of the region
 *  covered by the image
 *
 *  Compiler-generated copy constructor and assignment operator are sufficient
 */
class SIX_SICD_API GeoData : public GeoDataBase
{
public:

    /*!
     *  Scene Center Point in full image.  This is the
     *  precise location
     */
    SCP scp;

    bool operator==(const GeoData& rhs) const;
    bool operator!=(const GeoData& rhs) const
    {
        return !(*this == rhs);
    }

    //Doesn't currently do anything
    void fillDerivedFields(const ImageData& imageData,
            const scene::ProjectionModel& model);
    bool validate(logging::Logger& log) const;

private:
    static const double ECF_THRESHOLD;
};
}
}

#endif
