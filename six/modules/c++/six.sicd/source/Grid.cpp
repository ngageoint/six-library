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
#include "six/sicd/CollectionInformation.h"
#include "six/sicd/Grid.h"
#include "six/sicd/ImageData.h"
#include "six/sicd/Utilities.h"

using namespace six;
using namespace six::sicd;

WeightType::WeightType() :
    windowName(Init::undefined<std::string>())
{
}

DirectionParameters::DirectionParameters() :
    unitVector(Init::undefined<Vector3>()),
    sampleSpacing(Init::undefined<double>()),
    impulseResponseWidth(Init::undefined<double>()),
    sign(Init::undefined<FFTSign>()),
    impulseResponseBandwidth(Init::undefined<double>()),
    kCenter(Init::undefined<double>()),
    deltaK1(Init::undefined<double>()),
    deltaK2(Init::undefined<double>()),
    deltaKCOAPoly(Init::undefined<Poly2D>())
{
}

DirectionParameters* DirectionParameters::clone() const 
{
    return new DirectionParameters(*this);
}

bool DirectionParameters::operator==(const DirectionParameters& rhs) const
{
    return (unitVector == rhs.unitVector &&
        sampleSpacing == rhs.sampleSpacing &&
        impulseResponseWidth == rhs.impulseResponseWidth &&
        sign == rhs.sign &&
        impulseResponseBandwidth == rhs.impulseResponseBandwidth &&
        kCenter == rhs.kCenter &&
        deltaK1 == rhs.deltaK1 &&
        deltaK2 == rhs.deltaK2 &&
        deltaKCOAPoly == rhs.deltaKCOAPoly &&
        weightType == rhs.weightType &&
        weights == rhs.weights);
}

std::vector<double> DirectionParameters::calculateDeltaKs(const ImageData& imageData) const
{
    double derivedDeltaK1 = 0;
    double derivedDeltaK2 = 0;

    std::vector<std::vector<sys::SSize_T> > vertices = calculateImageVertices(imageData);

    if (!Init::isUndefined<Poly2D>(deltaKCOAPoly))
    {
        derivedDeltaK1 = std::numeric_limits<double>::infinity();
        derivedDeltaK2 = -std::numeric_limits<double>::infinity();

        for (size_t ii = 0; ii < vertices[0].size(); ++ii)
        {
            double currentDeltaK = deltaKCOAPoly.atY(
                    static_cast<double>(vertices[1][ii]))(
                    static_cast<double>(vertices[0][ii]));
            derivedDeltaK1 = std::min(currentDeltaK, derivedDeltaK1);
            derivedDeltaK2 = std::max(currentDeltaK, derivedDeltaK2);
        }
    }

    derivedDeltaK1 -= (impulseResponseBandwidth / 2);
    derivedDeltaK2 += (impulseResponseBandwidth / 2);

    // Wrapped spectrum
    if (derivedDeltaK1 < -(1 / Utilities::nonZeroDenominator(sampleSpacing)) / 2 ||
            derivedDeltaK2 > (1 / Utilities::nonZeroDenominator(sampleSpacing)) / 2)
    {
        derivedDeltaK1 = -(1 / Utilities::nonZeroDenominator(sampleSpacing)) / 2;
        derivedDeltaK2 = -derivedDeltaK1;
    }

    std::vector<double> deltaKs;
    deltaKs.resize(2);
    deltaKs[0] = derivedDeltaK1;
    deltaKs[1] = derivedDeltaK2;
    return deltaKs;
}

std::vector<std::vector<sys::SSize_T> >
DirectionParameters::calculateImageVertices(const ImageData& imageData) const
{
    std::vector<std::vector<sys::SSize_T> > vertices;
    vertices.resize(2);

    if (imageData.validData.size() != 0)
    {
        //test vertices
        for (size_t ii = 0; ii < imageData.validData.size(); ++ii)
        {
            vertices[0].push_back(imageData.validData[ii].col);
        }
        for (size_t ii = 0; ii < imageData.validData.size(); ++ii)
        {
            vertices[1].push_back(imageData.validData[ii].row);
        }
    }
    else
    {
        //use edges of full image
        vertices[0].push_back(0);
        vertices[0].push_back(imageData.validData.size() - 1);
        vertices[0].push_back(imageData.validData.size() - 1);
        vertices[0].push_back(0);
        vertices[1].push_back(0);
        vertices[1].push_back(0);
        vertices[1].push_back(imageData.validData.size() - 1);
        vertices[1].push_back(imageData.validData.size() - 1);
    }
    return vertices;
}

