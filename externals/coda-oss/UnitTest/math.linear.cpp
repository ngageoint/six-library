#include "pch.h"
#include "CppUnitTest.h"

#include <import/math/linear.h>

namespace math_linear
{
TEST_CLASS(test_eigenvalue){ public:
#include "math.linear/unittests/test_eigenvalue.cpp"
};

TEST_CLASS(test_inf_equality){ public:
#include "math.linear/unittests/test_inf_equality.cpp"
};

TEST_CLASS(test_inverse){ public:
#include "math.linear/unittests/test_inverse.cpp"
};

TEST_CLASS(test_lin){ public:
#include "math.linear/unittests/test_lin.cpp"
};

#ifdef HAVE_BOOST
TEST_CLASS(test_Matrix2D_serialize){ public:
#include "math.linear/unittests/test_Matrix2D_serialize.cpp"
};
#endif

TEST_CLASS(test_mx){ public:
#include "math.linear/unittests/test_mx.cpp"
};

TEST_CLASS(test_Vector){ public:
#include "math.linear/unittests/test_Vector.cpp"
};

#ifdef HAVE_BOOST
TEST_CLASS(test_Vector_serialize){ public:
#include "math.linear/unittests/test_Vector_serialize.cpp"
};
#endif

TEST_CLASS(test_VectorN){ public:
#include "math.linear/unittests/test_VectorN.cpp"
};
}