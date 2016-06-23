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
#include "six/sicd/SCPCOA.h"
#include "six/sicd/RMA.h"
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
    //Here, we assume the min and max of DeltaKCOAPoly must be
    // on the vertices of the image, since it is smooth and monotonic in most cases--
    // although in actuality this is not always the case.  To be totally generic, 
    // we would have to search for an interior min and max as well
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

std::auto_ptr<Functor>
DirectionParameters::calculateWeightFunction() const
{
    std::auto_ptr<Functor> weightFunction;
    bool useWeightFunction = false;

    if (weightType.get() != NULL)
    {
        std::string windowName(weightType->windowName);
        str::upper(windowName);

        if (windowName == "UNIFORM")
        {
            weightFunction.reset(new Identity());
        }
        else if (windowName == "HAMMING")
        {
            double coef;
            if (weightType->parameters.empty() || weightType->parameters[0].str().empty())
            {
                //A Hamming window is defined in many places as a raised cosine of weight .54,
                //so this is the default. However, some data use a generalized raised cosine and
                //call it HAMMING, so we allow for both uses.
                coef = .54;
            }
            else
            {
                coef = str::toType<double>(weightType->parameters[0].str());
            }

            weightFunction.reset(new RaisedCos(coef));
        }
        else if (windowName == "HANNING")
        {
            weightFunction.reset(new RaisedCos(0.50));
        }
        else if (windowName == "KAISER")
        {
            weightFunction.reset(new Kaiser(str::toType<double>(
                    weightType->parameters[0].str())));
        }
        else
        {
            //TODO: windowName == "TAYLOR"
            useWeightFunction = true;
        }
    }
    else
    {
        useWeightFunction = true;
    }

    if (useWeightFunction)
    {
        if (weights.empty())
        {
            weightFunction.reset();
            return weightFunction;
        }
        else
        {
            //TODO: interpft(weightFunction)
        }
    }
    return weightFunction;
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
        vertices[0].push_back(imageData.numCols - 1);
        vertices[0].push_back(imageData.numCols - 1);
        vertices[0].push_back(0);
        vertices[1].push_back(0);
        vertices[1].push_back(0);
        vertices[1].push_back(imageData.numRows - 1);
        vertices[1].push_back(imageData.numRows - 1);
    }
    return vertices;
}

