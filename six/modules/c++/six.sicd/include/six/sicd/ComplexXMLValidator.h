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

namespace six
{
namespace sicd
{
class ComplexXMLValidator
{
public:
    ComplexXMLValidator(const ComplexData& data, logging::Logger* log);

    bool validate();

    static Poly1D polyAt(PolyXYZ poly, size_t idx);
    static std::vector<double> linspace(double start, double end, size_t count = 100);

    template <class T>
    static int sign(const T& val)
    {
        if (val < 0)
        {
            return -1;
        }
        if (val > 0)
        {
            return 1;
        }
        return 0;
    }

private:
    bool checkSupportParamsAgainstPFA();
    bool checkWeightFunctions();
    bool checkWeightFunctions(const DirectionParameters& direction, const std::string& name);
    Functor* calculateWeightFunction(const DirectionParameters& direction);
    bool checkWaveformDescription();
    double nonZeroDenominator(double denominator);
    bool checkGeoData();
    bool checkValidData();
    bool checkIFP();
    bool checkRGAZCOMP();

    template <class T>
    void assertExists(const T& type, const std::string& name)
    {
        if (type.get() == NULL)
        {
            mLog->warn(name + " is NULL");
            throw except::Exception(Ctxt(""));
        }
    }

    double nonZero(double arg);

    const std::string boundsErrorMessage = "Violation of spatial frequency extent bounds.";
    const std::string WF_INCONSISTENT_STR = "Waveform fields not consistent";

    const double UVECT_TOL = 1e-3;
    const double WF_TOL = 1e-3; // Relative tolerance
    const double WGT_TOL = 1e-3;
    const double IFP_POLY_TOL = 1e-5;
    const double SCPCOA_TOL = 1e-2;

    const ComplexData sicd;
    logging::Logger* mLog;
    std::ostringstream messageBuilder;
};
}
}
#endif