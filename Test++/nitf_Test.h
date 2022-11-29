#pragma once

#if defined(MS_CPP_UNITTESTFRAMEWORK)
#define __TEST_H__ // #include "../nitf/unittests/Test.h"
#define __TEST_CASE_H__ // #include "../nitf/unittests/TestCase.h"
#endif
#include "Test.h"

#include <string>

#include "import/nitf.h"

#include <nitf/UnitTests.hpp>

namespace Microsoft{ namespace VisualStudio {namespace CppUnitTestFramework
{

template<>
inline std::wstring ToString(const nitf::PixelValueType& q)
{
	return str::EncodedStringView(nitf::to_string(q)).wstring();
}

template<>
inline std::wstring ToString(const nitf::BlockingMode& q)
{
	return str::EncodedStringView(nitf::to_string(q)).wstring();
}

template<>
inline std::wstring ToString(const nitf::ImageCompression& q)
{
	return str::EncodedStringView(nitf::to_string(q)).wstring();
}

}}}