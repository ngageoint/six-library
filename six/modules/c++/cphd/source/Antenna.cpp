/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/Antenna.h>
#include <six/Init.h>

namespace cphd
{

AntCoordFrame::AntCoordFrame() :
    identifier(six::Init::undefined<std::string>())
{
}

AntPhaseCenter::AntPhaseCenter():
    identifier(six::Init::undefined<std::string>()),
    acfId(six::Init::undefined<std::string>())
{
}

AntPattern::AntPattern():
    freqZero(six::Init::undefined<double>()),
    gainZero(six::Init::undefined<double>()),
    ebFreqShift(six::Init::undefined<six::BooleanType>()),
    mlFreqDilation(six::Init::undefined<six::BooleanType>()),
    gainBSPoly(six::Init::undefined<Poly1D>())
{
}

Antenna::Antenna() :
    numACFs(six::Init::undefined<double>()),
    numAPCs(six::Init::undefined<double>()),
    numAntPats(six::Init::undefined<double>())
{
}

std::ostream& operator<< (std::ostream& os, const AntCoordFrame& a)
{
    os << "  AntCoordFrame:: \n"
        << "    Identifier     : " << a.identifier << "\n"
        << "    XAxisPoly      : " << a.xAxisPoly << "\n"
        << "    YAxisPoly      : " << a.yAxisPoly << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const AntPhaseCenter& a)
{
    os << "  AntPhaseCenter:: \n"
        << "    Identifier     : " << a.identifier << "\n"
        << "    ACFId          : " << a.acfId << "\n"
        << "    APCXYZ         : " << a.apcXYZ << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const AntPattern::EB& e)
{
    os << "    EB:: \n"
        << "      DCXPoly      : " << e.dcXPoly << "\n"
        << "      DCYPoly      : " << e.dcYPoly << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const AntPattern::ArrayElement& a)
{
    os << "      GainPoly     : " << a.gainPoly << "\n"
        << "      PhasePoly    : " << a.phasePoly << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const AntPattern::GainPhaseArray& g)
{
    os << "    GainPhaseArray:: \n"
        << "      Freq         : " << g.freq << "\n"
        << "      ArrayId      : " << g.arrayId << "\n"
        << "      ElementId    : " << g.elementId << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const AntPattern& a)
{
    os << "  AntPattern \n"
        << "    Identifier     : " << a.identifier << "\n"
        << "    FreqZero       : " << a.freqZero << "\n"
        << "    GainZero       : " << a.gainZero << "\n"
        << "    EBFreqShift    : " << a.ebFreqShift << "\n"
        << "    MLFreqDilation : " << a.mlFreqDilation << "\n"
        << "    GainBSPoly     : " << a.gainBSPoly << "\n"
        << a.eb << "\n"
        << "    Array:: \n"
        << a.array << "\n"
        << "    Element:: \n"
        << a.element << "\n";
    for (size_t i = 0; i < a.gainPhaseArray.size(); ++i)
    {
        os << a.gainPhaseArray[i] << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const Antenna& a)
{
    os << "Antenna:: \n"
        << "  NumACFs          : " << a.numACFs << "\n"
        << "  NumAPCs          : " << a.numAPCs << "\n";
    for (size_t i = 0; i < a.antCoordFrame.size(); ++i)
    {
        os << a.antCoordFrame[i] << "\n";
    }
    for (size_t i = 0; i < a.antPhaseCenter.size(); ++i)
    {
        os << a.antPhaseCenter[i] << "\n";
    }
    for (size_t i = 0; i < a.antPattern.size(); ++i)
    {
        os << a.antPattern[i] << "\n";
    }
    return os;
}
}
