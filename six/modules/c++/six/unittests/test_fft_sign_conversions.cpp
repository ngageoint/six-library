#include "TestCase.h"

#include <six/Utilities.h>

namespace
{
TEST_CASE(testToType)
{
    TEST_ASSERT_EQ(six::toType<six::FFTSign>("+1"), six::FFTSign::POS);
    TEST_ASSERT_EQ(six::toType<six::FFTSign>("1"), six::FFTSign::POS);
    TEST_ASSERT_EQ(six::toType<six::FFTSign>("-1"), six::FFTSign::NEG);
}
}

int main(int , char** )
{
    TEST_CHECK(testToType);
    return 0;
}
