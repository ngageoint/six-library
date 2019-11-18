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

#ifndef __SIX_SICD_SLANT_PLANE_PIXEL_TRANSFORMER_H__
#define __SIX_SICD_SLANT_PLANE_PIXEL_TRANSFORMER_H__

#include <string>
#include <vector>

#include <types/RowCol.h>
#include <scene/Types.h>
#include <scene/LLAToECEFTransform.h>
#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
/*!
 *  \class SlantPlanePixelTransformer
 *  \brief Projects a slant plane pixel into various ground plane coordinates
 *
 *  This class should be used to project coordinates between slant and
 *  ground planes.
 */
class SlantPlanePixelTransformer
{
public:
    /*!
     *  \fn Constructor
     *  \param data       - ComplexData object
     *  \param geom       - Geometry of slant to ground
     *  \param projection - Projection type for translation
     *
     *  NOTE: ProjectionModel is stored by reference. Make sure
     *        this object survives this class and its usefulness.
     */
    SlantPlanePixelTransformer(const six::sicd::ComplexData& data,
                               const scene::SceneGeometry& geom,
                               const scene::ProjectionModel& projection);

    /*!
     *  \fn toECEF
     *  \param pixel - Slant Plane pixel with (row,col) index
     *  \return      - Returns the ground plane location in ECEF
     */
    scene::Vector3 toECEF(const types::RowCol<double>& pixel) const;

    /*!
     *  \fn toLLA
     *  \param pixel - Slant Plane pixel with (row,col) index
     *  \return      - Returns the ground plane location in LLA
     */
    scene::LatLonAlt toLLA(const types::RowCol<double>& pixel) const;

    /*!
     *  \fn toLatLon
     *  \param pixel - Slant Plane pixel with (row,col) index
     *  \return      - Returns the ground plane location in LatLon
     */
    scene::LatLon toLatLon(const types::RowCol<double>& pixel) const;

private:
    const scene::SceneGeometry mGeom;
    const scene::ProjectionModel& mProjection;
    const six::sicd::ComplexData mSicdData;
    scene::Vector3 mGroundPlaneNormal;
};

}
}

#endif
