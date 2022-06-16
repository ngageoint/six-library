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

TEST_CLASS(test_filling_pfa) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_pfa.cpp"
};
	
TEST_CLASS(test_filling_position) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_position.cpp"
};

TEST_CLASS(test_filling_rgazcomp) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_rgazcomp.cpp"
};

TEST_CLASS(test_filling_rma) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_rma.cpp"
};

TEST_CLASS(test_filling_scpcoa) { public:
#include "six/modules/c++/six.sicd/unittests/test_filling_scpcoa.cpp"
};

TEST_CLASS(test_get_segment) { public:
#include "six/modules/c++/six.sicd/unittests/test_get_segment.cpp"
};

TEST_CLASS(test_projection_polynomial_fitter) { public:
#include "six/modules/c++/six.sicd/unittests/test_projection_polynomial_fitter.cpp"
};

TEST_CLASS(test_radar_collection) { public:
#include "six/modules/c++/six.sicd/unittests/test_radar_collection.cpp"
};

TEST_CLASS(test_update_sicd_version) { public:
#include "six/modules/c++/six.sicd/unittests/test_update_sicd_version.cpp"
};

TEST_CLASS(test_utilities) { public:
#include "six/modules/c++/six.sicd/unittests/test_utilities.cpp"
};

TEST_CLASS(test_valid_six) { public:
#include "six/modules/c++/six.sicd/unittests/test_valid_six.cpp"
};

}