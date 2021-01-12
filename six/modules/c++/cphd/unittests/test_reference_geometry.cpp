/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <iostream>
#include <memory>

#include <xml/lite/MinidomParser.h>
#include <six/Init.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>
#include <cphd/ReferenceGeometry.h>

#include "TestCase.h"

namespace
{
TEST_CASE(testEquality)
{
    cphd::ReferenceGeometry refGeo;
    cphd::ReferenceGeometry refGeoOther;
    refGeo.monostatic.reset(new cphd::Monostatic());
    refGeoOther.monostatic.reset(new cphd::Monostatic());

    // Fill Values
    refGeo.monostatic->azimuthAngle = 1.0;
    refGeo.monostatic->grazeAngle = 1.0;
    refGeo.monostatic->twistAngle = 1.0;
    refGeo.monostatic->slopeAngle = 1.0;
    refGeo.monostatic->layoverAngle = 1.0;
    refGeo.monostatic->sideOfTrack = six::SideOfTrackType("LEFT");
    refGeo.monostatic->slantRange = 20.0;

    refGeoOther.monostatic->azimuthAngle = 2.0;
    refGeoOther.monostatic->grazeAngle = 2.0;
    refGeoOther.monostatic->twistAngle = 2.0;
    refGeoOther.monostatic->slopeAngle = 2.0;
    refGeoOther.monostatic->layoverAngle = 2.0;
    refGeoOther.monostatic->sideOfTrack = six::SideOfTrackType("LEFT");
    refGeoOther.monostatic->slantRange = 20.0;

    TEST_ASSERT_NOT_EQ(refGeo, refGeoOther);
}
}

TEST_MAIN(
        TEST_CHECK(testEquality);
)
