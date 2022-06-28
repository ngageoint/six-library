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

#include <scene/Errors.h>

namespace
{
template <typename MatT>
void setToZero(MatT& matrix)
{
    for (size_t row = 0; row < matrix.rows(); ++row)
    {
        for (size_t col = 0; col < matrix.cols(); ++col)
        {
            matrix(row, col) = 0.0;
        }
    }
}
}

namespace scene
{
void Errors::clear()
{
    mFrameType = FrameType::RIC_ECF;
    setToZero(mSensorErrorCovar);
    setToZero(mUnmodeledErrorCovar);
    setToZero(mIonoErrorCovar);
    setToZero(mTropoErrorCovar);
    mPositionCorrCoefZero = 0.0;
    mPositionDecorrRate = 0.0;
    mRangeCorrCoefZero = 0.0;
    mRangeDecorrRate = 0.0;
}
}
