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

#include <six/sicd/ComplexXMLParser.h>
#include <six/sicd/ComplexDataBuilder.h>
#include <six/Utilities.h>


namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{
ComplexXMLParser::ComplexXMLParser(const std::string& version,
                                   bool addClassAttributes,
                                   std::auto_ptr<six::SICommonXMLParser> comParser,
                                   logging::Logger* log,
                                   bool ownLog) :
    XMLParser(versionToURI(version), addClassAttributes, log, ownLog),
    mCommon(comParser.release())
{
}

/*
std::vector<std::vector<double> > raised_cos_fun(size_t n, double coef)
{
    std::vector<std::vector<double> > ret;
    ret.resize(2);
    std::vector<double> w;
    for (size_t ii = 0; ii < std::ceil(n / 2); ++ii)
    {
        w.push_back(coef - (1 - coef) * std::cos(2 * M_PI * ii) / (n - 1));
    }
    ret.push_back(w);
    if (n % 2 == 1)
    {
        ret[1].resize(w.size());
        std::reverse_copy(w.begin(), w.end(), ret[1].begin());
    }
    else
    {
        ret[1].resize(w.size() - 1);
        std::reverse_copy(w.begin(), w.end() - 1, ret[1].begin());
    }
    return ret;
}

std::vector<double> kaiserNosptb(size_t L, double beta=0.5)
{
    std::vector<double> k;
    if (L == 0)
    {
        throw except::Exception(Ctxt("L must be positive"));
    }
    if (L == 1)
    {
        k.push_back(1);
        return k;
    }
    else
    {
        Bessel besseli0(0);
        size_t m = L - 1;
        for (size_t ii = 0; ii < L; ++ii)
        {
            double temp = 2 * beta / m * std::sqrt(ii * (m - ii));
            k.push_back(besseli0(temp) / besseli0(beta));
        }
        return k;
    }
}

std::vector<std::vector<double> > weightFn(const DirectionParameters& params, size_t n)
{
    std::vector<std::vector<double> > ret;
    if (params.weightType.get() != NULL)
    {
        const std::string& windowName = params.weightType->windowName;
        if (windowName == "UNIFORM")
        {
            return ret;
        }
        else if (windowName == "HAMMING")
        {
            double value = 0.54;
            if (!params.weightType->parameters.empty() && !params.weightType->parameters[0].str().empty())
            {
                //A Hamming window is defined in many places as a raised cosine of weight .54,
                //so this is the default. However, some data use a generalized raised cosine and
                //call it HAMMING, so we allow for both uses.
                value = str::toType<double>(params.weightType->parameters[0].str());
            }
            return raised_cos_fun(n, value);
        }
        else if (windowName == "HANNING")
        {
            return raised_cos_fun(n, 0.5);
        }
        else if (windowName == "KAISER")
        {
            ret.push_back(kaiserNosptb(n, str::toType<double>(params.weightType->parameters[0].str())));
            return ret;
        }
        else if (windowName == "TAYLOR")
        {
            const six::ParameterCollection collection& = params.weightType->parameters;
            double nbar = str::toType<double>(collection.findParameter("NBAR").str());
            double sll = str::toType<double>(collection.findParameter("SLL").str());
            sll = -1 * std::abs(sll);
        }
    }
}
*/

void ComplexXMLParser::validate(const ComplexData& sicd) const
{
    // Check for internal consistency of data
    // Most comments (and section numbers) taken from a 'validate_sicd.m' MATLAB file
    //2.1. Scalar TimeCOAPoly means SPOTLIGHT data
    if (sicd.grid.get() == NULL)
    {
        throw except::Exception(Ctxt("ComplexData.Grid required but not found"));
    }
    const std::string WF_INCONSISTENT_STR = "Waveform fields not consistent";
    const Poly2D timeCOAPoly = sicd.grid->timeCOAPoly;
    const std::string mode = sicd.collectionInformation->radarMode.toString();
    bool isScalar = true;

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
        log()->error("SPOTLIGHT data should only have scalar TimeCOAPoly.");
    }

    if (mode != "SPOTLIGHT" && isScalar)
    {
        log()->warn("Non-SPOTLIGHT data will generally have more than one nonzero"
            "term in TimeCOAPoly unless \"formed as spotlight\".");
    }

    //2.2. FFT signs in both dimensions almost certainly have to be equal

    //We've already checked that grid is non-null
    if (sicd.grid->row.get() == NULL || sicd.grid->col.get() == NULL)
    {
        throw except::Exception(Ctxt("ComplexData.Grid.Row and ComplexData.Grid.Col required but not found"));
    }

    if (sicd.grid->row->sign != sicd.grid->col->sign)
    {
        log()->error("FFT signs in row and column direction should be the same");
    }
    //2.3. Frequencey support parameters
    // We add the epsilons in all these tests to handle numerical precision issues 
    // for two values that are very nearly equal
    std::ostringstream details;
    //Bounds checking with grid->row/col
    //2.3.1
    const std::string boundsErrorMessage = "Violation of spatial frequency extent bounds.";
    if (sicd.grid->col->deltaK2 <= sicd.grid->col->deltaK1)
    {
        details << boundsErrorMessage << "\n"
            << "Expect SICD.Grid.Col.DeltaK2 > SICD.Grid.Col.DeltaK1\n"
            << "DeltaK2: " << sicd.grid->col->deltaK2 << "\n"
            << "DetalK1: " << sicd.grid->col->deltaK1 << "\n";
        log()->error(details.str());
    }
    else
    {
        //2.3.2
        if (sicd.grid->col->deltaK2 > 1 / (2 * sicd.grid->col->sampleSpacing + std::numeric_limits<double>::epsilon()))
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "\nExpect SICD.Grid.Col.DeltaK2 > 1/(2*Grid.Col.SampleSpacing)\n"
                << "DeltaK2: " << sicd.grid->col->deltaK2 << "\n"
                << "SampleSpacing: " << sicd.grid->col->sampleSpacing << "\n";
            log()->error(details.str());
        }
        //2.3.3
        if (sicd.grid->col->deltaK1 < -1 / (2 * sicd.grid->col->sampleSpacing - std::numeric_limits<double>::epsilon()))
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "\nExpect SICD.Grid.Col.DeltaK1 < -1/(2*Grid.Col.SampleSpacing)\n"
                << "DeltaK1: " << sicd.grid->col->deltaK1 << "\n"
                << "SampleSpacing: " << sicd.grid->col->sampleSpacing << "\n";
            log()->error(details.str());
        }
        //2.3.4
        if (sicd.grid->col->impulseResponseBandwidth > sicd.grid->col->deltaK2 - sicd.grid->col->deltaK1 + std::numeric_limits<double>::epsilon())
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "\nExpect SICD.Grid.Col.impulseResponseBandwidth > SICD.Grid.Col.DeltaK2 - SICD.Grid.Col.DeltaK2)\n"
                << "ImpulseResponseBandwidth: " << sicd.grid->col->impulseResponseBandwidth << "\n"
                << "DeltaK2: " << sicd.grid->col->deltaK2 << "\n"
                << "DeltaK1: " << sicd.grid->col->deltaK1 << "\n";
            log()->error(details.str());
        }
    }

    //2.3.5
    if (sicd.grid->row->deltaK2 <= sicd.grid->row->deltaK1)
    {
        details << boundsErrorMessage << "\n"
            << "Expect SICD.Grid.Col.DeltaK2 > SICD.Grid.Col.DeltaK1\n"
            << "DeltaK2: " << sicd.grid->row->deltaK2 << "\n"
            << "DetalK1: " << sicd.grid->row->deltaK1 << "\n";
        log()->error(details.str());
    }
    else
    {
        //2.3.6
        if (sicd.grid->row->deltaK2 > 1 / (2 * sicd.grid->row->sampleSpacing + std::numeric_limits<double>::epsilon()))
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "\nExpect SICD.Grid.Col.DeltaK2 > 1/(2*Grid.Col.SampleSpacing)\n"
                << "DeltaK2: " << sicd.grid->row->deltaK2 << "\n"
                << "SampleSpacing: " << sicd.grid->row->sampleSpacing << "\n";
            log()->error(details.str());
        }
        //2.3.7
        if (sicd.grid->row->deltaK1 < -1 / (2 * sicd.grid->row->sampleSpacing - std::numeric_limits<double>::epsilon()))
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "\nExpect SICD.Grid.Col.DeltaK1 < -1/(2*Grid.Col.SampleSpacing)\n"
                << "DeltaK1: " << sicd.grid->row->deltaK1 << "\n"
                << "SampleSpacing: " << sicd.grid->row->sampleSpacing << "\n";
            log()->error(details.str());
        }
        //2.3.8
        if (sicd.grid->row->impulseResponseBandwidth > sicd.grid->row->deltaK2 - sicd.grid->row->deltaK1 + std::numeric_limits<double>::epsilon())
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "\nExpect SICD.Grid.Col.impulseResponseBandwidth > SICD.Grid.Col.DeltaK2 - SICD.Grid.Col.DeltaK2)\n"
                << "ImpulseResponseBandwidth: " << sicd.grid->row->impulseResponseBandwidth << "\n"
                << "DeltaK2: " << sicd.grid->row->deltaK2 << "\n"
                << "DeltaK1: " << sicd.grid->row->deltaK1 << "\n";
            log()->error(details.str());
        }
    }

    // 2.3.9. Compute our own DeltaK1/K2 and test for consistency with DelaKCOAPoly,
    // ImpRespBW, and SS.  Here, we assume the min and max of DeltaKCOAPoly must be
    // on the vertices of the image, since it is smooth and monotonic in most cases--
    // lathough in actuality this is not always the case.  To be totally generic, 
    // we would have to search for an interior min and max as well
    std::vector<std::vector<int> > vertices;
    vertices.resize(2);

    if (sicd.imageData->validData.size() != 0)
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
            //TODO: Make sure I have the order right here
            currentDeltaK = sicd.grid->row->deltaKCOAPoly.atY(vertices[0][ii])(vertices[1][ii]);
            minRowDk = std::min(currentDeltaK, minRowDk);
            maxRowDk = std::max(currentDeltaK, maxRowDk);
        }
        if (!Init::isUndefined<Poly2D>(sicd.grid->col->deltaKCOAPoly))
        {
            currentDeltaK = sicd.grid->col->deltaKCOAPoly.atY(vertices[0][ii])(vertices[1][ii]);
            minColDk = std::min(currentDeltaK, minColDk);
            maxColDk = std::max(currentDeltaK, maxColDk);
        }
    }
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

    double DK_TOL = 0.01;
    //2.3.9.1
    if (std::abs(sicd.grid->row->deltaK1 / maxRowDk - 1) > DK_TOL)
    {
        details.str("");
        details << boundsErrorMessage << "\n"
            << "SICD.Grid.Row.DeltaK1: " << sicd.grid->row->deltaK1 << "\n"
            << "Derived Grid.Row.DeltaK1: " << minRowDk << "\n";
        log()->error(details.str());
    }
    //2.3.9.2
    if (std::abs(sicd.grid->row->deltaK2 / maxRowDk - 1) > DK_TOL)
    {
        details.str("");
        details << boundsErrorMessage << "\n"
            << "SICD.Grid.Row.DeltaK2: " << sicd.grid->row->deltaK2 << "\n"
            << "Derived Grid.Row.DeltaK2: " << minRowDk << "\n";
        log()->error(details.str());
    }
    //2.3.8.3
    if (std::abs(sicd.grid->col->deltaK1 / maxRowDk - 1) > DK_TOL)
    {
        details.str("");
        details << boundsErrorMessage << "\n"
            << "SICD.Grid.Col.DeltaK1: " << sicd.grid->col->deltaK1 << "\n"
            << "Derived Grid.Col.DeltaK1: " << minRowDk << "\n";
        log()->error(details.str());
    }
    //2.3.9.4
    if (std::abs(sicd.grid->col->deltaK2 / maxRowDk - 1) > DK_TOL)
    {
        details.str("");
        details << boundsErrorMessage << "\n"
            << "SICD.Grid.Col.DeltaK2: " << sicd.grid->col->deltaK2 << "\n"
            << "Derived Grid.Col.DeltaK2: " << minRowDk << "\n";
        log()->error(details.str());
    }

    if (sicd.pfa.get() != NULL)
    {
        //Slow-time deskew would allow for PFA.Kaz2-PFA.Kaz1>(1/Grid.Col.SS),
        //since Kaz bandwidth is compressed from original polar annulus.
        if (sicd.pfa->slowTimeDeskew.get() == NULL || sicd.pfa->slowTimeDeskew->applied.toString() != "IS_TRUE")
        {
            //2.3.10
            if (sicd.pfa->kaz2 - sicd.grid->col->kCenter > 1 / ((2 * sicd.grid->col->sampleSpacing) + std::numeric_limits<double>::epsilon()))
            {
                details.str("");
                details << boundsErrorMessage << "\n"
                    << "Expect 0.5 / SICD.Grid.Col.SampleSpacing >= PFA.Kaz2 - Grid.Col.KCenter\n"
                    << "0.5/SICD.Grid.Col.SampleSpacing: " << 0.5 / sicd.grid->col->sampleSpacing << "\n"
                    << "PFA.Kaz2 - Grid.Col.KCenter: " << sicd.pfa->kaz2 - sicd.grid->col->kCenter << "\n";
                log()->error(details.str());
            }
            //2.3.11
            if (sicd.pfa->kaz1 - sicd.grid->col->kCenter < -1 / ((2 * sicd.grid->col->sampleSpacing) - std::numeric_limits<double>::epsilon()))
            {
                details.str("");
                details << boundsErrorMessage << "\n"
                    << "Expect -0.5 / SICD.Grid.Col.SampleSpacing <= PFA.Kaz1 - Grid.Col.KCenter\n"
                    << "-0.5/SICD.Grid.Col.SampleSpacing: " << -0.5 / sicd.grid->col->sampleSpacing << "\n"
                    << "PFA.Kaz1 - Grid.Col.KCenter: " << sicd.pfa->kaz1 - sicd.grid->col->kCenter << "\n";
                log()->error(details.str());
            }
        }
        //2.3.12
        if (sicd.pfa->krg2 - sicd.grid->row->kCenter > 1 / (2 * sicd.grid->row->sampleSpacing + std::numeric_limits<double>::epsilon()))
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "Expect PFA.Krg2 - Grid.Row.KCenter <= 0.5 / SICD.Grid.Row.SampleSpacing\n"
                << "0.5/SICD.Grid.Row.SampleSpacing: " << 0.5 / sicd.grid->row->sampleSpacing << "\n"
                << "PFA.Krg2 - Grid.Row.KCenter: " << sicd.pfa->krg2 - sicd.grid->row->kCenter << "\n";
            log()->error(details.str());
        }
        //2.3.13
        if (sicd.pfa->krg1 - sicd.grid->row->kCenter < -1 / (2 * sicd.grid->row->sampleSpacing - std::numeric_limits<double>::epsilon()))
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "Expect PFA.Krg1 - Grid.Row.KCenter <= -0.5 / SICD.Grid.Row.SampleSpacing\n"
                << "-0.5/SICD.Grid.Row.SampleSpacing: " << -0.5 / sicd.grid->row->sampleSpacing << "\n"
                << "PFA.Krg1 - Grid.Row.KCenter: " << sicd.pfa->krg1 - sicd.grid->row->kCenter << "\n";
            log()->error(details.str());
        }
        //2.3.14
        if (sicd.grid->col->impulseResponseBandwidth > sicd.pfa->kaz2 - sicd.pfa->kaz1 + std::numeric_limits<double>::epsilon())
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "Expect SICD.Grid.Col.ImpulseResponseBandwidth <= SICD.PFA.Kaz2 - SICD.PFA.Kaz1"
                << "Grid.Col.ImpulseResponseBandwidth: " << sicd.grid->col->impulseResponseBandwidth << "\n"
                << "SICD.PFA.Kaz2 - SICD.PFA.Kaz1: " << sicd.pfa->kaz2 - sicd.pfa->kaz1 << "\n";
            log()->error(details.str());
        }
        //2.3.15
        if (sicd.grid->col->impulseResponseBandwidth > sicd.pfa->krg2 - sicd.pfa->krg1 + std::numeric_limits<double>::epsilon())
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "Expect SICD.Grid.Col.ImpulseResponseBandwidth <= SICD.PFA.Krg2 - SICD.PFA.Krg1"
                << "Grid.Col.ImpulseResponseBandwidth: " << sicd.grid->col->impulseResponseBandwidth << "\n"
                << "SICD.PFA.Krg2 - SICD.PFA.Krg1: " << sicd.pfa->krg2 - sicd.pfa->krg1 << "\n";
            log()->error(details.str());
        }
        //2.3.16
        if (sicd.grid->col->kCenter != 0 && std::abs(sicd.grid->col->kCenter - (sicd.pfa->kaz1 + sicd.pfa->kaz2) / 2) > 1e-5)
        {
            details.str("");
            details << boundsErrorMessage << "\n"
                << "Expect SICD.Grid.Col.KCenter == 0 or |SICD.Grid.Col.KCenter - mean(SICD.PFA.Kaz1, SICD.PFA.Kaz2)| <= 1e-5"
                << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << "\n"
                << "mean(SICD.PFA.Kaz1, SICD.PFA.Kaz2): " << (sicd.pfa->kaz1 + sicd.pfa->kaz2) / 2 << "\n";
            log()->error(details.str());
        }
    }

    // 2.4. Does WgtFunct agree with WgtType?
    const int DEFAULT_WGT_SIZE = 512;
    const double WGT_TOL = 1e-3;
    const double WF_TOL = 1e-3; //Relative tolerance

    // Skipping 2.4.1 and 2.4.2 because they're over my head
    // 2.4.3
    if (sicd.grid->col->weightType.get() != NULL && sicd.grid->col->weightType->windowName != "UNIFORM" &&
        sicd.grid->col->weightType->windowName != "UNKNOWN" && sicd.grid->col->weights.size() == 0)
    {
        details.str("");
        details << "Unrecognized weighting description\n" <<
            "SICD.Grid.Col.WgtType.WindowName: " << sicd.grid->col->weightType->windowName
            << std::endl;
        log()->warn(details.str());
    }

    // 2.4.4
    if (sicd.grid->row->weightType.get() != NULL && sicd.grid->row->weightType->windowName != "UNIFORM" &&
        sicd.grid->row->weightType->windowName != "UNKNOWN" && sicd.grid->row->weights.size() == 0)
    {
        details.str("");
        details << "Unrecognized weighting description\n" <<
            "SICD.Grid.Row.WgtType.WindowName: " << sicd.grid->row->weightType->windowName
            << std::endl;
        log()->warn(details.str());
    }

    // 2.5 depends on something I didn't compute in 2.4.1/2.4.2.
    // Omitting

    // 2.6
    if (sicd.position->arpPoly.order() < 2) // Must be able to derived at least position and velocity
    {
        details.str("");
        details << "ARPPoly should have at least position and velocity terms."
            << std::endl;
        log()->error(details.str());
    }

    // Omitting 2.7
    if (sicd.radarCollection.get() != NULL)
    {
        // 2.8 Waveform description consistency
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
            if (std::abs(wfMin / sicd.radarCollection->txFrequencyMin - 1) > WF_TOL)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.TxFreqStart: "
                    << wfMin << std::endl
                    << "SICD.RadarCollection.TxFrequency.Min: " << sicd.radarCollection->txFrequencyMin
                    << std::endl;
                log()->error(details.str());
            }
        }

        //2.8.2
        if (wfMin != std::numeric_limits<double>::infinity())
        {
            if (std::abs(wfMax / sicd.radarCollection->txFrequencyMax - 1) > WF_TOL)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.TxFreqStart + TxFRBandwidth: "
                    << wfMax << std::endl
                    << "SICD.RadarCollection.TxFrequency.Max: " << sicd.radarCollection->txFrequencyMax
                    << std::endl;
                log()->error(details.str());
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
            if (std::abs(wfParam.txRFBandwidth / (wfParam.txPulseLength * wfParam.txFMRate)) - 1 > WF_TOL)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.TxFRBandwidth: "
                    << wfParam.txRFBandwidth << std::endl
                    << "SICD.RadarCollection.TxFrequency.txFMRate * txPulseLength: "
                    << wfParam.txFMRate * wfParam.txPulseLength << std::endl;
                log()->error(details.str());
            }

            //2.8.4
            if (wfParam.rcvDemodType.toString() == "CHIRP" && wfParam.rcvFMRate != 0)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
                    << wfParam.rcvDemodType << std::endl
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvFMRate: "
                    << wfParam.rcvFMRate << std::endl;
                log()->error(details.str());
            }

            //2.8.5
            if (wfParam.rcvDemodType.toString() == "STRETCH" &&
                std::abs(wfParam.rcvFMRate / wfParam.txFMRate - 1) > WGT_TOL)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
                    << wfParam.rcvDemodType << std::endl
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvFMRate: "
                    << wfParam.rcvFMRate << std::endl
                    << "SICD>RadarCollection.Waveform.WFParameters.TxFMRate: "
                    << wfParam.txFMRate << std::endl;
                log()->error(details.str());
            }

            //2.8.6
            //Absolute frequencies must be positive
            if (six::Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) && sicd.radarCollection->txFrequencyMin <= 0)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.txFrequencyMin: "
                    << sicd.radarCollection->txFrequencyMin << std::endl;
                log()->error(details.str());
            }

            //2.8.7
            //Absolute frequencies must be positive
            if (six::Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) && wfParam.txFrequencyStart <= 0)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.txFreqStart: "
                    << wfParam.txFrequencyStart << std::endl;
                log()->error(details.str());
            }

            //2.8.8
            //Absolute frequencies must be positive
            if (six::Init::isUndefined<int>(sicd.radarCollection->refFrequencyIndex) && wfParam.rcvFrequencyStart <= 0)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.rcvFreqStart: "
                    << wfParam.rcvFrequencyStart << std::endl;
                log()->error(details.str());
            }

            //2.8.9
            if (wfParam.txPulseLength > wfParam.rcvWindowLength)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.TxPulseLength: "
                    << wfParam.txPulseLength << std::endl
                    << "SICD.RadarCollection.Waveform.WFPArameters.RcvWindowLength: "
                    << wfParam.rcvWindowLength << std::endl;
                log()->error(details.str());
            }

            //2.8.10
            if (wfParam.rcvIFBandwidth > wfParam.adcSampleRate)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvIFBandwidth: "
                    << wfParam.rcvIFBandwidth << std::endl
                    << "SICD.RadarCollection.Waveform.WFPArameters.ADCSampleRate: "
                    << wfParam.adcSampleRate << std::endl;
                log()->error(details.str());
            }

            //2.8.11
            if (wfParam.rcvDemodType.toString() == "CHIRP" && wfParam.txRFBandwidth > wfParam.adcSampleRate)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
                    << wfParam.rcvDemodType.toString() << std::endl
                    << "SICD.RadarCollection.Waveform.WFParameters.TxRFBandwidth: "
                    << wfParam.txRFBandwidth << std::endl
                    << "SICD.RadarCollection.Waveform.WFPArameters.ADCSampleRate: "
                    << wfParam.adcSampleRate << std::endl;
                log()->error(details.str());
            }

            //2.8.12
            double freq_tol = (wfParam.rcvWindowLength - wfParam.txPulseLength) * wfParam.txFMRate;
            if (wfParam.rcvFrequencyStart >= (wfParam.txFrequencyStart + wfParam.txRFBandwidth + freq_tol) ||
                wfParam.rcvFrequencyStart <= wfParam.txFrequencyStart - freq_tol)
            {
                details.str("");
                details << WF_INCONSISTENT_STR
                    << "SICD.RadarCollection.Waveform.WFParameters.RcvFreqStart: "
                    << wfParam.rcvFrequencyStart << std::endl;
                log()->error(details.str());
            }
        }
    }

    // Skipping 2.9

    // 2.10 GeoData.SCP
    const double SCPCOA_TOL = 1e-2;
    if (sicd.geoData.get() != NULL)
    {
        SCP scp = sicd.geoData->scp;
        scene::LLAToECEFTransform transformer;
        Vector3 ecf2 = transformer.transform(scp.llh);
        double ecf_diff = (scp.ecf - ecf2).norm();

        if (ecf_diff > SCPCOA_TOL)
        {
            details.str("");
            details << "GeoData.SCP.ECF and GeoData.SCP.LLH not consistent.\n"
                << "SICD.GeoData.SCP.ECF - SICD.GeoData.SCP.LLH: "
                << ecf_diff << " (m)" << std::endl;
            log()->error(details.str());
        }
    }

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
    if (imageValidData && !geoValidData)
    {
        details.str("");
        details << "ImageData.ValidData/GeoData.ValidData required together.\n"
            << "ImageData.ValidData exists, but GeoData.ValidData does not.";
        log()->error(details.str());
    }
    if (!imageValidData && geoValidData)
    {
        details.str("");
        details << "ImageData.ValidData/GeoData.ValidData required together.\n"
            << "GeoData.ValidData exists, but ImageData.ValidData does not.";
        log()->error(details.str());
    }
    /*
        * As with the corner coordinates, we don't check for precision in the agreement
        * between IMageData.ValidData and GeoData.ValidData here, sinc the spec allows
        * from approximate Lat/Lons, and since it's not possible to know the HAE used
        * to compute Lat/Lon for each corner. One could potentially use the SICD sensor
        * model to check that the image coordinates and the resulting ground coordinates
        * from projection to a flat plane (or constant HAE) roughly agreed with GeoData.
        * ValidData to within a generous tolerance.
        */

        // 2.12 IFP-specific checks
    if (sicd.geoData.get() != NULL && sicd.grid.get() != NULL && sicd.grid->row.get() != NULL && sicd.grid->col.get() != NULL && sicd.imageFormation.get() != NULL)
    {
        Vector3 scp = sicd.geoData->scp.ecf;
        Vector3 rowUnitVector = sicd.grid->row->unitVector;
        Vector3 colUnitVector = sicd.grid->col->unitVector;
        const double UVECT_TOL = 1e-3;
        const double IFP_POLY_TOL = 1e-5;

        ImageFormationType formType = sicd.imageFormation->imageFormationAlgorithm;
        if (formType.toString() == "RGAZCOMP")
        {
            // 2.12.1.1
            if (sicd.grid->imagePlane.toString() != "SLANT")
            {
                details.str("");
                details << "RGAZCOMP image formation should result in a SLANT plane image.\n"
                    << "Grid.ImagePlane: " << sicd.grid->imagePlane.toString();
                log()->error(details.str());
            }

            //2.12.1.2
            if (sicd.grid->type.toString() != "RGAZIM")
            {
                details.str("");
                details << "RGAZCOMP image formation should result in a RGAZIM grid.\n"
                    << "Grid.Type: " << sicd.grid->type.toString();
                log()->error(details.str());
            }
            if (sicd.scpcoa.get() != NULL)
            {
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
                    details.str("");
                    details << "RGAZCOMP image formation declared, but no RgAzComp metadata given.\n"
                        << "No RgAzcomp filed in this SICD";
                    log()->error(details.str());
                }
                else
                {
                    //2.12.1.4
                    double derivedAzSF = -look * std::sin(M_PI / 180 * sicd.scpcoa->dopplerConeAngle) / sicd.scpcoa->slantRange;
                    if (std::abs(sicd.rgAzComp->azSF - derivedAzSF) > 1e-6)
                    {
                        details.str("");
                        details << "RGAZCOMP fields inconsistent.\n"
                            << "RgAzComp.AzSF: " << sicd.rgAzComp->azSF << std::endl
                            << "Derived RgAzComp.AzSF: " << derivedAzSF;
                        log()->error(details.str());
                    }
                    //2.12.1.5 omitted
                }
                //2.12.1.6
                Vector3 drvect = unitRG;
                if ((drvect - rowUnitVector).norm() > UVECT_TOL)
                {
                    details.str("");
                    details << "UVect fields inconsistent.\n"
                        << "Grid.Row.UVectECEF: " << rowUnitVector << std::endl
                        << "Derived Grid.Row.UVectECEF: " << drvect;
                    log()->error(details.str());
                }

                //2.12.1.7
                Vector3 dcvect = cross(spn, unitRG);
                if ((dcvect - colUnitVector).norm() > UVECT_TOL)
                {
                    details.str("");
                    details << "UVectr fields inconsistent.\n"
                        << "Grid.Col.UVectECF: " << colUnitVector << std::endl
                        << "Derived Grid.Col.UVectECF: " << dcvect;
                    log()->error(details.str());
                }

                //2.12.1.8
                if (!Init::isUndefined(sicd.grid->col->deltaKCOAPoly) &&
                    sicd.grid->col->deltaKCOAPoly.orderX() == 1 && sicd.grid->col->deltaKCOAPoly.orderY() == 1
                    && std::abs(sicd.grid->col->kCenter - (-sicd.grid->col->deltaKCOAPoly[0][0])) > std::numeric_limits<double>::epsilon())
                {
                    details.str("");
                    details << "Grid.Col.KCenter must be equal to -Grid.Col.DeltaKCOAPoly for RGAZCOMP data.\n"
                        << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << std::endl
                        << "Grid.Col.DeltaKCOAPoly: " << sicd.grid->col->deltaKCOAPoly[0][0];
                    log()->error(details.str());
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
                                    details.str("");
                                    details << WF_INCONSISTENT_STR
                                        << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << std::endl
                                        << "Center frequency * 2/c - Grid.Row.DeltaKCOAPoly: "
                                        << kfc - poly[ii][jj];
                                    log()->error(details.str());
                                    break;
                                }
                            }
                        }
                    }
                    else if (std::abs(sicd.grid->row->kCenter - kfc) > std::numeric_limits<double>::epsilon())
                    {
                        details.str("");
                        details << WF_INCONSISTENT_STR
                            << "Grid.Col.KCenter: " << sicd.grid->col->kCenter << std::endl
                            << "Center frequency * 2/c: " << kfc;
                        log()->error(details.str());
                    }
                }

                //2.12.1.10
                if (!Init::isUndefined(sicd.grid->col->deltaKCOAPoly) && sicd.grid->col->deltaKCOAPoly.orderX() > 1)
                {
                    details.str("");
                    details << "Grid.Col.DetlaKCOAPoly must be a single value for RGAZCOMP data";
                    log()->error(details.str());
                }

                //2.12.1.11
                if (!Init::isUndefined(sicd.grid->row->deltaKCOAPoly) && sicd.grid->row->deltaKCOAPoly.orderX() > 1)
                {
                    details.str("");
                    details << "Grid.Row.DetlaKCOAPoly must be a single value for RGAZCOMP data";
                    log()->error(details.str());
                }
            }
        }
        else if (formType.toString() == "PFA")
        {
            //2.12.2.1
            if (sicd.grid->type.toString() != "RGAZIM")
            {
                details.str("");
                details << "PFA image formation should result in a RGAZIM grid\n"
                    << "Grid.Type: " << sicd.grid->type.toString();
                log()->error(details.str());
            }

            //2.12.2.2
            if (sicd.pfa.get() == NULL)
            {
                details.str("");
                details << "PFA image formation declared, but no PFA metadata given.\n"
                    << "No PFA field in this SICD";
                log()->error(details.str());
            }

            else
            {
                //2.12.2.3
                if (sicd.scpcoa.get() == NULL || sicd.pfa->polarAngleRefTime - sicd.scpcoa->scpTime > std::numeric_limits<double>::epsilon())
                {
                    details.str("");
                    details << "Polar angle reference time and center of aperture time for center are usuallly the same.\n"
                        << "PFA.PolarAngRefTime: " << sicd.pfa->polarAngleRefTime << std::endl
                        << "SCPCOA.SCPTime: " << (sicd.scpcoa.get() == NULL ? "NULL" : str::toString(sicd.scpcoa->scpTime));
                    log()->warn(details.str());
                }

                // Skipping to 2.12.2.10
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
                            details.str("");
                            details << WF_INCONSISTENT_STR
                                << "Grid.Row.KCtr: " << sicd.grid->row->kCenter << std::endl
                                << "Derived KapCtr: " << kapCtr;
                            log()->error(details.str());
                        }
                    }
                }
            }
        }
        else if (formType.toString() == "RMA")
        {
            //2.12.3.1
            if (sicd.rma.get() == NULL)
            {
                details.str("");
                details << "RMA image formation declared, but no RMA metadat given.\n"
                    << "No RMA field in this SICD.";
                log()->error(details.str());
            }
            else if (sicd.rma->algoType.toString() == "RMAT")
            {
                // 2.12.3.2.1
                if (sicd.grid->type.toString() != "XCTYAT")
                {
                    details.str("");
                    details << "RMA/RMAT image formation should result in a XCTYAT grid.\n"
                        << "RMA.ImageType: RMAT, Grid.Type: " << sicd.grid->type.toString();
                    log()->error(details.str());
                }

                // 2.12.3.2.2
                if (sicd.rma->rmat.get() == NULL)
                {
                    details.str("");
                    details << "RMA/RMAT image formation declared, but no RMAT metadata given.\n"
                        << "No RMAT field in this SICD.";
                    log()->error(details.str());
                }
                else
                {
                    //scp defined above. Given again here as reminder
                    //scp = sicd.geoData->scp.ecf;
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
                        details.str("");
                        details << "UVect fields inconsistent.\n"
                            << "Grid.Row.UVectECF: " << rowUnitVector
                            << "Derived grid.Row.UVectECT: " << uXCT;
                        log()->error(details.str());
                    }

                    // 2.12.3.2.4
                    if ((colUnitVector - uYAT).norm() > UVECT_TOL)
                    {
                        details.str("");
                        details << "UVect fields inconsistent.\n"
                            << "Grid.Col.UVectECF: " << colUnitVector
                            << "Derived Grid.Col.UVectECF: " << uYAT;
                        log()->error(details.str());
                    }

                    // 2.12.3.2.5
                    if (std::abs(dcaRef - sicd.rma->rmat->dopConeAngleRef) > 1e-6)
                    {
                        details.str("");
                        details << "RMA fields inconsistent.\n"
                            << "RMA.RMAT.DopConeAngleRef: " << sicd.rma->rmat->dopConeAngleRef
                            << std::endl << "Derived RMA.RMAT.DopConeAngleRef: " << dcaRef;
                        log()->error(details.str());
                    }
                }
                if (sicd.radarCollection.get() == NULL || Init::isUndefined(sicd.radarCollection->refFrequencyIndex))
                {
                    double fcProc = (sicd.imageFormation->txFrequencyProcMin + sicd.imageFormation->txFrequencyProcMax) / 2;
                    double kfc = fcProc * (2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC);

                    // 2.12.3.2.6
                    if ((kfc * std::sin(sicd.rma->rmat->dopConeAngleRef * math::Constants::RADIANS_TO_DEGREES) / sicd.grid->row->kCenter) - 1 > WF_TOL)
                    {
                        details.str("");
                        details << WF_INCONSISTENT_STR
                            << "Grid.Row.KCtr: " << sicd.grid->row->kCenter << std::endl
                            << "Derived Grid.Row.KCtr: " << kfc * std::sin(sicd.rma->rmat->dopConeAngleRef * math::Constants::RADIANS_TO_DEGREES);
                        log()->warn(details.str());
                    }

                    // 2.12.3.2.7
                    if ((kfc * std::cos(sicd.rma->rmat->dopConeAngleRef * math::Constants::DEGREES_TO_RADIANS) / sicd.grid->col->kCenter) - 1 > WF_TOL)
                    {
                        details.str("");
                        details << WF_INCONSISTENT_STR
                            << "Grid.Col.KCtr: " << sicd.grid->col->kCenter << std::endl
                            << "Derived Grid.Col.KCtr: " << kfc * std::cos(sicd.rma->rmat->dopConeAngleRef * math::Constants::DEGREES_TO_RADIANS);
                        log()->warn(details.str());
                    }
                }
            }
            else if (sicd.rma->algoType.toString() == "RMCR")
            {
                // 2.12.3.3.1
                if (sicd.grid->type.toString() != "XRGYCR")
                {
                    details.str("");
                    details << "RMA/RMCR image formation should result in a XRGYCR grid.\n"
                        << "RMA.ImageType: RMCR, Grid.Type: " << sicd.grid->type.toString();
                    log()->error(details.str());
                }

                // 2.12.3.3.2
                if (sicd.rma->rmcr.get() == NULL)
                {
                    details.str("");
                    details << "RMA/RMCR image formation declared, but no RMCR metadata given.\n"
                        << "No RMCR field in this SICD.";
                    log()->error(details.str());
                }
                else
                {
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
                        details.str("");
                        details << "UVect fields inconsistent.\n"
                            << "Grid.Row.UVectECF: " << rowUnitVector << std::endl
                            << "Derived Grid.Row.UVectECF: " << uXRG;
                        log()->error(details.str());
                    }

                    // 2.12.3.3.4
                    if ((colUnitVector - uYCR).norm() > UVECT_TOL)
                    {
                        details.str("");
                        details << "UVect fields inconsistent.\n"
                            << "Grid.Col.UVectECF: " << colUnitVector << std::endl
                            << "Derived Grid.Col.UVectECF: " << uYCR;
                        log()->error(details.str());
                    }

                    // 2.12.3.3.5
                    if (std::abs(dcaRef - sicd.rma->rmcr->dopConeAngleRef) > 1e-6)
                    {
                        details.str("");
                        details << "RMA fields inconsistent.\n"
                            << "RMA.RMCR.DopConeAngleRef: " << sicd.rma->rmcr->dopConeAngleRef << std::endl
                            << "Derived RMA.RMCR.DopConeAngleRef: " << dcaRef;
                        log()->error(details.str());
                    }
                }

                // 2.12.3.3.6
                if (sicd.grid->col->kCenter != 0)
                {
                    details.str("");
                    details << "Grid.Col.KCtr must be zero for RMA/RMCR data.\n"
                        << "Grid.Col.KCtr = " << sicd.grid->col->kCenter;
                    log()->error(details.str());
                }

                // 2.12.3.3.7
                if (sicd.radarCollection.get() == NULL || Init::isUndefined(sicd.radarCollection->refFrequencyIndex))
                {
                    double fcProc = (sicd.imageFormation->txFrequencyProcMin + sicd.imageFormation->txFrequencyProcMax) / 2;
                    double kfc = fcProc * (2 / math::Constants::SPEED_OF_LIGHT_METERS_PER_SEC);
                    if ((sicd.grid->row->kCenter / kfc) - 1 > WF_TOL)
                    {
                        details.str("");
                        details << WF_INCONSISTENT_STR << std::endl
                            << "Grid.Row.KCtr: " << sicd.grid->row->kCenter << std::endl
                            << "Center frequency * 2/c: " << kfc;
                        log()->warn(details.str());
                    }
                }
            }
            else if (sicd.rma->algoType.toString() == "INCA")
            {
                // 2.12.3.4.1
                if (sicd.grid->type.toString() != "RGZERO")
                {
                    details.str("");
                    details << "RMA/INCA image formation should result in a RGZERO grid.\n"
                        << "Grid.Type: " << sicd.grid->type.toString();
                    log()->error(details.str());
                }

                // 2.12.3.4.2
                if (sicd.rma->inca.get() == NULL)
                {
                    details.str("");
                    details << "RMA/INCA image formation declared, but no INCA metadata given.\n"
                        << "No INCA field in this SICD.";
                    log()->error(details.str());
                }
                else
                {
                    //2.12.3.4.3
                    if (sicd.collectionInformation.get() != NULL)
                    {
                        if (sicd.collectionInformation->radarMode.toString() == "SPOTLIGHT" &&
                            (!Init::isUndefined(sicd.rma->inca->dopplerCentroidPoly) ||
                                !Init::isUndefined(sicd.rma->inca->dopplerCentroidCOA)))
                        {
                            details.str("");
                            details << "RMA.INCA fields inconsistent.\n"
                                << "RMA.INCA.DopplerCentroidPoly/DopplerCentroidCOA not used for SPOTLIGHT collection.";
                            log()->error(details.str());
                        }
                        //2.12.3.4.4
                        else if (sicd.collectionInformation->radarMode.toString() != "SPOTLIGHT")
                        {
                            if (Init::isUndefined(sicd.rma->inca->dopplerCentroidPoly) || Init::isUndefined(sicd.rma->inca->dopplerCentroidCOA))
                            {
                                details.str("");
                                details << "RMA.INCA fields inconsistent.\n"
                                    << "RMA.INCA.DopplerCentroidPoly/COA required for non-SPOTLIGHT collection.";
                                log()->error(details.str());
                            }

                            // Skipping the rest of 2.12.3.4
                        }
                    }
                }
            }
        }
        else if (formType.toString() == "OTHER")
        {
            //2.12.3
            details.str("");
            details << "Image formation not fully defined.\n"
                << "SICD.ImageFormation.ImageFormAlgo = OTHER.";
            log()->warn(details.str());
        }
    }
}

