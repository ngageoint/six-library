#include "TestCase.h"

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

}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(testToType);
    )
