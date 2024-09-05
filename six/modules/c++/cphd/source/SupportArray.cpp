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

#include <unordered_map>
#include <six/Init.h>
#include <cphd/SupportArray.h>
#include <cphd/Utilities.h>

namespace cphd
{

SupportArrayParameter::SupportArrayParameter() :
    x0(six::Init::undefined<double>()),
    y0(six::Init::undefined<double>()),
    xSS(six::Init::undefined<double>()),
    ySS(six::Init::undefined<double>()),
    identifier(six::Init::undefined<std::string>())
{
}

SupportArrayParameter::SupportArrayParameter(
        const std::string& format,
        const std::string& id,
        double x0_in,
        double y0_in,
        double xSS_in,
        double ySS_in) :
    elementFormat(format),
    x0(x0_in),
    y0(y0_in),
    xSS(xSS_in),
    ySS(ySS_in),
    identifier(id)
{
    initializeParams();
}

void SupportArrayParameter::initializeParams()
{
    validateFormat(elementFormat);
}

AdditionalSupportArray::AdditionalSupportArray()
{
}

AdditionalSupportArray::AdditionalSupportArray(
        const std::string& format, const std::string& id,
        double x0_in, double y0_in, double xSS_in, double ySS_in,
        const std::string& xUnits_in, const std::string& yUnits_in,
        const std::string& zUnits_in) :
    identifier(id),
    xUnits(xUnits_in),
    yUnits(yUnits_in),
    zUnits(zUnits_in)
{
    elementFormat = format;
    x0 = x0_in;
    y0 = y0_in;
    xSS = xSS_in;
    ySS = ySS_in;
    initializeParams();
}

static SupportArrayParameter getSupportArray(const std::vector<SupportArrayParameter>& params, const std::string& key)
{
    std::vector<int> validKeys;
    for(size_t ii = 0; ii < params.size(); ++ii)
    {
        if (params[ii].getIdentifier()==key)
        {
            // found correct key. so add to validKeys
            validKeys.push_back(ii);
        }
    }

    if(validKeys.empty())
    {
        // if no matching key found, throw an exception
        std::ostringstream oss;
        oss << "SA_ID was not found " << (key);
        throw except::Exception(Ctxt(oss.str()));
    }
    if (validKeys.size()>1)
    {
        // if find repeated key, throw an exception
        std::ostringstream oss;
        oss << "Found multiple support arrays with same SA_ID: " << (key);
        throw except::Exception(Ctxt(oss.str()));        
    }

    return params[validKeys[0]];
}

SupportArrayParameter SupportArray::getIAZSupportArray(const std::string& key) const
{
    return getSupportArray(iazArray, key);
}

SupportArrayParameter SupportArray::getAGPSupportArray(const std::string& key) const
{
    return getSupportArray(antGainPhase, key);
}

SupportArrayParameter SupportArray::getDWTSupportArray(const std::string& key) const
{
    return getSupportArray(dwellTimeArray, key);
}

AdditionalSupportArray SupportArray::getAddedSupportArray(const std::string& key) const
{
    if (addedSupportArray.count(key) == 0 || addedSupportArray.count(key) > 1)
    {
        std::ostringstream oss;
        oss << "SA_ID was not found " << (key);
        throw except::Exception(Ctxt(oss));
    }
    return addedSupportArray.find(key)->second;
}

size_t SupportArray::getSupportDataBytesPerSwap(const std::string& key,
                                                size_t bytesPerElement) const
{
    std::string elemFmt;
    // Unfortunately, need to search each vector/map of support arrays
    if(elemFmt.empty())
    {
        for(size_t ii = 0; ii < iazArray.size(); ++ii)
        {
            if (iazArray[ii].getIdentifier() == key)
            {
                // found correct id, so get element format
                elemFmt = iazArray[ii].elementFormat;
                break;
            }
        }
    }

    if(elemFmt.empty())
    {
        for(size_t ii = 0; ii < antGainPhase.size(); ++ii)
        {
            if (antGainPhase[ii].getIdentifier() == key)
            {
                // found correct id, so get element format
                elemFmt = antGainPhase[ii].elementFormat;
                break;
            }
        }
    }

    if(elemFmt.empty())
    {
        for(size_t ii = 0; ii < dwellTimeArray.size(); ++ii)
        {
            if (dwellTimeArray[ii].getIdentifier() == key)
            {
                // found correct id, so get element format
                elemFmt = dwellTimeArray[ii].elementFormat;
                break;
            }
        }
    }

    if(elemFmt.empty())
    {
        if (addedSupportArray.count(key) > 0)
            {
                // found correct id, so get element format
                elemFmt = addedSupportArray.find(key)->second.elementFormat;
            }
    }

    if(elemFmt.empty())
    {
        std::ostringstream oss;
        oss << "SA_ID was not found" << (key);
        throw except::Exception(Ctxt(oss.str()));
    }

    //Assuming homogeneous component types
    //TODO: Test for validity of this assumption?
    auto eqLoc = elemFmt.find("=");
    auto numSwapsPerElement = str::split(elemFmt, ";").size();
    if (elemFmt[eqLoc + 1] == 'C')
    {
        //Byteswap complex components individually too
        numSwapsPerElement *= 2;
    }
    return bytesPerElement / numSwapsPerElement;
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
    os << (SupportArrayParameter)a
        << "    XUnits         : " << a.xUnits << "\n"
        << "    YUnits         : " << a.yUnits << "\n"
        << "    ZUnits         : " << a.zUnits << "\n";
    for (size_t ii = 0; ii < a.parameter.size(); ++ii)
    {
        out(os, a.parameter[ii]);
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const SupportArray& s)
{
    os << "SupportArray:: \n";
    for (size_t ii = 0; ii < s.iazArray.size(); ++ii)
    {
        os << "  IAZ Array:: \n"
            << s.iazArray[ii];
    }
    for (size_t ii = 0; ii < s.antGainPhase.size(); ++ii)
    {
        os << "  Ant Gain Phase:: \n"
            << s.antGainPhase[ii];
    }
    for (size_t ii = 0; ii < s.dwellTimeArray.size(); ++ii)
    {
        os << "  Dwell Time Array:: \n"
            << s.dwellTimeArray[ii];
    }
    for (auto it = s.addedSupportArray.begin(); it != s.addedSupportArray.end(); ++it)
    {
        os << "  Added Support Array:: \n"
            << "    " << it->first << ": \n"
            << it->second;
    }
    return os;
}

}