ComplexData* ComplexXMLParser::fromXML(const xml::lite::Document* doc) const
{
    ComplexDataBuilder builder;
    ComplexData *sicd = builder.steal();

    XMLElem root = doc->getRootElement();

    XMLElem collectionInfoXML  = getFirstAndOnly(root, "CollectionInfo");
    XMLElem imageCreationXML   = getOptional(root, "ImageCreation");
    XMLElem imageDataXML       = getFirstAndOnly(root, "ImageData");
    XMLElem geoDataXML         = getFirstAndOnly(root, "GeoData");
    XMLElem gridXML            = getFirstAndOnly(root, "Grid");
    XMLElem timelineXML        = getFirstAndOnly(root, "Timeline");
    XMLElem positionXML        = getFirstAndOnly(root, "Position");
    XMLElem radarCollectionXML = getFirstAndOnly(root, "RadarCollection");
    XMLElem imageFormationXML  = getFirstAndOnly(root, "ImageFormation");
    XMLElem scpcoaXML          = getFirstAndOnly(root, "SCPCOA");
    XMLElem radiometricXML     = getOptional(root, "Radiometric");
    XMLElem antennaXML         = getOptional(root, "Antenna");
    XMLElem errorStatisticsXML = getOptional(root, "ErrorStatistics");
    XMLElem matchInfoXML       = getOptional(root, "MatchInfo");
    XMLElem pfaXML             = getOptional(root, "PFA");
    XMLElem rmaXML             = getOptional(root, "RMA");
    XMLElem rgAzCompXML        = getOptional(root, "RGAZCOMP");
    if (rgAzCompXML != NULL)
    {
        // In 0.5, the element was in all caps and contained additional
        // elements that disappeared in 1.0.  For the time being at least,
        // don't support this.
        throw except::Exception(Ctxt(
                "SIX library does not support RGAZCOMP element"));
    }
    rgAzCompXML                = getOptional(root, "RgAzComp"); // added in 1.0.0

    parseCollectionInformationFromXML(collectionInfoXML, sicd->collectionInformation.get());

    if (imageCreationXML != NULL)
    {
        builder.addImageCreation();
        parseImageCreationFromXML(imageCreationXML, sicd->imageCreation.get());
    }

    parseImageDataFromXML(imageDataXML, sicd->imageData.get());
    parseGeoDataFromXML(geoDataXML, sicd->geoData.get());
    parseGridFromXML(gridXML, sicd->grid.get());
    parseTimelineFromXML(timelineXML, sicd->timeline.get());
    parsePositionFromXML(positionXML, sicd->position.get());
    parseRadarCollectionFromXML(radarCollectionXML, sicd->radarCollection.get());
    parseImageFormationFromXML(imageFormationXML, sicd->imageFormation.get());
    parseSCPCOAFromXML(scpcoaXML, sicd->scpcoa.get());

    if (radiometricXML != NULL)
    {
        builder.addRadiometric();
        common().parseRadiometryFromXML(radiometricXML, 
                                       sicd->radiometric.get());
    }

    if (antennaXML != NULL)
    {
        builder.addAntenna();
        parseAntennaFromXML(antennaXML, sicd->antenna.get());
    }

    if (errorStatisticsXML != NULL)
    {
        builder.addErrorStatistics();
        common().parseErrorStatisticsFromXML(errorStatisticsXML, 
                                            sicd->errorStatistics.get());
    }

    if (matchInfoXML != NULL)
    {
        builder.addMatchInformation();
        parseMatchInformationFromXML(matchInfoXML, sicd->matchInformation.get());
    }

    if (pfaXML != NULL)
    {
        sicd->pfa.reset(new PFA());
        parsePFAFromXML(pfaXML, sicd->pfa.get());
    }
    if (rmaXML != NULL)
    {
        sicd->rma.reset(new RMA());
        parseRMAFromXML(rmaXML, sicd->rma.get());
    }
    if (rgAzCompXML != NULL)
    {
        sicd->rgAzComp.reset(new RgAzComp());
        parseRgAzCompFromXML(rgAzCompXML, sicd->rgAzComp.get());
    }
    validate(*sicd);
    return sicd;
}

