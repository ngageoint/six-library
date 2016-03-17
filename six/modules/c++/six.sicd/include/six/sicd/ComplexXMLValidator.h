/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
#ifndef __SIX_SICD_COMPLEX_XML_VALIDATOR_H__
#define __SIX_SICD_COMPLEX_XML_VALIDATOR_H__


#include <six/sicd/ComplexData.h>
#include <six/sicd/Functor.h>
//#include <import/six/sicd.h>

namespace six
{
namespace sicd
{
class ComplexXMLValidator
{
public:
    ComplexXMLValidator(const ComplexData& data, logging::Logger* log);

    bool validate();

private:
    bool checkTimeCOAPoly();
    bool checkFFTSigns();
    bool checkFrequencySupportParameters();
    bool checkFrequencySupportParameters(const DirectionParameters& direction, const std::string& name);
    bool checkSupportParamsAgainstPFA();
    bool checkWeightFunctions();
    bool checkWeightFunctions(const DirectionParameters& direction, const std::string& name);
    Functor* calculateWeightFunction(const DirectionParameters& direction);
    bool checkARPPoly();
    bool checkWaveformDescription();
    double nonZeroDenominator(double denominator);
    bool checkGeoData();
    bool checkValidData();
    bool checkIFP();
    bool checkRGAZCOMP();
    bool checkPFA();
    bool checkRMA();

    template <class T>
    void assertExists(const T& type, const std::string& name)
    {
        if (type.get() == NULL)
        {
            mLog->warn(name + " is NULL");
            throw except::Exception(Ctxt(""));
        }
    }

    const std::string boundsErrorMessage = "Violation of spatial frequency extent bounds.";
    const std::string WF_INCONSISTENT_STR = "Waveform fields not consistent";

    const double UVECT_TOL = 1e-3;
    const double WF_TOL = 1e-3; // Relative tolerance
    const double WGT_TOL = 1e-3;
    const double IFP_POLY_TOL = 1e-5;

    const ComplexData sicd;
    logging::Logger* mLog;
    std::ostringstream messageBuilder;
};
}
}
#endif