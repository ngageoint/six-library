// 8AMPI_PHSI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <assert.h>

#include <iostream>
#include <span>
#include <algorithm>
#include <iterator>
#include <future>
#include <vector>
//#include <execution>
#include <chrono>

#include "six/AmplitudeTable.h"
#include "six/sicd/Utilities.h"

using namespace six;

static void toComplex(six::Amp8iPhs8iLookup_t values, std::span<const AMP8I_PHS8I_t> inputs, std::span<six::zfloat> results)
{
    const auto toComplex_ = [&values](const auto& v)
    {
        return values(v.amplitude, v.phase);
    };
    
    std::transform(inputs.begin(), inputs.end(), results.begin(), toComplex_);
    //std::transform(std::execution::par, inputs.begin(), inputs.end(), results.begin(), toComplex_);
}
void toComplex(std::span<const AMP8I_PHS8I_t> inputs, std::span<six::zfloat> results)
{
    const auto values = six::sicd::ImageData::getLookup(nullptr);
    toComplex(values, inputs, results);
}

auto make_inputs(size_t count)
{
    std::vector<AMP8I_PHS8I_t> retval;
    retval.reserve(count);
    for (size_t i = 0; i < count; i++)
    {
        const auto amplitude = static_cast<uint8_t>(i * i);
        const auto phase = static_cast<uint8_t>(~amplitude);
        retval.push_back(AMP8I_PHS8I_t{ amplitude, phase });
    }
    return retval;
}

static std::vector<AMP8I_PHS8I_t> fromComplex_nearest_neighbors(std::span<const six::zfloat> inputs)
{
    static const six::sicd::ImageData imageData;
    assert(imageData.amplitudeTable.get() == nullptr);
    return imageData.fromComplex(inputs);
}
//static std::vector<AMP8I_PHS8I_t> fromComplex_nearest_neighbors2(std::span<const six::zfloat> inputs)
//{
//    return six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors2(inputs, nullptr);
//}
//
//static std::vector<AMP8I_PHS8I_t> fromComplex_nearest_neighbors_par(std::span<const six::zfloat> inputs)
//{
//    return six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors(std::execution::par, inputs, nullptr);
//}
//static std::vector<AMP8I_PHS8I_t> fromComplex_nearest_neighbors_par2(std::span<const six::zfloat> inputs)
//{
//    return six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_par2(inputs, nullptr);
//}

//static std::vector<AMP8I_PHS8I_t> fromComplex_nearest_neighbors_unseq(std::span<const six::zfloat> inputs)
//{
//    return six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors(std::execution::unseq, inputs, nullptr);
//}
//static std::vector<AMP8I_PHS8I_t> fromComplex_nearest_neighbors_par_unseq(std::span<const six::zfloat> inputs)
//{
//    return six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors(std::execution::par_unseq, inputs, nullptr);
//}

//static void fromComplex_nearest_neighbors_threaded(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results)
//{
//    // make a structure to quickly find the nearest neighbor
//    auto& converter = six::sicd::details::ComplexToAMP8IPHS8I::make(nullptr);
//    converter.nearest_neighbors_threaded(inputs, results);
//}
//static void fromComplex_nearest_neighbors_simd(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results)
//{
//    // make a structure to quickly find the nearest neighbor
//    auto& converter = six::sicd::details::ComplexToAMP8IPHS8I::make(nullptr);
//    converter.nearest_neighbors_simd(inputs, results);
//}

#ifdef NDEBUG
constexpr auto iterations = 10;
#else
constexpr auto iterations = 1;
#endif
template<typename TFunc>
static std::chrono::duration<double> test(TFunc f, const std::vector<six::zfloat>& inputs)
{
    auto ap_results = f(inputs);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        ap_results = f(inputs);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return end - start;
}

int main()
{
    #ifdef NDEBUG
    constexpr auto inputs_size = 25000000;
    #else
    constexpr auto inputs_size = 100;
    #endif
    const auto inputs = make_inputs(inputs_size * 4);
    std::vector<six::zfloat> results(inputs.size());

    toComplex(inputs, results);

    /*********************************************************************************/
    auto diff = test(fromComplex_nearest_neighbors, results);
    std::cout << "Time (nearest_neighbors): " << std::setw(9) << diff.count() << "\n";

    //diff = test(fromComplex_nearest_neighbors2, results);
    //std::cout << "Time (nearest_neighbors2): " << std::setw(9) << diff.count() << "\n";

    //diff = test(fromComplex_nearest_neighbors_par, results);
    //std::cout << "Time (nearest_neighbors_par): " << std::setw(9) << diff.count() << "\n";

    //diff = test(fromComplex_nearest_neighbors_par2, results);
    //std::cout << "Time (nearest_neighbors_par2): " << std::setw(9) << diff.count() << "\n";

    //diff = test(fromComplex_nearest_neighbors_unseq, results);
    //std::cout << "Time (nearest_neighbors_unseq): " << std::setw(9) << diff.count() << "\n";

    //diff = test(fromComplex_nearest_neighbors_par_unseq, results);
    //std::cout << "Time (nearest_neighbors_par_unseq): " << std::setw(9) << diff.count() << "\n";

    //fromComplex_transform(results, ap_results);
    //start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < iterations; i++)
    //{
    //    fromComplex_transform(results, ap_results);
    //}
    //end = std::chrono::high_resolution_clock::now();
    //diff = end - start;
    //std::cout << "Time (transform): " << std::setw(9) << diff.count() << "\n";
    //const auto transform_results = ap_results;

    ///*********************************************************************************/

    //fromComplex_nearest_neighbors_threaded(results, ap_results);
    //start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < iterations; i++)
    //{
    //    fromComplex_nearest_neighbors_threaded(results, ap_results);
    //}
    //end = std::chrono::high_resolution_clock::now();
    //diff = end - start;
    //std::cout << "Time (nearest_neighbors_threaded): " << std::setw(9) << diff.count() << "\n";
    //const auto nearest_neighbors_threaded_results = ap_results;

    //fromComplex_transform_par(results, ap_results);
    //start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < iterations; i++)
    //{
    //    fromComplex_transform_par(results, ap_results);
    //}
    //end = std::chrono::high_resolution_clock::now();
    //diff = end - start;
    //std::cout << "Time (transform_par): " << std::setw(9) << diff.count() << "\n";
    //const auto transform_par_results = ap_results;

    ///*********************************************************************************/

    //fromComplex_nearest_neighbors_simd(results, ap_results);
    //start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < iterations; i++)
    //{
    //    fromComplex_nearest_neighbors_simd(results, ap_results);
    //}
    //end = std::chrono::high_resolution_clock::now();
    //diff = end - start;
    //std::cout << "Time (nearest_neighbors_simd): " << std::setw(9) << diff.count() << "\n";
    //const auto nearest_neighbors_threaded_simd = ap_results;

    //fromComplex_transform_unseq(results, ap_results);
    //start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i < iterations; i++)
    //{
    //    fromComplex_transform_unseq(results, ap_results);
    //}
    //end = std::chrono::high_resolution_clock::now();
    //diff = end - start;
    //std::cout << "Time (transform_unseq): " << std::setw(9) << diff.count() << "\n";
    //const auto transform_unseq_results = ap_results;

}

