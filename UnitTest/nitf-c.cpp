#include "pch.h"
#include "CppUnitTest.h"

#include <import/nitf.h>
#include <nitf/TestingTest.h>
#include <nitf/unittests/Test.h>

namespace nitf_c
{

TEST_CLASS(test_create){ public:
#include "nitf/unittests/test_create.c"
};

TEST_CLASS(test_create_nitf){ public:
#include "nitf/unittests/test_create_nitf.c"
};

TEST_CLASS(test_field){ public:
#include "nitf/unittests/test_field.c"
};

TEST_CLASS(test_geo_utils_){ public: // "TEST_CASE(test_geo_utils)" conflicts
#include "nitf/unittests/test_geo_utils.c"
};

TEST_CLASS(test_image_io){ public:
#include "nitf/unittests/test_image_io.c"
};

TEST_CLASS(test_mem_source){ public:
#include "nitf/unittests/test_mem_source.c"
};

TEST_CLASS(test_moveTREs){ public:
#include "nitf/unittests/test_moveTREs.c"
};

TEST_CLASS(test_zero_field){ public:
#include "nitf/unittests/test_zero_field.c"
};

}