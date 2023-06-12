/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "six/sicd/ImageData.h"

#include <stdexcept>
#include <array>
#include <std/memory>
#include <tuple> // std::ignore
#include <algorithm>
#include <iterator>
#include <future>

#include <gsl/gsl.h>
#include <mt/Algorithm.h>

#include "six/sicd/GeoData.h"
#include "six/sicd/Utilities.h"
#include "six/sicd/ComplexToAMP8IPHS8I.h"

 // There was in coda-oss, but I removed it.
 //
 // First of all, C++11's std::async() is now (in 2023) thought of as maybe a
 // bit "half baked," and perhaps shouldn't be emulated.  Then, C++17 added
 // parallel algorithms which might be a better way of satisfying our immediate
 // needs (below) ... although we're still at C++14.
namespace
{
    namespace details
    {
        template <typename InputIt, typename OutputIt, typename TFunc>
        inline OutputIt transform_async(const InputIt first1, const InputIt last1, OutputIt d_first, TFunc f,
            typename std::iterator_traits<InputIt>::difference_type cutoff, std::launch policy)
        {
            // https://en.cppreference.com/w/cpp/thread/async
            const auto len = std::distance(first1, last1);
            if (len < cutoff)
            {
                return std::transform(first1, last1, d_first, f);
            }

            const auto mid1 = first1 + len / 2;
            const auto d_mid = d_first + len / 2;
            auto handle = std::async(policy, transform_async<InputIt, OutputIt, TFunc>, mid1, last1, d_mid, f, cutoff, policy);
            details::transform_async(first1, mid1, d_first, f, cutoff, policy);
            return handle.get();
        }
    }
    template <typename InputIt, typename OutputIt, typename TFunc>
    inline OutputIt transform_async(const InputIt first1, const InputIt last1, OutputIt d_first, TFunc f,
        typename std::iterator_traits<InputIt>::difference_type cutoff, std::launch policy)
    {
        // details::... eliminates the overload
        return details::transform_async(first1, last1, d_first, f, cutoff, policy);
    }
}

using namespace six;
using namespace six::sicd;

bool ImageData::operator==(const ImageData& rhs) const
{
    return (pixelType == rhs.pixelType &&
        amplitudeTable == rhs.amplitudeTable &&
        numRows == rhs.numRows &&
        numCols == rhs.numCols &&
        firstRow == rhs.firstRow &&
        firstCol == rhs.firstCol &&
        fullImage == rhs.fullImage &&
        scpPixel == rhs.scpPixel &&
        validData == rhs.validData);
}

