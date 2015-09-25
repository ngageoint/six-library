/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * The CSM SICD Plugin is free software; you can redistribute it and/or modify
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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SIX_CSM_UTILITIES_H__
#define __SIX_CSM_UTILITIES_H__

#include "csm.h"

#include <scene/Types.h>

namespace six
{
namespace CSM
{
inline
scene::Vector3 toVector3(const csm::EcefCoord& pt)
{
    scene::Vector3 vec;
    vec[0] = pt.x;
    vec[1] = pt.y;
    vec[2] = pt.z;
    return vec;
}

inline
csm::EcefVector toEcefVector(const scene::Vector3& vec)
{
    return csm::EcefVector(vec[0], vec[1], vec[2]);
}

inline
csm::EcefCoord toEcefCoord(const scene::Vector3& vec)
{
    return csm::EcefCoord(vec[0], vec[1], vec[2]);
}

inline
csm::ImageCoord toImageCoord(const types::RowCol<double>& pt)
{
    return csm::ImageCoord(pt.row, pt.col);
}

inline
types::RowCol<double> fromImageCoord(const csm::ImageCoord& pos)
{
    return types::RowCol<double>(pos.line, pos.samp);
}
}
}

#endif
