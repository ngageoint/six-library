#include "pch.h"
#include "CppUnitTest.h"

#include <import/nrt.h>
#include <nrt/unittests/Test.h>

namespace nrt
{

TEST_CLASS(test_buffer_adapter){ public:
#include "nrt/unittests/test_buffer_adapter.c"
};

TEST_CLASS(test_core_values){ public:
#include "nrt/unittests/test_core_values.c"
};

TEST_CLASS(test_list){ public:
#include "nrt/unittests/test_list.c"
};

TEST_CLASS(test_nrt_byte_swap){ public:
#include "nrt/unittests/test_nrt_byte_swap.c"
};

TEST_CLASS(test_nrt_datetime){ public:
#include "nrt/unittests/test_nrt_datetime.c"
};

TEST_CLASS(test_tree){ public:
#include "nrt/unittests/test_tree.c"
};

TEST_CLASS(test_utils){ public:
#include "nrt/unittests/test_utils.c"
};

}