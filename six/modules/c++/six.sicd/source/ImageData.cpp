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

#include <gsl/gsl.h>
#include <mt/Algorithm.h>

#include "six/sicd/GeoData.h"
#include "six/sicd/Utilities.h"
#include "six/sicd/ComplexToAMP8IPHS8I.h"

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

struct KDNode_t final
{
    cx_float result;
    AMP8I_PHS8I_t amp_and_value;
};
static std::vector<KDNode_t> make_nodes(const six::AmplitudeTable* pAmplitudeTable)
{
    // For all possible amp/phase values (there are "only" 256*256), get and save the
    // complex<float> value.
    //
    // Be careful with indexing so that we don't wrap-around in the loops.
    std::vector<KDNode_t> retval;
    retval.reserve(UINT8_MAX * UINT8_MAX);
    for (uint16_t input_amplitude = 0; input_amplitude <= UINT8_MAX; input_amplitude++)
    {
        KDNode_t v;
        v.amp_and_value.first = gsl::narrow<uint8_t>(input_amplitude);

        for (uint16_t input_value = 0; input_value <= UINT8_MAX; input_value++)
        {
            v.amp_and_value.second = gsl::narrow<uint8_t>(input_value);
            v.result = six::sicd::Utilities::from_AMP8I_PHS8I(v.amp_and_value.first, v.amp_and_value.second, pAmplitudeTable);
            retval.push_back(v);
        }
    }
    return retval;
}

// input_amplitudes_t is too big for the stack
static std::unique_ptr<input_amplitudes_t> AMP8I_PHS8I_to_RE32F_IM32F_(const six::AmplitudeTable* pAmplitudeTable)
{
    // Get all 256x256 values for the AmplitudeTable
    auto nodes = make_nodes(pAmplitudeTable);

    auto retval = std::make_unique<input_amplitudes_t>();
    auto& values = *retval;
    for (auto&& n : nodes)
    {
        values[n.amp_and_value.first][n.amp_and_value.second] = std::move(n.result);
    }

    return retval;
}

// This is a non-templatized function so that there is copy of the "static" data with a NULL AmplutdeTable.
static const input_amplitudes_t* get_cached_RE32F_IM32F_values(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto RE32F_IM32F_values_no_amp = AMP8I_PHS8I_to_RE32F_IM32F_(nullptr);
        return RE32F_IM32F_values_no_amp.get();
    }
    return nullptr;
}

std::complex<float> ImageData::from_AMP8I_PHS8I(const AMP8I_PHS8I_t& input) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    auto const pAmplitudeTable = amplitudeTable.get();
    auto const pValues = get_cached_RE32F_IM32F_values(pAmplitudeTable);

    // Do we have a cahced result to use (no amplitude table)?
    // Or must it be recomputed (have an amplutude table)?
    if (pValues != nullptr)
    {
        return (*pValues)[input.first][input.second];
    }

    const auto S = Utilities::from_AMP8I_PHS8I(input.first, input.second, pAmplitudeTable);
    return std::complex<float>(gsl::narrow_cast<float>(S.real()), gsl::narrow_cast<float>(S.imag()));
}

const input_amplitudes_t& ImageData::get_RE32F_IM32F_values(const six::AmplitudeTable* pAmplitudeTable,
    std::unique_ptr<input_amplitudes_t>& pValues_)
{
    const input_amplitudes_t* pValues = get_cached_RE32F_IM32F_values(pAmplitudeTable);
    if (pValues == nullptr)
    {
        assert(pAmplitudeTable != nullptr);
        pValues_ = AMP8I_PHS8I_to_RE32F_IM32F_(pAmplitudeTable);
        pValues = pValues_.get();
    }
    assert(pValues != nullptr);
    return *pValues;
}

void ImageData::from_AMP8I_PHS8I(std::span<const AMP8I_PHS8I_t> inputs, std::span<std::complex<float>> results,
    ptrdiff_t cutoff_) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    std::unique_ptr<input_amplitudes_t> pValues_;
    const auto& values = get_RE32F_IM32F_values(amplitudeTable.get(), pValues_);
    from_AMP8I_PHS8I(values, inputs, results, cutoff_);
}

void ImageData::from_AMP8I_PHS8I(const input_amplitudes_t& values, std::span<const AMP8I_PHS8I_t> inputs, std::span<std::complex<float>> results,
    ptrdiff_t cutoff_)
{
    const auto get_RE32F_IM32F_value_f = [&values](const six::sicd::AMP8I_PHS8I_t& v)
    {
        return values[v.first][v.second];
    };

    if (cutoff_ < 0)
    {
        (void) std::transform(inputs.begin(), inputs.end(), results.begin(),
            get_RE32F_IM32F_value_f);
    }
    else
    {
        // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
        constexpr auto dimension = 128 * 8;
        constexpr auto default_cutoff = dimension * dimension;
        const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
        (void) mt::transform_async(inputs.begin(), inputs.end(), results.begin(),
            get_RE32F_IM32F_value_f, cutoff, std::launch::async);
    }
}

template<typename TConverter>
static void to_AMP8I_PHS8I_(std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results,
    const TConverter& tree, ptrdiff_t cutoff_)
{
    const auto nearest_neighbor_f = [&](const std::complex<float>& v)
    {
        return tree.nearest_neighbor(v);
    };

    if (cutoff_ < 0)
    {
        (void) std::transform(inputs.begin(), inputs.end(), results.begin(),
            nearest_neighbor_f);
    }
    else
    {
        // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
        constexpr auto dimension = 128 * 8;
        constexpr auto default_cutoff = dimension * dimension;
        const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
        (void) mt::transform_async(inputs.begin(), inputs.end(), results.begin(),
            nearest_neighbor_f, cutoff, std::launch::async);
    }
}
void ImageData::to_AMP8I_PHS8I(std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results,
    ptrdiff_t cutoff) const
{
    to_AMP8I_PHS8I(amplitudeTable.get(), inputs, results, cutoff);
}
void  ImageData::to_AMP8I_PHS8I(const AmplitudeTable* pAmplitudeTable,
    std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results, ptrdiff_t cutoff)
{
    // make a structure to quickly find the nearest neighbor
    std::unique_ptr<six::sicd::details::ComplexToAMP8IPHS8I> pConvert; // not-cached, non-NULL amplitudeTable
    const auto& converter = *(six::sicd::details::ComplexToAMP8IPHS8I::make(pAmplitudeTable, pConvert));
    to_AMP8I_PHS8I_(inputs, results, converter, cutoff);
}