void DirectionParameters::fillDerivedFields(const ImageData& imageData)
{
    // Calulating resolution requires fzero and fft functions

    // DeltaK1/2 are approximated from DeltaKCOAPoly
    if (!Init::isUndefined(deltaKCOAPoly) &&
        !Init::isUndefined(impulseResponseBandwidth) &&
        !Init::isUndefined(sampleSpacing) &&
        Init::isUndefined(deltaK1) &&
        Init::isUndefined(deltaK2))
    {
        // Here, we assume the min and max of DeltaKCOAPoly must be on the vertices
        // of the image, since it is smooth and monotonic in most cases--although in
        // actuality this is not always the case. To be totally generic, we would 
        // have to search for an interior min and max as well.

        std::vector<double> deltas = calculateDeltaKs(imageData);

        deltaK1 = deltas[0];
        deltaK2 = deltas[1];
    }

    if (weightType.get() != NULL &&
        weights.empty() &&
        weightType->windowName != "UNKNOWN")
    {
        size_t defaultWgtSize = 512;
        weights = (*calculateWeightFunction())(defaultWgtSize);
    }
    return;
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
        if (deltaK2 > (1 / (Utilities::nonZeroDenominator(2 * sampleSpacing)))
                + std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid.Row/Col.SampleSpacing: " <<
                0.5 / Utilities::nonZeroDenominator(sampleSpacing) << std::endl
                << "SICD.Grid.Row/Col.DetalK2: " << deltaK2 << std::endl;
            log.error(messageBuilder.str());
            valid = false;
        }

        // 2.3.3, 2.3.7
        if (deltaK1 < (-1 / (Utilities::nonZeroDenominator(2 * sampleSpacing)))
                - std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid.Row/Col.SampleSpacing: " <<
                0.5 / Utilities::nonZeroDenominator(sampleSpacing) << std::endl
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
    // ImpRespBW, and SS.
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

void Grid::fillDerivedFields(
        const CollectionInformation& collectionInformation,
        const ImageData& imageData,
        const SCPCOA& scpcoa)
{
    if (!Init::isUndefined<double>(scpcoa.scpTime) &&
        collectionInformation.radarMode == RadarModeType::SPOTLIGHT)
    {
        if (Init::isUndefined<Poly2D>(timeCOAPoly))
        {
            // I'm assuming this is what it means to assign a double to a Poly2D.
            // It matches what's getting checked in valiidateTimeCOAPoly.
            timeCOAPoly = Poly2D(1, 1);
            timeCOAPoly[0][0] = scpcoa.scpTime;
        }
    }

    row->fillDerivedFields(imageData);
    col->fillDerivedFields(imageData);
}

void Grid::fillDerivedFields(const RMA& rma, const Vector3& scp,
        const PolyXYZ& arpPoly)
{
    if (rma.rmat.get())
    {
        fillDerivedFields(*rma.rmat, scp);
    }
    else if (rma.rmcr.get())
    {
        fillDerivedFields(*rma.rmcr, scp);
    }
    else if (rma.inca.get())
    {
        fillDerivedFields(*rma.inca, scp, arpPoly);
    }
}

void Grid::fillDerivedFields(const RMAT& rmat, const Vector3& scp)
{
    // Row/Col.UnitVector and Derived fields
    if (Init::isUndefined<Vector3>(row->unitVector) &&
        Init::isUndefined<Vector3>(col->unitVector))
    {
        row->unitVector = rmat.uXCT(scp);
        col->unitVector = rmat.uYAT(scp);
    }
}

void Grid::fillDerivedFields(const RMCR& rmcr, const Vector3& scp)
{
    // Row/Col.UnitVector and Derived fields
    if (Init::isUndefined<Vector3>(row->unitVector) &&
        Init::isUndefined<Vector3>(col->unitVector))
    {
        row->unitVector = rmcr.uXRG(scp);
        col->unitVector = rmcr.uYCR(scp);
    }
}

void Grid::fillDerivedFields(const INCA& inca, const Vector3& scp,
        const PolyXYZ& arpPoly)
{

    if (!Init::isUndefined<Poly1D>(inca.timeCAPoly) &&
        !Init::isUndefined<PolyXYZ>(arpPoly) &&
        Init::isUndefined<Vector3>(row->unitVector) &&
        Init::isUndefined<Vector3>(col->unitVector))
    {
        row->unitVector = inca.uRG(scp, arpPoly);
        col->unitVector = inca.uAZ(scp, arpPoly);
    }

    if (Init::isUndefined<double>(col->kCenter))
    {
        col->kCenter = 0;
    }

    if (!Init::isUndefined<double>(inca.freqZero) &&
        Init::isUndefined<double>(row->kCenter))
    {
        row->kCenter = derivedRowKCenter(inca);
    }
}

double Grid::derivedRowKCenter(const INCA& inca) const
{
    return inca.freqZero * 2 /
        math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC;
}

void Grid::fillDefaultFields(const RMA& rma, double fc)
{
    if (type == ComplexImageGridType::NOT_SET)
    {
        type = expectedGridType(rma);
    }
    if (rma.rmat.get())
    {
        fillDefaultFields(*rma.rmat, fc);
    }
    else if (rma.rmcr.get())
    {
        fillDefaultFields(*rma.rmcr, fc);
    }
}

void Grid::fillDefaultFields(const RMAT& rmat, double fc)
{
    if (imagePlane == ComplexImagePlaneType::NOT_SET)
    {
        imagePlane = ComplexImagePlaneType::SLANT;
    }
    if (!Init::isUndefined<double>(fc))
    {
        if (Init::isUndefined<double>(row->kCenter))
        {
            row->kCenter = derivedRowKCenter(rmat, fc);
        }

        if (Init::isUndefined<double>(col->kCenter))
        {
            col->kCenter = derivedColKCenter(rmat, fc);
        }
    }
}

double Grid::derivedRowKCenter(const RMAT& rmat, double fc) const
{
    const double kfc = fc * 2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC;
    return kfc * std::sin(rmat.dopConeAngleRef *
            math::Constants::RADIANS_TO_DEGREES);
}

double Grid::derivedColKCenter(const RMAT& rmat, double fc) const
{
    const double kfc = fc * 2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC;
    return kfc * std::cos(rmat.dopConeAngleRef *
        math::Constants::RADIANS_TO_DEGREES);
}

void Grid::fillDefaultFields(const RMCR& rmcr, double fc)
{
    if (imagePlane == ComplexImagePlaneType::NOT_SET)
    {
        imagePlane = ComplexImagePlaneType::SLANT;
    }
    if (!Init::isUndefined<double>(fc))
    {
        if (Init::isUndefined<double>(row->kCenter))
        {
            row->kCenter = derivedRowKCenter(rmcr, fc);
        }
        if (Init::isUndefined<double>(col->kCenter))
        {
            col->kCenter = 0;
        }
    }
}

bool Grid::validate(const RMA& rma, const Vector3& scp,
        const PolyXYZ& arpPoly, double fc,
        logging::Logger& log) const
{
    bool valid = true;
    // 2.12.3.2.1, 2.12.3.4.1
    if (type != expectedGridType(rma))
    {
        std::ostringstream messageBuilder;
        messageBuilder << "Given image formation algorithm expects "
            << expectedGridType(rma).toString() << ".\nFound " << type;
        log.error(messageBuilder.str());
        valid = false;
    }

    if (rma.rmat.get())
    {
        return validate(*rma.rmat, scp, fc, log);
    }

    else if (rma.rmcr.get())
    {
        return validate(*rma.rmcr, scp, fc, log);
    }

    else if (rma.inca.get())
    {
        return validate(*rma.inca, scp, arpPoly, fc, log);
    }

    // If no image formation algorithm is present, the problem isn't
    // with the Grid, so we'll let RMA deal with it that error
    return valid;
}

bool Grid::validate(const RMAT& rmat, const Vector3& scp,
    double fc, logging::Logger& log) const
{
    (void)rmat; // only for overloading
    std::ostringstream messageBuilder;
    bool valid = true;

    // 2.12.3.2.3
    if ((row->unitVector - rmat.uXCT(scp)).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVect fields inconsistent." << std::endl
            << "Grid.Row.UVectECF: " << row->unitVector
            << "Derived grid.Row.UVectECT: " << rmat.uXCT(scp);
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.2.4
    if ((col->unitVector - rmat.uYAT(scp)).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVect fields inconsistent." << std::endl
            << "Grid.Col.UVectECF: " << col->unitVector
            << "Derived Grid.Col.UVectECF: " << rmat.uYAT(scp);
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.2.6
    if (std::abs(derivedRowKCenter(rmat, fc) / row->kCenter - 1) > WF_TOL)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR
            << "Grid.Row.KCtr: " << row->kCenter << std::endl
            << "Derived KCtr: " << derivedRowKCenter(rmat, fc);
        log.warn(messageBuilder.str());
        valid = false;
    }

    //2.12.3.2.7
    if (std::abs(derivedColKCenter(rmat, fc) / col->kCenter - 1) > WF_TOL)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR
            << "Grid.Col.KCtr: " << col->kCenter << std::endl
            << "Derived KCtr: " << derivedColKCenter(rmat, fc);
        log.warn(messageBuilder.str());
        valid = false;
    }

    return valid;
}

bool Grid::validate(const RMCR& rmcr, const Vector3& scp,
        double fc, logging::Logger& log) const
{
    (void)rmcr; // Only for overloading
    bool valid = true;
    std::ostringstream messageBuilder;

    //2.12.3.3.3
    if ((row->unitVector - rmcr.uXRG(scp)).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVect fields inconsistent." << std::endl
            << "Grid.Row.UVectECF: " << row->unitVector << std::endl
            << "Derived Grid.Row.UVectECF: " << rmcr.uXRG(scp);
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.3.4
    if ((col->unitVector - rmcr.uYCR(scp)).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVect fields inconsistent." << std::endl
            << "Grid.Col.UVectECF: " << col->unitVector << std::endl
            << "Derived Grid.Col.UVectECF: " << rmcr.uYCR(scp);
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.3.6
    if (col->kCenter != 0)
    {
        messageBuilder.str("");
        messageBuilder << "Grid.Col.KCtr must be zero for RMA/RMCR data." 
            << std::endl << "Grid.Col.KCtr = " << col->kCenter;
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.3.7
    if (!Init::isUndefined<double>(fc))
    {
        if (std::abs(row->kCenter / derivedRowKCenter(rmcr, fc) - 1) > WF_TOL)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR << std::endl
                << "Grid.Row.KCtr: " << row->kCenter << std::endl
                << "Center frequency * 2/c: " << derivedRowKCenter(rmcr, fc);
            log.warn(messageBuilder.str());
            valid = false;
        }
    }

    return valid;
}

double Grid::derivedRowKCenter(const RMCR& rmcr, double fc) const
{
    return fc * (2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC);
}

bool Grid::validate(const INCA& inca, const Vector3& scp,
        const PolyXYZ& arpPoly, double fc,
        logging::Logger& log) const
{
    (void)inca; // Only for overloading
    bool valid = true;
    std::ostringstream messageBuilder;
    const double IFP_POLY_TOL = 1e-5;

    if (!Init::isUndefined<Poly2D>(inca.dopplerCentroidPoly) &&
        inca.dopplerCentroidCOA == BooleanType::IS_TRUE)
    {
        const Poly2D& kcoaPoly = col->deltaKCOAPoly;
        const Poly2D& centroidPoly = inca.dopplerCentroidPoly;

        if (kcoaPoly.orderX() != centroidPoly.orderX() &&
            kcoaPoly.orderY() != centroidPoly.orderY())
        {
            valid = false;
            messageBuilder.str("");
            messageBuilder << "Grid.Col.delaKCOAPoly and "
                << "RMA.INCA.dopplerCentroidPoly have diferent sizes.";
            log.error(messageBuilder.str());
        }
        else
        {
            Poly2D differencePoly = kcoaPoly - (centroidPoly * inca.timeCAPoly[1]);
            double norm = 0;

            for (size_t ii = 0; ii < differencePoly.orderX(); ++ii)
            {
                for (size_t jj = 0; jj < differencePoly.orderY(); ++jj)
                {
                    norm += std::pow(differencePoly[ii][jj], 2);
                }
            }
            norm = std::sqrt(norm);

            if(norm > IFP_POLY_TOL)
            {
                messageBuilder.str("");
                messageBuilder << "RMA.INCA fields inconsistent." << std::endl
                    << "Compare Grid.Col.KCOAPoly to RMA.INCA.DopCentroidPoly "
                    << "* RMA.INCA.TimeCAPoly[1].";
                log.error(messageBuilder.str());
                valid = false;
            }
        }
    }

    // 2.12.3.4.6
    if ((inca.uRG(scp, arpPoly) - row->unitVector).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVectFields inconsistent" << std::endl
            << "Grid.Row.UVectECF: " << row->unitVector
            << "Derived Grid.Row.UVectECF: " << inca.uRG(scp, arpPoly);
        log.error(messageBuilder.str());
        valid =  false;
    }

    // 2.12.3.4.7
    if ((inca.uAZ(scp, arpPoly) - col->unitVector).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVectFields inconsistent" << std::endl
            << "Grid.Col.UVectECF: " << col->unitVector
            << "Derived Grid.Col.UVectECF: " << inca.uAZ(scp, arpPoly);
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.4.8
    if (col->kCenter != 0)
    {
        messageBuilder.str("");
        messageBuilder << "Grid.Col.KCtr  must be zero "
            << "for RMA/INCA data." << std::endl
            << "Grid.Col.KCtr: " << col->kCenter;
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.4.11
    if (Init::isUndefined<double>(fc) &&
        std::abs(row->kCenter - derivedRowKCenter(inca)) >
        std::numeric_limits<double>::epsilon())
    {
        const std::string WF_INCONSISTENT_STR =
                "Waveform fields not consistent";
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "RMA.INCA.FreqZero * 2 / c: " << derivedRowKCenter(inca)
            << "Grid.Row.KCenter: " << row->kCenter;
        log.error(messageBuilder.str());
        valid = false;
    }
    return valid;
}

ComplexImageGridType Grid::expectedGridType(const RMA& rma) const
{
    if (rma.rmat.get())
    {
        return ComplexImageGridType::XCTYAT;
    }
    else if (rma.rmcr.get())
    {
        return ComplexImageGridType::XRGYCR;
    }
    else if (rma.inca.get())
    {
        return ComplexImageGridType::RGZERO;
    }
}
