#include "pch.h"

#include "sicd_Test.h"
#include "Test.h"

#ifdef _MSC_VER
#pragma warning(disable: 4464) // relative include path contains '..'
#endif
#include "../tests/TestUtilities.h"

namespace sicd
{

TEST_CLASS(test_AMP8I_PHS8I) { public:
#include "six/modules/c++/six.sicd/unittests/test_AMP8I_PHS8I.cpp"
};

TEST_CLASS(test_area_plane) { public:
    test_area_plane() {
        // initialization code here
        sys::OS().setEnv("NITF_PLUGIN_PATH", NITF_PLUGIN_PATH, true /*overwrite*/);
    }
#include "six/modules/c++/six.sicd/unittests/test_area_plane.cpp"
};

TEST_CLASS(test_CollectionInfo) { public:
#include "six/modules/c++/six.sicd/unittests/test_CollectionInfo.cpp"
};

TEST_CLASS(test_filling_geo_data) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_geo_data.cpp"
};

TEST_CLASS(test_filling_grid) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_grid.cpp"
};
	
}