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
#ifndef __SIX_SICD_UTILITIES_H__
#define __SIX_SICD_UTILITIES_H__

#include <import/scene.h>
#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
class Utilities
{
public:

    static scene::SceneGeometry* getSceneGeometry(const ComplexData* data);
    static scene::ProjectionModel* getProjectionModel(const ComplexData* data, 
            const scene::SceneGeometry* geom);

};
}
}
#endif