xml::lite::Document* ComplexXMLParser::toXML(const ComplexData* sicd) const
{
    xml::lite::Document* doc = new xml::lite::Document();

    XMLElem root = newElement("SICD");
    doc->setRootElement(root);

    convertCollectionInformationToXML(sicd->collectionInformation.get(), root);
    if (sicd->imageCreation.get())
    {
        convertImageCreationToXML(sicd->imageCreation.get(), root);
    }
    convertImageDataToXML(sicd->imageData.get(), root);
    convertGeoDataToXML(sicd->geoData.get(), root);
    convertGridToXML(sicd->grid.get(), root);
    convertTimelineToXML(sicd->timeline.get(), root);
    convertPositionToXML(sicd->position.get(), root);
    convertRadarCollectionToXML(sicd->radarCollection.get(), root);
    convertImageFormationToXML(sicd->imageFormation.get(), root);
    convertSCPCOAToXML(sicd->scpcoa.get(), root);
    if (sicd->radiometric.get())
    {
        common().convertRadiometryToXML(sicd->radiometric.get(), root);
    }
    if (sicd->antenna.get())
    {
        convertAntennaToXML(sicd->antenna.get(), root);
    }
    if (sicd->errorStatistics.get())
    {
        common().convertErrorStatisticsToXML(sicd->errorStatistics.get(), root);
    }
    if (sicd->matchInformation.get() && !sicd->matchInformation->types.empty())
        convertMatchInformationToXML(sicd->matchInformation.get(), root);

    // parse the choice per version
    convertImageFormationAlgoToXML(sicd->pfa.get(), sicd->rma.get(), sicd->rgAzComp.get(), root);

    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());
    //        root->setNamespacePrefix("si", common().getSICommonURI());

    return doc;
}

