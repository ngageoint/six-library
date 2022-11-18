#include "pch.h"
#include "CppUnitTest.h"

namespace types
{
TEST_CLASS(test_page_row_col){ public:
#include "types/unittests/test_page_row_col.cpp"
};

TEST_CLASS(test_range){ public:
#include "types/unittests/test_range.cpp"
};

TEST_CLASS(test_range_list){ public:
#include "types/unittests/test_range_list.cpp"
};

}