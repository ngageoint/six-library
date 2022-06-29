#include "pch.h"

#include "cphd_Test.h"

namespace cphd
{

TEST_CLASS(test_channel) { public:
#include "six/modules/c++/cphd/unittests/test_channel.cpp"
};

TEST_CLASS(test_compressed_signal_block_round) { public:
#include "six/modules/c++/cphd/unittests/test_compressed_signal_block_round.cpp"
};

TEST_CLASS(test_cphd_xml_control) { public:
#include "six/modules/c++/cphd/unittests/test_cphd_xml_control.cpp"
};

TEST_CLASS(test_cphd_xml_optional) { public:
#include "six/modules/c++/cphd/unittests/test_cphd_xml_optional.cpp"
};
										
TEST_CLASS(test_dwell) { public:
#include "six/modules/c++/cphd/unittests/test_dwell.cpp"
};

TEST_CLASS(test_file_header) { public:
#include "six/modules/c++/cphd/unittests/test_file_header.cpp"
};

TEST_CLASS(test_pvp) { public:
#include "six/modules/c++/cphd/unittests/test_pvp.cpp"
};

TEST_CLASS(test_pvp_block) { public:
#include "six/modules/c++/cphd/unittests/test_pvp_block.cpp"
};

TEST_CLASS(test_pvp_block_round) { public:
#include "six/modules/c++/cphd/unittests/test_pvp_block_round.cpp"
};

TEST_CLASS(test_read_wideband) { public:
#include "six/modules/c++/cphd/unittests/test_read_wideband.cpp"
};

TEST_CLASS(test_reference_geometry) { public:
#include "six/modules/c++/cphd/unittests/test_reference_geometry.cpp"
};

TEST_CLASS(test_signal_block_round) { public:
#include "six/modules/c++/cphd/unittests/test_signal_block_round.cpp"
};

TEST_CLASS(test_support_block_round) { public:
#include "six/modules/c++/cphd/unittests/test_support_block_round.cpp"
};

}