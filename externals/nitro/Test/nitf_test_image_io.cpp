#include "pch.h"

#include <windows.h>
#include <combaseapi.h>
#undef interface

#include "nitf_Test.h"

#define TEST_CASE(X) TEST(nitf_test_image_io, X)

#include "nitf/unittests/test_image_io.c"
