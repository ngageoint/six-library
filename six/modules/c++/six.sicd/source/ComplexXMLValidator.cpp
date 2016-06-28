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
* GNU Lesser General Public License for more messageBuilder.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; If not,
* see <http://www.gnu.org/licenses/>.
*
*/

#include <six/sicd/ComplexXMLValidator.h>
#include <six/sicd/Functor.h>


namespace six
{
namespace sicd
{

ComplexXMLValidator::ComplexXMLValidator(const ComplexData& data, logging::Logger* log) :
    sicd(data),
    mLog(log)
{
    if (mLog == NULL)
    {
        throw except::Exception(Ctxt("Null pointer to logger passed to ComplexXMLValidator ctor"));
    }
}

double ComplexXMLValidator::nonZero(double arg)
{
    return arg == 0 ? arg : std::numeric_limits<double>::epsilon();
}

bool ComplexXMLValidator::validate()
{
    return (
        checkSupportParamsAgainstPFA() &&        // 2.3.10 - 2.3.16
        checkWeightFunctions() &&                // 2.4  (omitting 2.5: requires fft, fzero)
        checkWaveformDescription() &&            // 2.8  (omitting 2.9: required add_sicd_corners.m)
        checkGeoData() &&                        // 2.10
        checkValidData() &&                      // 2.11
        checkIFP());                             // 2.12
}

bool ComplexXMLValidator::checkIFP()
{
    ImageFormationType formType = sicd.imageFormation->imageFormationAlgorithm;

    switch (formType)
    {
    case ImageFormationType::RGAZCOMP: // 2.12.1
        return checkRGAZCOMP();
        break;
    default:
        //2.12.3 (This is not a typo)
        messageBuilder.str("");
        messageBuilder << "Image formation not fully defined." << std::endl
            << "SICD.ImageFormation.ImageFormAlgo = OTHER or is not set.";
        mLog->warn(messageBuilder.str());
        return false;
        break;
    }
}

bool ComplexXMLValidator::checkRGAZCOMP()
{
    bool valid = true;

    // 2.12.1.1
    if (sicd.grid->imagePlane != ComplexImagePlaneType::SLANT)
    {
        messageBuilder.str("");
        messageBuilder << "RGAZCOMP image formation should result in a SLANT plane image." << std::endl
            << "Grid.ImagePlane: " << sicd.grid->imagePlane.toString();
        mLog->error(messageBuilder.str());
        valid = false;
    }

    //2.12.1.2
    if (sicd.grid->type != ComplexImageGridType::RGAZIM)
    {
        messageBuilder.str("");
        messageBuilder << "RGAZCOMP image formation should result in a RGAZIM grid." << std::endl
            << "Grid.Type: " << sicd.grid->type.toString();
        mLog->error(messageBuilder.str());
        valid = false;
    }

    //2.12.1.8
    if (!Init::isUndefined(sicd.grid->col->deltaKCOAPoly) &&
        sicd.grid->col->deltaKCOAPoly.orderX() == 1 && sicd.grid->col->deltaKCOAPoly.orderY() == 1
        && std::abs(sicd.grid->col->kCenter - (-sicd.grid->col->deltaKCOAPoly[0][0])) > std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << "Grid.Col.KCenter must be equal to -Grid.Col.DeltaKCOAPoly for RGAZCOMP data." << std::endl
            << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << std::endl
            << "Grid.Col.DeltaKCOAPoly: " << sicd.grid->col->deltaKCOAPoly[0][0];
        mLog->error(messageBuilder.str());
        valid = false;
    }
    else if (sicd.grid->col->kCenter != 0)
    {
        messageBuilder.str("");
        messageBuilder << "Grid.Col.KCetner must be zero for RGAZCOMP data with no "
            << "Grid.Col.DeltaKCOAPoly fied" << std::endl
            << "Grid.Col.KCenter: " << sicd.grid->col->kCenter;
        mLog->error(messageBuilder.str());
        valid = false;
    }

    //2.12.1.9
    if (sicd.radarCollection.get() == NULL || Init::isUndefined(sicd.radarCollection->refFrequencyIndex))
    {
        double fcProc = (sicd.imageFormation->txFrequencyProcMin + sicd.imageFormation->txFrequencyProcMax) / 2;
        double kfc = fcProc * (2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC);
        if (!Init::isUndefined(sicd.grid->row->deltaKCOAPoly))
        {
            Poly2D poly = sicd.grid->row->deltaKCOAPoly;
            for (size_t ii = 0; ii < poly.orderX(); ++ii)
            {
                for (size_t jj = 0; jj < poly.orderY(); ++jj)
                {
                    if (std::abs(sicd.grid->row->kCenter - (kfc - poly[ii][jj]) > std::numeric_limits<double>::epsilon()))
                    {
                        messageBuilder.str("");
                        messageBuilder << WF_INCONSISTENT_STR
                            << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << std::endl
                            << "Center frequency * 2/c - Grid.Row.DeltaKCOAPoly: "
                            << kfc - poly[ii][jj];
                        mLog->error(messageBuilder.str());
                        valid = false;
                        break;
                    }
                }
            }
        }
        else if (std::abs(sicd.grid->row->kCenter - kfc) > std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << std::endl
                << "Center frequency * 2/c: " << kfc;
            mLog->error(messageBuilder.str());
            valid = false;
        }
    }

    //2.12.1.10
    if (!Init::isUndefined(sicd.grid->col->deltaKCOAPoly) && sicd.grid->col->deltaKCOAPoly.orderX() > 1)
    {
        messageBuilder.str("");
        messageBuilder << "Grid.Col.DetlaKCOAPoly must be a single value for RGAZCOMP data";
        mLog->error(messageBuilder.str());
        valid = false;
    }

    //2.12.1.11
    if (!Init::isUndefined(sicd.grid->row->deltaKCOAPoly) && sicd.grid->row->deltaKCOAPoly.orderX() > 1)
    {
        messageBuilder.str("");
        messageBuilder << "Grid.Row.DetlaKCOAPoly must be a single value for RGAZCOMP data";
        mLog->error(messageBuilder.str());
        valid = false;
    }

    Vector3 rowUnitVector = sicd.grid->row->unitVector;
    Vector3 colUnitVector = sicd.grid->col->unitVector;

    Vector3 scp = sicd.geoData->scp.ecf;
    Vector3 arp = sicd.scpcoa->arpPos;
    Vector3 arpVel = sicd.scpcoa->arpVel;
    Vector3 unitRG = (scp - arp) / (scp - arp).norm();
    Vector3 left = cross((arp / arp.norm()), (arpVel / arp.norm()));
    int look = sign(left.dot(unitRG));
    Vector3 spn = cross(unitRG, arpVel) * -look;
    spn = spn / spn.norm();

    //2.12.1.3
    if (sicd.rgAzComp.get() == NULL)
    {
        messageBuilder.str("");
        messageBuilder << "RGAZCOMP image formation declared, but no RgAzComp metadata given." << std::endl
            << "No RgAzcomp filed in this SICD";
        mLog->error(messageBuilder.str());
        valid = false;
    }
    else
    {
        //2.12.1.4
        double derivedAzSF = -look * std::sin(math::Constants::DEGREES_TO_RADIANS * sicd.scpcoa->dopplerConeAngle) / sicd.scpcoa->slantRange;
        if (std::abs(sicd.rgAzComp->azSF - derivedAzSF) > 1e-6)
        {
            messageBuilder.str("");
            messageBuilder << "RGAZCOMP fields inconsistent." << std::endl
                << "RgAzComp.AzSF: " << sicd.rgAzComp->azSF << std::endl
                << "Derived RgAzComp.AzSF: " << derivedAzSF;
            mLog->error(messageBuilder.str());
            valid = false;
        }
        // 2.12.1.5 omitted, polyval, polyder
        if (sicd.timeline.get() != NULL && sicd.timeline->interPulsePeriod.get() != NULL &&
            sicd.timeline->interPulsePeriod->sets.size() == 1)
        {
            double krgCoa = sicd.grid->row->kCenter;

            // TODO: In the MATLAB code, krgCoa gets added to sicd.grid->row->deltaKCOAPoly if it exists
            // I'm not sure that makes sense
            Poly1D interPulsePeriodPoly = sicd.timeline->interPulsePeriod->sets[0].interPulsePeriodPoly;
            double stRateCoa = interPulsePeriodPoly.derivative()(sicd.scpcoa->scpTime);
            double deltaKazPerDeltaV = look * krgCoa * (arpVel.norm() * std::sin(
                     sicd.scpcoa->dopplerConeAngle * math::Constants::DEGREES_TO_RADIANS) / sicd.scpcoa->slantRange) / stRateCoa;
            Poly1D derivedKazPoly = deltaKazPerDeltaV * interPulsePeriodPoly;

            double polyNorm = 0;
            for (size_t ii = 0; ii < std::min(sicd.rgAzComp->kazPoly.size(), derivedKazPoly.size()); ++ii)
            {
                polyNorm += std::pow(sicd.rgAzComp->kazPoly[ii] - derivedKazPoly[ii], 2);
            }
            if (std::sqrt(polyNorm) > 1e-3 || sicd.rgAzComp->kazPoly.order() != derivedKazPoly.order())
            {
                messageBuilder.str("");
                messageBuilder << "RGAZCOMP fields inconsistent." << std::endl
                    << "RgAzComp.KazPoly: " << sicd.rgAzComp->kazPoly << std::endl
                    << "Derived RgAzComp.KazPoly: " << derivedKazPoly;
                mLog->error(messageBuilder.str());
                valid = false;
            }
        }
    }
    //2.12.1.6
    Vector3 drvect = unitRG;
    if ((drvect - rowUnitVector).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVect fields inconsistent." << std::endl
            << "Grid.Row.UVectECEF: " << rowUnitVector << std::endl
            << "Derived Grid.Row.UVectECEF: " << drvect;
        mLog->error(messageBuilder.str());
        valid = false;
    }

    //2.12.1.7
    Vector3 dcvect = cross(spn, unitRG);
    if ((dcvect - colUnitVector).norm() > UVECT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "UVectr fields inconsistent." << std::endl
            << "Grid.Col.UVectECF: " << colUnitVector << std::endl
            << "Derived Grid.Col.UVectECF: " << dcvect;
        mLog->error(messageBuilder.str());
        valid = false;
    }

    return valid;
}

std::vector<double> ComplexXMLValidator::linspace(double start, double end, size_t count)
{
    std::vector<double> ret;
    double stepSize = (end - start) / (count-1);
    ret[0] = start;
    for (size_t ii = 1; ii < count; ++ii)
    {
        ret[ii] = ret[ii - 1] + stepSize;
    }
    return ret;
}

Poly1D ComplexXMLValidator::polyAt(PolyXYZ poly, size_t idx)
{
    if (idx > 2)
    {
        throw except::Exception(Ctxt("Idx should be < 3"));
    }

    std::vector<double> coefs;
    for (size_t ii = 0; ii < poly.size(); ++ii)
    {
        coefs.push_back(poly[ii][idx]);
    }
    Poly1D ret(coefs);
    return ret;
}

//CheckPFA remnants
    /* This is a start on the next few sections
     * It requires reimplementation of polyfit, bsxfun, and pfa_polar_coords
    size_t numPfaCoefs = sicd.pfa->polarAnglePoly.order();
    std::vector<double> times = linspace(0, sicd.timeline->collectDuration, numPfaCoefs + 1);
    std::vector<std::vector<double> > pos;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        std::vector<double> current;
        Poly1D poly = polyAt(sicd.position->arpPoly, ii);
        for (size_t jj = 0; jj < times.size(); ++jj)
        {
            current.push_back(poly(times[jj]));
        }
        pos.push_back(current);
    }

    std::vector<double> polRefPos;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        Poly1D poly = polyAt(sicd.position->arpPoly, ii);
        polRefPos.push_back(poly(sicd.pfa->polarAngleRefTime));
    }
    */

bool ComplexXMLValidator::checkValidData()
{
    // 2.11 ValidData
    // Both ImageData.ValidData and GeoData.ValidData are optional,
    // but are conditionally required upon each other. If one exists,
    // the other must also
    bool imageValidData = false;
    bool geoValidData = false;

    if (sicd.imageData.get() != NULL && !sicd.imageData->validData.empty())
    {
        imageValidData = true;
    }
    if (sicd.geoData.get() != NULL && !sicd.geoData->validData.empty())
    {
        geoValidData = true;
    }
    // 2.11.1
    if (imageValidData && !geoValidData)
    {
        messageBuilder.str("");
        messageBuilder << "ImageData.ValidData/GeoData.ValidData required together." << std::endl
            << "ImageData.ValidData exists, but GeoData.ValidData does not.";
        mLog->error(messageBuilder.str());
        return false;
    }
    // 2.11.2
    if (!imageValidData && geoValidData)
    {
        messageBuilder.str("");
        messageBuilder << "ImageData.ValidData/GeoData.ValidData required together." << std::endl
            << "GeoData.ValidData exists, but ImageData.ValidData does not.";
        mLog->error(messageBuilder.str());
        return false;
    }
    /*
    * As with the corner coordinates, we don't check for precision in the agreement
    * between ImageData.ValidData and GeoData.ValidData here, sinc the spec allows
    * from approximate Lat/Lons, and since it's not possible to know the HAE used
    * to compute Lat/Lon for each corner. One could potentially use the SICD sensor
    * model to check that the image coordinates and the resulting ground coordinates
    * from projection to a flat plane (or constant HAE) roughly agreed with GeoData.
    * ValidData to within a generous tolerance.
    */
    return true;
}

bool ComplexXMLValidator::checkGeoData()
{
    // 2.10 GeoData.SCP
    SCP scp = sicd.geoData->scp;

    scene::LLAToECEFTransform transformer;
    Vector3 ecf2 = transformer.transform(scp.llh);
    double ecf_diff = (scp.ecf - ecf2).norm();

    if (ecf_diff > SCPCOA_TOL)
    {
        messageBuilder.str("");
        messageBuilder << "GeoData.SCP.ECF and GeoData.SCP.LLH not consistent." << std::endl
            << "SICD.GeoData.SCP.ECF - SICD.GeoData.SCP.LLH: "
            << ecf_diff << " (m)" << std::endl;
        mLog->error(messageBuilder.str());
        return false;
    }
    return true;
}

double ComplexXMLValidator::nonZeroDenominator(double denominator)
{
    if (denominator == 0)
    {
        return std::numeric_limits<double>::epsilon();
    }
    return denominator;
}

bool ComplexXMLValidator::checkWaveformDescription()
{
    // 2.8 Waveform description consistency
    bool valid = true;
    double wfMin = std::numeric_limits<double>::infinity();
    double wfMax = -std::numeric_limits<double>::infinity();

    for (size_t ii = 0; ii < sicd.radarCollection->waveform.size(); ++ii)
    {
        if (sicd.radarCollection->waveform[ii].get() != NULL)
        {
            double wfCurrent = sicd.radarCollection->waveform[ii]->txFrequencyStart;
            if (wfCurrent < wfMin)
            {
                wfMin = wfCurrent;
            }

            wfCurrent += sicd.radarCollection->waveform[ii]->txRFBandwidth;
            if (wfCurrent > wfMax)
            {
                wfMax = wfCurrent;
            }
        }
    }

    // 2.8.1
    if (wfMin != std::numeric_limits<double>::infinity())
    {
        if (std::abs((wfMin / nonZeroDenominator(sicd.radarCollection->txFrequencyMin)) - 1) > WF_TOL)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.TxFreqStart: "
                << wfMin << std::endl
                << "SICD.RadarCollection.TxFrequency.Min: " << sicd.radarCollection->txFrequencyMin
                << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }
    }

