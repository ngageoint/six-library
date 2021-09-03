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

#include <stdexcept>
#include <array>
#include <std/memory>

#include <gsl/gsl.h>

#include "six/sicd/GeoData.h"
#include "six/sicd/ImageData.h"
#include "six/sicd/Utilities.h"
#include "six/sicd/KDTree.h"

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

static std::vector<ImageData::KDNode> make_KDNodes(const six::AmplitudeTable* pAmplitudeTable)
{
    std::vector<ImageData::KDNode> retval;
    for (uint16_t input_amplitude = 0; input_amplitude <= UINT8_MAX; input_amplitude++)
    {
        ImageData::KDNode v;
        v.amp_and_value.first = gsl::narrow<uint8_t>(input_amplitude);

        for (uint16_t input_value = 0; input_value <= UINT8_MAX; input_value++)
        {
            v.amp_and_value.second = gsl::narrow<uint8_t>(input_value);
            v.result = Utilities::from_AMP8I_PHS8I(v.amp_and_value.first, v.amp_and_value.second, pAmplitudeTable);
            retval.push_back(v);
        }
    }
    return retval;
}

using input_values_t = std::array<std::complex<float>, UINT8_MAX + 1>;
using input_amplitudes_t = std::array<input_values_t, UINT8_MAX + 1>;

// input_amplitudes_t is too big for the stack
static std::unique_ptr<input_amplitudes_t> AMP8I_PHS8I_to_RE32F_IM32F_(const six::AmplitudeTable* pAmplitudeTable)
{
    auto nodes = make_KDNodes(pAmplitudeTable);

    auto retval = std::make_unique<input_amplitudes_t>();
    auto& values = *retval;
    for (auto&& n : nodes)
    {
        values[n.amp_and_value.first][n.amp_and_value.second] = std::move(n.result);
    }

    return retval;
}

static const input_amplitudes_t* get_RE32F_IM32F_values(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto RE32F_IM32F_values_no_amp = AMP8I_PHS8I_to_RE32F_IM32F_(nullptr);
        return RE32F_IM32F_values_no_amp.get();
    }
    return nullptr;
}

static inline std::complex<float> from_AMP8I_PHS8I_(uint8_t input_amplitude, uint8_t input_value,
    const six::AmplitudeTable* pAmplitudeTable, const input_amplitudes_t* pValues)
{
    return pValues != nullptr ? (*pValues)[input_amplitude][input_value] :
        Utilities::from_AMP8I_PHS8I(input_amplitude, input_value, pAmplitudeTable);
}

std::complex<float> ImageData::from_AMP8I_PHS8I(uint8_t input_amplitude, uint8_t input_value) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    auto const pAmplitudeTable = amplitudeTable.get();
    auto const pValues = get_RE32F_IM32F_values(pAmplitudeTable);
    return from_AMP8I_PHS8I_(input_amplitude, input_value, pAmplitudeTable, pValues);
}
std::complex<float> ImageData::from_AMP8I_PHS8I(const AMP8I_PHS8I_t& input) const
{
    return from_AMP8I_PHS8I(input.first, input.second);
}

static inline std::complex<float> from_AMP8I_PHS8I_(const  ImageData::AMP8I_PHS8I_t& input,
    const six::AmplitudeTable* pAmplitudeTable, const input_amplitudes_t* pValues)
{
    return from_AMP8I_PHS8I_(input.first, input.second, pAmplitudeTable, pValues);
}
std::vector<std::complex<float>> ImageData::from_AMP8I_PHS8I(const std::span<const AMP8I_PHS8I_t>& inputs) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    // Can't cache the results because amplitudeTable could change at any time.
    auto const pAmplitudeTable = amplitudeTable.get();
    auto const pValues = get_RE32F_IM32F_values(pAmplitudeTable);

    std::vector<std::complex<float>> retval;
    for (size_t i = 0; i < inputs.size(); i++) // (const auto& input : inputs)
    {
        const auto& input = inputs[i]; // no iterators for std::span with old GCC
        retval.push_back(from_AMP8I_PHS8I_(input, pAmplitudeTable, pValues));
    }
    return retval;
}

std::vector<std::complex<float>> ImageData::from_AMP8I_PHS8I(const std::span<const uint8_t>& input_amplitudes,
    const std::span<const uint8_t>& input_values) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }
    if (input_amplitudes.size() != input_values.size())
    {
        throw std::invalid_argument("input_amplitudes.size() != input_values.size()");
    }

    // Can't cache the results because amplitudeTable could change at any time.
    auto const pAmplitudeTable = amplitudeTable.get();
    auto const pValues = get_RE32F_IM32F_values(pAmplitudeTable);

    std::vector<std::complex<float>> retval;
    for (size_t i = 0; i < input_amplitudes.size(); i++)
    {
        auto result = from_AMP8I_PHS8I_(input_amplitudes[i], input_values[i], pAmplitudeTable, pValues);
        retval.push_back(std::move(result));
    }
    return retval;
}

