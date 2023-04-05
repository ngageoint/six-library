#include "pch.h"

#include <import/sys.h>

#include "nitf_Test.h"

CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 4464) // relative include path contains '..'
#endif
#include <nitf/../../unittests/nitro_image_.c_>
CODA_OSS_disable_warning_pop

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_create_nitf__) {
public:
	nitf_test_create_nitf__() {
		// initialization code here
		nitf::Test::setNitfPluginPath();
	}

	~nitf_test_create_nitf__() = default;
	nitf_test_create_nitf__(const nitf_test_create_nitf__&) = delete;
	nitf_test_create_nitf__& operator=(const nitf_test_create_nitf__&) = delete;

	#define TEST_CASE(X) TEST_METHOD(X)
	#include "nitf/unittests/test_create_nitf++.cpp"

};