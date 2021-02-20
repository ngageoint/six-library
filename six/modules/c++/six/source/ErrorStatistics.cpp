/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <six/ErrorStatistics.h>

namespace six
{

bool CorrCoefs::operator==(const CorrCoefs& rhs) const
{
    return (p1p2 == rhs.p1p2 &&
        p1p3 == rhs.p1p3 &&
        p1v1 == rhs.p1v1 &&
        p1v2 == rhs.p1v2 &&
        p1v3 == rhs.p1v3 &&
        p2p3 == rhs.p2p3 &&
        p2v1 == rhs.p2v1 &&
        p2v2 == rhs.p2v2 &&
        p2v3 == rhs.p2v3 &&
        p3v1 == rhs.p3v1 &&
        p3v2 == rhs.p3v2 &&
        p3v3 == rhs.p3v3 &&
        v1v2 == rhs.v1v2 &&
        v1v3 == rhs.v1v3 &&
        v2v3 == rhs.v2v3);
}

bool PosVelError::operator==(const PosVelError& rhs) const
{
    return (frame == rhs.frame &&
        p1 == rhs.p1 &&
        p2 == rhs.p2 &&
        p3 == rhs.p3 &&
        v1 == rhs.v1 &&
        v2 == rhs.v2 &&
        v3 == rhs.v3 &&
        corrCoefs == rhs.corrCoefs &&
        getPositionDecorr() == rhs.getPositionDecorr());
}
}
