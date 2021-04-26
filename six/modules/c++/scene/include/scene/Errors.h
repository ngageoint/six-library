/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SCENE_ERRORS_H__
#define __SCENE_ERRORS_H__

#include <scene/sys_Conf.h>
#include <math/linear/MatrixMxN.h>
#include <scene/FrameType.h>

namespace scene
{
struct Errors
{
    Errors() = default;

    void clear();

    FrameType mFrameType = FrameType::RIC_ECF;
    math::linear::MatrixMxN<7, 7> mSensorErrorCovar = 0.0;
    math::linear::MatrixMxN<2, 2> mUnmodeledErrorCovar = 0.0;
    math::linear::MatrixMxN<2, 2> mIonoErrorCovar = 0.0;
    math::linear::MatrixMxN<1, 1> mTropoErrorCovar = 0.0;
    double mPositionCorrCoefZero = 0.0;
    double mPositionDecorrRate = 0.0;
    double mRangeCorrCoefZero = 0.0;
    double mRangeDecorrRate = 0.0;
};
}

#endif
