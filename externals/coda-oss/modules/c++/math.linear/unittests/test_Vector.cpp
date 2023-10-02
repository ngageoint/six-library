#include <std/span>
#include <cstdlib>
#include "TestCase.h"
#include "math/linear/Vector.h"


TEST_CASE(testDefaultConstructor)
{
    using namespace math::linear;

    Vector<double> v;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(0));
}


TEST_CASE(testScalarConstructor)
{
    using namespace math::linear;

    Vector<double> v(3, 42);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], 42);
    TEST_ASSERT_EQ(v[1], 42);
    TEST_ASSERT_EQ(v[2], 42);
    // check for subscript out of bounds?
}


TEST_CASE(testRawConstructor)
{
    using namespace math::linear;

    double raw[] = {1,2,3};
    Vector<double> v(3, raw);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], 1);
    TEST_ASSERT_EQ(v[1], 2);
    TEST_ASSERT_EQ(v[2], 3);
}


TEST_CASE(testCopyConstructor)
{
    using namespace math::linear;

    Vector<double> vsrc(3);
    vsrc[0] = 1;
    vsrc[1] = 2;
    vsrc[2] = 3;
    Vector<double> v(vsrc);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], vsrc[0]);
    TEST_ASSERT_EQ(v[1], vsrc[1]);
    TEST_ASSERT_EQ(v[2], vsrc[2]);

    vsrc[0] = -1;
    TEST_ASSERT_EQ(vsrc[0], -1); // should change
    TEST_ASSERT_EQ(v[0], 1); // shouldn't change
}


TEST_CASE(testStdVectorConstructor)
{
    using namespace math::linear;

    std::vector<double> stdvec{10, 11, 12};

    Vector<double> v(stdvec);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], stdvec[0]);
    TEST_ASSERT_EQ(v[1], stdvec[1]);
    TEST_ASSERT_EQ(v[2], stdvec[2]);

    stdvec[0] *= -1;
    stdvec[1] *= -1;
    stdvec[2] *= -1;

    TEST_ASSERT_EQ(v[0], 10);
    TEST_ASSERT_EQ(v[1], 11);
    TEST_ASSERT_EQ(v[2], 12);
}


TEST_CASE(testAssignmentOperator)
{
    using namespace math::linear;

    Vector<double> vsrc(2);
    vsrc[0] = 42;
    vsrc[1] = 99;
    Vector<double> v;
    v = vsrc;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(v[0], 42);
    TEST_ASSERT_EQ(v[1], 99);
    // TODO: What if I change the length of v5?  Does v7 remain unchanged (length & content)?
}


TEST_CASE(testScalarAssignment)
{
    using namespace math::linear;

    Vector<double> v(5, 123.456);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(5));
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v[i], 123.456);

    v = 99;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(v[0], 99);
}


TEST_CASE(testStdVectorAssignment)
{
    using namespace math::linear;

    const std::vector<double> stdvec{10, 11, 12};

    Vector<double> v(20, -1);
    v = stdvec;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], 10);
    TEST_ASSERT_EQ(v[1], 11);
    TEST_ASSERT_EQ(v[2], 12);
}


TEST_CASE(testDotProduct)
{
    using namespace math::linear;

    Vector<double> vd1(5, 2);
    Vector<double> vd2(5, 3);
    const double dotprod(vd1.dot(vd2));
    TEST_ASSERT_EQ(dotprod, 2*3*5);

    bool threw(false);
    try 
    {
        Vector<double> wrongSize(2);
        vd1.dot(wrongSize);
    }
    catch(...)
    {
        threw = true;
    }
    TEST_ASSERT(threw);
}


TEST_CASE(testNorm)
{
    using namespace math::linear;

    const std::vector<double> stdvec{10, 11, 12};
    Vector<double> vnorm1(stdvec);
    const double norm(vnorm1.norm());
    const double arg(10.*10 + 11.*11 + 12.*12);
    const double expectedValue(::sqrt(arg));
    TEST_ASSERT_EQ(norm, expectedValue);
}


TEST_CASE(testNormalize)
{
    using namespace math::linear;

    Vector<double> vnorm(4, 4.0);
    vnorm.normalize();
    TEST_ASSERT_EQ(vnorm[0], 0.5);
    TEST_ASSERT_EQ(vnorm[1], 0.5);
    TEST_ASSERT_EQ(vnorm[2], 0.5);
    TEST_ASSERT_EQ(vnorm[3], 0.5);
}


TEST_CASE(testScale)
{
    using namespace math::linear;

    Vector<double> vscale(2,4.0);
    vscale.scale(1.0 / 4.0);
    TEST_ASSERT_EQ(vscale[0], 1.0);
    TEST_ASSERT_EQ(vscale[1], 1.0);
}

