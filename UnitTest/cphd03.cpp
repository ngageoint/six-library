#include "pch.h"

#include "cphd03_Test.h"

namespace cphd03
{

TEST_CLASS(test_cphd_read_unscaled_int) { public:
#include "six/modules/c++/cphd03/unittests/test_cphd_read_unscaled_int.cpp"
};

TEST_CLASS(test_cphd_write) { public:
#include "six/modules/c++/cphd03/unittests/test_cphd_write.cpp"
};

TEST_CLASS(test_vbm) { public:
#include "six/modules/c++/cphd03/unittests/test_vbm.cpp"
};

}