XMLElem ComplexXMLParser::createFFTSign(const std::string& name, six::FFTSign sign,
                                         XMLElem parent) const
{
    return createInt(name, getDefaultURI(), 
                     (sign == FFTSign::NEG) ? "-1" : "+1", parent);
}

XMLElem ComplexXMLParser::convertCollectionInformationToXML(
    const CollectionInformation *collInfo,
    XMLElem parent) const
{
    XMLElem collInfoXML = newElement("CollectionInfo", parent);

    const std::string si = common().getSICommonURI();

    createString("CollectorName", si, collInfo->collectorName, collInfoXML);
    if (!collInfo->illuminatorName.empty())
        createString("IlluminatorName", si, collInfo->illuminatorName,
                     collInfoXML);
    createString("CoreName", si, collInfo->coreName, collInfoXML);
    if (!Init::isUndefined<CollectType>(collInfo->collectType))
        createString("CollectType", si,
                     six::toString<six::CollectType>(collInfo->collectType),
                     collInfoXML);

    XMLElem radarModeXML = newElement("RadarMode", si, collInfoXML);
    createString("ModeType", si, six::toString(collInfo->radarMode),
                 radarModeXML);
    if (!collInfo->radarModeID.empty())
        createString("ModeID", si, collInfo->radarModeID, radarModeXML);

    //TODO maybe add more class. info in the future
    createString("Classification", si, collInfo->classification.level,
                 collInfoXML);

    for (std::vector<std::string>::const_iterator it =
            collInfo->countryCodes.begin(); it != collInfo->countryCodes.end(); ++it)
    {
        createString("CountryCode", si, *it, collInfoXML);
    }
    common().addParameters("Parameter", si, collInfo->parameters, collInfoXML);
    return collInfoXML;
}

XMLElem ComplexXMLParser::convertImageCreationToXML(
    const ImageCreation *imageCreation,
    XMLElem parent) const
{
    XMLElem imageCreationXML = newElement("ImageCreation", parent);

    const std::string si = common().getSICommonURI();

    if (!imageCreation->application.empty())
        createString("Application", si, imageCreation->application,
                     imageCreationXML);
    if (!Init::isUndefined<DateTime>(imageCreation->dateTime))
        createDateTime("DateTime", si, imageCreation->dateTime,
                       imageCreationXML);
    if (!imageCreation->site.empty())
        createString("Site", si, imageCreation->site, imageCreationXML);
    if (!imageCreation->profile.empty())
        createString("Profile", si, imageCreation->profile, imageCreationXML);
    return imageCreationXML;
}

XMLElem ComplexXMLParser::convertImageDataToXML(
    const ImageData *imageData, XMLElem parent) const
{
    XMLElem imageDataXML = newElement("ImageData", parent);

    createString("PixelType", six::toString(imageData->pixelType), imageDataXML);
    if (imageData->amplitudeTable)
    {
        AmplitudeTable& ampTable = *(imageData->amplitudeTable);
        XMLElem ampTableXML = newElement("AmpTable", imageDataXML);
        setAttribute(ampTableXML, "size", str::toString(ampTable.numEntries));
        for (unsigned int i = 0; i < ampTable.numEntries; ++i)
        {
            XMLElem ampXML = createDouble("Amplitude", *(double*) ampTable[i],
                                          ampTableXML);
            setAttribute(ampXML, "index", str::toString(i));
        }
    }
    createInt("NumRows", imageData->numRows, imageDataXML);
    createInt("NumCols", imageData->numCols, imageDataXML);
    createInt("FirstRow", imageData->firstRow, imageDataXML);
    createInt("FirstCol", imageData->firstCol, imageDataXML);

    common().createRowCol("FullImage", "NumRows", "NumCols", imageData->fullImage,
                 imageDataXML);
    common().createRowCol("SCPPixel", imageData->scpPixel, imageDataXML);

    //only if 3+ vertices
    const size_t numVertices = imageData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", imageDataXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML = common().createRowCol("Vertex", imageData->validData[ii],
                                                      vXML);
            setAttribute(vertexXML, "index", str::toString(ii + 1));
        }
    }
    return imageDataXML;
}

XMLElem ComplexXMLParser::convertGeoDataToXML(
    const GeoData *geoData, XMLElem parent) const
{
    XMLElem geoDataXML = newElement("GeoData", parent);

    createEarthModelType("EarthModel", geoData->earthModel, geoDataXML);

    XMLElem scpXML = newElement("SCP", geoDataXML);
    common().createVector3D("ECF", geoData->scp.ecf, scpXML);
    common().createLatLonAlt("LLH", geoData->scp.llh, scpXML);

    createLatLonFootprint("ImageCorners", "ICP", geoData->imageCorners, geoDataXML);

    //only if 3+ vertices
    const size_t numVertices = geoData->validData.size();
    if (numVertices >= 3)
    {
        XMLElem vXML = newElement("ValidData", geoDataXML);
        setAttribute(vXML, "size", str::toString(numVertices));

        for (size_t ii = 0; ii < numVertices; ++ii)
        {
            XMLElem vertexXML = common().createLatLon("Vertex", geoData->validData[ii],
                                                      vXML);
            setAttribute(vertexXML, "index", str::toString(ii + 1));
        }
    }

    for (size_t ii = 0; ii < geoData->geoInfos.size(); ++ii)
    {
        convertGeoInfoToXML(geoData->geoInfos[ii].get(), geoDataXML);
    }

    return geoDataXML;
}

XMLElem ComplexXMLParser::convertGridToXML(
    const Grid *grid, XMLElem parent) const
{
    XMLElem gridXML = newElement("Grid", parent);

    createString("ImagePlane", six::toString(grid->imagePlane), gridXML);
    createString("Type", six::toString(grid->type), gridXML);
    common().createPoly2D("TimeCOAPoly", grid->timeCOAPoly, gridXML);

    XMLElem rowDirXML = newElement("Row", gridXML);

    common().createVector3D("UVectECF", grid->row->unitVector, rowDirXML);
    createDouble("SS", grid->row->sampleSpacing, rowDirXML);
    createDouble("ImpRespWid", grid->row->impulseResponseWidth, rowDirXML);
    createFFTSign("Sgn", grid->row->sign, rowDirXML);
    createDouble("ImpRespBW", grid->row->impulseResponseBandwidth, rowDirXML);
    createDouble("KCtr", grid->row->kCenter, rowDirXML);
    createDouble("DeltaK1", grid->row->deltaK1, rowDirXML);
    createDouble("DeltaK2", grid->row->deltaK2, rowDirXML);

    if (!Init::isUndefined<Poly2D>(grid->row->deltaKCOAPoly))
    {
        common().createPoly2D("DeltaKCOAPoly", grid->row->deltaKCOAPoly, rowDirXML);
    }

    if (grid->row->weightType.get())
    {
        convertWeightTypeToXML(*grid->row->weightType, rowDirXML);
    }

    size_t numWeights = grid->row->weights.size();
    if (numWeights > 0)
    {
        XMLElem wgtFuncXML = newElement("WgtFunct", rowDirXML);
        setAttribute(wgtFuncXML, "size", str::toString(numWeights));

        for (size_t i = 1; i <= numWeights; ++i)
        {
            XMLElem wgtXML = createDouble("Wgt", grid->row->weights[i - 1],
                                          wgtFuncXML);
            setAttribute(wgtXML, "index", str::toString(i));
        }
    }

    XMLElem colDirXML = newElement("Col", gridXML);

    common().createVector3D("UVectECF", grid->col->unitVector, colDirXML);
    createDouble("SS", grid->col->sampleSpacing, colDirXML);
    createDouble("ImpRespWid", grid->col->impulseResponseWidth, colDirXML);
    createFFTSign("Sgn", grid->col->sign, colDirXML);
    createDouble("ImpRespBW", grid->col->impulseResponseBandwidth, colDirXML);
    createDouble("KCtr", grid->col->kCenter, colDirXML);
    createDouble("DeltaK1", grid->col->deltaK1, colDirXML);
    createDouble("DeltaK2", grid->col->deltaK2, colDirXML);

    if (!Init::isUndefined<Poly2D>(grid->col->deltaKCOAPoly))
    {
        common().createPoly2D("DeltaKCOAPoly", grid->col->deltaKCOAPoly, colDirXML);
    }

    if (grid->col->weightType.get())
    {
        convertWeightTypeToXML(*grid->col->weightType, colDirXML);
    }

    numWeights = grid->col->weights.size();
    if (numWeights > 0)
    {
        XMLElem wgtFuncXML = newElement("WgtFunct", colDirXML);
        setAttribute(wgtFuncXML, "size", str::toString(numWeights));

        for (size_t i = 1; i <= numWeights; ++i)
        {
            XMLElem wgtXML = createDouble("Wgt", grid->col->weights[i - 1],
                                          wgtFuncXML);
            setAttribute(wgtXML, "index", str::toString(i));
        }
    }

    return gridXML;
}

