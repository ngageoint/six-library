#include "pch.h"

#include <import/sys.h>

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_tre_mods__) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_tre_mods++.cpp"

};

// Be sure this runs AFTER the tre_mods tests ... not really sure why ...
#undef TEST_CASE
TEST_CLASS(test_image_writer) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_image_writer.cpp"

};

#undef TEST_CASE
TEST_CLASS(test_load_plugins) {
public:
	test_load_plugins() {
		// initialization code here
		nitf::Test::setNitfPluginPath();
	}
	~test_load_plugins() = default;
	test_load_plugins(const test_load_plugins&) = delete;
	test_load_plugins& operator=(const test_load_plugins&) = delete;

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_load_plugins.cpp"

};
