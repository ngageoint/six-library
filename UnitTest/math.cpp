#include "pch.h"
#include "CppUnitTest.h"

#include <import/math.h>

namespace math
{
TEST_CLASS(test_bessel_functions){ public:
#include "math/unittests/test_bessel_functions.cpp"
};

TEST_CLASS(test_convex_hull){ public:
#include "math/unittests/test_convex_hull.cpp"
};

TEST_CLASS(test_is_nan){ public:
#include "math/unittests/test_is_nan.cpp"
};

TEST_CLASS(test_n_choose_k){ public:
#include "math/unittests/test_n_choose_k.cpp"
};

TEST_CLASS(test_round){ public:
#include "math/unittests/test_round.cpp"
};

TEST_CLASS(test_sign){ public:
#include "math/unittests/test_sign.cpp"
};
}