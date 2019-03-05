#include <iostream>
#include <string>
#include <vector>

#include "TestCase.h"
#include "scene/ProjectionPolynomialFitter.h"
#include "scene/ProjectionModel.h"
#include "six/sicd/ComplexData.h"
#include "six/sicd/Utilities.h"

namespace
{
std::auto_ptr<scene::ProjectionPolynomialFitter>
loadPolynomialFitter(const sys::Path& exePath)
{
    const sys::Path sixHome = exePath.
        join("..").join("..").join("..").join("..");
    const sys::Path sicdPathname = sixHome.
        join("croppedNitfs").
        join("SICD").
        join("cropped_sicd_110.nitf").getAbsolutePath();

    if (!sicdPathname.isFile())
    {
        std::ostringstream oss;
        oss << "Environment error: Cannot find SICD file: " << sicdPathname;
        throw except::Exception(Ctxt(oss.str()));
    }

    std::vector<std::string> schemaPaths;
    std::auto_ptr<six::sicd::ComplexData> complexData;
    std::vector<std::complex<float> > buffer;
    six::sicd::Utilities::readSicd(sicdPathname, schemaPaths, complexData, buffer);
    return six::sicd::Utilities::getPolynomialFitter(*complexData);
}

class TestTransform
{
public:
    TestTransform(double scale, double offset) :
        mScale(scale), mOffset(offset)
    {
    }

    inline double operator()(double val) const
    {
        return mScale * val + mOffset;
    }
private:
    const double mScale;
    const double mOffset;
};
}

std::auto_ptr<scene::ProjectionPolynomialFitter> globalFitter;

TEST_CASE(testFitOutputToSlant)
{
    math::poly::TwoD<double> outputToSlantRow;
    math::poly::TwoD<double> outputToSlantCol;
    types::RowCol<double> meanResidualError(0, 0);
    globalFitter->fitOutputToSlantPolynomials(
            types::RowCol<size_t>(0, 0),
            types::RowCol<double>(100, 200),
            types::RowCol<double>(50, 100),
            types::RowCol<double>(75, 75),
            4, 5,
            outputToSlantRow,
            outputToSlantCol,
            &meanResidualError.row,
            &meanResidualError.col);
    TEST_ASSERT_EQ(outputToSlantRow.orderX(), 4);
    TEST_ASSERT_EQ(outputToSlantRow.orderY(), 5);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[0][0], -7.46223388);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[0][1], 1.0399861e-5);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[0][2], 0);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[1][0], 0.0081644);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[2][0], 0);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[3][5], 0);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow[4][4], 0);

    TEST_ASSERT_EQ(outputToSlantCol.orderX(), 4);
    TEST_ASSERT_EQ(outputToSlantCol.orderY(), 5);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[0][0], 28.37544471);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[0][1],  0.011188276);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[0][2], 0);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[1][0], -2.498894866e-5);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[2][0], 0);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[3][5], 0);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol[4][4], 0);

    TEST_ASSERT_ALMOST_EQ(meanResidualError.row, 0);
    TEST_ASSERT_ALMOST_EQ(meanResidualError.col, 0);
}

TEST_CASE(testFitSlantToOutput)
{
    math::poly::TwoD<double> slantToOutputRow;
    math::poly::TwoD<double> slantToOutputCol;
    types::RowCol<double> meanResidualError(0, 0);
    globalFitter->fitSlantToOutputPolynomials(
            types::RowCol<size_t>(0, 0),
            types::RowCol<double>(100, 200),
            types::RowCol<double>(50, 100),
            types::RowCol<double>(75, 75),
            4, 5,
            slantToOutputRow,
            slantToOutputCol,
            &meanResidualError.row,
            &meanResidualError.col);
    TEST_ASSERT_EQ(slantToOutputRow.orderX(), 4);
    TEST_ASSERT_EQ(slantToOutputRow.orderY(), 5);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[0][0], -2.099001948148667e17);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[0][1], 3.693853355802044e16);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[0][2], -2600197686984346);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[1][0], -1.12844898417075856e17);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[2][0], -2.2750011537883492e16);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[3][5], 110098567.4533378);
    TEST_ASSERT_ALMOST_EQ(slantToOutputRow[4][4], -525535831.180812);

    TEST_ASSERT_EQ(slantToOutputCol.orderX(), 4);
    TEST_ASSERT_EQ(slantToOutputCol.orderY(), 5);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[0][0], -1.18789782944479152e17);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[0][1], 20905861932914252);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[0][2], -1471692199279142.5);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[1][0], -63891085376658856);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[2][0], -12886407881280032);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[3][5], 62407166.96594446897);
    TEST_ASSERT_ALMOST_EQ(slantToOutputCol[4][4], -298005623.64804006);

    TEST_ASSERT_ALMOST_EQ(meanResidualError.row, 8150673.785804484);
    TEST_ASSERT_ALMOST_EQ(meanResidualError.col, 728442.6896463062);
}

