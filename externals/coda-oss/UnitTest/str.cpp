#include "pch.h"
#include "CppUnitTest.h"

#include <string>

#include <import/str.h>
#include <str/EncodedString.h>
#include <str/EncodedStringView.h>

namespace str
{
    TEST_CLASS(test_base_convert){ public:
    #include "str/unittests/test_base_convert.cpp"
    };

    TEST_CLASS(test_str){ public:
    #include "str/unittests/test_str.cpp"
    };
}