XMLElem ComplexXMLParser::convertTimelineToXML(
    const Timeline *timeline, XMLElem parent) const
{
    XMLElem timelineXML = newElement("Timeline", parent);

    createDateTime("CollectStart", timeline->collectStart, timelineXML);
    createDouble("CollectDuration", timeline->collectDuration, timelineXML);

    if (timeline->interPulsePeriod.get())
    {
        XMLElem ippXML = newElement("IPP", timelineXML);
        size_t setSize = timeline->interPulsePeriod->sets.size();
        ippXML->attribute("size") = str::toString<size_t>(setSize);

        for (size_t i = 0; i < setSize; ++i)
        {
            const TimelineSet& timelineSet = timeline->interPulsePeriod->sets[i];
            XMLElem setXML = newElement("Set", ippXML);
            setXML->attribute("index") = str::toString<int>(i + 1);

            createDouble("TStart", timelineSet.tStart, setXML);
            createDouble("TEnd", timelineSet.tEnd, setXML);
            createInt("IPPStart", timelineSet.interPulsePeriodStart, setXML);
            createInt("IPPEnd", timelineSet.interPulsePeriodEnd, setXML);
            common().createPoly1D("IPPPoly", timelineSet.interPulsePeriodPoly, setXML);
        }
    }

    return timelineXML;
}

XMLElem ComplexXMLParser::convertPositionToXML(
    const Position *position, XMLElem parent) const
{
    XMLElem positionXML = newElement("Position", parent);

    common().createPolyXYZ("ARPPoly", position->arpPoly, positionXML);
    if (!Init::isUndefined<PolyXYZ>(position->grpPoly))
        common().createPolyXYZ("GRPPoly", position->grpPoly, positionXML);
    if (!Init::isUndefined<PolyXYZ>(position->txAPCPoly))
        common().createPolyXYZ("TxAPCPoly", position->txAPCPoly, positionXML);
    if (position->rcvAPC.get() && !position->rcvAPC->rcvAPCPolys.empty())
    {
        size_t numPolys = position->rcvAPC->rcvAPCPolys.size();
        XMLElem rcvXML = newElement("RcvAPC", positionXML);
        setAttribute(rcvXML, "size", str::toString(numPolys));

        for (size_t i = 0; i < numPolys; ++i)
        {
            PolyXYZ xyz = position->rcvAPC->rcvAPCPolys[i];
            XMLElem xyzXML = common().createPolyXYZ("RcvAPCPoly", xyz, rcvXML);
            setAttribute(xyzXML, "index", str::toString(i + 1));
        }
    }
    return positionXML;
}

XMLElem ComplexXMLParser::createTxFrequency(const RadarCollection* radar,
                                            XMLElem parent) const
{
    XMLElem txFreqXML = newElement("TxFrequency", parent);
    createDouble("Min", radar->txFrequencyMin, txFreqXML);
    createDouble("Max", radar->txFrequencyMax, txFreqXML);
    return txFreqXML;
}

XMLElem ComplexXMLParser::createTxSequence(const RadarCollection* radar,
                                           XMLElem parent) const
{
    if (radar->txSequence.empty())
    {
        return NULL;
    }
    else
    {
        XMLElem txSeqXML = newElement("TxSequence", parent);
        setAttribute(txSeqXML, "size", str::toString(radar->txSequence.size()));

        for (size_t ii = 0; ii < radar->txSequence.size(); ++ii)
        {
            const TxStep* const tx = radar->txSequence[ii].get();

            XMLElem txStepXML = newElement("TxStep", txSeqXML);
            setAttribute(txStepXML, "index", str::toString(ii + 1));

            if (!Init::isUndefined<int>(tx->waveformIndex))
            {
                createInt("WFIndex", tx->waveformIndex, txStepXML);
            }
            if (tx->txPolarization != PolarizationType::NOT_SET)
            {
                createString("TxPolarization",
                             six::toString(tx->txPolarization), txStepXML);
            }
        }

        return txSeqXML;
    }
}

XMLElem ComplexXMLParser::createWaveform(const RadarCollection* radar,
                                         XMLElem parent) const
{
    if (radar->waveform.empty())
    {
        return NULL;
    }
    else
    {
        const size_t numWaveforms = radar->waveform.size();
        XMLElem wfXML = newElement("Waveform", parent);
        setAttribute(wfXML, "size", str::toString(numWaveforms));

        for (size_t ii = 0; ii < numWaveforms; ++ii)
        {
            const WaveformParameters* const wf = radar->waveform[ii].get();

            XMLElem wfpXML = newElement("WFParameters", wfXML);
            setAttribute(wfpXML, "index", str::toString(ii + 1));

            if (!Init::isUndefined<double>(wf->txPulseLength))
                createDouble("TxPulseLength", wf->txPulseLength, wfpXML);
            if (!Init::isUndefined<double>(wf->txRFBandwidth))
                createDouble("TxRFBandwidth", wf->txRFBandwidth, wfpXML);
            if (!Init::isUndefined<double>(wf->txFrequencyStart))
                createDouble("TxFreqStart", wf->txFrequencyStart, wfpXML);
            if (!Init::isUndefined<double>(wf->txFMRate))
                createDouble("TxFMRate", wf->txFMRate, wfpXML);
            if (wf->rcvDemodType != DemodType::NOT_SET)
                createString("RcvDemodType", six::toString(wf->rcvDemodType),
                             wfpXML);
            if (!Init::isUndefined<double>(wf->rcvWindowLength))
                createDouble("RcvWindowLength", wf->rcvWindowLength, wfpXML);
            if (!Init::isUndefined<double>(wf->adcSampleRate))
                createDouble("ADCSampleRate", wf->adcSampleRate, wfpXML);
            if (!Init::isUndefined<double>(wf->rcvIFBandwidth))
                createDouble("RcvIFBandwidth", wf->rcvIFBandwidth, wfpXML);
            if (!Init::isUndefined<double>(wf->rcvFrequencyStart))
                createDouble("RcvFreqStart", wf->rcvFrequencyStart, wfpXML);
            if (!Init::isUndefined<double>(wf->rcvFMRate))
                createDouble("RcvFMRate", wf->rcvFMRate, wfpXML);
        }

        return wfXML;
    }
}

XMLElem ComplexXMLParser::createArea(const RadarCollection* radar,
                                     XMLElem parent) const
{
    if (radar->area.get() == NULL)
    {
        return NULL;
    }
    else
    {
        XMLElem areaXML = newElement("Area", parent);
        const Area* const area = radar->area.get();

        bool haveACPCorners = true;

        for (size_t ii = 0; ii < LatLonAltCorners::NUM_CORNERS; ++ii)
        {
            if (Init::isUndefined<LatLonAlt>(area->acpCorners.getCorner(ii)))
            {
                haveACPCorners = false;
                break;
            }
        }

        if (haveACPCorners)
        {
            createLatLonAltFootprint("Corner", "ACP", area->acpCorners, areaXML);
        }

        const AreaPlane* const plane = area->plane.get();
        if (plane)
        {
            XMLElem planeXML = newElement("Plane", areaXML);
            XMLElem refPtXML = newElement("RefPt", planeXML);

            ReferencePoint refPt = plane->referencePoint;
            if (!refPt.name.empty())
                setAttribute(refPtXML, "name", refPt.name);

            common().createVector3D("ECF", refPt.ecef, refPtXML);
            createDouble("Line", refPt.rowCol.row, refPtXML);
            createDouble("Sample", refPt.rowCol.col, refPtXML);

            areaLineDirectionParametersToXML("XDir", plane->xDirection.get(),
                                             planeXML);
            areaSampleDirectionParametersToXML("YDir", plane->yDirection.get(),
                                               planeXML);

            if (!plane->segmentList.empty())
            {
                XMLElem segListXML = newElement("SegmentList", planeXML);
                setAttribute(segListXML, "size",
                             str::toString(plane->segmentList.size()));

                for (size_t ii = 0; ii < plane->segmentList.size(); ++ii)
                {
                    const Segment* const segment = plane->segmentList[ii].get();
                    XMLElem segXML = newElement("Segment", segListXML);
                    setAttribute(segXML, "index", str::toString(ii + 1));

                    createInt("StartLine", segment->startLine, segXML);
                    createInt("StartSample", segment->startSample, segXML);
                    createInt("EndLine", segment->endLine, segXML);
                    createInt("EndSample", segment->endSample, segXML);
                    createString("Identifier", segment->identifier, segXML);
                }
            }

            if (!Init::isUndefined<OrientationType>(plane->orientation))
            {
                createString("Orientation",
                             six::toString<OrientationType>(plane->orientation),
                             planeXML);
            }
        }

        return areaXML;
    }
}

XMLElem ComplexXMLParser::areaLineDirectionParametersToXML(
    const std::string& name,
    const AreaDirectionParameters *adp,
    XMLElem parent) const
{
    XMLElem adpXML = newElement(name, parent);
    common().createVector3D("UVectECF", adp->unitVector, adpXML);
    createDouble("LineSpacing", adp->spacing, adpXML);
    createInt("NumLines", adp->elements, adpXML);
    createInt("FirstLine", adp->first, adpXML);
    return adpXML;
}

XMLElem ComplexXMLParser::areaSampleDirectionParametersToXML(
    const std::string& name,
    const AreaDirectionParameters *adp,
    XMLElem parent) const
{
    XMLElem adpXML = newElement(name, parent);
    common().createVector3D("UVectECF", adp->unitVector, adpXML);
    createDouble("SampleSpacing", adp->spacing, adpXML);
    createInt("NumSamples", adp->elements, adpXML);
    createInt("FirstSample", adp->first, adpXML);
    return adpXML;
}

XMLElem ComplexXMLParser::convertSCPCOAToXML(
    const SCPCOA *scpcoa, 
    XMLElem parent) const
{
    XMLElem scpcoaXML = newElement("SCPCOA", parent);
    createDouble("SCPTime", scpcoa->scpTime, scpcoaXML);
    common().createVector3D("ARPPos", scpcoa->arpPos, scpcoaXML);
    common().createVector3D("ARPVel", scpcoa->arpVel, scpcoaXML);
    common().createVector3D("ARPAcc", scpcoa->arpAcc, scpcoaXML);
    createString("SideOfTrack", six::toString(scpcoa->sideOfTrack), scpcoaXML);
    createDouble("SlantRange", scpcoa->slantRange, scpcoaXML);
    createDouble("GroundRange", scpcoa->groundRange, scpcoaXML);
    createDouble("DopplerConeAng", scpcoa->dopplerConeAngle, scpcoaXML);
    createDouble("GrazeAng", scpcoa->grazeAngle, scpcoaXML);
    createDouble("IncidenceAng", scpcoa->incidenceAngle, scpcoaXML);
    createDouble("TwistAng", scpcoa->twistAngle, scpcoaXML);
    createDouble("SlopeAng", scpcoa->slopeAngle, scpcoaXML);

    return scpcoaXML;
}

XMLElem ComplexXMLParser::convertAntennaToXML(
    const Antenna *antenna, 
    XMLElem parent) const
{
    XMLElem antennaXML = newElement("Antenna", parent);

    if (antenna->tx.get())
    {
        convertAntennaParametersToXML("Tx", antenna->tx.get(), antennaXML);
    }
    if (antenna->rcv.get())
    {
        convertAntennaParametersToXML("Rcv", antenna->rcv.get(), antennaXML);
    }
    if (antenna->twoWay.get())
    {
        convertAntennaParametersToXML("TwoWay", antenna->twoWay.get(), antennaXML);
    }
    return antennaXML;
}

XMLElem ComplexXMLParser::convertAntennaParametersToXML(
    const std::string& name,
    AntennaParameters *params,
    XMLElem parent) const
{
    XMLElem apXML = newElement(name, parent);

    common().createPolyXYZ("XAxisPoly", params->xAxisPoly, apXML);
    common().createPolyXYZ("YAxisPoly", params->yAxisPoly, apXML);
    createDouble("FreqZero", params->frequencyZero, apXML);

    if (params->electricalBoresight.get())
    {
        XMLElem ebXML = newElement("EB", apXML);
        common().createPoly1D("DCXPoly", params->electricalBoresight->dcxPoly, ebXML);
        common().createPoly1D("DCYPoly", params->electricalBoresight->dcyPoly, ebXML);
    }

    this->convertHPBWToXML(params->halfPowerBeamwidths.get(), apXML);

    this->convertAntennaParamArrayToXML(name, params->array.get(), apXML);

    if (params->element.get())
    {
        XMLElem elemXML = newElement("Elem", apXML);
        common().createPoly2D("GainPoly", params->element->gainPoly, elemXML);
        common().createPoly2D("PhasePoly", params->element->phasePoly, elemXML);
    }
    if (!params->gainBSPoly.empty())
    {
        common().createPoly1D("GainBSPoly", params->gainBSPoly, apXML);
    }

    createBooleanType("EBFreqShift", params->electricalBoresightFrequencyShift,
                      apXML);
    createBooleanType("MLFreqDilation", params->mainlobeFrequencyDilation,
                      apXML);

    return apXML;
}

XMLElem ComplexXMLParser::convertPFAToXML(
    const PFA *pfa, 
    XMLElem parent) const
{
    XMLElem pfaXML = newElement("PFA", parent);

    common().createVector3D("FPN", pfa->focusPlaneNormal, pfaXML);
    common().createVector3D("IPN", pfa->imagePlaneNormal, pfaXML);
    createDouble("PolarAngRefTime", pfa->polarAngleRefTime, pfaXML);
    common().createPoly1D("PolarAngPoly", pfa->polarAnglePoly, pfaXML);
    common().createPoly1D("SpatialFreqSFPoly", pfa->spatialFrequencyScaleFactorPoly,
                 pfaXML);
    createDouble("Krg1", pfa->krg1, pfaXML);
    createDouble("Krg2", pfa->krg2, pfaXML);
    createDouble("Kaz1", pfa->kaz1, pfaXML);
    createDouble("Kaz2", pfa->kaz2, pfaXML);
    if (pfa->slowTimeDeskew.get())
    {
        XMLElem stdXML = newElement("STDeskew", pfaXML);
        require(createBooleanType("Applied", pfa->slowTimeDeskew->applied,
                                  stdXML), "Applied");

        common().createPoly2D("STDSPhasePoly",
                     pfa->slowTimeDeskew->slowTimeDeskewPhasePoly, stdXML);
    }

    return pfaXML;
}