TEST_CASE(testFitTimeCOA)
{
    math::poly::TwoD<double> timeCOAPoly;
    double meanResidualError(0);
    globalFitter->fitTimeCOAPolynomial(
            types::RowCol<size_t>(50, 50),
            types::RowCol<size_t>(1, 2),
            3, 2,
            timeCOAPoly,
            &meanResidualError);

    TEST_ASSERT_ALMOST_EQ(timeCOAPoly.orderX(), 3);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly.orderY(), 2);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[0][0], .5786451704);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[1][0], -4.86554e-7);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[2][0], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[3][0], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[0][1], 0.00011022);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[1][1], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[2][1], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[3][1], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[0][2], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[1][2], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[2][2], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[3][2], 0);
    TEST_ASSERT_ALMOST_EQ(meanResidualError, 0);
}

TEST_CASE(testFitPixelBasedTimeCOA)
{
    TestTransform rowTransform(2, 0);
    TestTransform colTransform(3, -1);

    math::poly::TwoD<double> timeCOAPoly;
    double meanResidualError(0);
    globalFitter->fitPixelBasedTimeCOAPolynomial(
            rowTransform,
            colTransform,
            3, 2,
            timeCOAPoly,
            &meanResidualError);

    TEST_ASSERT_ALMOST_EQ(timeCOAPoly.orderX(), 3);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly.orderY(), 2);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[0][0], .56772138);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[1][0], -2.47297e-7);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[2][0], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[3][0], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[0][1], 7.34791e-5);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[1][1], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[2][1], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[3][1], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[0][2], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[1][2], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[2][2], 0);
    TEST_ASSERT_ALMOST_EQ(timeCOAPoly[3][2], 0);
    TEST_ASSERT_ALMOST_EQ(meanResidualError, 0);
}

TEST_CASE(testProjectSlantToOutput)
{
    math::poly::TwoD<double> outputToSlantRow;
    math::poly::TwoD<double> outputToSlantCol;
    types::RowCol<double> meanResidualError(0, 0);
    globalFitter->fitOutputToSlantPolynomials(
            types::RowCol<size_t>(100, 100),
            types::RowCol<double>(4839, 4299),
            types::RowCol<double>(6671.89705, 5864.140367),
            types::RowCol<double>(0.6595917, 0.9378943),
            4, 5,
            outputToSlantRow,
            outputToSlantCol,
            &meanResidualError.row,
            &meanResidualError.col);

    TEST_ASSERT_ALMOST_EQ(outputToSlantCol.atY(50)(10), 44.289045411);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow.atY(50)(10), 8.06718398);
    TEST_ASSERT_ALMOST_EQ(outputToSlantCol.atY(3)(11), 2.83987462366);
    TEST_ASSERT_ALMOST_EQ(outputToSlantRow.atY(3)(11), 10.39346737);
}

int main(int argc, char** argv)
{
    if (argc == 0)
    {
        std::cerr << "This test makes assumptions about the directory structure."
            << " Make sure to call with the executable name as argv[0] so "
            << " we can find the necessary files.\n";
        return 1;
    }
    // Making this global so we don't have to re-read the file every test
    globalFitter = loadPolynomialFitter(std::string(argv[0]));
    TEST_CHECK(testFitOutputToSlant);
    TEST_CHECK(testFitSlantToOutput);
    TEST_CHECK(testFitTimeCOA);
    TEST_CHECK(testFitPixelBasedTimeCOA);
    TEST_CHECK(testProjectSlantToOutput);
    return 0;
}
