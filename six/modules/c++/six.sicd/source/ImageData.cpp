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
#include <std/mdspan>

#include <gsl/gsl.h>
#include <mt/Algorithm.h>
#include <sys/Span.h>

#include "six/AmplitudeTable.h"
#include "six/sicd/GeoData.h"
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
        log.error(messageBuilder);
        valid = false;
    }

    // 2.11.2
    if (validData.empty() && !geoData.validData.empty())
    {
        messageBuilder << "ImageData.ValidData/GeoData.ValidData "
            << "required together." << std::endl
            << "GeoData.ValidData exists, but ImageData.ValidData does not.";
        log.error(messageBuilder);
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
            log.error(messageBuilder);
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
                log.error(messageBuilder);
                valid = false;
            }
        }
        if (!Utilities::isClockwise(validData))
        {
            messageBuilder << "ImageData.ValidData should be arrange clockwise";
            log.error(messageBuilder);
            valid = false;
        }
    }

    return valid;
}

constexpr std::array<size_t, 2> lookupDims{ AmplitudeTableSize, AmplitudeTableSize }; // size 256 x 256 matrix of complex values.
template<typename TToComplexFunc>
static auto createLookup(TToComplexFunc toComplex)
{
    std::vector<six::zfloat> retval(lookupDims[0] * lookupDims[1]); 
    std::mdspan<six::zfloat, std::dextents<size_t, 2>> values(retval.data(), lookupDims);

    // For all possible amp/phase values (there are "only" 256*256=65536), get and save the
    // complex<float> value.
    for (const auto amplitude : Utilities::iota_0_256())
    {
        for (const auto phase : Utilities::iota_0_256())
        {
            values(amplitude, phase) = toComplex(amplitude, phase);
        }
    }

    return retval;
}
static auto createLookup(const six::AmplitudeTable& amplitudeTable)
{
    const auto toComplex = [&amplitudeTable](auto amplitude, auto phase) {
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

static const std::vector<six::zfloat>* getCachedLookup(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto lookup_no_table = createLookup();
        return &lookup_no_table;
    }

    // Maybe one has already been created and stored on the table?
    return pAmplitudeTable->getLookup();
}

six::Amp8iPhs8iLookup_t ImageData::getLookup(const six::AmplitudeTable* pAmplitudeTable)
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
    return six::Amp8iPhs8iLookup_t(pLookup->data(), lookupDims);
}

void ImageData::toComplex(six::Amp8iPhs8iLookup_t values, std::span<const AMP8I_PHS8I_t> inputs, std::span<six::zfloat> results)
{
    const auto toComplex_ = [&values](const auto& v)
    {
        return values(v.amplitude, v.phase);
    };
    std::ignore = mt::Transform_par(inputs.begin(), inputs.end(), results.begin(), toComplex_);
}
std::vector<six::zfloat> ImageData::toComplex(std::span<const AMP8I_PHS8I_t> inputs) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    const auto& values = getLookup(amplitudeTable.get());

    std::vector<six::zfloat> retval(inputs.size());
    toComplex(values, inputs, sys::make_span(retval));
    return retval;
}

std::vector<AMP8I_PHS8I_t> ImageData::fromComplex(std::span<const six::zfloat> inputs) const
{
    return six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors(inputs, amplitudeTable.get());
}