void ComplexXMLParser::convertDRateSFPolyToXML(
    const INCA* inca, XMLElem incaXML) const
{
    //! Poly2D in 0.4.1
    common().createPoly2D("DRateSFPoly", 
        inca->dopplerRateScaleFactorPoly, incaXML);
}

XMLElem ComplexXMLParser::convertRMCRToXML(
    const RMCR* rmcr, 
    XMLElem rmaXML) const
{
    createString("ImageType", "RMCR", rmaXML);

    XMLElem rmcrXML = newElement("RMCR", rmaXML);

    common().createVector3D("PosRef", rmcr->refPos, rmcrXML);
    common().createVector3D("VelRef", rmcr->refVel, rmcrXML);
    createDouble("DopConeAngRef", rmcr->dopConeAngleRef, rmcrXML);

    return rmcrXML;
}

XMLElem ComplexXMLParser::convertINCAToXML(
    const INCA* inca, 
    XMLElem rmaXML) const
{
    createString("ImageType", "INCA", rmaXML);

    XMLElem incaXML = newElement("INCA", rmaXML);

    common().createPoly1D("TimeCAPoly", inca->timeCAPoly, incaXML);
    createDouble("R_CA_SCP", inca->rangeCA, incaXML);
    createDouble("FreqZero", inca->freqZero, incaXML);

    convertDRateSFPolyToXML(inca, incaXML);

    if (!inca->dopplerCentroidPoly.empty())
    {
        common().createPoly2D("DopCentroidPoly",
                        inca->dopplerCentroidPoly,
                        incaXML);
    }

    if (!Init::isUndefined<BooleanType>(inca->dopplerCentroidCOA))
    {
        createBooleanType("DopCentroidCOA",
                          inca->dopplerCentroidCOA,
                          incaXML);
    }

    return incaXML;
}

XMLElem ComplexXMLParser::convertRcvChanProcToXML(
    const std::string& version,
    const RcvChannelProcessed* rcvChanProc, 
    XMLElem imageFormationXML) const
{
    if (rcvChanProc)
    {
        XMLElem rcvChanXML = newElement("RcvChanProc", imageFormationXML);
        createInt("NumChanProc",
                  rcvChanProc->numChannelsProcessed,
                  rcvChanXML);
        if (!Init::isUndefined<double>(rcvChanProc->prfScaleFactor))
            createDouble("PRFScaleFactor",
                         rcvChanProc->prfScaleFactor,
                         rcvChanXML);

        for (std::vector<int>::const_iterator it =
                rcvChanProc->channelIndex.begin(); 
             it != rcvChanProc->channelIndex.end(); ++it)
        {
            createInt("ChanIndex", *it, rcvChanXML);
        }
        return rcvChanXML;
    }
    else
    {
        throw except::Exception(Ctxt(FmtX(
            "[RcvChanProc] is a manditory field in ImageFormation in %s", 
            version.c_str())));
    }
}

XMLElem ComplexXMLParser::convertDistortionToXML(
    const std::string& version,
    const Distortion* distortion, 
    XMLElem pcXML) const
{
    if (distortion)
    {
        XMLElem distortionXML = newElement("Distortion", pcXML);

        //This should be optionally added...
        createDateTime("CalibrationDate", distortion->calibrationDate,
                        distortionXML);
        createDouble("A", distortion->a, distortionXML);
        common().createComplex("F1", distortion->f1, distortionXML);
        common().createComplex("Q1", distortion->q1, distortionXML);
        common().createComplex("Q2", distortion->q2, distortionXML);
        common().createComplex("F2", distortion->f2, distortionXML);
        common().createComplex("Q3", distortion->q3, distortionXML);
        common().createComplex("Q4", distortion->q4, distortionXML);

        if (!Init::isUndefined<double>(distortion->gainErrorA))
            createDouble("GainErrorA", distortion->gainErrorA,
                            distortionXML);
        if (!Init::isUndefined<double>(distortion->gainErrorF1))
            createDouble("GainErrorF1", distortion->gainErrorF1,
                            distortionXML);
        if (!Init::isUndefined<double>(distortion->gainErrorF2))
            createDouble("GainErrorF2", distortion->gainErrorF2,
                            distortionXML);
        if (!Init::isUndefined<double>(distortion->phaseErrorF1))
            createDouble("PhaseErrorF1", distortion->phaseErrorF1,
                            distortionXML);
        if (!Init::isUndefined<double>(distortion->phaseErrorF2))
            createDouble("PhaseErrorF2", distortion->phaseErrorF2,
                            distortionXML);

        return distortionXML;
    }
    else
    {
        throw except::Exception(Ctxt(FmtX(
            "[Distortion] is a maditory field of ImageFormation in %s",
            version.c_str())));
    }
}

XMLElem ComplexXMLParser::convertRgAzCompToXML(
    const RgAzComp* rgAzComp, 
    XMLElem parent) const
{
    XMLElem rgAzCompXML = newElement("RgAzComp", parent);

    createDouble("AzSF", rgAzComp->azSF, rgAzCompXML);
    common().createPoly1D("KazPoly", rgAzComp->kazPoly, rgAzCompXML);

    return rgAzCompXML;
}

void ComplexXMLParser::parseDRateSFPolyFromXML(
    const XMLElem incaElem, INCA* inca) const
{
    //! Poly2D in 0.4.1
    common().parsePoly2D(getFirstAndOnly(incaElem, "DRateSFPoly"), 
                         inca->dopplerRateScaleFactorPoly);
}

void ComplexXMLParser::parseCollectionInformationFromXML(
    const XMLElem collectionInfoXML,
    CollectionInformation *collInfo) const
{
    parseString(getFirstAndOnly(collectionInfoXML, "CollectorName"),
                collInfo->collectorName);

    XMLElem element = getOptional(collectionInfoXML, "IlluminatorName");
    if (element)
        parseString(element, collInfo->illuminatorName);

    element = getOptional(collectionInfoXML, "CoreName");
    if (element)
        parseString(element, collInfo->coreName);

    element = getOptional(collectionInfoXML, "CollectType");
    if (element)
        collInfo->collectType
                = six::toType<six::CollectType>(element->getCharacterData());

    XMLElem radarModeXML = getFirstAndOnly(collectionInfoXML, "RadarMode");

    collInfo->radarMode
            = six::toType<RadarModeType>(getFirstAndOnly(radarModeXML,
                                         "ModeType")->getCharacterData());

    element = getOptional(radarModeXML, "ModeID");
    if (element)
        parseString(element, collInfo->radarModeID);

    parseString(getFirstAndOnly(collectionInfoXML, "Classification"),
                collInfo->classification.level);

    std::vector < XMLElem > countryCodeXML;
    collectionInfoXML->getElementsByTagName("CountryCode", countryCodeXML);

    //optional
    for (std::vector<XMLElem>::const_iterator it = countryCodeXML.begin(); it
            != countryCodeXML.end(); ++it)
    {
        std::string cc;

        parseString(*it, cc);
        collInfo->countryCodes.push_back(cc);
    }

    //optional
    common().parseParameters(collectionInfoXML, "Parameter", collInfo->parameters);
}

void ComplexXMLParser::parseImageCreationFromXML(
    const XMLElem imageCreationXML,
    ImageCreation *imageCreation) const
{
    // Optional
    XMLElem element = getOptional(imageCreationXML, "Application");
    if (element)
        parseString(element, imageCreation->application);

    element = getOptional(imageCreationXML, "DateTime");
    if (element)
        parseDateTime(element, imageCreation->dateTime);

    element = getOptional(imageCreationXML, "Site");
    if (element)
        parseString(element, imageCreation->site);

    element = getOptional(imageCreationXML, "Profile");
    if (element)
        parseString(element, imageCreation->profile);
}

void ComplexXMLParser::parseImageDataFromXML(
    const XMLElem imageDataXML,
    ImageData *imageData) const
{
    imageData->pixelType 
            = six::toType<PixelType>(
                    getFirstAndOnly(imageDataXML, 
                                    "PixelType")->getCharacterData());

    XMLElem ampTableXML = getOptional(imageDataXML, "AmpTable");

    if (ampTableXML != NULL)
    {
        std::vector < XMLElem > ampsXML;
        ampTableXML->getElementsByTagName("Amplitude", ampsXML);
        imageData->amplitudeTable = new AmplitudeTable();

        AmplitudeTable& ampTable = *(imageData->amplitudeTable);
        for (std::vector<XMLElem>::const_iterator it = ampsXML.begin(); it
                != ampsXML.end(); ++it)
        {
            XMLElem ampXML = *it;
            xml::lite::Attributes atts = ampXML->getAttributes();
            if (atts.contains("index"))
            {
                int index = str::toType<int>(atts.getValue("index"));
                if (index < 0 || index > 255)
                {
                    log()->warn(Ctxt(
                            "Unable to parse ampTable value - invalid index: " +
                            str::toString(index)));
                }
                else
                {
                    parseDouble(*it, *(double*) ampTable[index]);
                }
            }
            else
            {
                log()->warn(Ctxt(
                        "Unable to parse ampTable value - no index provided"));
            }
        }
    }

    parseUInt(getFirstAndOnly(imageDataXML, "NumRows"), imageData->numRows);
    parseUInt(getFirstAndOnly(imageDataXML, "NumCols"), imageData->numCols);
    parseUInt(getFirstAndOnly(imageDataXML, "FirstRow"), imageData->firstRow);
    parseUInt(getFirstAndOnly(imageDataXML, "FirstCol"), imageData->firstCol);

    common().parseRowColInt(getFirstAndOnly(imageDataXML, "FullImage"), "NumRows",
                   "NumCols", imageData->fullImage);

    common().parseRowColInt(getFirstAndOnly(imageDataXML, "SCPPixel"), "Row", "Col",
                   imageData->scpPixel);

    XMLElem validDataXML = getOptional(imageDataXML, "ValidData");
    if (validDataXML)
    {
        common().parseRowColInts(validDataXML, "Vertex", imageData->validData);
    }
}

void ComplexXMLParser::parseGeoDataFromXML(
    const XMLElem geoDataXML, 
    GeoData *geoData) const
{
    parseEarthModelType(getFirstAndOnly(geoDataXML, "EarthModel"),
                        geoData->earthModel);

    XMLElem tmpElem = getFirstAndOnly(geoDataXML, "SCP");
    common().parseVector3D(getFirstAndOnly(tmpElem, "ECF"), geoData->scp.ecf);
    common().parseLatLonAlt(getFirstAndOnly(tmpElem, "LLH"), geoData->scp.llh);

    common().parseFootprint(getFirstAndOnly(geoDataXML, "ImageCorners"), "ICP",
                   geoData->imageCorners);

    tmpElem = getOptional(geoDataXML, "ValidData");
    if (tmpElem != NULL)
    {
        common().parseLatLons(tmpElem, "Vertex", geoData->validData);
    }

    std::vector < XMLElem > geoInfosXML;
    geoDataXML->getElementsByTagName("GeoInfo", geoInfosXML);

    //optional
    size_t idx(geoData->geoInfos.size());
    geoData->geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoData->geoInfos[idx].reset(new GeoInfo());
        parseGeoInfoFromXML(*it, geoData->geoInfos[idx].get());
    }

}

void ComplexXMLParser::parseGeoInfoFromXML(const XMLElem geoInfoXML, GeoInfo* geoInfo) const
{
    std::vector < XMLElem > geoInfosXML;
    geoInfoXML->getElementsByTagName("GeoInfo", geoInfosXML);
    geoInfo->name = geoInfoXML->getAttributes().getValue("name");

    //optional
    size_t idx(geoInfo->geoInfos.size());
    geoInfo->geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
            != geoInfosXML.end(); ++it, ++idx)
    {
        geoInfo->geoInfos[idx].reset(new GeoInfo());
        parseGeoInfoFromXML(*it, geoInfo->geoInfos[idx].get());
    }

    //optional
    common().parseParameters(geoInfoXML, "Desc", geoInfo->desc);

    XMLElem tmpElem = getOptional(geoInfoXML, "Point");
    if (tmpElem)
    {
        LatLon ll;
        common().parseLatLon(tmpElem, ll);
        geoInfo->geometryLatLon.push_back(ll);
    }
    else
    {
        std::string pointName = "Endpoint";
        tmpElem = getOptional(geoInfoXML, "Line");
        if (!tmpElem)
        {
            pointName = "Vertex";
            tmpElem = getOptional(geoInfoXML, "Polygon");
        }
        if (tmpElem)
        {
            common().parseLatLons(tmpElem, pointName, geoInfo->geometryLatLon);
        }
    }
}

