/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#ifndef __SIX_SICD_GEOLOCATOR_H__
#define __SIX_SICD_GEOLOCATOR_H__

#include <scene/GridECEFTransform.h>
#include <scene/ECEFToLLATransform.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
class GeoLocator
{
/*!
 * \brief Utility for finding location of pixel when projected to
 * output plane
 */
public:
    /*!
     * Constructor
     * \param complexData Complex Data object for input
     * \param shadowsDown If true, rotate output plane to shadows-down orientation
     */
    GeoLocator(const ComplexData& complexData, bool shadowsDown=true);

    GeoLocator(const GeoLocator&) = delete;
    GeoLocator& operator=(const GeoLocator&) = delete;

    /*!
     * Find the location of a SICD pixel in the output plane
     * \param rowCol Pixel location in SICD
     * \return Corresponding location in output plane
     */
    LatLonAlt geolocate(const RowColDouble& rowCol) const;

private:
    scene::PlanarGridECEFTransform buildTransformer(
            const ComplexData& complexData, bool shadowsDown) const;
    const scene::ECEFToLLATransform mEcefToLla;
    const scene::PlanarGridECEFTransform mRowColToEcef;
};
}
}
#endif
