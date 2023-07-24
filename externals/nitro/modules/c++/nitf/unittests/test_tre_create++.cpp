#include <sys/OS.h>

#include <import/nitf.hpp>
#include <nitf/UnitTests.hpp>

#include "TestCase.h"

TEST_CASE(test_tre_create_329)
{
    // https://github.com/mdaus/nitro/issues/329

    nitf::TRE tre("HISTOA", "HISTOA"); // allocates fields SYSTEM .. NEVENTS
    tre.setField("SYSTYPE", "M1");
    TEST_ASSERT_TRUE(true);

    //tre.setField("PDATE[0]", "20200210184728"); // aborts program; worked prior to 2.10
    TEST_ASSERT_TRUE(true);
}

TEST_CASE(test_tre_clone_329)
{
    // https://github.com/mdaus/nitro/issues/329
    const std::string rd = "begin1020030004ABCDEFend";

    nitf_TRE* rawTre = nullptr;
    {
        nitf_Error error;
        rawTre = nitf::TRE::create("TESTxyz", NITF_TRE_RAW, error);
        TEST_ASSERT_NOT_NULL(rawTre);

        nitf::TRE::setField(rawTre, "raw_data", rd, error);
        TEST_ASSERT_TRUE(true);
    }

    const nitf::TRE tre(rawTre);
    TEST_ASSERT_TRUE(true);
    const auto cloneTre = tre.clone();  // leads to free(): invalid size(): in libc.so.6 when the program exits; worked prior to 2.10
    TEST_ASSERT_TRUE(true);
}

TEST_MAIN(
    nitf::Test::setNitfPluginPath();

    TEST_CHECK(test_tre_create_329);
    TEST_CHECK(test_tre_clone_329);
    )