    //2.8.2
    if (wfMax != -std::numeric_limits<double>::infinity())
    {
        if (std::abs((wfMax / nonZeroDenominator(sicd.radarCollection->txFrequencyMax)) - 1) > WF_TOL)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.TxFreqStart + TxFRBandwidth: "
                << wfMax << std::endl
                << "SICD.RadarCollection.TxFrequency.Max: " << sicd.radarCollection->txFrequencyMax
                << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }
    }

    for (size_t ii = 0; ii < sicd.radarCollection->waveform.size(); ++ii)
    {
        if (sicd.radarCollection->waveform[ii].get() == NULL)
        {
            continue;
        }
        six::sicd::WaveformParameters wfParam = *sicd.radarCollection->waveform[ii];

        //2.8.3
        if (std::abs((wfParam.txRFBandwidth / nonZeroDenominator((wfParam.txPulseLength * wfParam.txFMRate))) - 1) > WF_TOL)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.TxFRBandwidth: "
                << wfParam.txRFBandwidth << std::endl
                << "SICD.RadarCollection.TxFrequency.txFMRate * txPulseLength: "
                << wfParam.txFMRate * wfParam.txPulseLength << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.4
        if (wfParam.rcvDemodType.toString() == "CHIRP" && wfParam.rcvFMRate != 0)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
                << wfParam.rcvDemodType.toString() << std::endl
                << "SICD.RadarCollection.Waveform.WFParameters.RcvFMRate: "
                << wfParam.rcvFMRate << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.5
        if (wfParam.rcvDemodType.toString() == "STRETCH" &&
            std::abs((wfParam.rcvFMRate / nonZeroDenominator(wfParam.txFMRate)) - 1) > WGT_TOL)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
                << wfParam.rcvDemodType << std::endl
                << "SICD.RadarCollection.Waveform.WFParameters.RcvFMRate: "
                << wfParam.rcvFMRate << std::endl
                << "SICD>RadarCollection.Waveform.WFParameters.TxFMRate: "
                << wfParam.txFMRate << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.6
        //Absolute frequencies must be positive
        if (six::Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) && sicd.radarCollection->txFrequencyMin <= 0)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.txFrequencyMin: "
                << sicd.radarCollection->txFrequencyMin << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.7
        //Absolute frequencies must be positive
        if (six::Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) && wfParam.txFrequencyStart <= 0)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.txFreqStart: "
                << wfParam.txFrequencyStart << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.8
        //Absolute frequencies must be positive
        if (six::Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) && wfParam.rcvFrequencyStart <= 0)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.rcvFreqStart: "
                << wfParam.rcvFrequencyStart << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.9
        if (wfParam.txPulseLength > wfParam.rcvWindowLength)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.TxPulseLength: "
                << wfParam.txPulseLength << std::endl
                << "SICD.RadarCollection.Waveform.WFPArameters.RcvWindowLength: "
                << wfParam.rcvWindowLength << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.10
        if (wfParam.rcvIFBandwidth > wfParam.adcSampleRate)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.RcvIFBandwidth: "
                << wfParam.rcvIFBandwidth << std::endl
                << "SICD.RadarCollection.Waveform.WFPArameters.ADCSampleRate: "
                << wfParam.adcSampleRate << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.11
        if (wfParam.rcvDemodType.toString() == "CHIRP" && wfParam.txRFBandwidth > wfParam.adcSampleRate)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
                << wfParam.rcvDemodType.toString() << std::endl
                << "SICD.RadarCollection.Waveform.WFParameters.TxRFBandwidth: "
                << wfParam.txRFBandwidth << std::endl
                << "SICD.RadarCollection.Waveform.WFPArameters.ADCSampleRate: "
                << wfParam.adcSampleRate << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        //2.8.12
        double freq_tol = (wfParam.rcvWindowLength - wfParam.txPulseLength) * wfParam.txFMRate;
        if (wfParam.rcvFrequencyStart >= (wfParam.txFrequencyStart + wfParam.txRFBandwidth + freq_tol) ||
            wfParam.rcvFrequencyStart <= wfParam.txFrequencyStart - freq_tol)
        {
            messageBuilder.str("");
            messageBuilder << WF_INCONSISTENT_STR
                << "SICD.RadarCollection.Waveform.WFParameters.RcvFreqStart: "
                << wfParam.rcvFrequencyStart << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }
    }
    return valid;
}