static std::vector<ImageData::KDNode> get_KDNodes(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto nodes_no_amp = make_KDNodes(nullptr);
        // KDTree needs a copy; make that here
        return nodes_no_amp;
    }
    else
    {
        return make_KDNodes(pAmplitudeTable);
    }
}

template<typename InRandomIt, typename OutRandomIt>
static void to_AMP8I_PHS8I_(const six::sicd::KDTree& tree,
    InRandomIt in_beg, InRandomIt in_end,
    OutRandomIt out_beg, OutRandomIt out_end)
{
    assert((in_end - in_beg) == (out_end - out_beg));
    std::for_each(in_beg, in_end, [&](const cx_float& v)
        {
            six::sicd::ImageData::KDNode result;
            tree.nearest_neighbor(six::sicd::ImageData::KDNode{ v }, result);
            *out_beg = std::move(result.amp_and_value);
            ++out_beg;
        });
    assert(out_beg == out_end);
}

template<typename InRandomIt, typename OutRandomIt>
static void to_AMP8I_PHS8I_parallel_(const six::sicd::KDTree& tree,
    InRandomIt in_beg, InRandomIt in_end,
    OutRandomIt out_beg, OutRandomIt out_end)
{
    const auto in_len = in_end - in_beg;
    constexpr auto cutoff = 128 * 8;
    if (in_len < cutoff * cutoff)
    {
        to_AMP8I_PHS8I_(tree, in_beg, in_end, out_beg, out_end);
        return;
    }
    const auto out_len = out_end - out_beg;
    assert(out_len == in_len);

    const auto in_mid = in_beg + in_len / 2;
    const auto out_mid = out_beg + out_len / 2;
    auto handle = std::async(std::launch::async, [&]() {
        to_AMP8I_PHS8I_parallel_(tree, in_mid, in_end, out_mid, out_end); });

    to_AMP8I_PHS8I_parallel_(tree, in_beg, in_mid, out_beg, out_mid);
    handle.get();
}

static void to_AMP8I_PHS8I_parallel(const six::sicd::KDTree& tree,
    const std::vector<cx_float>& cx_floats, std::vector<six::sicd::ImageData::AMP8I_PHS8I_t>& result)
{
    to_AMP8I_PHS8I_parallel_(tree, cx_floats.begin(), cx_floats.end(), result.begin(), result.end());
}

static void to_AMP8I_PHS8I_loop(const six::sicd::KDTree& tree,
    const std::vector<cx_float>& cx_floats, std::vector<six::sicd::ImageData::AMP8I_PHS8I_t>& result)
{
    to_AMP8I_PHS8I_(tree, cx_floats.begin(), cx_floats.end(), result.begin(), result.end());
}

static std::vector<six::sicd::ImageData::AMP8I_PHS8I_t> to_AMP8I_PHS8I(const std::vector<cx_float>& cx_floats)
{
    // create all of of the possible KDNodes values
    auto nodes = make_KDNodes();

    // make the KDTree to quickly find the nearest neighbor
    const six::sicd::KDTree tree(std::move(nodes));

    std::vector<six::sicd::ImageData::AMP8I_PHS8I_t> retval;
    retval.resize(cx_floats.size());

    //to_AMP8I_PHS8I_loop(tree, cx_floats, retval);
    to_AMP8I_PHS8I_parallel(tree, cx_floats, retval);
    return retval;
}

std::vector<ImageData::AMP8I_PHS8I_t> ImageData::to_AMP8I_PHS8I(const std::span<const cx_float>& cx_floats) const
{
    // create all of of the possible KDNodes values
    auto const pAmplitudeTable = amplitudeTable.get();
    auto nodes = get_KDNodes(pAmplitudeTable);

    // make the KDTree to quickly find the nearest neighbor
    const KDTree tree(std::move(nodes));
    
    std::vector<ImageData::AMP8I_PHS8I_t> retval;
    for (size_t i = 0; i < cx_floats.size(); i++) // for (const auto& cx_float : cx_floats)
    {
        const auto& cx_float = cx_floats[i]; // no iterators for std::span with old GCC

        KDNode result;
        tree.nearest_neighbor(KDNode{ cx_float }, result);
        retval.push_back(std::move(result.amp_and_value));
    }

    return retval;
}
void  ImageData::to_AMP8I_PHS8I(const std::span<const cx_float>& cx_floats, std::vector<AMP8I_PHS8I_t>& result) const
{
    result = to_AMP8I_PHS8I(cx_floats);
}
