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

using input_values_t = std::array<std::complex<float>, UINT8_MAX + 1>;
using input_amplitudes_t = std::array<input_values_t, UINT8_MAX + 1>;

// input_amplitudes_t is too big for the stack
static std::unique_ptr<input_amplitudes_t> AMP8I_PHS8I_to_RE32F_IM32F_(const six::AmplitudeTable* pAmplitudeTable)
{
    auto retval = std::make_unique<input_amplitudes_t>();
    auto& values = *retval;
    for (uint16_t input_amplitude = 0; input_amplitude <= UINT8_MAX; input_amplitude++)
    {
        for (uint16_t input_value = 0; input_value <= UINT8_MAX; input_value++)
        {
            auto cx_result = Utilities::from_AMP8I_PHS8I(gsl::narrow<uint8_t>(input_amplitude), gsl::narrow<uint8_t>(input_value), pAmplitudeTable);
            values[input_amplitude][input_value] = std::move(cx_result);
        }
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
    for (const auto& input : inputs)
    {
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
