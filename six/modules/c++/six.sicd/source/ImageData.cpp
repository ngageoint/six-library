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
#include <mt/Algorithm.h>

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
    // Do we have a cahced result to use (no amplitude table)?
    // Or must it be recomputed (have an amplutude table)?
    return pValues != nullptr ? (*pValues)[input_amplitude][input_value] :
        Utilities::from_AMP8I_PHS8I(input_amplitude, input_value, pAmplitudeTable);
}
std::complex<float> ImageData::from_AMP8I_PHS8I(const AMP8I_PHS8I_t& input) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    auto const pAmplitudeTable = amplitudeTable.get();
    auto const pValues = get_RE32F_IM32F_values(pAmplitudeTable);
    return from_AMP8I_PHS8I_(input.first, input.second, pAmplitudeTable, pValues);
}

using from_AMP8I_PHS8I_output_t = std::vector<std::complex<float>>::iterator;
using from_AMP8I_PHS8I_transform_t = std::function<std::complex<float>(const ImageData::AMP8I_PHS8I_t&)>;
template<typename TTransform>
static void from_AMP8I_PHS8I_(const six::AmplitudeTable* pAmplitudeTable,
    const std::span<const  ImageData::AMP8I_PHS8I_t>& inputs, std::vector<std::complex<float>>& results,
    TTransform transform)
{
    auto const pValues = get_RE32F_IM32F_values(pAmplitudeTable);

    const auto begin = &(inputs[0]);
    const auto end = begin + inputs.size();
    results.resize(inputs.size());

    const from_AMP8I_PHS8I_transform_t f = [&](const ImageData::AMP8I_PHS8I_t& v)
    {
        return from_AMP8I_PHS8I_(v.first, v.second, pAmplitudeTable, pValues);
    };
    (void)transform(begin, end, results.begin(), f);
}

void ImageData::from_AMP8I_PHS8I(const std::span<const AMP8I_PHS8I_t>& inputs, std::vector<std::complex<float>>& results) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    const auto std_transform = [&](const AMP8I_PHS8I_t* pBegin, const AMP8I_PHS8I_t* pEnd,
        from_AMP8I_PHS8I_output_t out, from_AMP8I_PHS8I_transform_t f)
    {
        return std::transform(pBegin, pEnd, out, f);
    };
    from_AMP8I_PHS8I_(amplitudeTable.get(), inputs, results, std_transform);

}
void ImageData::from_AMP8I_PHS8I(std::launch policy, const std::span<const AMP8I_PHS8I_t>& inputs, std::vector<std::complex<float>>& results,
    ptrdiff_t cutoff_) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    constexpr auto default_cutoff = 128 * 8;
    const auto cutoff = cutoff_ < 0 ? default_cutoff : cutoff_;

    const auto mt_transform_async = [&](const AMP8I_PHS8I_t* pBegin, const AMP8I_PHS8I_t* pEnd,
        from_AMP8I_PHS8I_output_t out, from_AMP8I_PHS8I_transform_t f)
    {
        return mt::transform_async(pBegin, pEnd, out, f, cutoff, policy);
    };
    from_AMP8I_PHS8I_(amplitudeTable.get(), inputs, results, mt_transform_async);
}

static KDTree make_KDTree(const six::AmplitudeTable* pAmplitudeTable)
{
    // create all of of the possible KDNodes values
    std::vector<ImageData::KDNode> nodes;
    if (pAmplitudeTable == nullptr)
    {
        static const auto nodes_no_amp = make_KDNodes(nullptr);
        nodes = nodes_no_amp; // KDTree needs a copy; make that here
    }
    else
    {
        nodes = make_KDNodes(pAmplitudeTable);
    }

    return KDTree(std::move(nodes));
}

using to_AMP8I_PHS8I_output_t = std::vector<ImageData::AMP8I_PHS8I_t>::iterator;
using to_AMP8I_PHS8I_transform_t = std::function<ImageData::AMP8I_PHS8I_t(const std::complex<float>&)>;
template<typename TTransform>
static void to_AMP8I_PHS8I_(const six::AmplitudeTable* pAmplitudeTable,
    const std::span<const ImageData::cx_float>& inputs, std::vector<ImageData::AMP8I_PHS8I_t>& results,
    TTransform transform)
{
    // make the KDTree to quickly find the nearest neighbor
    const KDTree tree = make_KDTree(pAmplitudeTable);

    const auto begin = &(inputs[0]);
    const auto end = begin + inputs.size();
    results.resize(inputs.size());

    const to_AMP8I_PHS8I_transform_t f = [&](const std::complex<float>& v)
    {
        auto result = tree.nearest_neighbor(six::sicd::ImageData::KDNode{ v });
        return result.amp_and_value;
    };
    (void)transform(begin, end, results.begin(), f);
}
void ImageData::to_AMP8I_PHS8I(const std::span<const cx_float>& cx_floats, std::vector<AMP8I_PHS8I_t>& results) const
{
    const auto std_transform = [&](const ImageData::cx_float* pBegin, const ImageData::cx_float* pEnd,
        to_AMP8I_PHS8I_output_t out, to_AMP8I_PHS8I_transform_t f)
    {
        return std::transform(pBegin, pEnd, out, f);
    };
    to_AMP8I_PHS8I_(amplitudeTable.get(), cx_floats, results, std_transform);
}
void ImageData::to_AMP8I_PHS8I(std::launch policy, const std::span<const cx_float>& cx_floats, std::vector<AMP8I_PHS8I_t>& results,
    ptrdiff_t cutoff_) const
{
    constexpr auto default_cutoff = 128 * 8;
    const auto cutoff = cutoff_ < 0 ? default_cutoff : cutoff_;

    const auto mt_transform_async = [&](const ImageData::cx_float* pBegin, const ImageData::cx_float* pEnd,
        to_AMP8I_PHS8I_output_t out, to_AMP8I_PHS8I_transform_t f)
    {
        return mt::transform_async(pBegin, pEnd, out, f, cutoff, policy);
    };
    to_AMP8I_PHS8I_(amplitudeTable.get(), cx_floats, results, mt_transform_async);
}
