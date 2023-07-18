#include "pch.h"
#include "CppUnitTest.h"

#include <nitf/UnitTests.hpp>

// https://learn.microsoft.com/en-us/visualstudio/test/microsoft-visualstudio-testtools-cppunittestframework-api-reference?view=vs-2022
TEST_MODULE_INITIALIZE(methodName)
{
    // module initialization code
    nitf::Test::setNitfPluginPath();
}