void ComplexXMLParser::parseGridFromXML(const XMLElem gridXML, Grid *grid) const
{
    grid->imagePlane = six::toType<ComplexImagePlaneType>(
        getFirstAndOnly(gridXML, "ImagePlane")->getCharacterData());
    grid->type = six::toType<ComplexImageGridType>(
        getFirstAndOnly(gridXML, "Type")->getCharacterData());

    XMLElem tmpElem = getFirstAndOnly(gridXML, "TimeCOAPoly");
    common().parsePoly2D(tmpElem, grid->timeCOAPoly);

    tmpElem = getFirstAndOnly(gridXML, "Row");
    common().parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->row->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->row->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
                grid->row->impulseResponseWidth);
    grid->row->sign = six::toType<six::FFTSign>(
        getFirstAndOnly(tmpElem, "Sgn")->getCharacterData());
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespBW"),
                grid->row->impulseResponseBandwidth);
    parseDouble(getFirstAndOnly(tmpElem, "KCtr"), grid->row->kCenter);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK1"), grid->row->deltaK1);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK2"), grid->row->deltaK2);

    XMLElem optElem = getOptional(tmpElem, "DeltaKCOAPoly");
    if (optElem)
    {
        //optional
        common().parsePoly2D(optElem, grid->row->deltaKCOAPoly);
    }

    parseWeightTypeFromXML(tmpElem, grid->row->weightType);

    XMLElem weightFuncXML = getOptional(tmpElem, "WgtFunct");
    if (weightFuncXML)
    {
        //optional
        //TODO make sure there is at least one and not more than 512 wgts
        std::vector < XMLElem > weightsXML;
        weightFuncXML->getElementsByTagName("Wgt", weightsXML);
        for (std::vector<XMLElem>::const_iterator it = weightsXML.begin(); it
                != weightsXML.end(); ++it)
        {
            double value;
            parseDouble(*it, value);
            grid->row->weights.push_back(value);
        }
    }

    tmpElem = getFirstAndOnly(gridXML, "Col");
    common().parseVector3D(getFirstAndOnly(tmpElem, "UVectECF"), grid->col->unitVector);
    parseDouble(getFirstAndOnly(tmpElem, "SS"), grid->col->sampleSpacing);
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespWid"),
                grid->col->impulseResponseWidth);
    grid->col->sign = six::toType<six::FFTSign>(
        getFirstAndOnly(tmpElem, "Sgn")->getCharacterData());
    parseDouble(getFirstAndOnly(tmpElem, "ImpRespBW"),
                grid->col->impulseResponseBandwidth);
    parseDouble(getFirstAndOnly(tmpElem, "KCtr"), grid->col->kCenter);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK1"), grid->col->deltaK1);
    parseDouble(getFirstAndOnly(tmpElem, "DeltaK2"), grid->col->deltaK2);

    optElem = getOptional(tmpElem, "DeltaKCOAPoly");
    if (optElem)
    {
        //optional
        common().parsePoly2D(optElem, grid->col->deltaKCOAPoly);
    }

    parseWeightTypeFromXML(tmpElem, grid->col->weightType);

    weightFuncXML = getOptional(tmpElem, "WgtFunct");
    if (weightFuncXML)
    {
        //optional

        //TODO make sure there is at least one and not more than 512 wgts
        std::vector < XMLElem > weightsXML;
        weightFuncXML->getElementsByTagName("Wgt", weightsXML);
        for (std::vector<XMLElem>::const_iterator it = weightsXML.begin(); it
                != weightsXML.end(); ++it)
        {
            double value;
            parseDouble(*it, value);
            grid->col->weights.push_back(value);
        }
    }
}

void ComplexXMLParser::parseTimelineFromXML(
    const XMLElem timelineXML,
    Timeline *timeline) const
{
    parseDateTime(getFirstAndOnly(timelineXML, "CollectStart"),
                  timeline->collectStart);
    parseDouble(getFirstAndOnly(timelineXML, "CollectDuration"),
                timeline->collectDuration);

    XMLElem ippXML = getOptional(timelineXML, "IPP");
    if (ippXML)
    {
        timeline->interPulsePeriod.reset(new InterPulsePeriod());
        //TODO make sure there is at least one
        std::vector < XMLElem > setsXML;
        ippXML->getElementsByTagName("Set", setsXML);
        for (std::vector<XMLElem>::const_iterator it = setsXML.begin(); it
                != setsXML.end(); ++it)
        {
            // Use the first set that is already available.
            timeline->interPulsePeriod->sets.resize(
                    timeline->interPulsePeriod->sets.size() + 1);
            TimelineSet& ts(timeline->interPulsePeriod->sets.back());

            parseDouble(getFirstAndOnly(*it, "TStart"), ts.tStart);
            parseDouble(getFirstAndOnly(*it, "TEnd"), ts.tEnd);
            parseInt(getFirstAndOnly(*it, "IPPStart"),
                     ts.interPulsePeriodStart);
            parseInt(getFirstAndOnly(*it, "IPPEnd"), ts.interPulsePeriodEnd);
            common().parsePoly1D(getFirstAndOnly(*it, "IPPPoly"),
                        ts.interPulsePeriodPoly);
        }

        // Required to have at least one timeline set.
        // TODO: Does it really make sense to do this?
        if (timeline->interPulsePeriod->sets.empty())
        {
            timeline->interPulsePeriod->sets.resize(1);
        }
    }
}

void ComplexXMLParser::parsePositionFromXML(
    const XMLElem positionXML,
    Position *position) const
{
    XMLElem tmpElem = getFirstAndOnly(positionXML, "ARPPoly");
    common().parsePolyXYZ(tmpElem, position->arpPoly);

    tmpElem = getOptional(positionXML, "GRPPoly");
    if (tmpElem)
    {
        //optional
        common().parsePolyXYZ(tmpElem, position->grpPoly);
    }

    tmpElem = getOptional(positionXML, "TxAPCPoly");
    if (tmpElem)
    {
        //optional
        common().parsePolyXYZ(tmpElem, position->txAPCPoly);
    }

    tmpElem = getOptional(positionXML, "RcvAPC");
    if (tmpElem)
    {
        //optional
        position->rcvAPC.reset(new RcvAPC());

        //TODO make sure there is at least one
        std::vector < XMLElem > polysXML;
        tmpElem->getElementsByTagName("RcvAPCPoly", polysXML);
        for (std::vector<XMLElem>::const_iterator it = polysXML.begin(); it
                != polysXML.end(); ++it)
        {
            PolyXYZ p;
            common().parsePolyXYZ(*it, p);
            position->rcvAPC->rcvAPCPolys.push_back(p);
        }
    }
}

void ComplexXMLParser::parseImageFormationFromXML(
    const XMLElem imageFormationXML,
    ImageFormation *imageFormation) const
{
    XMLElem tmpElem = getOptional(imageFormationXML, "SegmentIdentifier");
    if (tmpElem)
    {
        //optional
        parseString(tmpElem, imageFormation->segmentIdentifier);
    }

    tmpElem = getFirstAndOnly(imageFormationXML, "RcvChanProc");

    parseUInt(getFirstAndOnly(tmpElem, "NumChanProc"),
              imageFormation->rcvChannelProcessed->numChannelsProcessed);

    XMLElem prfXML = getOptional(tmpElem, "PRFScaleFactor");
    if (prfXML)
    {
        //optional
        parseDouble(prfXML, imageFormation->rcvChannelProcessed->prfScaleFactor);
    }

    //TODO make sure there is at least one
    std::vector < XMLElem > chansXML;
    tmpElem->getElementsByTagName("ChanIndex", chansXML);
    for (std::vector<XMLElem>::const_iterator it = chansXML.begin(); it
            != chansXML.end(); ++it)
    {
        int value;
        parseInt(*it, value);
        imageFormation->rcvChannelProcessed->channelIndex.push_back(value);
    }

    //! version specific TxRcvPolarization
    parseTxRcvPolFromXML(imageFormationXML, 
                         imageFormation->txRcvPolarizationProc);

    imageFormation->imageFormationAlgorithm = six::toType<ImageFormationType>(
        getFirstAndOnly(imageFormationXML, 
                        "ImageFormAlgo")->getCharacterData());

    parseDouble(getFirstAndOnly(imageFormationXML, "TStartProc"),
                imageFormation->tStartProc);

    parseDouble(getFirstAndOnly(imageFormationXML, "TEndProc"),
                imageFormation->tEndProc);

    tmpElem = getFirstAndOnly(imageFormationXML, "TxFrequencyProc");

    parseDouble(getFirstAndOnly(tmpElem, "MinProc"),
                imageFormation->txFrequencyProcMin);

    parseDouble(getFirstAndOnly(tmpElem, "MaxProc"),
                imageFormation->txFrequencyProcMax);

    imageFormation->slowTimeBeamCompensation = 
        six::toType<SlowTimeBeamCompensationType>(getFirstAndOnly(
            imageFormationXML, "STBeamComp")->getCharacterData());

    imageFormation->imageBeamCompensation = 
        six::toType<ImageBeamCompensationType>(getFirstAndOnly(
            imageFormationXML, "ImageBeamComp")->getCharacterData());

    imageFormation->azimuthAutofocus = six::toType<AutofocusType>(
        getFirstAndOnly(imageFormationXML, 
                        "AzAutofocus")->getCharacterData());

    imageFormation->rangeAutofocus = six::toType<AutofocusType>(
        getFirstAndOnly(imageFormationXML,
                        "RgAutofocus")->getCharacterData());

    std::vector < XMLElem > procXML;
    imageFormationXML->getElementsByTagName("Processing", procXML);

    for (unsigned int i = 0; i < procXML.size(); ++i)
    {
        Processing* proc = new Processing();

        parseString(getFirstAndOnly(procXML[i], "Type"), proc->type);
        parseBooleanType(getFirstAndOnly(procXML[i], "Applied"), proc->applied);
        common().parseParameters(procXML[i], "Parameter", proc->parameters);

        imageFormation->processing.push_back(*proc);
    }

    XMLElem polCalXML = getOptional(imageFormationXML,
                                    "PolarizationCalibration");
    if (polCalXML)
    {
        //optional
        imageFormation->polarizationCalibration.reset(new PolarizationCalibration());
        imageFormation->polarizationCalibration->distortion.reset(new Distortion());

        // parses HVAngleCompApplied and DistortionCorrectionApplied
        parsePolarizationCalibrationFromXML(
            polCalXML, imageFormation->polarizationCalibration.get());

        XMLElem distortionXML = getFirstAndOnly(polCalXML, "Distortion");

        XMLElem calibDateXML = getOptional(distortionXML, "CalibrationDate");
        if (calibDateXML)
        {
            parseDateTime(calibDateXML, imageFormation->
                polarizationCalibration->distortion->calibrationDate);
        }

        parseDouble(getFirstAndOnly(distortionXML, "A"),
                    imageFormation->polarizationCalibration->distortion->a);

        parseComplex(getFirstAndOnly(distortionXML, "F1"),
                     imageFormation->polarizationCalibration->distortion->f1);
        parseComplex(getFirstAndOnly(distortionXML, "Q1"),
                     imageFormation->polarizationCalibration->distortion->q1);
        parseComplex(getFirstAndOnly(distortionXML, "Q2"),
                     imageFormation->polarizationCalibration->distortion->q2);
        parseComplex(getFirstAndOnly(distortionXML, "F2"),
                     imageFormation->polarizationCalibration->distortion->f2);
        parseComplex(getFirstAndOnly(distortionXML, "Q3"),
                     imageFormation->polarizationCalibration->distortion->q3);
        parseComplex(getFirstAndOnly(distortionXML, "Q4"),
                     imageFormation->polarizationCalibration->distortion->q4);

        XMLElem gainErrorXML = getOptional(distortionXML, "GainErrorA");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->
                polarizationCalibration ->distortion->gainErrorA);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF1");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->
                polarizationCalibration->distortion->gainErrorF1);
        }

        gainErrorXML = getOptional(distortionXML, "GainErrorF2");
        if (gainErrorXML)
        {
            parseDouble(gainErrorXML, imageFormation->
                polarizationCalibration ->distortion->gainErrorF2);
        }

        XMLElem phaseErrorXML = getOptional(distortionXML, "PhaseErrorF1");
        if (phaseErrorXML)
        {
            parseDouble(phaseErrorXML, imageFormation->
                polarizationCalibration ->distortion->phaseErrorF1);
        }

        phaseErrorXML = getOptional(distortionXML, "PhaseErrorF2");
        if (phaseErrorXML)
        {
            parseDouble(phaseErrorXML, imageFormation->
                polarizationCalibration ->distortion->phaseErrorF2);
        }

    }
}

void ComplexXMLParser::parseSCPCOAFromXML(
    const XMLElem scpcoaXML,
    SCPCOA *scpcoa) const
{
    parseDouble(getFirstAndOnly(scpcoaXML, "SCPTime"), scpcoa->scpTime);

    common().parseVector3D(getFirstAndOnly(scpcoaXML, "ARPPos"), scpcoa->arpPos);
    common().parseVector3D(getFirstAndOnly(scpcoaXML, "ARPVel"), scpcoa->arpVel);
    common().parseVector3D(getFirstAndOnly(scpcoaXML, "ARPAcc"), scpcoa->arpAcc);

    parseSideOfTrackType(getFirstAndOnly(scpcoaXML, "SideOfTrack"),
                         scpcoa->sideOfTrack);
    parseDouble(getFirstAndOnly(scpcoaXML, "SlantRange"), scpcoa->slantRange);
    parseDouble(getFirstAndOnly(scpcoaXML, "GroundRange"), scpcoa->groundRange);
    parseDouble(getFirstAndOnly(scpcoaXML, "DopplerConeAng"),
                scpcoa->dopplerConeAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "GrazeAng"), scpcoa->grazeAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "IncidenceAng"),
                scpcoa->incidenceAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "TwistAng"), scpcoa->twistAngle);
    parseDouble(getFirstAndOnly(scpcoaXML, "SlopeAng"), scpcoa->slopeAngle);
}

void ComplexXMLParser::parseAntennaParametersFromXML(
    const XMLElem antennaParamsXML,
    AntennaParameters* params) const
{
    common().parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "XAxisPoly"),
                 params->xAxisPoly);
    common().parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "YAxisPoly"),
                 params->yAxisPoly);
    parseDouble(getFirstAndOnly(antennaParamsXML, "FreqZero"),
                params->frequencyZero);

    XMLElem tmpElem = getOptional(antennaParamsXML, "EB");
    if (tmpElem)
    {
        params->electricalBoresight.reset(new ElectricalBoresight());
        common().parsePoly1D(getFirstAndOnly(tmpElem, "DCXPoly"),
                    params->electricalBoresight->dcxPoly);
        common().parsePoly1D(getFirstAndOnly(tmpElem, "DCYPoly"),
                    params->electricalBoresight->dcyPoly);
    }

    //! this field was deprecated in 1.0.0, but we can do this
    //  since it was previously optional
    tmpElem = getOptional(antennaParamsXML, "HPBW");
    if (tmpElem)
    {
        params->halfPowerBeamwidths.reset(new HalfPowerBeamwidths());
        parseDouble(getFirstAndOnly(tmpElem, "DCX"),
                    params->halfPowerBeamwidths->dcx);
        parseDouble(getFirstAndOnly(tmpElem, "DCY"),
                    params->halfPowerBeamwidths->dcy);
    }

    //! this field turned manditory in 1.0.0
    parseAntennaParamArrayFromXML(antennaParamsXML, params);

    tmpElem = getOptional(antennaParamsXML, "Elem");
    if (tmpElem)
    {
        params->element.reset(new GainAndPhasePolys());
        common().parsePoly2D(getFirstAndOnly(tmpElem, "GainPoly"),
                    params->element->gainPoly);
        common().parsePoly2D(getFirstAndOnly(tmpElem, "PhasePoly"),
                    params->element->phasePoly);
    }

    tmpElem = getOptional(antennaParamsXML, "GainBSPoly");
    if (tmpElem)
    {
        //optional
        common().parsePoly1D(tmpElem, params->gainBSPoly);
    }

    tmpElem = getOptional(antennaParamsXML, "EBFreqShift");
    if (tmpElem)
    {
        //optional
        parseBooleanType(tmpElem, params->electricalBoresightFrequencyShift);
    }

    tmpElem = getOptional(antennaParamsXML, "MLFreqDilation");
    if (tmpElem)
    {
        //optional
        parseBooleanType(tmpElem, params->mainlobeFrequencyDilation);
    }
}

