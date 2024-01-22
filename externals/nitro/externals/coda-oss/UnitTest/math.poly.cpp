#include "pch.h"
#include "CppUnitTest.h"

#include <import/math/linear.h>
#include <import/math/poly.h>

namespace math_poly
{
TEST_CLASS(test_1d_poly){ public:
#include "math.poly/unittests/test_1d_poly.cpp"
};

#ifdef HAVE_BOOST
TEST_CLASS(test_1d_poly_serialize){ public:
#include "math.poly/unittests/test_1d_poly_serialize.cpp"
};
#endif									 

TEST_CLASS(test_2d_poly){ public:
#include "math.poly/unittests/test_2d_poly.cpp"
};

#ifdef HAVE_BOOST
TEST_CLASS(test_2d_poly_serialize){ public:
#include "math.poly/unittests/test_2d_poly_serialize.cpp"
};
#endif

TEST_CLASS(test_fixed_1d_poly){ public:
#include "math.poly/unittests/test_fixed_1d_poly.cpp"
};

TEST_CLASS(test_fixed_2d_poly){ public:
#include "math.poly/unittests/test_fixed_2d_poly.cpp"
};

TEST_CLASS(test_llsq){ public:
#include "math.poly/unittests/test_llsq.cpp"
};

}