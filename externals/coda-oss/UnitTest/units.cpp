#include "pch.h"
#include "CppUnitTest.h"

#include <units/Angles.h>
#include <units/Lengths.h>

namespace units
{
    TEST_CLASS(test_units){ public:
    #include "units/unittests/test_units.cpp"
    };
}
