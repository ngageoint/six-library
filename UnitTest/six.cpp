#include "pch.h"

#include "six_Test.h"

namespace six
{

TEST_CLASS(test_fft_sign_conversions) { public:
#include "six/modules/c++/six/unittests/test_fft_sign_conversions.cpp"
};

TEST_CLASS(test_polarization_type_conversions) { public:
#include "six/modules/c++/six/unittests/test_polarization_type_conversions.cpp"
};

TEST_CLASS(test_serialize) { public:
#include "six/modules/c++/six/unittests/test_serialize.cpp"
};

TEST_CLASS(test_xml_control) { public:
#include "six/modules/c++/six/unittests/test_xml_control.cpp"
};
	
}