TEST_CASE(testUnit)
{
    using namespace math::linear;

    Vector<double> v(4,4.0);
    Vector<double> vunit = v.unit();
    TEST_ASSERT_EQ(vunit[0], 0.5);
    TEST_ASSERT_EQ(vunit[1], 0.5);
    TEST_ASSERT_EQ(vunit[2], 0.5);
    TEST_ASSERT_EQ(vunit[3], 0.5);
}

TEST_CASE(testOperatorPlusEquals)
{
    using namespace math::linear;

    Vector<double> v1(3, 1);
    Vector<double> v2(3, -1);

    v2 += v1;

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v2[0], 0);
    TEST_ASSERT_EQ(v2[1], 0);
    TEST_ASSERT_EQ(v2[2], 0);

    TEST_ASSERT_EQ(v1[0], 1);
    TEST_ASSERT_EQ(v1[1], 1);
    TEST_ASSERT_EQ(v1[2], 1);
}


TEST_CASE(testOperatorPlus)
{
    using namespace math::linear;

    Vector<double> v1(3, 42);
    Vector<double> v2(3, 11);
    Vector<double> v3;

    v3 = v1 + v2;

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v1[0], 42);
    TEST_ASSERT_EQ(v1[1], 42);
    TEST_ASSERT_EQ(v1[2], 42);

    TEST_ASSERT_EQ(v2[0], 11);
    TEST_ASSERT_EQ(v2[1], 11);
    TEST_ASSERT_EQ(v2[2], 11);

    TEST_ASSERT_EQ(v3[0], 53);
    TEST_ASSERT_EQ(v3[1], 53);
    TEST_ASSERT_EQ(v3[2], 53);
}


TEST_CASE(testOperatorMinusEquals)
{
    using namespace math::linear;

    //TODO: Vector<double>& operator-=(const Vector<double>& v)
    Vector<double> v1(5, 13);
    Vector<double> v2(5, -5);

    v2 -= v1;
    TEST_ASSERT_EQ(std::ssize(v1), 5);
    TEST_ASSERT_EQ(std::ssize(v2), 5);
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v2[i], -18);
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v1[i], 13);
}


TEST_CASE(testNegate)
{
    using namespace math::linear;

    Vector<double> X(3);
    Vector<double> Y(3);
    X[0] = Y[2] =  1.;
    X[1] = Y[1] =  0.;
    X[2] = Y[0] = -1.;
    TEST_ASSERT_EQ(X, -Y);
}


TEST_CASE(testAdd)
{
    using namespace math::linear;

    Vector<double> v1(3, 2.4);
    Vector<double> v2(3, 1.4);

    // TODO: Test what happens if v1 & v2 are of different lengths.
    Vector<double> v3(v2.add(v1));

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v1[0], 2.4);
    TEST_ASSERT_EQ(v1[1], 2.4);
    TEST_ASSERT_EQ(v1[2], 2.4);

    TEST_ASSERT_EQ(v2[0], 1.4);
    TEST_ASSERT_EQ(v2[1], 1.4);
    TEST_ASSERT_EQ(v2[2], 1.4);

    TEST_ASSERT_EQ(v3[0], 3.8);
    TEST_ASSERT_EQ(v3[1], 3.8);
    TEST_ASSERT_EQ(v3[2], 3.8);
}


TEST_CASE(testSubtract)
{
    using namespace math::linear;

    Vector<double> v1(3, 2.4);
    Vector<double> v2(3, 1.4);

    // TODO: Test what happens if v1 & v2 are of different lengths.
    Vector<double> v3(v2.subtract(v1));

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v1[0], 2.4);
    TEST_ASSERT_EQ(v1[1], 2.4);
    TEST_ASSERT_EQ(v1[2], 2.4);

    TEST_ASSERT_EQ(v2[0], 1.4);
    TEST_ASSERT_EQ(v2[1], 1.4);
    TEST_ASSERT_EQ(v2[2], 1.4);

    TEST_ASSERT_EQ(v3[0], -1);
    TEST_ASSERT_EQ(v3[1], -1);
    TEST_ASSERT_EQ(v3[2], -1);
}


TEST_CASE(testOperatorMinus)
{
    using namespace math::linear;

    Vector<double> v1(4), v2(4);
    for (int i = 0; i < 4; i++)
    {
        v1[i] = i;
        v2[i] = -i;
    }
    Vector<double> v3(v2 - v1);
    // TODO: Test what happens if v1 & v2 are of different lengths.

    TEST_ASSERT_EQ(std::ssize(v1), 4);
    TEST_ASSERT_EQ(std::ssize(v2), 4);
    TEST_ASSERT_EQ(std::ssize(v3), 4);

    for (int i = 0; i < 4; i++)
    {
        TEST_ASSERT_EQ(v1[i], i);
        TEST_ASSERT_EQ(v2[i], -i);
    }

    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v3[i], -2 * i);
}