void ComplexXMLParser::parseAntennaFromXML(
    const XMLElem antennaXML,
    Antenna *antenna) const
{
    XMLElem antennaParamsXML = getOptional(antennaXML, "Tx");
    if (antennaParamsXML)
    {
        antenna->tx.reset(new AntennaParameters());
        parseAntennaParametersFromXML(antennaParamsXML, antenna->tx.get());
    }

    antennaParamsXML = getOptional(antennaXML, "Rcv");
    if (antennaParamsXML)
    {
        antenna->rcv.reset(new AntennaParameters());
        parseAntennaParametersFromXML(antennaParamsXML, antenna->rcv.get());
    }

    antennaParamsXML = getOptional(antennaXML, "TwoWay");
    if (antennaParamsXML)
    {
        antenna->twoWay.reset(new AntennaParameters());
        parseAntennaParametersFromXML(antennaParamsXML, antenna->twoWay.get());
    }
}

void ComplexXMLParser::parsePFAFromXML(
    const XMLElem pfaXML, 
    PFA *pfa) const
{
    common().parseVector3D(getFirstAndOnly(pfaXML, "FPN"), pfa->focusPlaneNormal);
    common().parseVector3D(getFirstAndOnly(pfaXML, "IPN"), pfa->imagePlaneNormal);
    parseDouble(getFirstAndOnly(pfaXML, "PolarAngRefTime"),
                pfa->polarAngleRefTime);
    common().parsePoly1D(getFirstAndOnly(pfaXML, "PolarAngPoly"), pfa->polarAnglePoly);
    common().parsePoly1D(getFirstAndOnly(pfaXML, "SpatialFreqSFPoly"),
                pfa->spatialFrequencyScaleFactorPoly);
    parseDouble(getFirstAndOnly(pfaXML, "Krg1"), pfa->krg1);
    parseDouble(getFirstAndOnly(pfaXML, "Krg2"), pfa->krg2);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz1"), pfa->kaz1);
    parseDouble(getFirstAndOnly(pfaXML, "Kaz2"), pfa->kaz2);

    XMLElem deskewXML = getOptional(pfaXML, "STDeskew");
    if (deskewXML)
    {
        pfa->slowTimeDeskew.reset(new SlowTimeDeskew());
        parseBooleanType(getFirstAndOnly(deskewXML, "Applied"),
                         pfa->slowTimeDeskew->applied);

        common().parsePoly2D(getFirstAndOnly(deskewXML, "STDSPhasePoly"),
                    pfa->slowTimeDeskew->slowTimeDeskewPhasePoly);
    }
}

void ComplexXMLParser::parseRMAFromXML(
    const XMLElem rmaXML, 
    RMA* rma) const
{
    rma->algoType = six::toType<RMAlgoType>(
        getFirstAndOnly(rmaXML, "RMAlgoType")->getCharacterData());

    XMLElem rmatElem = getOptional(rmaXML, "RMAT");
    if (rmatElem)
    {
        rma->rmat.reset(new RMAT());
        parseRMATFromXML(rmatElem, rma->rmat.get());
    }

    XMLElem rmcrElem = getOptional(rmaXML, "RMCR");
    if (rmcrElem)
    {
        rma->rmcr.reset(new RMCR());
        parseRMCRFromXML(rmcrElem, rma->rmcr.get());
    }

    XMLElem incaElem = getOptional(rmaXML, "INCA");
    if (incaElem)
    {
        rma->inca.reset(new INCA());
        parseINCAFromXML(incaElem, rma->inca.get());
    }
}

void ComplexXMLParser::parseINCAFromXML(
    const XMLElem incaElem, INCA* inca) const
{
    common().parsePoly1D(getFirstAndOnly(incaElem, "TimeCAPoly"), inca->timeCAPoly);
    parseDouble(getFirstAndOnly(incaElem, "R_CA_SCP"), inca->rangeCA);
    parseDouble(getFirstAndOnly(incaElem, "FreqZero"), inca->freqZero);

    parseDRateSFPolyFromXML(incaElem, inca);

    XMLElem tmpElem = getOptional(incaElem, "DopCentroidPoly");
    if (tmpElem)
    {
        common().parsePoly2D(tmpElem, inca->dopplerCentroidPoly);
    }

    tmpElem = getOptional(incaElem, "DopCentroidCOA");
    if (tmpElem)
    {
        parseBooleanType(tmpElem, inca->dopplerCentroidCOA);
    }
}

void ComplexXMLParser::parseRgAzCompFromXML(
    const XMLElem rgAzCompXML, 
    RgAzComp* rgAzComp) const
{
    parseDouble(getFirstAndOnly(rgAzCompXML, "AzSF"), rgAzComp->azSF);
    common().parsePoly1D(getFirstAndOnly(rgAzCompXML, "KazPoly"), rgAzComp->kazPoly);
}

void ComplexXMLParser::parseWaveformFromXML(
    const XMLElem waveformXML,
    std::vector<mem::ScopedCloneablePtr<WaveformParameters> >& waveform) const
{
    std::vector<XMLElem> wfParamsXML;
    waveformXML->getElementsByTagName("WFParameters", wfParamsXML);
    if (wfParamsXML.empty())
    {
        throw except::Exception(Ctxt(
                "Expected at least one WFParameters element"));
    }

    for (std::vector<XMLElem>::const_iterator it = wfParamsXML.begin(); it
            != wfParamsXML.end(); ++it)
    {
        waveform.resize(waveform.size() + 1);
        waveform.back().reset(new WaveformParameters());
        WaveformParameters* const wfParams = waveform.back().get();

        XMLElem optElem = getOptional(*it, "TxPulseLength");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->txPulseLength);
        }

        optElem = getOptional(*it, "TxRFBandwidth");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->txRFBandwidth);
        }

        optElem = getOptional(*it, "TxFreqStart");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->txFrequencyStart);
        }

        optElem = getOptional(*it, "TxFMRate");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->txFMRate);
        }

        optElem = getOptional(*it, "RcvDemodType");
        if (optElem)
        {
            //optional
            wfParams->rcvDemodType
                    = six::toType<DemodType>(optElem->getCharacterData());
        }

        optElem = getOptional(*it, "RcvWindowLength");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->rcvWindowLength);
        }

        optElem = getOptional(*it, "ADCSampleRate");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->adcSampleRate);
        }

        optElem = getOptional(*it, "RcvIFBandwidth");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->rcvIFBandwidth);
        }

        optElem = getOptional(*it, "RcvFreqStart");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->rcvFrequencyStart);
        }

        optElem = getOptional(*it, "RcvFMRate");
        if (optElem)
        {
            //optional
            parseDouble(optElem, wfParams->rcvFMRate);
        }
    }
}

void ComplexXMLParser::parseAreaFromXML(
        const XMLElem areaXML,
        bool cornersRequired,
        bool planeOrientationRequired,
        mem::ScopedCloneablePtr<Area>& area) const
{
    area.reset(new Area());

    XMLElem optElem = getOptional(areaXML, "Corner");
    if (optElem)
    {
        //optional
        common().parseFootprint(optElem, "ACP", area->acpCorners);
    }
    else if (cornersRequired)
    {
        throw except::Exception(Ctxt("Corner element required"));
    }

    XMLElem planeXML = getOptional(areaXML, "Plane");
    if (planeXML)
    {
        //optional
        area->plane.reset(new AreaPlane());

        XMLElem refPtXML = getFirstAndOnly(planeXML, "RefPt");
        try
        {
            area->plane->referencePoint.name
                    = refPtXML->getAttributes().getValue("name");
        }
        catch (const except::Exception &ex)
        {
        }

        common().parseVector3D(getFirstAndOnly(refPtXML, "ECF"),
                      area->plane->referencePoint.ecef);
        parseDouble(getFirstAndOnly(refPtXML, "Line"),
                    area->plane->referencePoint.rowCol.row);
        parseDouble(getFirstAndOnly(refPtXML, "Sample"),
                    area->plane->referencePoint.rowCol.col);

        XMLElem dirXML = getFirstAndOnly(planeXML, "XDir");
        common().parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                      area->plane->xDirection->unitVector);
        parseDouble(getFirstAndOnly(dirXML, "LineSpacing"),
                    area->plane->xDirection->spacing);
        parseUInt(getFirstAndOnly(dirXML, "NumLines"),
                  area->plane->xDirection->elements);
        parseUInt(getFirstAndOnly(dirXML, "FirstLine"),
                  area->plane->xDirection->first);

        dirXML = getFirstAndOnly(planeXML, "YDir");
        common().parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                      area->plane->yDirection->unitVector);
        parseDouble(getFirstAndOnly(dirXML, "SampleSpacing"),
                    area->plane->yDirection->spacing);
        parseUInt(getFirstAndOnly(dirXML, "NumSamples"),
                  area->plane->yDirection->elements);
        parseUInt(getFirstAndOnly(dirXML, "FirstSample"),
                  area->plane->yDirection->first);

        XMLElem segmentListXML = getOptional(planeXML, "SegmentList");
        if (segmentListXML != NULL)
        {
            std::vector<XMLElem> segmentsXML;
            segmentListXML->getElementsByTagName("Segment", segmentsXML);
            if (segmentsXML.empty())
            {
                throw except::Exception(Ctxt(
                        "Expected at least one Segment element"));
            }

            for (std::vector<XMLElem>::const_iterator it =
                    segmentsXML.begin(); it != segmentsXML.end(); ++it)
            {
                area->plane->segmentList.resize(
                    area->plane->segmentList.size() + 1);
                area->plane->segmentList.back().reset(
                    new Segment());
                Segment* const seg =
                    area->plane->segmentList.back().get();

                parseInt(getFirstAndOnly(*it, "StartLine"), seg->startLine);
                parseInt(getFirstAndOnly(*it, "StartSample"),
                         seg->startSample);
                parseInt(getFirstAndOnly(*it, "EndLine"), seg->endLine);
                parseInt(getFirstAndOnly(*it, "EndSample"), seg->endSample);
                parseString(getFirstAndOnly(*it, "Identifier"),
                            seg->identifier);
            }
        }

        XMLElem orientation = getOptional(planeXML, "Orientation");
        if (orientation)
        {
            area->plane->orientation = six::toType<OrientationType>(
                    orientation->getCharacterData());
        }
        else if (planeOrientationRequired)
        {
            throw except::Exception(Ctxt("Orientation element required"));
        }
    }
}

void ComplexXMLParser::parseTxSequenceFromXML(
        const XMLElem txSequenceXML,
        std::vector<mem::ScopedCloneablePtr<TxStep> >& steps) const
{
    std::vector<XMLElem> txStepsXML;
    txSequenceXML->getElementsByTagName("TxStep", txStepsXML);
    if (txStepsXML.empty())
    {
        throw except::Exception(Ctxt(
                "Expected at least one TxStep element"));
    }

    for (std::vector<XMLElem>::const_iterator it = txStepsXML.begin(); it
            != txStepsXML.end(); ++it)
    {
        steps.resize(steps.size() + 1);
        mem::ScopedCloneablePtr<TxStep>& step(steps.back());
        step.reset(new TxStep());

        XMLElem optElem = getOptional(*it, "WFIndex");
        if (optElem)
        {
            //optional
            parseInt(optElem, step->waveformIndex);
        }

        optElem = getOptional(*it, "TxPolarization");
        if (optElem)
        {
            //optional
            step->txPolarization = six::toType<PolarizationType>(
                    optElem->getCharacterData());
        }
    }
}

XMLElem ComplexXMLParser::createLatLonFootprint(const std::string& name,
                                                const std::string& cornerName,
                                                const LatLonCorners& corners,
                                                XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex = common().createLatLon(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1:FRFC");

    vertex = common().createLatLon(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2:FRLC");

    vertex = common().createLatLon(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3:LRLC");

    vertex = common().createLatLon(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4:LRFC");

    return footprint;
}

XMLElem ComplexXMLParser::createLatLonAltFootprint(const std::string& name,
                                                   const std::string& cornerName,
                                                   const LatLonAltCorners& corners,
                                                   XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex =
        common().createLatLonAlt(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1");

    vertex = common().createLatLonAlt(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2");

    vertex = common().createLatLonAlt(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3");

    vertex = common().createLatLonAlt(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4");

    return footprint;
}


XMLElem ComplexXMLParser::createEarthModelType(const std::string& name,
                                               const EarthModelType& value,
                                               XMLElem parent) const
{
    return createString(name, six::toString(value), parent);
}

XMLElem ComplexXMLParser::createSideOfTrackType(const std::string& name,
                                                const SideOfTrackType& value,
                                                XMLElem parent) const
{
    return createString(name, six::toString(value), parent);
}

void ComplexXMLParser::parseEarthModelType(XMLElem element,
                                           EarthModelType& value) const
{
    value = six::toType<EarthModelType>(element->getCharacterData());
}

void ComplexXMLParser::parseSideOfTrackType(XMLElem element,
                                            SideOfTrackType& value) const
{
    value = six::toType<SideOfTrackType>(element->getCharacterData());
}

}
}

