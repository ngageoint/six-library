#include "pch.h"

#include "sidd_Test.h"
#include "Test.h"

namespace sidd
{

TEST_CLASS(test_annotations_equality) { public:
#include "six/modules/c++/six.sidd/unittests/test_annotations_equality.cpp"
};

TEST_CLASS(test_geometric_chip) { public:
#include "six/modules/c++/six.sidd/unittests/test_geometric_chip.cpp"
};

TEST_CLASS(test_read_sidd_legend) { public:
    test_read_sidd_legend()
    {
        // initialization code here
        const auto SIX_SCHEMA_PATH = six::Test::buildSchemaDir();
        sys::OS().setEnv("SIX_SCHEMA_PATH", SIX_SCHEMA_PATH.string(), true /*overwrite*/);
    }

#include "six/modules/c++/six.sidd/unittests/test_read_sidd_legend.cpp"
};

TEST_CLASS(unittest_sidd_byte_provider) { public:
    unittest_sidd_byte_provider()
    {
        // initialization code here
        const auto SIX_SCHEMA_PATH = six::Test::buildSchemaDir();
        sys::OS().setEnv("SIX_SCHEMA_PATH", SIX_SCHEMA_PATH.string(), true /*overwrite*/);
    }

#include "six/modules/c++/six.sidd/unittests/unittest_sidd_byte_provider.cpp"
};

TEST_CLASS(test_valid_sixsidd) { public:
#include "six/modules/c++/six.sidd/unittests/test_valid_sixsidd.cpp"
};
	
}