TEST_CASE(testOperatorTimesEquals)
{
    using namespace math::linear;

    Vector<double> v1(4);
    for (int i = 0; i < 4; i++)
        v1[i] = i;
    Vector<double> v2(4);
    for (int i = 0; i < 4; i++)
        v2[i] = -i;

    v2 *= v1;

    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v1[i], i);
    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v2[i], -i * i);

    // TODO: Test what happens if v1 & v2 are of different lengths.
}


TEST_CASE(testOperatorTimesEqualsScalar)
{
    using namespace math::linear;

    Vector<double> v1(5);
    for (int i = 0; i < 5; i++)
        v1[i] = i;

    v1 *= 3;

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v1[i], 3 * i);
}


TEST_CASE(testOperatorTimesScalar)
{
    using namespace math::linear;

    Vector<double> v1(5);
    for (int i = 0; i < 5; i++)
        v1[i] = i;

    Vector<double> v2(v1 * 2);

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v1[i], i);
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v2[i], 2 * i);
}

TEST_CASE(testElementDivision)
{
    using namespace math::linear;

    Vector<double> numerator(3);
    numerator[0] = 0;
    numerator[1] = 1;
    numerator[2] = 2;

    Vector<double> denominator(3);
    denominator[0] = 1;
    denominator[1] = 2;
    denominator[2] = 3;

    Vector<double> quotient(numerator / denominator);
    TEST_ASSERT_EQ(quotient[0], 0);
    TEST_ASSERT_EQ(quotient[1], 1./2.);
    TEST_ASSERT_EQ(quotient[2], 2./3.);
}


TEST_CASE(testOperatorTimes)
{
    using namespace math::linear;

    Vector<double> v1(4);
    Vector<double> v2(4);
    for (int i = 0; i < 4; i++)
    {
        v1[i] = i;
        v2[i] = i*2.;
    }

    Vector<double> v3(v1 * v2);
    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v3[i], i * i * 2.);
}

TEST_CASE(testOperatorDivide)
{
    using namespace math::linear;

    Vector<double> v1(4);
    Vector<double> v2(4);
    for (int i = 0; i < 4; i++)
    {
        v1[i] = i;
        v2[i] = i + 2.;
    }

    Vector<double> v3(v1 / v2);
    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v3[i], i / (i + 2.));
}

TEST_MAIN(
    TEST_CHECK(testDefaultConstructor);
    TEST_CHECK(testScalarConstructor);
    TEST_CHECK(testRawConstructor);
    TEST_CHECK(testCopyConstructor);
    TEST_CHECK(testStdVectorConstructor);
    TEST_CHECK(testAssignmentOperator);
    TEST_CHECK(testScalarAssignment);
    TEST_CHECK(testStdVectorAssignment);
    TEST_CHECK(testDotProduct);
    TEST_CHECK(testNorm);
    TEST_CHECK(testNormalize);
    TEST_CHECK(testUnit);
    TEST_CHECK(testOperatorPlusEquals);
    TEST_CHECK(testOperatorPlus);
    TEST_CHECK(testOperatorMinus);
    TEST_CHECK(testNegate);
    TEST_CHECK(testAdd);
    TEST_CHECK(testSubtract);
    TEST_CHECK(testOperatorMinus);
    TEST_CHECK(testOperatorTimesEquals);
    TEST_CHECK(testOperatorTimesEqualsScalar);
    TEST_CHECK(testOperatorTimesScalar);
    TEST_CHECK(testElementDivision);
    TEST_CHECK(testOperatorTimes);
    TEST_CHECK(testOperatorDivide);

    // Methods not (yet) tested:

    //TODO: Vector<double>(Matrix2D) constructor.
    //TODO: Vector<double>& operator=(const Matrix2D<_T>& mx)
    //TODO: Matrix2D<_T>& matrix()
    //TODO: const Matrix2D<_T>& matrix() const
    //TODO: const std::vector<_T>& vec() const
    //TODO: template<typename Vector<double>_T> bool operator==(const Vector<double>_T& v) const
    //TODO: template<typename Vector<double>_T> bool operator!=(const Vector<double>_T& v) const
    //TODO: template<typename _T> Vector<double><_T> cross(const Vector<double><_T>& u, const Vector<double><_T>& v)
    //TODO: template<typename _T> Vector<double><_T> constantVector<double>(size_t sz, _T cv = 0)
    //TODO: template<typename _T> math::linear::Vector<double><_T> operator*(const math::linear::Matrix2D<_T>& m, const math::linear::Vector<double><_T>& v)
    //TODO: template<typename _T> math::linear::Vector<double><_T> operator*(_T scalar, const math::linear::Vector<double><_T>& v)
    //TODO: template<typename _T> std::ostream& operator<<(std::ostream& os, const math::linear::Vector<double><_T>& v)
)
