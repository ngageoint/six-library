#include <iostream>
#include <string>
#include <vector>
#include <std/filesystem>

#include <import/sys.h>

#include "TestCase.h"
#include "scene/ProjectionPolynomialFitter.h"
#include "scene/ProjectionModel.h"
#include "six/sicd/ComplexData.h"
#include "six/sicd/Utilities.h"

std::unique_ptr<scene::ProjectionPolynomialFitter>
loadPolynomialFitter()
{
  static const auto modulePath = std::filesystem::path("croppedNitfs") / "SICD";
    static const auto sicdPathname = six::testing::getModuleFile(modulePath, "cropped_sicd_110.nitf");
    std::clog << sicdPathname << "\n";

    std::vector<std::string> schemaPaths;
    std::unique_ptr<six::sicd::ComplexData> complexData;
    std::vector<six::zfloat > buffer;
    six::sicd::Utilities::readSicd(sicdPathname.string(), schemaPaths, complexData, buffer);
    return six::sicd::Utilities::getPolynomialFitter(*complexData);
}

// Making this global so we don't have to re-read the file every test
std::unique_ptr<scene::ProjectionPolynomialFitter> globalFitter;

static constexpr size_t NUM_POINTS = 9;
inline const double* slant_plane_points(size_t i)
{
    static const double SLANT_PLANE_POINTS[NUM_POINTS][2] =
    {
        { 9.520652980, 44.892233669},
        {10.393467082,  2.839874624},
        { 1.107603012,  1.070485237},
        { 9.473386201,  9.104675156},
        {18.768850889, 18.031636327},
        {28.065732402, 26.959699738},
        {37.369870640, 35.893508536},
        {46.697832227, 44.846143620},
        {56.085818242, 53.846073695}
    };
    return SLANT_PLANE_POINTS[i];
}

inline const double* output_plane_points(size_t i)
{
    static const double OUTPUT_PLANE_POINTS[NUM_POINTS][2] =
    {
        {10, 50},
        { 11, 3},
        { 1,  1},
        {10, 10},
        {20, 20},
        {30, 30},
        {40, 40},
        {50, 50},
        {60, 60},
    };
    return OUTPUT_PLANE_POINTS[i];
}

TEST_CASE(testProjectOutputToSlant)
{
    if (globalFitter == nullptr)
    {
        globalFitter = loadPolynomialFitter();
    }

    math::poly::TwoD<double> outputToSlantRow;
    math::poly::TwoD<double> outputToSlantCol;
    globalFitter->fitOutputToSlantPolynomials(
            types::RowCol<size_t>(100, 100),
            types::RowCol<double>(4839, 4299),
            types::RowCol<double>(6671.89705, 5864.140367),
            types::RowCol<double>(0.6595917, 0.9378943),
            3, 3,
            outputToSlantRow,
            outputToSlantCol);

    for (size_t ii = 0; ii < NUM_POINTS; ++ii)
    {
        const double* output = output_plane_points(ii);
        const double* slant = slant_plane_points(ii);
        TEST_ASSERT_ALMOST_EQ(
                outputToSlantRow.atY(output[1])(output[0]),
                slant[0]);
        TEST_ASSERT_ALMOST_EQ(
                outputToSlantCol.atY(output[1])(output[0]),
                slant[1]);
    }
}

TEST_CASE(testProjectSlantToOutput)
{
    if (globalFitter == nullptr)
    {
        globalFitter = loadPolynomialFitter();
    }

    math::poly::TwoD<double> slantToOutputRow;
    math::poly::TwoD<double> slantToOutputCol;
    globalFitter->fitSlantToOutputPolynomials(
            types::RowCol<size_t>(100, 100),
            types::RowCol<double>(4839, 4299),
            types::RowCol<double>(6671.89705, 5864.140367),
            types::RowCol<double>(0.6595917, 0.9378943),
            3, 3,
            slantToOutputRow,
            slantToOutputCol);

    for (size_t ii = 0; ii < NUM_POINTS; ++ii)
    {
        const double* output = output_plane_points(ii);
        const double* slant = slant_plane_points(ii);
        TEST_ASSERT_ALMOST_EQ_EPS(
                slantToOutputRow.atY(slant[1])(slant[0]), output[0],
                10e-3);
        TEST_ASSERT_ALMOST_EQ_EPS(
                slantToOutputCol.atY(slant[1])(slant[0]), output[1],
                10e-3);
    }
}

TEST_MAIN(
    TEST_CHECK(testProjectOutputToSlant);
    TEST_CHECK(testProjectSlantToOutput);
    )
