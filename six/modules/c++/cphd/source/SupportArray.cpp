/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#include <cphd/SupportArray.h>
#include <six/Init.h>

namespace cphd
{

SupportArrayParameter::SupportArrayParameter() :
    elementFormat(six::Init::undefined<std::string>()),
    x0(six::Init::undefined<double>()),
    y0(six::Init::undefined<double>()),
    xSS(six::Init::undefined<double>()),
    ySS(six::Init::undefined<double>()),
    identifier(six::Init::undefined<size_t>())
{
}

AdditionalSupportArray::AdditionalSupportArray() :
    identifier(six::Init::undefined<std::string>()),
    xUnits(six::Init::undefined<std::string>()),
    yUnits(six::Init::undefined<std::string>()),
    zUnits(six::Init::undefined<std::string>())
{
}

std::ostream& operator<< (std::ostream& os, const SupportArrayParameter& s)
{
    if (!six::Init::isUndefined(s.getIdentifier()))
    {
        os << "    Identifier     : " << s.getIdentifier() << "\n";
    }
    os << "    Element Format : " << s.elementFormat << "\n"
        << "    X0             : " << s.x0 << "\n"
        << "    Y0             : " << s.y0 << "\n"
        << "    xSS            : " << s.xSS << "\n"
        << "    ySS            : " << s.ySS << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const AdditionalSupportArray& a)
{
    os << "    Identifier     : " << a.identifier << "\n"
        << (SupportArrayParameter)a
        << "    XUnits         : " << a.xUnits << "\n"
        << "    YUnits         : " << a.yUnits << "\n"
        << "    ZUnits         : " << a.zUnits << "\n";
    for (size_t i = 0; i < a.parameter.size(); ++i)
    {
        os << "    Parameter Name : " << a.parameter[i].getName() << "\n"
            << "    Parameter Value : " << a.parameter[i].str() << "\n";
    }
    return os;
}


std::ostream& operator<< (std::ostream& os, const SupportArray& s)
{
    os << "SupportArray:: \n";
    for (size_t i = 0; i < s.iazArray.size(); ++i)
    {
        os << "  IAZ Array:: \n"
            << s.iazArray[i];
    }
    for (size_t i = 0; i < s.antGainPhase.size(); ++i)
    {
        os << "  Ant Gain Phase:: \n"
            << s.antGainPhase[i];
    }
    for (size_t i = 0; i < s.addedSupportArray.size(); ++i)
    {
        os << "  Added Support Array:: \n"
            << s.addedSupportArray[i];
    }
    return os;
}

}