bool ImageData::validate(const GeoData& geoData, logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    // 2.11.1
    if (!validData.empty() && geoData.validData.empty())
    {
        messageBuilder << "ImageData.ValidData/GeoData.ValidData "
            << "required together." << std::endl
            << "ImageData.ValidData exists, but GeoData.ValidData does not.";
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.11.2
    if (validData.empty() && !geoData.validData.empty())
    {
        messageBuilder << "ImageData.ValidData/GeoData.ValidData "
            << "required together." << std::endl
            << "GeoData.ValidData exists, but ImageData.ValidData does not.";
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.11.3 In ValidData, first vertex should have (1) minimum row index
    // and (2) minimum column index if 2 vertices exist with equal minimum
    // row index.
    if (!validData.empty())
    {
        bool minimumRowComesFirst = true;

        for (size_t ii = 1; ii < validData.size(); ++ii)
        {
            if (validData[ii].row < validData[0].row)
            {
                minimumRowComesFirst = false;
                break;
            }
        }
        if (!minimumRowComesFirst)
        {
            messageBuilder << "ImageData.ValidData first row should have"
                << "minimum row index";
            log.error(messageBuilder.str());
            valid = false;
        }
        else
        {
            bool minimumColComesFirst = true;
            std::vector<size_t> minimumIndices;
            for (size_t ii = 0; ii < validData.size(); ++ii)
            {
                if (validData[0].row == validData[ii].row &&
                    validData[ii].col < validData[0].col)
                {
                    minimumColComesFirst = false;
                    break;
                }
            }
            if (!minimumColComesFirst)
            {
                messageBuilder << "ImageData.ValidData first col of matching"
                    << "minimum row index should have minimum col index";
                log.error(messageBuilder.str());
                valid = false;
            }
        }
        if (!Utilities::isClockwise(validData))
        {
            messageBuilder << "ImageData.ValidData should be arrange clockwise";
            log.error(messageBuilder.str());
            valid = false;
        }
    }

    return valid;
}

template<typename TToComplexFunc>
static auto createLookup(TToComplexFunc toComplex)
{
    // input_amplitudes_t is too big for the stack
    auto retval = std::make_unique<six::Amp8iPhs8iLookup_t>();
    auto& values = *retval;

    // For all possible amp/phase values (there are "only" 256*256=65536), get and save the
    // complex<float> value.
    for (const auto amplitude : Utilities::iota_0_256())
    {
        for (const auto phase : Utilities::iota_0_256())
        {
            values[amplitude][phase] = toComplex(amplitude, phase);
        }
    }

    return retval;
}
static auto createLookup(const six::AmplitudeTable& amplitudeTable)
{
    const auto toComplex = [&](auto amplitude, auto phase) {
        return six::sicd::Utilities::toComplex(amplitude, phase, amplitudeTable);
    };
    return createLookup(toComplex);
}
static auto createLookup()
{
    static const auto toComplex = [](auto amplitude, auto phase) {
        return six::sicd::Utilities::toComplex(amplitude, phase);
    };
    return createLookup(toComplex);
}

static const six::Amp8iPhs8iLookup_t* getCachedLookup(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto lookup_no_table = createLookup();
        return lookup_no_table.get();
    }

    // Maybe one has already been created and stored on the table?
    return pAmplitudeTable->getLookup();
}

const six::Amp8iPhs8iLookup_t& ImageData::getLookup(const six::AmplitudeTable* pAmplitudeTable)
{
    auto pLookup = getCachedLookup(pAmplitudeTable);
    if (pLookup == nullptr)
    {
        assert(pAmplitudeTable != nullptr);
        auto& amplitudeTable = *pAmplitudeTable;
        auto lookup = createLookup(amplitudeTable);
        amplitudeTable.cacheLookup_(std::move(lookup));
        pLookup = amplitudeTable.getLookup();
    }
    assert(pLookup != nullptr);
    return *pLookup;
}

void ImageData::from_AMP8I_PHS8I(std::span<const AMP8I_PHS8I_t> inputs, std::span<std::complex<float>> results) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    const auto& values = getLookup(amplitudeTable.get());
    from_AMP8I_PHS8I(values, inputs, results);
}

void ImageData::from_AMP8I_PHS8I(const six::Amp8iPhs8iLookup_t& values, std::span<const AMP8I_PHS8I_t> inputs, std::span<std::complex<float>> results)
{
    const auto get_RE32F_IM32F_value_f = [&values](const six::sicd::AMP8I_PHS8I_t& v)
    {
        return values[v.amplitude][v.phase];
    };

    constexpr ptrdiff_t cutoff_ = 0; // too slow w/o multi-threading
    if (cutoff_ < 0)
    {
        std::ignore = std::transform(inputs.begin(), inputs.end(), results.begin(),
            get_RE32F_IM32F_value_f);
    }
    else
    {
        // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
        constexpr auto dimension = 128 * 8;
        constexpr auto default_cutoff = dimension * dimension;
        const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
        std::ignore = transform_async(inputs.begin(), inputs.end(), results.begin(),
            get_RE32F_IM32F_value_f, cutoff, std::launch::async);
    }
}

template<typename TConverter>
static void to_AMP8I_PHS8I_(std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results,
    const TConverter& tree)
{
    const auto nearest_neighbor_f = [&](const std::complex<float>& v)
    {
        return tree.nearest_neighbor(v);
    };

    constexpr ptrdiff_t cutoff_ = 0; // too slow w/o multi-threading
    if (cutoff_ < 0)
    {
        std::ignore = std::transform(inputs.begin(), inputs.end(), results.begin(),
            nearest_neighbor_f);
    }
    else
    {
        // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
        constexpr auto dimension = 128 * 8;
        constexpr auto default_cutoff = dimension * dimension;
        const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
        std::ignore = transform_async(inputs.begin(), inputs.end(), results.begin(),
            nearest_neighbor_f, cutoff, std::launch::async);
    }
}
void ImageData::to_AMP8I_PHS8I(std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    to_AMP8I_PHS8I(amplitudeTable.get(), inputs, results);
}
void  ImageData::to_AMP8I_PHS8I(const AmplitudeTable* pAmplitudeTable,
    std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results)
{
    // make a structure to quickly find the nearest neighbor
    std::unique_ptr<six::sicd::details::ComplexToAMP8IPHS8I> pConvert; // not-cached, non-NULL amplitudeTable
    auto& converter = six::sicd::details::ComplexToAMP8IPHS8I::make(pAmplitudeTable, pConvert);
    to_AMP8I_PHS8I_(inputs, results, converter);
}
