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

int main(int , char** )
{
    TEST_CHECK(testToType);
    return 0;
}
