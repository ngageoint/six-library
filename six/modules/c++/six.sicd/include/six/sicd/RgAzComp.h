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
#pragma once
#ifndef __SIX_RG_AZ_COMP_H__
#define __SIX_RG_AZ_COMP_H__

#include "logging/Logger.h"
#include "six/Types.h"
#include "six/Init.h"
#include "six/sicd/GeoData.h"
#include "six/sicd/SCPCOA.h"
#include "six/sicd/Timeline.h"
#include "six/sicd/Exports.h"

namespace six
{
namespace sicd
{
struct Grid;
struct Timeline;
/*!
 *  \struct  RgAzComp
 *  \brief   Parameters included for a Range, Doppler image.
 *           Required parameters for ImageFormAlgo = RGAZCOMP.
 */
struct SIX_SICD_API RgAzComp
{
    //! Constructor
    RgAzComp() :
        azSF(Init::undefined<double>()),
        kazPoly(Init::undefined<Poly1D>())
    {
    }

    //! Scale factor that scales image coordinate az = ycol
    //  (meters) to a delta cosine of the Doppler Cone Angle
    //  at COA
    double azSF;

    //! Polynomial function that yields azimuth spatial frequency
    //  (Kaz = Kcol) as a function of slow time (variable 1). Slow
    //  Time (sec) -> Azimuth spatial frequency (cycles/meter). Time
    //  relative to collection start.
    Poly1D kazPoly;

    //! Equality operator
    bool operator==(const RgAzComp& rhs)
    {
        return azSF == rhs.azSF && kazPoly == rhs.kazPoly;
    }

    bool operator!=(const RgAzComp& rhs)
    {
        return !(*this == rhs);
    }

    void fillDerivedFields(const GeoData& geoData,
        const Grid& grid,
        const SCPCOA& scpcoa,
        const Timeline& timeline);

    bool validate(const GeoData& geoData,
            const Grid& grid,
            const SCPCOA& scpcoa,
            const Timeline& timeline,
            logging::Logger& log) const;

private:
    double derivedAzSf(const SCPCOA& scpcoa, const Vector3& scp) const;
    Poly1D derivedKazPoly(const Grid& grid,
            const SCPCOA& scpcoa,
            const Timeline& timeline,
            const Vector3& scp) const;

};

}
}
#endif

