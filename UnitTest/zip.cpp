#include "pch.h"
#include "CppUnitTest.h"

#include <import/zip.h>
#include <io/ReadUtils.h>

namespace zip
{
TEST_CLASS(unittest_GZip){ public:
#include "zip/unittests/unittest_GZip.cpp"
};

}