/* =========================================================================
 * This file is part of six.json-c++
 * =========================================================================
 *
 * (C) Copyright 2025 ARKA Group, L.P. All rights reserved
 *
 * six.json-c++ is free software; you can redistribute it and/or modify
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
 */

#include "TestCase.h"

#include <import/six/json.h>

using json = nlohmann::json;

TEST_CASE(TestPhaseSGNJson)
{
    json serialized;
    cphd::PhaseSGN deserialized;
    cphd::PhaseSGN minus1 = cphd::PhaseSGN::MINUS_1;
    cphd::PhaseSGN plus1 = cphd::PhaseSGN::PLUS_1;
    cphd::PhaseSGN notSet = cphd::PhaseSGN::NOT_SET;

    serialized = minus1;
    deserialized = serialized.template get<cphd::PhaseSGN>();
    TEST_ASSERT(deserialized == minus1)

    serialized = plus1;
    deserialized = serialized.template get<cphd::PhaseSGN>();
    TEST_ASSERT(deserialized == plus1)

    serialized = notSet;
    deserialized = serialized.template get<cphd::PhaseSGN>();
    TEST_ASSERT(deserialized == notSet)
}
TEST_CASE(TestCollectTypeJson)
{
    json serialized;
    six::CollectType deserialized;
    six::CollectType monostatic = six::CollectType::MONOSTATIC;
    six::CollectType bistatic = six::CollectType::BISTATIC;
    six::CollectType notSet = six::CollectType::NOT_SET;

    serialized = monostatic;
    deserialized = serialized.template get<six::CollectType>();
    TEST_ASSERT(deserialized == monostatic)

    serialized = bistatic;
    deserialized = serialized.template get<six::CollectType>();
    TEST_ASSERT(deserialized == bistatic)

    serialized = notSet;
    deserialized = serialized.template get<six::CollectType>();
    TEST_ASSERT(deserialized == notSet)
}
TEST_CASE(TestRadarModeTypeJson)
{
    json serialized;
    six::RadarModeType deserialized;
    six::RadarModeType spotlight = six::RadarModeType::SPOTLIGHT;
    six::RadarModeType stripmap = six::RadarModeType::STRIPMAP;
    six::RadarModeType dynamicStrip = six::RadarModeType::DYNAMIC_STRIPMAP;
    six::RadarModeType scansar = six::RadarModeType::SCANSAR;
    six::RadarModeType notSet = six::RadarModeType::NOT_SET;

    serialized = spotlight;
    deserialized = serialized.template get<six::RadarModeType>();
    TEST_ASSERT(deserialized == spotlight)

    serialized = stripmap;
    deserialized = serialized.template get<six::RadarModeType>();
    TEST_ASSERT(deserialized == stripmap)

    serialized = dynamicStrip;
    deserialized = serialized.template get<six::RadarModeType>();
    TEST_ASSERT(deserialized == dynamicStrip)

    serialized = scansar;
    deserialized = serialized.template get<six::RadarModeType>();
    TEST_ASSERT(deserialized == scansar)

    serialized = notSet;
    deserialized = serialized.template get<six::RadarModeType>();
    TEST_ASSERT(deserialized == notSet)
}
TEST_CASE(TestBooleanTypeJson)
{
    json serialized;
    six::BooleanType deserialized;
    six::BooleanType boolTrue = six::BooleanType::IS_TRUE;
    six::BooleanType boolFalse = six::BooleanType::IS_FALSE;
    six::BooleanType boolNotSet = six::BooleanType::NOT_SET;

    serialized = boolTrue;
    deserialized = serialized.template get<six::BooleanType>();
    TEST_ASSERT(deserialized == boolTrue)

    serialized = boolFalse;
    deserialized = serialized.template get<six::BooleanType>();
    TEST_ASSERT(deserialized == boolFalse)

    serialized = boolNotSet;
    deserialized = serialized.template get<six::BooleanType>();
    TEST_ASSERT(deserialized == boolNotSet)
}
TEST_CASE(TestDualPolarizationTypeJson)
{
    using PolT = six::DualPolarizationType;
    // Not going to test every case of this...
    json serialized;
    PolT deserialized;
    std::vector<PolT> polsToTest = {
        PolT::OTHER,
        PolT::X_RHC,
        PolT::V_V,
        PolT::V_X,
        PolT::V_Y,
        PolT::H_H,
        PolT::UNKNOWN
    };
    
    for (const auto& pol: polsToTest)
    {
        serialized = pol;
        deserialized = serialized.template get<PolT>();
        TEST_ASSERT(deserialized == pol)
    }
}
TEST_CASE(TestLatLonJson)
{
    json serialized;
    scene::LatLon deserialized;
    scene::LatLon ll(10, 20);

    serialized = ll;
    deserialized = serialized.template get<scene::LatLon>();
    TEST_ASSERT(deserialized == ll)
}
TEST_CASE(TestLatLonAltJson)
{
    json serialized;
    scene::LatLonAlt deserialized;
    scene::LatLonAlt lla(10, 20, 30);

    serialized = lla;
    deserialized = serialized.template get<scene::LatLonAlt>();
    TEST_ASSERT(deserialized == lla)
}
TEST_CASE(TestFrameTypeJson)
{
    json serialized;
    scene::FrameType deserialized;
    scene::FrameType ecf = scene::FrameType::ECF;
    scene::FrameType ricEcf = scene::FrameType::RIC_ECF;
    scene::FrameType ricEci = scene::FrameType::RIC_ECI;
    scene::FrameType notSet = scene::FrameType::NOT_SET;

    serialized = ecf;
    deserialized = serialized.template get<scene::FrameType>();
    TEST_ASSERT(deserialized == ecf);

    serialized = ricEcf;
    deserialized = serialized.template get<scene::FrameType>();
    TEST_ASSERT(deserialized == ricEcf);

    serialized = ricEci;
    deserialized = serialized.template get<scene::FrameType>();
    TEST_ASSERT(deserialized == ricEci);

    serialized = notSet;
    deserialized = serialized.template get<scene::FrameType>();
    TEST_ASSERT(deserialized == notSet);
}
TEST_CASE(TestEarthModelTypeJson)
{
    json serialized;
    six::EarthModelType deserialized;
    six::EarthModelType wgs = six::EarthModelType::WGS84;
    six::EarthModelType notSet = six::EarthModelType::NOT_SET;

    serialized = wgs;
    deserialized = serialized.template get<six::EarthModelType>();
    TEST_ASSERT(deserialized == wgs);

    serialized = notSet;
    deserialized = serialized.template get<six::EarthModelType>();
    TEST_ASSERT(deserialized == notSet);
}
TEST_CASE(TestParameterJson)
{
    json serialized;
    six::Parameter deserialized;
    six::Parameter param;
    param.setName("TestParm");
    param.setValue("mraPtseT");

    serialized = param;
    deserialized = serialized.template get<six::Parameter>();
    TEST_ASSERT(deserialized == param)
}
TEST_CASE(TestParameterCollectionJson)
{
    json serialized;
    six::ParameterCollection deserialized;
    six::ParameterCollection params;
    six::Parameter p0, p1;
    p0.setName("Parm0");
    p0.setValue("Val0");
    p1.setName("Parm1");
    p1.setValue("Val1");
    params.push_back(p0);
    params.push_back(p1);

    serialized = params;
    deserialized = serialized.template get<six::ParameterCollection>();
    TEST_ASSERT(deserialized == params)
}
TEST_CASE(TestCornersJson)
{
    json serialized;
    six::Corners<scene::LatLon> deserialized;
    six::Corners<scene::LatLon> corners;
    corners.upperLeft = scene::LatLon(-100, 100);
    corners.upperRight = scene::LatLon(100, 100);
    corners.lowerLeft = scene::LatLon(100, -100);
    corners.lowerRight = scene::LatLon(-100, -100);

    serialized = corners;
    deserialized = serialized.template get<six::Corners<scene::LatLon>>();
    TEST_ASSERT(deserialized == corners)
}
TEST_CASE(TestXsElementJson)
{
    json serialized;
    six::XsElement<int32_t> deserialized{"ElemName"};
    six::XsElement<int32_t> xsElem("ElemName", 42);
    serialized = xsElem;
    deserialized = serialized.template get<six::XsElement<int32_t>>();
    TEST_ASSERT(deserialized == xsElem)
}
TEST_CASE(TestSCPTypeJson)
{
    json serialized;
    six::SCPType deserialized;
    six::SCPType rowCol = six::SCPType::SCP_ROW_COL;
    six::SCPType rangeAz = six::SCPType::SCP_RG_AZ;
    six::SCPType notSet = six::SCPType::NOT_SET;

    serialized = rowCol;
    deserialized = serialized.template get<six::SCPType>();
    TEST_ASSERT(deserialized == rowCol);

    serialized = rangeAz;
    deserialized = serialized.template get<six::SCPType>();
    TEST_ASSERT(deserialized == rangeAz);

    serialized = notSet;
    deserialized = serialized.template get<six::SCPType>();
    TEST_ASSERT(deserialized == notSet);
}
TEST_CASE(TestDateTimeJson)
{
    json serialized;
    nitf::DateTime deserialized;
    nitf::DateTime dt(2025, 3, 1, 18, 59, 10);

    serialized = dt;
    deserialized = serialized.template get<nitf::DateTime>();
    TEST_ASSERT(deserialized == dt)
}

TEST_MAIN(
    TEST_CHECK(TestPhaseSGNJson)
    TEST_CHECK(TestCollectTypeJson)
    TEST_CHECK(TestRadarModeTypeJson)
    TEST_CHECK(TestBooleanTypeJson)
    TEST_CHECK(TestDualPolarizationTypeJson)
    TEST_CHECK(TestLatLonJson)
    TEST_CHECK(TestLatLonAltJson)
    TEST_CHECK(TestFrameTypeJson)
    TEST_CHECK(TestEarthModelTypeJson)
    TEST_CHECK(TestParameterJson)
    TEST_CHECK(TestParameterCollectionJson)
    TEST_CHECK(TestCornersJson)
    TEST_CHECK(TestXsElementJson)
    TEST_CHECK(TestSCPTypeJson)
    TEST_CHECK(TestDateTimeJson)
)
