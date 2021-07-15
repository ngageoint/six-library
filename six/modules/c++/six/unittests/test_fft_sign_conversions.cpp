#include "TestCase.h"

#include <scene/math.h>
#include <six/Utilities.h>
#include <six/Enums.h>

namespace
{
TEST_CASE(testToType)
{
    TEST_ASSERT_EQ(six::toType<six::FFTSign>("+1"),
            six::FFTSign(six::FFTSign::POS));
    TEST_ASSERT_EQ(six::toType<six::FFTSign>("1"),
            six::FFTSign(six::FFTSign::POS));
    TEST_ASSERT_EQ(six::toType<six::FFTSign>("-1"),
            six::FFTSign(six::FFTSign::NEG));

}

TEST_CASE(test_degrees)
{
    // These are part of "scene" not "six", but there aren't any unittests there
    const auto degrees_0 = scene::as_degrees(0.0);
    const auto degrees_90 = scene::as_degrees(90.0);
    const auto degrees_180 = scene::as_degrees(180.0);
    const auto degrees_270 = scene::as_degrees(270.0);
    const auto degrees_360 = scene::as_degrees(360.0);

    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_0));
    TEST_ASSERT_ALMOST_EQ(1.0, sin(degrees_90));
    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_180));
    TEST_ASSERT_ALMOST_EQ(-1.0, sin(degrees_270));
    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_360));

    TEST_ASSERT_ALMOST_EQ(1.0, cos(degrees_0));
    TEST_ASSERT_ALMOST_EQ(0.0, cos(degrees_90));
    TEST_ASSERT_ALMOST_EQ(-1.0, cos(degrees_180));
    TEST_ASSERT_ALMOST_EQ(0.0, cos(degrees_270));
    TEST_ASSERT_ALMOST_EQ(1.0, cos(degrees_360));
}

}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(testToType);
    TEST_CHECK(test_degrees);
    )
