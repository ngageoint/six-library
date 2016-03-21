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

bool ComplexXMLValidator::validate()
{
    // This function is a transcription of MATLAB file validate_sicd.m by Wade Schwartzkopf
    // Reference numbers (e.g. 2.3) reference the corresponding sections of the MATLAB file

    return (checkTimeCOAPoly() &&                // 2.1
        checkFFTSigns() &&                       // 2.2
        checkFrequencySupportParameters() &&     // 2.3.1  - 2.3.9
        checkSupportParamsAgainstPFA() &&        // 2.3.10 - 2.3.16
        checkWeightFunctions() &&                // 2.4  (omitting 2.5: requires fft, fzero)
        checkARPPoly() &&                        // 2.6  (omitting 2.7: requries derived_sicd_fields.m)
        checkWaveformDescription() &&            // 2.8  (omitting 2.9: required add_sicd_corners.m)
        checkGeoData() &&                        // 2.10
        checkValidData() &&                      // 2.11
        checkIFP());                             // 2.12
}

bool ComplexXMLValidator::checkIFP()
{
    try
    {
        assertExists(sicd.geoData, "SICD.GeoData");
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.grid->row, "SICD.Grid.Row");
        assertExists(sicd.grid->col, "SICD.Grid.Col");
        assertExists(sicd.imageFormation, "SICD.ImageFormation");
    }
    catch (except::Exception&)
    {
        return false;
    }

    ImageFormationType formType = sicd.imageFormation->imageFormationAlgorithm;

    switch (formType)
    {
    case ImageFormationType::RGAZCOMP: // 2.12.1
        return checkRGAZCOMP();
        break;
    case ImageFormationType::PFA:      // 2.12.2
        return checkPFA();
        break;
    case ImageFormationType::RMA:      // 2.12.3.*
        return checkRMA();
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

    try
    {
        assertExists(sicd.scpcoa, "SICD.SCPCOA");
    }
    catch (except::Exception&)
    {
        return false;
    }

    Vector3 rowUnitVector = sicd.grid->row->unitVector;
    Vector3 colUnitVector = sicd.grid->col->unitVector;
    

    Vector3 scp = sicd.geoData->scp.ecf;
    Vector3 arp = sicd.scpcoa->arpPos;
    Vector3 arpVel = sicd.scpcoa->arpVel;
    Vector3 unitRG = (scp - arp) / (scp - arp).norm();
    Vector3 left = cross((arp / arp.norm()), (arpVel / arp.norm()));
    double look = 0;

    if (left.dot(unitRG) > 0)
    {
        look = 1;
    }
    else if (left.dot(unitRG) < 0)
    {
        look = -1;
    }
    Vector3 spn = -look * cross(unitRG, arpVel);
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
            double deltaKazPerDeltvaV = look * krgCoa * (arpVel.norm() * std::sin(
                     sicd.scpcoa->dopplerConeAngle * math::Constants::DEGREES_TO_RADIANS) / sicd.scpcoa->slantRange) / stRateCoa;
            Poly1D derivedKazPoly = deltaKazPerDeltvaV * interPulsePeriodPoly;

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

std::vector<double> ComplexXMLValidator::linspace(double start, double end, size_t count) const
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

Poly1D ComplexXMLValidator::polyAt(PolyXYZ poly, size_t idx) const
{
    if (idx > 2)
    {
        throw except::Exception(Ctxt("Idx should be < 3"));
    }

    std::vector<double> coefs;
    for (size_t ii = 0; ii < poly.size(); ++ii)
    {
        coefs.push_back(poly(ii)[idx]);
    }
    Poly1D ret(coefs);
    return ret;
}

bool ComplexXMLValidator::checkPFA()
{
    bool valid = true;
    try
    {
        //2.12.2.2
        assertExists(sicd.pfa, "SICD.PFA");
    }
    catch (except::Exception&)
    {
        return false;
    }

    //2.12.2.1
    if (sicd.grid->type != ComplexImageGridType::RGAZIM)
    {
        messageBuilder.str("");
        messageBuilder << "PFA image formation should result in a RGAZIM grid." << std::endl
            << "Grid.Type: " << sicd.grid->type.toString();
        mLog->error(messageBuilder.str());
        valid = false;
    }

    //2.12.2.3
    if (sicd.scpcoa.get() == NULL || sicd.pfa->polarAngleRefTime - sicd.scpcoa->scpTime > std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << "Polar angle reference time and center of aperture time for center are usuallly the same." << std::endl
            << "PFA.PolarAngRefTime: " << sicd.pfa->polarAngleRefTime << std::endl
            << "SCPCOA.SCPTime: " << (sicd.scpcoa.get() == NULL ? "NULL" : str::toString(sicd.scpcoa->scpTime));
        mLog->warn(messageBuilder.str());
        valid = false;
    }

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

    // Make sure Row.kCtr is consistent with processed RF frequency bandwidth
    if (sicd.radarCollection.get() == NULL || Init::isUndefined(sicd.radarCollection->refFrequencyIndex))
    {
        if (sicd.imageFormation.get() != NULL)
        {
            double fcProc = (sicd.imageFormation->txFrequencyProcMin + sicd.imageFormation->txFrequencyProcMax) / 2;

            // PFA.SpatialFreqSFPoly affects Row.KCtr
            double kapCtr = fcProc * sicd.pfa->spatialFrequencyScaleFactorPoly[0] * 2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC;
            // PFA inscription could cause kapCtr and Row.KCtr to be somewhat different
            double theta = std::atan((sicd.grid->col->impulseResponseBandwidth / 2) / sicd.grid->row->kCenter); // aperture angle
            double kCtrTol = 1 - std::cos(theta); // Difference between krg and Kap (Krg = cos(theta)*kap)
            kCtrTol = std::max(0.01, kCtrTol); // % .01 should be plenty of tolerance for precision issues at small angles

            if (std::abs(sicd.grid->row->kCenter / kapCtr) - 1 > kCtrTol)
            {
                messageBuilder.str("");
                messageBuilder << WF_INCONSISTENT_STR
                    << "Grid.Row.KCtr: " << sicd.grid->row->kCenter << std::endl
                    << "Derived KapCtr: " << kapCtr;
                mLog->error(messageBuilder.str());
                valid = false;
            }
        }
    }
    return valid;
}

bool ComplexXMLValidator::checkRMA()
{
    //2.12.3.1
    try
    {
        assertExists(sicd.rma, "SICD.RMA");
        assertExists(sicd.geoData, "SICD.GeoData");
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.grid->col, "SICD.Grid.Col");
        assertExists(sicd.grid->row, "SICD.Grid.Row");
    }
    catch (except::Exception&)
    {
        return false;
    }

    bool valid = true;
    Vector3 scp = sicd.geoData->scp.ecf;
    Vector3 rowUnitVector = sicd.grid->row->unitVector;
    Vector3 colUnitVector = sicd.grid->col->unitVector;

    if (sicd.rma->rmat.get() != NULL)
    {
        // 2.12.3.2.1
        if (sicd.grid->type != ComplexImageGridType::XCTYAT)
        {
            messageBuilder.str("");
            messageBuilder << "RMA/RMAT image formation should result in a XCTYAT grid." << std::endl
                << "RMA.ImageType: RMAT, Grid.Type: " << sicd.grid->type.toString();
            mLog->error(messageBuilder.str());
        }

        // 2.12.3.2 has to do with the XML representation

        Vector3 posRef = sicd.rma->rmat->refPos;
        Vector3 velRef = sicd.rma->rmat->refVel;
        Vector3 uLOS = (scp - posRef) / (scp - posRef).norm();
        Vector3 left = cross(posRef / posRef.norm(), velRef / velRef.norm());
        int look = 0;
        if (left.dot(uLOS) > 0)
        {
            look = 1;
        }
        else if (left.dot(uLOS) < 0)
        {
            look = -1;
        }
        Vector3 uYAT = (velRef / velRef.norm()) * look;
        Vector3 spn = cross(uLOS, uYAT);
        spn = spn / spn.norm();
        Vector3 uXCT = cross(uYAT, spn);
        double dcaRef = std::acos((velRef / velRef.norm()).dot(uLOS)) * math::Constants::RADIANS_TO_DEGREES;

        // 2.12.3.2.3
        if ((rowUnitVector - uXCT).norm() > UVECT_TOL)
        {
            messageBuilder.str("");
            messageBuilder << "UVect fields inconsistent." << std::endl
                << "Grid.Row.UVectECF: " << rowUnitVector
                << "Derived grid.Row.UVectECT: " << uXCT;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.2.4
        if ((colUnitVector - uYAT).norm() > UVECT_TOL)
        {
            messageBuilder.str("");
            messageBuilder << "UVect fields inconsistent." << std::endl
                << "Grid.Col.UVectECF: " << colUnitVector
                << "Derived Grid.Col.UVectECF: " << uYAT;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.2.5
        if (std::abs(dcaRef - sicd.rma->rmat->dopConeAngleRef) > 1e-6)
        {
            messageBuilder.str("");
            messageBuilder << "RMA fields inconsistent." << std::endl
                << "RMA.RMAT.DopConeAngleRef: " << sicd.rma->rmat->dopConeAngleRef
                << std::endl << "Derived RMA.RMAT.DopConeAngleRef: " << dcaRef;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        if (sicd.radarCollection.get() == NULL || Init::isUndefined(sicd.radarCollection->refFrequencyIndex))
        {
            double fcProc = (sicd.imageFormation->txFrequencyProcMin + sicd.imageFormation->txFrequencyProcMax) / 2;
            double kfc = fcProc * (2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC);

            // 2.12.3.2.6
            if ((kfc * std::sin(sicd.rma->rmat->dopConeAngleRef * math::Constants::RADIANS_TO_DEGREES) / sicd.grid->row->kCenter) - 1 > WF_TOL)
            {
                messageBuilder.str("");
                messageBuilder << WF_INCONSISTENT_STR
                    << "Grid.Row.KCtr: " << sicd.grid->row->kCenter << std::endl
                    << "Derived Grid.Row.KCtr: " << kfc * std::sin(sicd.rma->rmat->dopConeAngleRef * math::Constants::RADIANS_TO_DEGREES);
                mLog->warn(messageBuilder.str());
            }

            // 2.12.3.2.7
            if ((kfc * std::cos(sicd.rma->rmat->dopConeAngleRef * math::Constants::DEGREES_TO_RADIANS) / sicd.grid->col->kCenter) - 1 > WF_TOL)
            {
                messageBuilder.str("");
                messageBuilder << WF_INCONSISTENT_STR
                    << "Grid.Col.KCtr: " << sicd.grid->col->kCenter << std::endl
                    << "Derived Grid.Col.KCtr: " << kfc * std::cos(sicd.rma->rmat->dopConeAngleRef * math::Constants::DEGREES_TO_RADIANS);
                mLog->warn(messageBuilder.str());
            }
        }
    }
    else if (sicd.rma->rmcr.get() != NULL)
    {
        // 2.12.3.3.1
        if (sicd.grid->type != ComplexImageGridType::XRGYCR)
        {
            messageBuilder.str("");
            messageBuilder << "RMA/RMCR image formation should result in a XRGYCR grid." << std::endl
                << "RMA.ImageType: RMCR, Grid.Type: " << sicd.grid->type.toString();
            mLog->error(messageBuilder.str());
        }

        // 2.12.3.3.2 deals with the XML representation

        Vector3 posRef = sicd.rma->rmcr->refPos;
        Vector3 velRef = sicd.rma->rmcr->refVel;
        Vector3 uXRG = (scp - posRef) / (scp - posRef).norm();
        Vector3 left = cross(posRef / posRef.norm(), velRef / velRef.norm());
        int look = 0;
        if (left.dot(uXRG) > 0)
        {
            look = 1;
        }
        else if (left.dot(uXRG) < 0)
        {
            look = -1;
        }
        Vector3 spn = cross(velRef / velRef.norm(), uXRG);
        spn = spn / spn.norm();
        Vector3 uYCR = cross(spn, uXRG);
        double dcaRef = std::acos((velRef / velRef.norm()).dot(uXRG)) * math::Constants::RADIANS_TO_DEGREES;

        //2.12.3.3.3
        if ((rowUnitVector - uXRG).norm() > UVECT_TOL)
        {
            messageBuilder.str("");
            messageBuilder << "UVect fields inconsistent." << std::endl
                << "Grid.Row.UVectECF: " << rowUnitVector << std::endl
                << "Derived Grid.Row.UVectECF: " << uXRG;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.3.4
        if ((colUnitVector - uYCR).norm() > UVECT_TOL)
        {
            messageBuilder.str("");
            messageBuilder << "UVect fields inconsistent." << std::endl
                << "Grid.Col.UVectECF: " << colUnitVector << std::endl
                << "Derived Grid.Col.UVectECF: " << uYCR;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.3.5
        if (std::abs(dcaRef - sicd.rma->rmcr->dopConeAngleRef) > 1e-6)
        {
            messageBuilder.str("");
            messageBuilder << "RMA fields inconsistent." << std::endl
                << "RMA.RMCR.DopConeAngleRef: " << sicd.rma->rmcr->dopConeAngleRef << std::endl
                << "Derived RMA.RMCR.DopConeAngleRef: " << dcaRef;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.3.6
        if (sicd.grid->col->kCenter != 0)
        {
            messageBuilder.str("");
            messageBuilder << "Grid.Col.KCtr must be zero for RMA/RMCR data." << std::endl
                << "Grid.Col.KCtr = " << sicd.grid->col->kCenter;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.3.7
        if (sicd.radarCollection.get() == NULL || Init::isUndefined(sicd.radarCollection->refFrequencyIndex))
        {
            double fcProc = (sicd.imageFormation->txFrequencyProcMin + sicd.imageFormation->txFrequencyProcMax) / 2;
            double kfc = fcProc * (2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC);
            if ((sicd.grid->row->kCenter / kfc) - 1 > WF_TOL)
            {
                messageBuilder.str("");
                messageBuilder << WF_INCONSISTENT_STR << std::endl
                    << "Grid.Row.KCtr: " << sicd.grid->row->kCenter << std::endl
                    << "Center frequency * 2/c: " << kfc;
                mLog->warn(messageBuilder.str());
                valid = false;
            }
        }
    }
    else if (sicd.rma->inca.get() != NULL)
    {
        // 2.12.3.4.1
        if (sicd.grid->type != ComplexImageGridType::RGZERO)
        {
            messageBuilder.str("");
            messageBuilder << "RMA/INCA image formation should result in a RGZERO grid." << std::endl
                << "Grid.Type: " << sicd.grid->type.toString();
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.12.3.4.2 deals with the XML representation


        //2.12.3.4.3
        if (sicd.collectionInformation.get() != NULL)
        {
            if (sicd.collectionInformation->radarMode == RadarModeType::SPOTLIGHT &&
                (!Init::isUndefined(sicd.rma->inca->dopplerCentroidPoly) ||
                    !Init::isUndefined(sicd.rma->inca->dopplerCentroidCOA)))
            {
                messageBuilder.str("");
                messageBuilder << "RMA.INCA fields inconsistent." << std::endl
                    << "RMA.INCA.DopplerCentroidPoly/DopplerCentroidCOA not used for SPOTLIGHT collection.";
                mLog->error(messageBuilder.str());
                valid = false;
            }
            //2.12.3.4.4
            else if (sicd.collectionInformation->radarMode != RadarModeType::SPOTLIGHT)
            {
                if (Init::isUndefined(sicd.rma->inca->dopplerCentroidPoly) || Init::isUndefined(sicd.rma->inca->dopplerCentroidCOA))
                {
                    messageBuilder.str("");
                    messageBuilder << "RMA.INCA fields inconsistent." << std::endl
                        << "RMA.INCA.DopplerCentroidPoly/COA required for non-SPOTLIGHT collection.";
                    mLog->error(messageBuilder.str());
                    valid = false;
                }
                else
                {
                    std::vector<std::vector<double> > difference;
                    difference.resize(2);
                    for (size_t ii = 0; ii < 2; ++ii)
                    {
                        Poly2D kcoaPoly = sicd.grid->col->deltaKCOAPoly;
                        Poly2D centroidPoly = sicd.rma->inca->dopplerCentroidPoly;
                        for (size_t jj = 0; jj < std::min(kcoaPoly.orderY(), centroidPoly.orderY()); ++jj)
                        {
                            double val = kcoaPoly[ii][jj] - (centroidPoly[ii][jj] * sicd.rma->inca->timeCAPoly(2));
                            difference[ii].push_back(val);
                        }
                    }
                    double norm = 0;
                    for (size_t ii = 0; ii < difference.size(); ++ii)
                    {
                        for (size_t jj = 0; jj << difference[ii].size(); ++jj)
                        {
                            norm += (difference[ii][jj] * difference[ii][jj]);
                        }
                    }
                    norm = std::sqrt(norm);

                    if (!Init::isUndefined(sicd.grid->col->deltaKCOAPoly) &&
                        sicd.rma->inca->dopplerCentroidCOA == BooleanType::IS_TRUE &&
                        norm > IFP_POLY_TOL)
                    {
                        messageBuilder.str("");
                        messageBuilder << "RMA.INCA fields inconsistent." << std::endl
                            << "Compare Grid.Col.KCOAPoly to RMA.INCA.DopCentroidPoly * RMA.INCA.TimeCAPoly(2).";
                        mLog->error(messageBuilder.str());
                        valid = false;
                    }

                }
            }
            // Skipping the rest of 2.12.3.4. Poly heavy
            // INCA UVects are defined from closest approach position/velocity
            // not center of aperture
            std::vector<double> caPos;
            caPos.resize(3);
            for (size_t ii = 0; ii < 3; ++ii)
            {
                Poly1D poly = polyAt(sicd.position->arpPoly, ii);
                caPos[ii] = poly(sicd.rma->inca->timeCAPoly(1));
            }

            std::vector<double> caVel;
            caVel.resize(3);
            for (size_t ii = 0; ii < 3; ++ii)
            {
                Poly1D poly = polyAt(sicd.position->arpPoly, ii).derivative();
                caVel[ii] = poly(sicd.rma->inca->timeCAPoly(1));
            }

            Vector3 caPosVector(caPos);
            Vector3 caVelVector(caVel);
            Vector3 uRG = (scp - caPosVector) / (scp - caPosVector).norm();
            Vector3 left = cross(caPosVector / caPosVector.norm(), caVelVector / caVelVector.norm());
            int look = 0;
            if (left.dot(uRG) < 0)
            {
                look = -1;
            }
            else if (left.dot(uRG) > 0)
            {
                look = 1;
            }
            Vector3 spn = cross(uRG, caVelVector) * -look;
            spn = spn / spn.norm();
            Vector3 uAZ = cross(spn, uRG);

            // 2.12.3.4.6
            if ((uRG - rowUnitVector).norm() > UVECT_TOL)
            {
                messageBuilder.str("");
                messageBuilder << "UVectFields inconsistent" << std::endl
                    << "Grid.Row.UVectECF: " << rowUnitVector
                    << "Derived Grid.Row.UVectECF: " << uRG;
                mLog->error(messageBuilder.str());
                return false;
            }

            // 2.12.3.4.7
            if ((uAZ - colUnitVector).norm() > UVECT_TOL)
            {
                messageBuilder.str("");
                messageBuilder << "UVectFields inconsistent" << std::endl
                    << "Grid.Col.UVectECF: " << colUnitVector
                    << "Derived Grid.Col.UVectECF: " << uAZ;
                mLog->error(messageBuilder.str());
            }

            // 2.12.3.4.8
            if (sicd.grid->col->kCenter != 0)
            {
                messageBuilder.str("");
                messageBuilder << "Grid.Col.KCtr  must be zero "
                    << "for RMA/INCA data." << std::endl
                    << "Grid.Col.KCtr: " << sicd.grid->col->kCenter;
                mLog->error(messageBuilder.str());
            }

            // 2.12.3.4.9
            double fc = (sicd.radarCollection->txFrequencyMin +
                sicd.radarCollection->txFrequencyMax) / 2;
            if ((sicd.rma->inca->freqZero / fc) - 1 > WF_TOL)
            {
                messageBuilder.str("");
                messageBuilder << "RMA.INCA.FreqZero is typically the center transmit frequency" << std::endl
                    << "RMA.INCA.FreqZero: " << sicd.rma->inca->freqZero
                    << "Center transmit frequency: " << fc;
                mLog->warn(messageBuilder.str());
            }

            // 2.12.3.4.10
            // I'm not sure I'm transcribing R_CA_SCP -> rangeCA correctly
            if ((caPosVector - scp).norm() - sicd.rma->inca->rangeCA > SCPCOA_TOL)
            {
                messageBuilder.str("");
                messageBuilder << "RMA.INCA fields inconsistent." << std::endl
                    << "RMA.INCA.rangeCA: " << sicd.rma->inca->rangeCA
                    << "Derived RMA.INCA.rangeCA: " << (caPosVector - scp).norm();
                mLog->error(messageBuilder.str());
            }

            // 2.12.3.4.11
            if (Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) &&
                std::abs(sicd.grid->row->kCenter - sicd.rma->inca->freqZero * 2 /
                    math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC) >
                std::numeric_limits<double>::epsilon())
            {
                messageBuilder.str("");
                messageBuilder << WF_INCONSISTENT_STR << std::endl
                    << "RMA.INCA.FreqZero * 2 / c: " << sicd.rma->inca->freqZero * 2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC
                    << "Grid.Row.KCenter: " << sicd.grid->row->kCenter;
                mLog->error(messageBuilder.str());
            }
        }
    }
    return valid;
}

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
    try
    {
        assertExists(sicd.geoData, "SICD.GeoData");
    }
    catch (except::Exception&)
    {
        return false;
    }

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
    try
    {
        assertExists(sicd.radarCollection, "SICD.RadarCollection");
    }
    catch (except::Exception&)
    {
        return false;
    }

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

bool ComplexXMLValidator::checkARPPoly()
{
    try
    {
        assertExists(sicd.position, "SICD.Position");
    }
    catch (except::Exception&)
    {
        return false;
    }

    if (sicd.position->arpPoly.order() < 2)
    {
        messageBuilder.str("");
        messageBuilder << "SICD.Position.ARPPoly should have at least position and velocity terms." << std::endl;
        mLog->error(messageBuilder.str());
        return false;
    }
    return true;
}

bool ComplexXMLValidator::checkTimeCOAPoly()
{
    bool valid = true;
    try
    {
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.collectionInformation, "SICD.CollectionInformation");
    }

    catch (except::Exception&)
    {
        return false;
    }

    const Poly2D timeCOAPoly = sicd.grid->timeCOAPoly;
    const std::string mode = sicd.collectionInformation->radarMode.toString();

    //2.1. Scalar TimeCOAPoly means SPOTLIGHT data
    bool isScalar = true;

    // I don't know that it's impossible for a one-degree polynomial to be expressed
    // as a polynomial of higher order for whatever reason, so I'm checking each term
    // manually
    for (size_t ii = 0; ii <= timeCOAPoly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= timeCOAPoly.orderY(); ++jj)
        {
            if (ii == 0 && jj == 0)
            {
                continue;
            }
            if (timeCOAPoly[ii][jj] != 0)
            {
                isScalar = false;
                break;
            }
        }
    }

    if (mode == "SPOTLIGHT" && !isScalar)
    {
        mLog->error("SPOTLIGHT data should only have scalar TimeCOAPoly.");
        valid = false;
    }

    if (mode != "SPOTLIGHT" && isScalar)
    {
        mLog->warn("Non-SPOTLIGHT data will generally have more than one nonzero"
            "term in TimeCOAPoly unless \"formed as spotlight\".");
        valid = false;
    }

    return valid;
}

bool ComplexXMLValidator::checkFFTSigns()
{
    try
    {
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.grid->row, "SICD.Grid.Row");
        assertExists(sicd.grid->col, "SICD.Grid.Col");
    }
    catch (except::Exception&)
    {
        return false;
    }

    //2.2. FFT signs in both dimensions almost certainly have to be equal
    FFTSign rowSign = sicd.grid->row->sign;
    FFTSign colSign = sicd.grid->col->sign;

    if (rowSign != colSign)
    {
        messageBuilder.str("");
        messageBuilder << "FFT signs in row and column direction should be the same." << std::endl
            << "Grid.Row.Sgn: " << rowSign.toString() << std::endl
            << "Grid.Col.Sgn: " << colSign.toString() << std::endl;
        mLog->error(messageBuilder.str());
        return false;
    }

    return true;
}

bool ComplexXMLValidator::checkFrequencySupportParameters(const DirectionParameters& direction, const std::string& name)
{
    bool valid = true;
    //2.3.1, 2.3.5
    if (direction.deltaK2 <= direction.deltaK1)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid." << name << ".DeltaK1: " << direction.deltaK1 << std::endl
            << "SICD.Grid." << name << ".DetalK2: " << direction.deltaK2 << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }

    else
    {
        // 2.3.2, 2.3.6
        if (direction.deltaK2 > (1 / (2 * direction.sampleSpacing)) + std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid." << name << ".SampleSpacing: " << 0.5/direction.sampleSpacing << std::endl
                << "SICD.Grid." << name << ".DetalK2: " << direction.deltaK2 << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.3.3, 2.3.7
        if (direction.deltaK1 < (-1 / (2 * direction.sampleSpacing)) - std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "0.5/SICD.Grid." << name << ".SampleSpacing: " << 0.5/direction.sampleSpacing << std::endl
                << "SICD.Grid." << name << ".DetalK1: " << direction.deltaK1 << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }

        // 2.3.4, 2.3.8
        if (direction.impulseResponseBandwidth > (direction.deltaK2 - direction.deltaK1) + std::numeric_limits<double>::epsilon())
        {
            messageBuilder.str("");
            messageBuilder << boundsErrorMessage << std::endl
                << "SICD.Grid." << name << ".impulseResponseBandwidth: " << direction.impulseResponseBandwidth << std::endl
                << "SICD.Grid." << name << ".DeltaK2 - SICD.Grid." << name << ".DeltaK1: "
                << direction.deltaK2 - direction.deltaK1 << std::endl;
            mLog->error(messageBuilder.str());
            valid = false;
        }
    }
    return valid;
}

bool ComplexXMLValidator::checkFrequencySupportParameters()
{
    try
    {
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.grid->col, "SICD.Grid.Col");
        assertExists(sicd.grid->row, "SICD.Grid.Row");
    }
    catch (except::Exception&)
    {
        return false;
    }

    bool valid = (checkFrequencySupportParameters(*sicd.grid->col, "Col") &&
        checkFrequencySupportParameters(*sicd.grid->row, "Row"));

    // 2.3.9. Compute our own DeltaK1/K2 and test for consistency with DelaKCOAPoly,
    // ImpRespBW, and SS.  Here, we assume the min and max of DeltaKCOAPoly must be
    // on the vertices of the image, since it is smooth and monotonic in most cases--
    // lathough in actuality this is not always the case.  To be totally generic, 
    // we would have to search for an interior min and max as well
    std::vector<std::vector<sys::SSize_T> > vertices;
    vertices.resize(2);

    if (sicd.imageData.get() != NULL && sicd.imageData->validData.size() != 0)
    {
        //test vertices
        for (size_t ii = 0; ii < sicd.imageData->validData.size(); ++ii)
        {
            vertices[0].push_back(sicd.imageData->validData[ii].col);
        }
        for (size_t ii = 0; ii < sicd.imageData->validData.size(); ++ii)
        {
            vertices[1].push_back(sicd.imageData->validData[ii].row);
        }
    }
    else
    {
        //use edges of full image
        vertices[0].push_back(0);
        vertices[0].push_back(sicd.imageData->validData.size() - 1);
        vertices[0].push_back(sicd.imageData->validData.size() - 1);
        vertices[0].push_back(0);
        vertices[1].push_back(0);
        vertices[1].push_back(0);
        vertices[1].push_back(sicd.imageData->validData.size() - 1);
        vertices[1].push_back(sicd.imageData->validData.size() - 1);
    }

    double minRowDk = 0;
    double maxRowDk = 0;
    double minColDk = 0;
    double maxColDk = 0;
    double currentDeltaK = 0;

    if (!Init::isUndefined<Poly2D>(sicd.grid->row->deltaKCOAPoly))
    {
        minRowDk = -std::numeric_limits<double>::infinity();
        maxRowDk = std::numeric_limits<double>::infinity();
    }
    if (!Init::isUndefined<Poly2D>(sicd.grid->col->deltaKCOAPoly))
    {
        minColDk = -std::numeric_limits<double>::infinity();
        maxColDk = std::numeric_limits<double>::infinity();
    }

    for (size_t ii = 0; ii < vertices[0].size(); ++ii)
    {
        if (!Init::isUndefined<Poly2D>(sicd.grid->row->deltaKCOAPoly))
        {
            currentDeltaK = sicd.grid->row->deltaKCOAPoly.atY((double)vertices[1][ii])((double)vertices[0][ii]);
            minRowDk = std::min(currentDeltaK, minRowDk);
            maxRowDk = std::max(currentDeltaK, maxRowDk);
        }
        if (!Init::isUndefined<Poly2D>(sicd.grid->col->deltaKCOAPoly))
        {
            currentDeltaK = sicd.grid->col->deltaKCOAPoly.atY((double)vertices[1][ii])((double)vertices[0][ii]);
            minColDk = std::min(currentDeltaK, minColDk);
            maxColDk = std::max(currentDeltaK, maxColDk);
        }
    }

    minRowDk -= (sicd.grid->row->impulseResponseBandwidth / 2);
    maxRowDk += (sicd.grid->row->impulseResponseBandwidth / 2);
    minColDk -= (sicd.grid->col->impulseResponseBandwidth / 2);
    maxColDk += (sicd.grid->col->impulseResponseBandwidth / 2);

    // Wrapped spectrum
    if (minRowDk < -(1 / sicd.grid->row->sampleSpacing) / 2 || maxRowDk >(1 / sicd.grid->row->sampleSpacing) / 2)
    {
        minRowDk = -(1 / sicd.grid->row->sampleSpacing) / 2;
        maxRowDk = -minRowDk;
    }

    if (minColDk < -(1 / sicd.grid->col->sampleSpacing) / 2 || maxColDk >(1 / sicd.grid->col->sampleSpacing) / 2)
    {
        minColDk = -(1 / sicd.grid->col->sampleSpacing) / 2;
        maxColDk = -minColDk;
    }

    double DK_TOL = 1e-2;

    //2.3.9.1
    if (std::abs(sicd.grid->row->deltaK1 / minRowDk - 1) > DK_TOL)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Row.DeltaK1: " << sicd.grid->row->deltaK1 << std::endl
            << "Derived Grid.Row.DeltaK1: " << minRowDk << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.9.2
    if (std::abs(sicd.grid->row->deltaK2 / maxRowDk - 1) > DK_TOL)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Row.DeltaK2: " << sicd.grid->row->deltaK2 << std::endl
            << "Derived Grid.Row.DeltaK2: " << maxRowDk << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.9.3
    if (std::abs(sicd.grid->col->deltaK1 / minColDk - 1) > DK_TOL)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Col.DeltaK1: " << sicd.grid->col->deltaK1 << std::endl
            << "Derived Grid.Col.DeltaK1: " << minColDk << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    //2.3.9.4
    if (std::abs(sicd.grid->col->deltaK2 / maxColDk - 1) > DK_TOL)
    {
        messageBuilder.str("");
        messageBuilder << boundsErrorMessage << std::endl
            << "SICD.Grid.Col.DeltaK2: " << sicd.grid->col->deltaK2 << std::endl
            << "Derived Grid.Col.DeltaK2: " << maxColDk << std::endl;
        mLog->error(messageBuilder.str());
        valid = false;
    }
    return valid;
}

bool ComplexXMLValidator::checkSupportParamsAgainstPFA()
{
    try
    {
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.grid->col, "SICD.Grid.Col");
        assertExists(sicd.grid->row, "SICD.Grid.Row");
        assertExists(sicd.pfa, "SICD.PFA");
    }
    catch (except::Exception&)
    {
        return false;
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

    // TODO: 2.5 

    if (weightFunction)
    {
        free(weightFunction);
    }

    return false;
}

bool ComplexXMLValidator::checkWeightFunctions()
{
    try
    {
        assertExists(sicd.grid, "SICD.Grid");
        assertExists(sicd.grid->col, "SICD.Grid.Col");
        assertExists(sicd.grid->row, "SICD.Grid.Row");
    }
    catch (except::Exception&)
    {
        return false;
    }

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