bool ComplexXMLValidator::checkSupportParamsAgainstPFA()
{
    // field is conditional
    if (sicd.pfa.get() == NULL)
    {
        return true;
    }

    bool valid = true;
    const DirectionParameters& row = *sicd.grid->row;
    const DirectionParameters& col = *sicd.grid->col;

    //Slow-time deskew would allow for PFA.Kaz2-PFA.Kaz1>(1/Grid.Col.SS),
    //since Kaz bandwidth is compressed from original polar annulus.
    if (sicd.pfa->slowTimeDeskew.get() == NULL || sicd.pfa->slowTimeDeskew->applied.toString() != "IS_TRUE")
    {
        //2.3.10
        if (sicd.pfa->kaz2 - col.kCenter > (1 / (2 * col.sampleSpacing)) + std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid.Col.SampleSpacing: " << 0.5 / col.sampleSpacing << std::endl
                << "PFA.Kaz2 - Grid.Col.KCenter: " << sicd.pfa->kaz2 - col.kCenter << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }
        //2.3.11
        if (sicd.pfa->kaz1 - col.kCenter < (-1 / (2 * col.sampleSpacing)) - std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid.Col.SampleSpacing: " << 0.5 / col.sampleSpacing << std::endl
                << "PFA.Kaz1 - Grid.Col.KCenter: " << sicd.pfa->kaz1 - col.kCenter << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }
    }
    //2.3.12
    if (sicd.pfa->krg2 - row.kCenter > (1 / (2 * sicd.grid->row->sampleSpacing)) + std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "0.5/SICD.Grid.Row.SampleSpacing: " << 0.5 / row.sampleSpacing << std::endl
            << "PFA.Krg2 - Grid.Row.KCenter: " << sicd.pfa->krg2 - row.kCenter << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.13
    if (sicd.pfa->krg1 - row.kCenter < (-1 / (2 * sicd.grid->row->sampleSpacing)) - std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "0.5/SICD.Grid.Row.SampleSpacing: " << 0.5 / row.sampleSpacing << std::endl
            << "PFA.Krg1 - Grid.Row.KCenter: " << sicd.pfa->krg1 - row.kCenter << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.14
    if (col.impulseResponseBandwidth > sicd.pfa->kaz2 - sicd.pfa->kaz1 + std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "Grid.Col.ImpulseResponseBandwidth: " << col.impulseResponseBandwidth << std::endl
            << "SICD.PFA.Kaz2 - SICD.PFA.Kaz1: " << sicd.pfa->kaz2 - sicd.pfa->kaz1 << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.15
    if (row.impulseResponseBandwidth > sicd.pfa->krg2 - sicd.pfa->krg1 + std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "Grid.Row.ImpulseResponseBandwidth: " << row.impulseResponseBandwidth << std::endl
            << "SICD.PFA.Krg2 - SICD.PFA.Krg1: " << sicd.pfa->krg2 - sicd.pfa->krg1 << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.16
    if (col.kCenter != 0 && std::abs(sicd.grid->col->kCenter - (sicd.pfa->kaz1 + sicd.pfa->kaz2) / 2) > 1e-5)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "Grid.Col.KCenter: " << col.kCenter << std::endl
            << "mean(SICD.PFA.Kaz1, SICD.PFA.Kaz2): " << (sicd.pfa->kaz1 + sicd.pfa->kaz2) / 2 << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    return valid;
}

Functor* ComplexXMLValidator::calculateWeightFunction(const DirectionParameters& direction)
{
    Functor* weightFunction = NULL;
    bool useWeightFunction = false;

    if (direction.weightType.get() != NULL)
    {
        WeightType weightType = *direction.weightType;
        std::string windowName(weightType.windowName);
        str::upper(windowName);

        if (windowName == "UNIFORM")
        {
            weightFunction = new Identity();
        }
        else if (windowName == "HAMMING")
        {
            double coef;
            if (weightType.parameters.empty() || weightType.parameters[0].str().empty())
            {
                //A Hamming window is defined in many places as a raised cosine of weight .54,
                //so this is the default. However, some data use a generalized raised cosine and
                //call it HAMMING, so we allow for both uses.
                coef = .54;
            }
            else
            {
                coef = str::toType<double>(weightType.parameters[0].str());
            }

            weightFunction = new RaisedCos(coef);
        }
        else if (windowName == "HANNING")
        {
            weightFunction = new RaisedCos(0.50);
        }
        else if (windowName == "KAISER")
        {
            weightFunction = new Kaiser(str::toType<double>(weightType.parameters[0].str()));
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
        if (direction.weights.empty())
        {
            return NULL;
        }
        else
        {
            //TODO: interpft(weightFunction)
        }
    }
    return weightFunction;
}

bool ComplexXMLValidator::checkWeightFunctions(const DirectionParameters& direction, const std::string& name)
{
    const size_t DEFAULT_WGT_SIZE = 512;
    std::vector<double> expectedWeights;
    bool valid = true;
    Functor* weightFunction = NULL;

    // 2.4.1, 2.4.2
    if (direction.weightType.get())
    {
        weightFunction = calculateWeightFunction(direction);

        if (weightFunction == NULL)
        {
            messageBuilder.str("");
            messageBuilder << "Unrecognized weighting description " << std::endl
                << "Grid." << name << ".WeightType.WindowName: " << direction.weightType->windowName << std::endl;
            mLog->warn(messageBuilder.str());
            valid = false;
        }
        else
        {
            if (!direction.weights.empty())
            {
                bool consistentValues = true;

                //Arg doesn't matter. Just checking for Uniform-type Functor
                if ((*weightFunction)(5).empty())
                {
                    double key = direction.weights[0];
                    for (size_t ii = 0; ii < direction.weights.size(); ++ii)
                    {
                        if (key != direction.weights[ii])
                        {
                            consistentValues = false;
                        }
                    }
                }
                else
                {
                    expectedWeights = (*weightFunction)(direction.weights.size());

                    for (size_t ii = 0; ii < direction.weights.size(); ++ii)
                    {
                        if (std::abs(expectedWeights[ii] - direction.weights[ii]) > WGT_TOL)
                        {
                            consistentValues = false;
                            break;
                        }
                    }
                }
                if (!consistentValues)
                {
                    messageBuilder.str("");
                    messageBuilder << name << ".weights values inconsistent with " << name << ".weightType" << std::endl
                        << "Grid." << name << ".WeightType.WindowName: " << direction.weightType->windowName << std::endl;
                    mLog->warn(messageBuilder.str());
                    return false;
                }
            }
            else
            {
                expectedWeights = (*weightFunction)(DEFAULT_WGT_SIZE);
            }
        }
    }

    // 2.4.3, 2.4.4
    if (direction.weightType.get() != NULL && direction.weightType->windowName != "UNIFORM" &&
            direction.weightType->windowName != "UNKNOWN" && direction.weights.empty())
    {
        messageBuilder.str("");
        messageBuilder << "Non-uniform weighting, but no WgtFunct provided" << std::endl
            << "SICD.Grid." << name << ".WgtType.WindowName: " << direction.weightType->windowName
            << std::endl;
        mLog->warn(messageBuilder.str());
    }

    // TODO: 2.5 (requires fzero)

    if (weightFunction)
    {
        free(weightFunction);
    }

    return false;
}

bool ComplexXMLValidator::checkWeightFunctions()
{
    DirectionParameters row = *sicd.grid->row;
    DirectionParameters col = *sicd.grid->col;

    bool valid = true;
    // 2.4 Does WgtFunct agree with WgtType?

    // 2.4.1, 2.4.3
    if (row.weightType.get() != NULL)
    {
         valid = valid && checkWeightFunctions(row, "Row");
    }

    // 2.4.2, 2.4.4
    if (col.weightType.get() != NULL)
    {
        valid = valid && checkWeightFunctions(col, "Col");
    }

    return valid;
}

}
}