bool DirectionParameters::validate(const ImageData& imageData,
        logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    //2.3.1, 2.3.5
    if (deltaK2 <= deltaK1)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Row/Col.DeltaK1: " << deltaK1 << std::endl
            << "SICD.Grid.Row/Col.DetalK2: " << deltaK2 << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    else
    {
        // 2.3.2, 2.3.6
        if (deltaK2 > (1 / (2 * sampleSpacing)) + std::numeric_limits<double>::epsilon())
        {
            //Non-0 denominator!!
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid.Row/Col.SampleSpacing: "
                << 0.5 / sampleSpacing << std::endl
                << "SICD.Grid.Row/Col.DetalK2: " << deltaK2 << std::endl;
            log.error(messageBuilder.str());
            valid = false;
        }

        // 2.3.3, 2.3.7
        if (deltaK1 < (-1 / (2 * sampleSpacing)) - std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid.Row/Col.SampleSpacing: " <<
                    0.5 / sampleSpacing << std::endl
                << "SICD.Grid.Row/Col.DetalK1: " << deltaK1 << std::endl;
            log.error(messageBuilder.str());
            valid = false;
        }

        // 2.3.4, 2.3.8
        if (impulseResponseBandwidth > (deltaK2 - deltaK1) +
                std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "SICD.Grid.Row/Col.impulseResponseBandwidth: " <<
                    impulseResponseBandwidth << std::endl
                << "SICD.Grid.Row/Col.DeltaK2 - SICD.Grid.Row/COl.DeltaK1: "
                << deltaK2 - deltaK1 << std::endl;
            log.error(messageBuilder.str());
            valid = false;
        }
    }

    // 2.3.9. Compute our own DeltaK1/K2 and test for consistency with DelaKCOAPoly,
    // ImpRespBW, and SS.  Here, we assume the min and max of DeltaKCOAPoly must be
    // on the vertices of the image, since it is smooth and monotonic in most cases--
    // although in actuality this is not always the case.  To be totally generic, 
    // we would have to search for an interior min and max as well

    std::vector<double> deltas = calculateDeltaKs(imageData);

    const double minDk = deltas[0];
    const double maxDk = deltas[1];

    const double DK_TOL = 1e-2;

    //2.3.9.1, 2.3.9.3
    if (std::abs((deltaK1 / Utilities::nonZeroDenominator(minDk)) - 1)
            > DK_TOL)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Row/Col.DeltaK1: " << deltaK1 << std::endl
            << "Derived DeltaK1: " << minDk << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }
    //2.3.9.2, 2.3.9.4
    if (std::abs((deltaK2 / Utilities::nonZeroDenominator(maxDk)) - 1)
            > DK_TOL)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Row/Col.DeltaK2: " << deltaK2 << std::endl
            << "Derived DeltaK2: " << maxDk << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }
    return valid;
}


Grid::Grid() :
    // This is a good assumption, I think
    imagePlane(ComplexImagePlaneType::SLANT),
    // Not so sure about this one
    type(ComplexImageGridType::RGAZIM),
    row(new DirectionParameters()),
    col(new DirectionParameters())
{
}

Grid* Grid::clone() const 
{
    return new Grid(*this);
}

bool Grid::operator==(const Grid& rhs) const
{
    return (imagePlane == rhs.imagePlane &&
        type == rhs.type &&
        timeCOAPoly == rhs.timeCOAPoly &&
        row == rhs.row && col == rhs.col);
}

bool Grid::validateTimeCOAPoly(
        const CollectionInformation& collectionInformation,
        logging::Logger& log) const
{
    const RadarModeType& mode = collectionInformation.radarMode;

    //2.1. Scalar TimeCOAPoly means SPOTLIGHT data
    bool isScalar = true;
    bool valid = true;

    // I don't know that it's impossible for a one-degree polynomial to be expressed
    // as a polynomial of higher order for whatever reason, so I'm checking each term
    // manually
    for (size_t ii = 0; ii <= timeCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= timeCOAPoly.orderY(); ++jj)
        {
            if (ii == 0 && jj == 0)
            {
                // Don't care what the (0,0) value is, but everything else
                // should be 0.
                continue;
            }
            if (timeCOAPoly[ii][jj] != 0)
            {
                isScalar = false;
                break;
            }
        }
    }

    if (mode == RadarModeType::SPOTLIGHT && !isScalar)
    {
        log.error("SPOTLIGHT data should only have scalar TimeCOAPoly.");
        valid = false;
    }

    if (mode != RadarModeType::SPOTLIGHT && isScalar)
    {
        log.warn("Non-SPOTLIGHT data will generally have more than one nonzero"
            "term in TimeCOAPoly unless \"formed as spotlight\".");
        valid = false;
    }

    return valid;
}

bool Grid::validateFFTSigns(logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    //2.2. FFT signs in both dimensions almost certainly have to be equal
    if (row->sign != col->sign)
    {
        messageBuilder.str("");
        messageBuilder <<
            "FFT signs in row and column direction should be the same." <<
            std::endl << "Grid.Row.Sign: " << row->sign.toString() << std::endl
            << "Grid.Col.Sign: " << col->sign.toString() << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }
    return valid;
}

bool Grid::validate(const CollectionInformation& collectionInformation,
        const ImageData& imageData,
        logging::Logger& log) const
{
    
    return (validateTimeCOAPoly(collectionInformation, log) &&  //2.1
        validateFFTSigns(log) &&                                //2.2
        row->validate(imageData, log) && 
        col->validate(imageData, log)                           //2.3.1 - 2.3.9
        );
}

