/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef __SIX_SIDD_UTILITIES_H__
#define __SIX_SIDD_UTILITIES_H__

#include <memory>

#include <import/scene.h>
#include <types/RgAz.h>
#include <six/sidd/DerivedData.h>

namespace six
{
namespace sidd
{
class Utilities
{
public:
    // TODO: This is only needed because getSceneGeometry() isn't implemented
    //       for all projection types
    static scene::SideOfTrack getSideOfTrack(const DerivedData* derived);

    static std::auto_ptr<scene::SceneGeometry>
    getSceneGeometry(const DerivedData* derived);

    static std::auto_ptr<scene::GridGeometry>
    getGridGeometry(const DerivedData* derived);

    static std::auto_ptr<scene::GridECEFTransform>
    getGridECEFTransform(const DerivedData* derived);

    static void setProductValues(Poly2D timeCOAPoly, PolyXYZ arpPoly,
            ReferencePoint ref, const Vector3* row, const Vector3* col,
            types::RgAz<double>res, Product* product);

    static void setProductValues(Vector3 arpVel, Vector3 arpPos,
            Vector3 refPos, const Vector3* row, const Vector3* col,
            types::RgAz<double>res, Product* product);

    static void setCollectionValues(Poly2D timeCOAPoly, PolyXYZ arpPoly,
            ReferencePoint ref, const Vector3* row, const Vector3* col,
            Collection* collection);

    static void setCollectionValues(Vector3 arpVel, Vector3 arpPos,
            Vector3 refPos, const Vector3* row, const Vector3* col,
            Collection* collection);

    static std::pair<six::PolarizationType, six::PolarizationType>
            convertDualPolarization(six::DualPolarizationType pol);

    static std::auto_ptr<scene::ProjectionModel>
    getProjectionModel(const DerivedData* data);

};
}
}
#endif

