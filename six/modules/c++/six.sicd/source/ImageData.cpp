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

#include "six/sicd/ComplexToAMP8IPHS8I.h"
#include "six/sicd/GeoData.h"
#include "six/sicd/ImageData.h"
#include "six/sicd/Utilities.h"
#include "six/sicd/KDTree.h"
#include "six/sicd/AMP8I_PHS8I_Converter.h"

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

std::complex<float> ImageData::from_AMP8I_PHS8I(const AMP8I_PHS8I_t& input) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    const AMP8I_PHS8I_Converter converter(amplitudeTable.get());
    return converter.from_AMP8I_PHS8I(input);
}

void ImageData::from_AMP8I_PHS8I(std::span<const AMP8I_PHS8I_t> inputs, std::span<std::complex<float>> results,
    ptrdiff_t cutoff) const
{
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw std::runtime_error("pxielType must be AMP8I_PHS8I");
    }

    const AMP8I_PHS8I_Converter converter(amplitudeTable.get());
    converter.from_AMP8I_PHS8I(inputs, results, cutoff);
}

void ImageData::to_AMP8I_PHS8I(std::span<const cx_float> inputs, std::span<AMP8I_PHS8I_t> results,
    ptrdiff_t cutoff) const
{
    const AMP8I_PHS8I_Converter converter(amplitudeTable.get());
    converter.to_AMP8I_PHS8I(inputs, results, cutoff);
}
